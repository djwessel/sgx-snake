#ifndef SNAKE_T_H__
#define SNAKE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "user_types.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum direction_t {
	LEFT = 0,
	RIGHT,
	UP,
	DOWN,
	LEFT_TURN,
	RIGHT_TURN,
	QUIT,
	NUM_KEYS,
} direction_t;

typedef struct snake_segment_t {
	int row;
	int col;
} snake_segment_t;

typedef struct snake_t {
	unsigned int speed;
	direction_t dir;
	int len;
	snake_segment_t body[100];
} snake_t;

typedef struct screen_t {
	int level;
	int score;
	int high_score;
	int gold;
	int obstacles;
	char grid[22][78];
} screen_t;


void ecall_start_game();
void ecall_speed_up(struct screen_t* screen, struct snake_t* snake);
void ecall_setup_level_on_start(struct screen_t* screen, struct snake_t* snake);
void ecall_level_up(struct screen_t* screen, struct snake_t* snake);
void ecall_update_screen_for_level(struct screen_t* screen, struct snake_t* snake, int level);
void ecall_fill_grid_blanks(struct screen_t* screen);
void ecall_fill_grid_objects(struct screen_t* screen);
void ecall_setup_snake_body(struct snake_t* snake, int level);
void ecall_setup_level(struct screen_t* screen, struct snake_t* snake, int level);
void ecall_determine_snake_direction(struct snake_t* snake, char* keys, char key);
void ecall_do_move(struct snake_t* snake);
void ecall_move(struct snake_t* snake, char* keys, char key);
int ecall_collide_walls(struct snake_t* snake);
int ecall_collide_object(struct snake_t* snake, struct screen_t* screen, char object);
int ecall_collide_self(struct snake_t* snake);
int ecall_collision(struct snake_t* snake, struct screen_t* screen);
int ecall_eat_gold(struct snake_t* snake, struct screen_t* screen);
void ecall_malloc_free();
void ecall_sgx_cpuid(int cpuinfo[4], int leaf);
void ecall_exception();
void ecall_map();
size_t ecall_increase_counter();
void ecall_producer();
void ecall_consumer();

sgx_status_t SGX_CDECL ocall_get_char(char* key);
sgx_status_t SGX_CDECL ocall_goto(int x, int y);
sgx_status_t SGX_CDECL ocall_set_usec_delay(unsigned int delay);
sgx_status_t SGX_CDECL ocall_print_game_over();
sgx_status_t SGX_CDECL ocall_print_another_game();
sgx_status_t SGX_CDECL ocall_blank_coordinates(int x, int y);
sgx_status_t SGX_CDECL ocall_display_snake(struct snake_t* snake);
sgx_status_t SGX_CDECL ocall_show_score(struct screen_t* screen);
sgx_status_t SGX_CDECL ocall_setup_playing_board(struct screen_t* screen);
sgx_status_t SGX_CDECL ocall_print_string(const char* str);
sgx_status_t SGX_CDECL ocall_rand(int* ret);
sgx_status_t SGX_CDECL sgx_oc_cpuidex(int cpuinfo[4], int leaf, int subleaf);
sgx_status_t SGX_CDECL sgx_thread_wait_untrusted_event_ocall(int* retval, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_untrusted_event_ocall(int* retval, const void* waiter);
sgx_status_t SGX_CDECL sgx_thread_setwait_untrusted_events_ocall(int* retval, const void* waiter, const void* self);
sgx_status_t SGX_CDECL sgx_thread_set_multiple_untrusted_events_ocall(int* retval, const void** waiters, size_t total);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
