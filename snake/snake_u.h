#ifndef SNAKE_U_H__
#define SNAKE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_satus_t etc. */

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

void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_get_char, (char* key));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_goto, (int x, int y));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_set_usec_delay, (unsigned int delay));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_game_over, ());
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_another_game, ());
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_blank_coordinates, (int x, int y));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_display_snake, (struct snake_t* snake));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_show_score, (struct screen_t* screen));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_setup_playing_board, (struct screen_t* screen));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_string, (const char* str));
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_rand, (int* ret));
void SGX_UBRIDGE(SGX_CDECL, sgx_oc_cpuidex, (int cpuinfo[4], int leaf, int subleaf));
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_wait_untrusted_event_ocall, (const void* self));
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_set_untrusted_event_ocall, (const void* waiter));
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_setwait_untrusted_events_ocall, (const void* waiter, const void* self));
int SGX_UBRIDGE(SGX_CDECL, sgx_thread_set_multiple_untrusted_events_ocall, (const void** waiters, size_t total));

sgx_status_t ecall_start_game(sgx_enclave_id_t eid);
sgx_status_t ecall_speed_up(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake);
sgx_status_t ecall_setup_level_on_start(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake);
sgx_status_t ecall_level_up(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake);
sgx_status_t ecall_update_screen_for_level(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake, int level);
sgx_status_t ecall_fill_grid_blanks(sgx_enclave_id_t eid, struct screen_t* screen);
sgx_status_t ecall_fill_grid_objects(sgx_enclave_id_t eid, struct screen_t* screen);
sgx_status_t ecall_setup_snake_body(sgx_enclave_id_t eid, struct snake_t* snake, int level);
sgx_status_t ecall_setup_level(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake, int level);
sgx_status_t ecall_determine_snake_direction(sgx_enclave_id_t eid, struct snake_t* snake, char* keys, char key);
sgx_status_t ecall_do_move(sgx_enclave_id_t eid, struct snake_t* snake);
sgx_status_t ecall_move(sgx_enclave_id_t eid, struct snake_t* snake, char* keys, char key);
sgx_status_t ecall_collide_walls(sgx_enclave_id_t eid, int* retval, struct snake_t* snake);
sgx_status_t ecall_collide_object(sgx_enclave_id_t eid, int* retval, struct snake_t* snake, struct screen_t* screen, char object);
sgx_status_t ecall_collide_self(sgx_enclave_id_t eid, int* retval, struct snake_t* snake);
sgx_status_t ecall_collision(sgx_enclave_id_t eid, int* retval, struct snake_t* snake, struct screen_t* screen);
sgx_status_t ecall_eat_gold(sgx_enclave_id_t eid, int* retval, struct snake_t* snake, struct screen_t* screen);
sgx_status_t ecall_malloc_free(sgx_enclave_id_t eid);
sgx_status_t ecall_sgx_cpuid(sgx_enclave_id_t eid, int cpuinfo[4], int leaf);
sgx_status_t ecall_exception(sgx_enclave_id_t eid);
sgx_status_t ecall_map(sgx_enclave_id_t eid);
sgx_status_t ecall_increase_counter(sgx_enclave_id_t eid, size_t* retval);
sgx_status_t ecall_producer(sgx_enclave_id_t eid);
sgx_status_t ecall_consumer(sgx_enclave_id_t eid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
