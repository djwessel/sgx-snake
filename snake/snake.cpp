#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>

#include "conio.h"

#include "snake_u.h"

unsigned int usec_delay = DEFAULT_DELAY;

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


void ocall_get_char(char* key)
{
    *key = (char) getchar();
}

void ocall_goto(int x, int y)
{
    gotoxy(x, y);
}

void ocall_set_usec_delay(unisgned int delay)
{
    usec_delay = delay;
}

void ocall_print_game_over()
{
    gotoxy (32, 6);
    textcolor (LIGHTRED);
    printf ("-G A M E  O V E R-");
}

void ocall_print_another_game()
{
    gotoxy (32, 9);
    textcolor (YELLOW);
    printf ("Another Game (y/n)? ");
}

void ocall_blank_coordinates(int x, int y)
{
    textattr (RESETATTR);
    gotoxy (x, y);
    puts (" ");
}

void ocall_display_snake([user_check] const snake_t *const snake)
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

void ocall_show_score([user_check] const screen_t *const screen)
{
    show_level(screen->level);
    show_gold(screen->gold);
    show_current_score(screen->score);
    show_high_score(screen->high_score);
}

void ocall_setup_playing_board([user_check] const screen_t *const screen)
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

