
#include "conio.h"
#include "sgx_trts.h"

#include <stdlib.h>
#include <stdio.h>

#include "snake_t.h"


#define GOLD      '$'
#define CACTUS    '*'

#define MAXROW    22
#define MAXCOL    78

#define START_LEN (4 + level)
#define START_COL (MAXCOL / 2 - START_LEN)
#define START_ROW ((MAXROW + 1) / 2)

#define DEFAULT_DELAY 200000
#define DEFAULT_KEYS  {'o', 'p', 'a', 'z', 'f', 'j', 'q'}

#ifdef DEBUG
#define DBG(fmt, args...) fprintf (stderr, fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

void printf(const char *fmt, ...)
{
    char buf[BUFSIZ] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}

int rand()
{
//TODO: check status
  int* r = new int(0);
  ocall_rand(r);
  return *r;
}

void ecall_speed_up(screen_t* screen, snake_t* snake)
{
    if ((screen->level % 5 == 0) && (snake->speed > 1)) {
        snake->speed--;        /* increase snake->speed every 5 levels */
    }
}

void ecall_setup_level_on_start(screen_t* screen, snake_t* snake)
{
    screen->score = 0;
    screen->obstacles = 4;
    screen->level = 1;
    snake->speed = 14;
    snake->dir = RIGHT;
}

void ecall_level_up(screen_t* screen, snake_t* snake)
{
    screen->score += screen->level * 1000;
    screen->obstacles += 2;
    screen->level++;          /* add to obstacles */
    ecall_speed_up(screen, snake);
}

void ecall_update_screen_for_level(screen_t* screen, snake_t* snake, int level)
{
    /* Set up global variables for new level */
    screen->gold = 0;
    snake->len = level + 4; // weird, snake len will be 5 for level 1 and 4 for all the following levels. 
    ocall_set_usec_delay(DEFAULT_DELAY - level * 10000);
}

void ecall_fill_grid_blanks(screen_t* screen)
{
    /* Fill grid with blanks */
   for (int row = 0; row < MAXROW; row++) {
      for (int col = 0; col < MAXCOL; col++) {
         screen->grid[row][col] = ' ';
      }
   }
}

void ecall_fill_grid_objects(screen_t* screen)
{
    /* Fill grid with objects */
    int row, col;
    for (int i = 0; i < screen->obstacles * 2; i++) {
        /* Find free space to place an object on. */
        do {
            row = rand() % MAXROW;
            col = rand() % MAXCOL;
        } while (screen->grid[row][col] != ' ');

        if (i < screen->obstacles) {
            screen->grid[row][col] = CACTUS;
        } else {
            screen->gold++;
            screen->grid[row][col] = GOLD;
        }
    }
}

void ecall_setup_snake_body(snake_t* snake, int level)
{
   /* Create snake array of length snake->len */
   //needs this for START_COL
   for (int i = 0; i < snake->len; i++)
   {
      snake->body[i].row = START_ROW;
      snake->body[i].col = snake->dir == LEFT ? START_COL - i : START_COL + i;
   }
}

void ecall_setup_level(screen_t* screen, snake_t* snake, int level)
{
   /* Initialize on (re)start */
   if (1 == level) {
       ecall_setup_level_on_start(screen, snake);
   } else {
      ecall_level_up(screen, snake);
   }
   ecall_update_screen_for_level(screen, snake, level);
   ecall_fill_grid_blanks(screen);
   ecall_fill_grid_objects(screen);
   ecall_setup_snake_body(snake, level);

//TODO: check return
   ocall_setup_playing_board(screen);
}

void ecall_determine_snake_direction(snake_t* snake, char* keys, char key)
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

void ecall_do_move(snake_t* snake)
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

void ecall_move(snake_t* snake, char* keys, char key)
{
   ecall_determine_snake_direction(snake, keys, key);
   ecall_do_move(snake);
   /* Blank last segment of snake */
   ocall_blank_coordinates(snake->body[0].col + 1, snake->body[0].row + 1);

   /* ... and remove it from the array */
   for (int i = 1; i <= snake->len; i++) {
      snake->body[i - 1] = snake->body[i];
   }

   /* Display snake in yellow */
   ocall_display_snake(snake);
}

int ecall_collide_walls(snake_t *snake)
{
   snake_segment_t *head = &snake->body[snake->len - 1];

   if ((head->row > MAXROW) || (head->row < 1) ||
       (head->col > MAXCOL) || (head->col < 1)) {
      DBG("Wall collision.\n");
      return 1;
   }

   return 0;
}

int ecall_collide_object(snake_t *snake, screen_t *screen, char object)
{
   snake_segment_t *head = &snake->body[snake->len - 1];

   if (screen->grid[head->row - 1][head->col - 1] == object) {
      DBG("Object '%c' collision.\n", object);
      return 1;
   }

   return 0;
}

int ecall_collide_self(snake_t *snake)
{
   int i;
   snake_segment_t *head = &snake->body[snake->len - 1];

   for (i = 0; i < snake->len - 1; i++) {
      snake_segment_t *body = &snake->body[i];

      if (head->row == body->row && head->col == body->col) {
         DBG("Self collision.\n");
         return 1;
      }
   }

   return 0;
}

int ecall_collision(snake_t *snake, screen_t *screen)
{
   return ecall_collide_walls (snake) ||
      ecall_collide_object (snake, screen, CACTUS) ||
      ecall_collide_self (snake);
}

int ecall_eat_gold(snake_t* snake, screen_t* screen)
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

void ecall_start_game()
{
    char keypress;
    snake_t snake;
    screen_t screen;
    char keys[NUM_KEYS] = DEFAULT_KEYS;

    do {
        ecall_setup_level (&screen, &snake, 1);
        do {
//TODO: check return
	    ocall_get_char(&keypress);
            /* Move the snake one position. */
            ecall_move (&snake, keys, keypress);
            /* keeps cursor flashing in one place instead of following snake */
//TODO: check return
            ocall_goto (1, 1);

            if (ecall_collision (&snake, &screen)) {
                keypress = keys[QUIT];
                break;
            } else if (ecall_collide_object (&snake, &screen, GOLD)) {
                /* If no gold left after consuming this one... */
                if (!ecall_eat_gold (&snake, &screen)) {
                    /* ... then go to next level. */
                    ecall_setup_level (&screen, &snake, 0);
                }
//TODO: check return
                ocall_show_score (&screen);
            }
        }
        while (keypress != keys[QUIT]);

//TODO: check return
        ocall_show_score (&screen);

//TODO: check return
	ocall_print_game_over();
//TODO: check return
	ocall_print_another_game();
        do {
//TODO: check return
            ocall_get_char(&keypress);
        }
        while ((keypress != 'y') && (keypress != 'n'));
    } while (keypress == 'y');
}

