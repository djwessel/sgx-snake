/* Micro Snake, based on a simple simple snake game by Simon Huggins
 *
 * Copyright (c) 2003, 2004  Simon Huggins <webmaster@simonhuggins.com>
 * Copyright (c) 2009  Joachim Nilsson <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Borland Builder C/C++ snake code available at Simon's home page
 * http://www.simonhuggins.com/courses/cbasics/course_notes/snake.htm
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

#include "conio.h"
#include "snake.h"

#ifdef DEBUG
#define DBG(fmt, args...) fprintf (stderr, fmt, ##args)
#else
#define DBG(fmt, args...)
#endif


/*
********** UNTRUSTED SECTION *****************
*/
unsigned int usec_delay = DEFAULT_DELAY;

void set_usec_delay(unsigned int delay)
{
    usec_delay = delay;
}

/* Default 0.2 sec between snake movement. */
int sigsetup (int signo, void (*callback)(int))
{
   struct sigaction action;

   sigemptyset(&action.sa_mask);
   //sigaddset(&action.sa_mask, signo);
   action.sa_flags = 0;
   action.sa_handler = callback;
   if (SIGALRM == signo)
   {
#ifdef SA_INTERRUPT
      action.sa_flags |= SA_INTERRUPT; /* SunOS 4.x */
#endif
   }
   else
   {
#ifdef SA_RESTART
      action.sa_flags |= SA_RESTART; /* SVR4, 4.4BSD */
#endif
   }

   return sigaction (signo, &action, NULL);
}

void sig_handler (int signal __attribute__ ((unused)))
{
   clrscr ();
   DBG("Received signal %d\n", signal);
   exit (WEXITSTATUS(system ("stty sane")));
}

// unstrusted
void alarm_handler (int signal __attribute__ ((unused)))
{
   static struct itimerval val;

   if (!signal)
   {
      sigsetup (SIGALRM, alarm_handler);
   }

   val.it_value.tv_sec  = 0;
   val.it_value.tv_usec = usec_delay;

   setitimer (ITIMER_REAL, &val, NULL);
}

void show_level(int level)
{
   textcolor (LIGHTCYAN);
   gotoxy (3, MAXROW + 2);
   printf ("Level: %05d", level);
}

void show_gold(int gold)
{
    textcolor (YELLOW);
    gotoxy (21, MAXROW + 2);
    printf ("Gold Left: %05d", gold);
}

void show_current_score(int score)
{
   textcolor (LIGHTGREEN);
   gotoxy (43, MAXROW + 2);
   printf ("Score: %05d", score);
}

void show_high_score(int high_score)
{
   textcolor (LIGHTMAGENTA);
   gotoxy (61, MAXROW + 2);
   printf ("High Score: %05d", high_score);
}

void show_score (const screen_t *const screen)
{
    show_level(screen->level);
    show_gold(screen->gold);
    show_current_score(screen->score);
    show_high_score(screen->high_score);
}

void draw_line (int col, int row)
{
   int i;

   gotoxy (col, row);
   textbackground (LIGHTBLUE);
   textcolor (LIGHTBLUE);

   for (i = 0; i < MAXCOL + 2; i++) {
      if (i == 0 || i == MAXCOL + 1)
         printf ("+");
      else
         printf ("-");
   }
   textattr (RESETATTR);
}

void setup_playing_board(const screen_t *const screen)
{
    int row = 0;
    int col = 0;
    /* Draw playing board */
    clrscr();
    draw_line (1, 1);

    for (row = 0; row < MAXROW; row++) {
        gotoxy (1, row + 2);
        textcolor (LIGHTBLUE);
        textbackground (LIGHTBLUE);
        printf ("|");
        textattr (RESETATTR);

        textcolor (WHITE);
        for (col = 0; col < MAXCOL; col++) {
            printf ("%c", screen->grid[row][col]);
        }

        textcolor (LIGHTBLUE);
        textbackground (LIGHTBLUE);
        printf ("|");
        textattr (RESETATTR);
    }

    draw_line (1, MAXROW + 2);

    show_score (screen);

    textcolor (LIGHTRED);
    //gotoxy (3, 1);
    //printf ("h:Help");
    gotoxy (30, 1);
    printf ("[ Micro Snake v%s ]", VERSION);
}

void blank_coordinates(int x, int y)
{
    textattr (RESETATTR);
    gotoxy (x, y);
    puts (" ");
}

void display_snake(const snake_t *const snake)
{
    textbackground (YELLOW);
    for (int i = 0; i < snake->len; i++) {
        gotoxy (snake->body[i].col + 1, snake->body[i].row + 1);
        puts (" ");
    }
    textattr (RESETATTR);
#ifdef DEBUG
    gotoxy (71, 1);
    printf ("(%02d,%02d)", snake->body[snake->len - 1].col, snake->body[snake->len - 1].row);
#endif
}

int check_screen()
{
    if (WEXITSTATUS(system ("stty cbreak -echo stop u")))
    {
        fprintf (stderr, "Failed setting up the screen, is 'stty' missing?\n");
        return 0;
    }
    return 1;
}

char get_char()
{
    return (char) getchar();
}


/*
**************** TRUSTED SECTION *****************
****************** move to enclave ***************
*/

void speed_up(screen_t *screen, snake_t *snake)
{
    if ((screen->level % 5 == 0) && (snake->speed > 1)) {
        snake->speed--;        /* increase snake->speed every 5 levels */
    }
}

void setup_level_on_start(screen_t *screen, snake_t *snake)
{
    screen->score = 0;
    screen->obstacles = 4;
    screen->level = 1;
    snake->speed = 14;
    snake->dir = RIGHT;
}

void level_up(screen_t *screen, snake_t *snake)
{
    screen->score += screen->level * 1000;
    screen->obstacles += 2;
    screen->level++;          /* add to obstacles */
    speed_up(screen, snake);
}

void update_screen_for_level(screen_t* screen, snake_t* snake, int level)
{
    /* Set up global variables for new level */
    screen->gold = 0;
    snake->len = level + 4; // weird, snake len will be 5 for level 1 and 4 for all the following levels. 
    set_usec_delay(DEFAULT_DELAY - level * 10000);
}

void fill_grid_blanks(screen_t* screen)
{
    /* Fill grid with blanks */
   for (int row = 0; row < MAXROW; row++) {
      for (int col = 0; col < MAXCOL; col++) {
         screen->grid[row][col] = ' ';
      }
   }
}

void fill_grid_objects(screen_t* screen)
{
    /* Fill grid with objects */
    int row, col;
    for (int i = 0; i < screen->obstacles * 2; i++) {
        /* Find free space to place an object on. */
        do {
            row = rand () % MAXROW;
            col = rand () % MAXCOL;
        } while (screen->grid[row][col] != ' ');

        if (i < screen->obstacles) {
            screen->grid[row][col] = CACTUS;
        } else {
            screen->gold++;
            screen->grid[row][col] = GOLD;
        }
    }
}

void setup_snake_body(snake_t* snake, int level)
{
   /* Create snake array of length snake->len */
   //needs this for START_COL
   for (int i = 0; i < snake->len; i++)
   {
      snake->body[i].row = START_ROW;
      snake->body[i].col = snake->dir == LEFT ? START_COL - i : START_COL + i;
   }
}

/* If level==0 then just move on to the next level
 * if level==1 restart game
 * Otherwise start game at that level. */
void setup_level (screen_t *screen, snake_t *snake, int level)
{
   /* Initialize on (re)start */
   if (1 == level) {
       setup_level_on_start(screen, snake);
   } else {
      level_up(screen, snake);
   }
   update_screen_for_level(screen, snake, level);
   fill_grid_blanks(screen);
   fill_grid_objects(screen);
   setup_snake_body(snake, level);

   setup_playing_board(screen);
}

void determine_snake_direction(snake_t *snake, char keys[], char key)
{
    direction_t prev = snake->dir;

    if (key == keys[RIGHT]) {
        snake->dir = RIGHT;
    } else if (key == keys[LEFT]) {
        snake->dir = LEFT;
    } else if (key == keys[UP]) {
        snake->dir = UP;
    } else if (key == keys[DOWN]) {
        snake->dir = DOWN;
    } else if (key == keys[LEFT_TURN]) {
        switch (prev) {
        case LEFT:
            snake->dir = DOWN;
            break;
        case RIGHT:
            snake->dir = UP;
            break;
        case UP:
            snake->dir = LEFT;
            break;
        case DOWN:
            snake->dir = RIGHT;
            break;
        default:
            break;
        }
    } else if (key == keys[RIGHT_TURN]) {
        switch (prev) {
        case LEFT:
            snake->dir = UP;
            break;

        case RIGHT:
            snake->dir = DOWN;
            break;

        case UP:
            snake->dir = RIGHT;
            break;

        case DOWN:
            snake->dir = LEFT;
            break;

        default:
            break;
        }
    }
}

void do_move(snake_t* snake)
{
    switch (snake->dir) {
    case LEFT:
        snake->body[snake->len].row = snake->body[snake->len - 1].row;
        snake->body[snake->len].col = snake->body[snake->len - 1].col - 1;
        break;

    case RIGHT:
        snake->body[snake->len].row = snake->body[snake->len - 1].row;
        snake->body[snake->len].col = snake->body[snake->len - 1].col + 1;
        break;

    case UP:
        snake->body[snake->len].row = snake->body[snake->len - 1].row - 1;
        snake->body[snake->len].col = snake->body[snake->len - 1].col;
        break;

    case DOWN:
        snake->body[snake->len].row = snake->body[snake->len - 1].row + 1;
        snake->body[snake->len].col = snake->body[snake->len - 1].col;
        break;

    default:
        /* NOP */
        break;
    }
}

void move(snake_t *snake, char keys[], char key)
{
   determine_snake_direction(snake, keys, key);
   do_move(snake);
   /* Blank last segment of snake */
   blank_coordinates(snake->body[0].col + 1, snake->body[0].row + 1);

   /* ... and remove it from the array */
   for (int i = 1; i <= snake->len; i++) {
      snake->body[i - 1] = snake->body[i];
   }

   /* Display snake in yellow */
   display_snake(snake);
}

int collide_walls (const snake_t *const snake)
{
   const snake_segment_t *const head = &snake->body[snake->len - 1];

   if ((head->row > MAXROW) || (head->row < 1) ||
       (head->col > MAXCOL) || (head->col < 1)) {
      DBG("Wall collision.\n");
      return 1;
   }

   return 0;
}

int collide_object (const snake_t *const snake, const screen_t *const screen, char object)
{
   const snake_segment_t *const head = &snake->body[snake->len - 1];

   if (screen->grid[head->row - 1][head->col - 1] == object) {
      DBG("Object '%c' collision.\n", object);
      return 1;
   }

   return 0;
}

int collide_self (const snake_t *const snake)
{
   int i;
   const snake_segment_t *const head = &snake->body[snake->len - 1];

   for (i = 0; i < snake->len - 1; i++)
   {
      const snake_segment_t *const body = &snake->body[i];

      if (head->row == body->row && head->col == body->col)
      {
         DBG("Self collision.\n");
         return 1;
      }
   }

   return 0;
}

int collision (const snake_t *const snake, const screen_t *const screen)
{
   return collide_walls (snake) ||
      collide_object (snake, screen, CACTUS) ||
      collide_self (snake);
}

int eat_gold (snake_t *snake, screen_t *screen)
{
   snake_segment_t *head = &snake->body[snake->len - 1];

   /* We're called after collide_object() so we know it's
    * a piece of gold at this position.  Eat it up! */
   screen->grid[head->row - 1][head->col - 1] = ' ';

   screen->gold--;
   screen->score += snake->len * screen->obstacles;
   snake->len++;

   if (screen->score > screen->high_score) {
      screen->high_score = screen->score; /* New high score! */
   }

   return screen->gold;
}

void print_game_over()
{
     gotoxy (32, 6);
     textcolor (LIGHTRED);
     printf ("-G A M E  O V E R-");
}

void print_another_game()
{
	gotoxy (32, 9);
	textcolor (YELLOW);
	printf ("Another Game (y/n)? ");
}

void start_game()
{
    char keypress;
    snake_t snake;
    screen_t screen;
    char keys[NUM_KEYS] = DEFAULT_KEYS;

    do {
        setup_level (&screen, &snake, 1);
        do {
            keypress = get_char();
            /* Move the snake one position. */
            move (&snake, keys, keypress);
            /* keeps cursor flashing in one place instead of following snake */
            gotoxy (1, 1);

            if (collision (&snake, &screen)) {
                keypress = keys[QUIT];
                break;
            } else if (collide_object (&snake, &screen, GOLD)) {
                /* If no gold left after consuming this one... */
                if (!eat_gold (&snake, &screen)) {
                    /* ... then go to next level. */
                    setup_level (&screen, &snake, 0);
                }
                show_score (&screen);
            }
        }
        while (keypress != keys[QUIT]);

        show_score (&screen);

	print_game_over();
	print_another_game();

	do
        {
            keypress = getchar ();
        }
        while ((keypress != 'y') && (keypress != 'n'));
    } while (keypress == 'y');
}

/****************** MAIN UNTRUSTED *************/

int main (void)
{
   if (!check_screen()) {
        return 1;
   }
   printf("start\n");
   srand ((unsigned int)time (NULL));
    /* Call it once to initialize the timer. */
   alarm_handler (0);

   sigsetup (SIGINT, sig_handler);
   sigsetup (SIGHUP, sig_handler);
   sigsetup (SIGTERM, sig_handler);

   start_game();

   clrscr ();

   return WEXITSTATUS(system ("stty sane"));
}


/**
 * Local Variables:
 *  version-control: t
 *  c-file-style: "ellemtel"
 * End:
 */
