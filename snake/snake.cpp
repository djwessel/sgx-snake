#include "snake.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <string.h>

# include <unistd.h>
# include <pwd.h>
# define MAX_PATH FILENAME_MAX

#include "sgx_urts.h"
#include "snake_u.h"

#include "conio.h"

void ocall_write_file (uint8_t *buf, uint32_t size) {
    
    FILE *pFile = fopen("test.seal", "wb");

    if (pFile){
        fwrite(buf, size, 1, pFile);
        fclose(pFile);
    }

}

void ocall_read_file (uint8_t *buf, uint32_t size) {
    FILE *pFile = fopen("test.seal", "rb");

    if (pFile){
        fread(buf, size, 1, pFile);
        fclose(pFile);
    }

}

unsigned int usec_delay = DEFAULT_DELAY;
void gotoxy(int x, int y)
{
    printf("\e[%d;%dH", y, x);
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

void ocall_print_string(const char *str)
{
    /* Proxy/Bridge will check the length and null-terminate 
     * the input string to prevent buffer overflow. 
     */
    printf("%s", str);
}

void ocall_rand(int* ret)
{
    *ret = rand();
}

void ocall_get_char(char* key)
{
    *key = (char) getchar();
}

void ocall_goto(int x, int y)
{
    gotoxy(x, y);
}

void ocall_set_usec_delay(unsigned int delay)
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

void ocall_display_snake(snake_t *snake)
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

void ocall_show_score(screen_t *screen)
{
    show_level(screen->level);
    show_gold(screen->gold);
    show_current_score(screen->score);
    show_high_score(screen->high_score);
}

void ocall_setup_playing_board(screen_t *screen)
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

    ocall_show_score (screen);

    textcolor (LIGHTRED);
    //gotoxy (3, 1);
    //printf ("h:Help");
    gotoxy (30, 1);
    printf ("[ Micro Snake v%s ]", VERSION);
}

/* Global EID shared by multiple threads */
sgx_enclave_id_t global_eid = 0;

typedef struct _sgx_errlist_t {
    sgx_status_t err;
    const char *msg;
    const char *sug; /* Suggestion */
} sgx_errlist_t;

/* Error code returned by sgx_create_enclave */
static sgx_errlist_t sgx_errlist[] = {
    {
        SGX_ERROR_UNEXPECTED,
        "Unexpected error occurred.",
        NULL
    },
    {
        SGX_ERROR_INVALID_PARAMETER,
        "Invalid parameter.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_MEMORY,
        "Out of memory.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_LOST,
        "Power transition occurred.",
        "Please refer to the sample \"PowerTransition\" for details."
    },
    {
        SGX_ERROR_INVALID_ENCLAVE,
        "Invalid enclave image.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ENCLAVE_ID,
        "Invalid enclave identification.",
        NULL
    },
    {
        SGX_ERROR_INVALID_SIGNATURE,
        "Invalid enclave signature.",
        NULL
    },
    {
        SGX_ERROR_OUT_OF_EPC,
        "Out of EPC memory.",
        NULL
    },
    {
        SGX_ERROR_NO_DEVICE,
        "Invalid SGX device.",
        "Please make sure SGX module is enabled in the BIOS, and install SGX driver afterwards."
    },
    {
        SGX_ERROR_MEMORY_MAP_CONFLICT,
        "Memory map conflicted.",
        NULL
    },
    {
        SGX_ERROR_INVALID_METADATA,
        "Invalid enclave metadata.",
        NULL
    },
    {
        SGX_ERROR_DEVICE_BUSY,
        "SGX device was busy.",
        NULL
    },
    {
        SGX_ERROR_INVALID_VERSION,
        "Enclave version was invalid.",
        NULL
    },
    {
        SGX_ERROR_INVALID_ATTRIBUTE,
        "Enclave was not authorized.",
        NULL
    },
    {
        SGX_ERROR_ENCLAVE_FILE_ACCESS,
        "Can't open enclave file.",
        NULL
    },
};

/* Check error conditions for loading enclave */
void print_error_message(sgx_status_t ret)
{
    size_t idx = 0;
    size_t ttl = sizeof sgx_errlist/sizeof sgx_errlist[0];

    for (idx = 0; idx < ttl; idx++) {
        if(ret == sgx_errlist[idx].err) {
            if(NULL != sgx_errlist[idx].sug)
                printf("Info: %s\n", sgx_errlist[idx].sug);
            printf("Error: %s\n", sgx_errlist[idx].msg);
            break;
        }
    }
    
    if (idx == ttl)
        printf("Error: Unexpected error occurred.\n");
}

/* Initialize the enclave:
 *   Step 1: try to retrieve the launch token saved by last transaction
 *   Step 2: call sgx_create_enclave to initialize an enclave instance
 *   Step 3: save the launch token if it is updated
 */
int initialize_enclave(void)
{
    char token_path[MAX_PATH] = {'\0'};
    sgx_launch_token_t token = {0};
    sgx_status_t ret = SGX_ERROR_UNEXPECTED;
    int updated = 0;
    
    /* Step 1: try to retrieve the launch token saved by last transaction 
     *         if there is no token, then create a new one.
     */
    /* try to get the token saved in $HOME */
    const char *home_dir = getpwuid(getuid())->pw_dir;
    
    if (home_dir != NULL && 
        (strlen(home_dir)+strlen("/")+sizeof(TOKEN_FILENAME)+1) <= MAX_PATH) {
        /* compose the token path */
        strncpy(token_path, home_dir, strlen(home_dir));
        strncat(token_path, "/", strlen("/"));
        strncat(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME)+1);
    } else {
        /* if token path is too long or $HOME is NULL */
        strncpy(token_path, TOKEN_FILENAME, sizeof(TOKEN_FILENAME));
    }

    FILE *fp = fopen(token_path, "rb");
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL) {
        printf("Warning: Failed to create/open the launch token file \"%s\".\n", token_path);
    }

    if (fp != NULL) {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t)) {
            /* if token is invalid, clear the buffer */
            memset(&token, 0x0, sizeof(sgx_launch_token_t));
            printf("Warning: Invalid launch token read from \"%s\".\n", token_path);
        }
    }
    /* Step 2: call sgx_create_enclave to initialize an enclave instance */
    /* Debug Support: set 2nd parameter to 1 */
    ret = sgx_create_enclave(ENCLAVE_FILENAME, SGX_DEBUG_FLAG, &token, &updated, &global_eid, NULL);
    if (ret != SGX_SUCCESS) {
        print_error_message(ret);
        if (fp != NULL) fclose(fp);
        return -1;
    }

    /* Step 3: save the launch token if it is updated */
    if (updated == FALSE || fp == NULL) {
        /* if the token is not updated, or file handler is invalid, do not perform saving */
        if (fp != NULL) fclose(fp);
        return 0;
    }

    /* reopen the file with write capablity */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL) return 0;
    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);
    if (write_num != sizeof(sgx_launch_token_t))
        printf("Warning: Failed to save launch token to \"%s\".\n", token_path);
    fclose(fp);
    return 0;
}

int check_screen()
{
    int status = system("stty cbreak -echo stop u");
    if (WEXITSTATUS(status)) 
    {
        fprintf (stderr, "Failed setting up the screen, is 'stty' missing?\n");
        return 0;
    }
    return 1;
}

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
   int status = system("stty sane");
   exit (WEXITSTATUS(status));
}


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

/* Application entry */
int SGX_CDECL main(int argc, char *argv[])
{
    (void)(argc);
    (void)(argv);


    /* Initialize the enclave */
    if(initialize_enclave() < 0){
        printf("Enter a character before exit ...\n");
        getchar();
        return -1; 
    }

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

    ecall_start_game(global_eid);

    clrscr ();


    /* Utilize trusted libraries */
    ecall_libc_functions();
    ecall_libcxx_functions();
    ecall_thread_functions();

    /* Destroy the enclave */
    sgx_destroy_enclave(global_eid);
    
    printf("Info: SampleEnclave successfully returned.\n");

    printf("Enter a character before exit ...\n");
    getchar();
    int status = system("stty sane");
    return WEXITSTATUS(status);
}

