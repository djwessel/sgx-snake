#include "snake_u.h"
#include <errno.h>


typedef struct ms_ecall_speed_up_t {
	struct screen_t* ms_screen;
	struct snake_t* ms_snake;
} ms_ecall_speed_up_t;

typedef struct ms_ecall_setup_level_on_start_t {
	struct screen_t* ms_screen;
	struct snake_t* ms_snake;
} ms_ecall_setup_level_on_start_t;

typedef struct ms_ecall_level_up_t {
	struct screen_t* ms_screen;
	struct snake_t* ms_snake;
} ms_ecall_level_up_t;

typedef struct ms_ecall_update_screen_for_level_t {
	struct screen_t* ms_screen;
	struct snake_t* ms_snake;
	int ms_level;
} ms_ecall_update_screen_for_level_t;

typedef struct ms_ecall_fill_grid_blanks_t {
	struct screen_t* ms_screen;
} ms_ecall_fill_grid_blanks_t;

typedef struct ms_ecall_fill_grid_objects_t {
	struct screen_t* ms_screen;
} ms_ecall_fill_grid_objects_t;

typedef struct ms_ecall_setup_snake_body_t {
	struct snake_t* ms_snake;
	int ms_level;
} ms_ecall_setup_snake_body_t;

typedef struct ms_ecall_setup_level_t {
	struct screen_t* ms_screen;
	struct snake_t* ms_snake;
	int ms_level;
} ms_ecall_setup_level_t;

typedef struct ms_ecall_determine_snake_direction_t {
	struct snake_t* ms_snake;
	char* ms_keys;
	char ms_key;
} ms_ecall_determine_snake_direction_t;

typedef struct ms_ecall_do_move_t {
	struct snake_t* ms_snake;
} ms_ecall_do_move_t;

typedef struct ms_ecall_move_t {
	struct snake_t* ms_snake;
	char* ms_keys;
	char ms_key;
} ms_ecall_move_t;

typedef struct ms_ecall_collide_walls_t {
	int ms_retval;
	struct snake_t* ms_snake;
} ms_ecall_collide_walls_t;

typedef struct ms_ecall_collide_object_t {
	int ms_retval;
	struct snake_t* ms_snake;
	struct screen_t* ms_screen;
	char ms_object;
} ms_ecall_collide_object_t;

typedef struct ms_ecall_collide_self_t {
	int ms_retval;
	struct snake_t* ms_snake;
} ms_ecall_collide_self_t;

typedef struct ms_ecall_collision_t {
	int ms_retval;
	struct snake_t* ms_snake;
	struct screen_t* ms_screen;
} ms_ecall_collision_t;

typedef struct ms_ecall_eat_gold_t {
	int ms_retval;
	struct snake_t* ms_snake;
	struct screen_t* ms_screen;
} ms_ecall_eat_gold_t;


typedef struct ms_ecall_sgx_cpuid_t {
	int* ms_cpuinfo;
	int ms_leaf;
} ms_ecall_sgx_cpuid_t;



typedef struct ms_ecall_increase_counter_t {
	size_t ms_retval;
} ms_ecall_increase_counter_t;



typedef struct ms_ocall_get_char_t {
	char* ms_key;
} ms_ocall_get_char_t;

typedef struct ms_ocall_goto_t {
	int ms_x;
	int ms_y;
} ms_ocall_goto_t;

typedef struct ms_ocall_set_usec_delay_t {
	unsigned int ms_delay;
} ms_ocall_set_usec_delay_t;



typedef struct ms_ocall_blank_coordinates_t {
	int ms_x;
	int ms_y;
} ms_ocall_blank_coordinates_t;

typedef struct ms_ocall_display_snake_t {
	struct snake_t* ms_snake;
} ms_ocall_display_snake_t;

typedef struct ms_ocall_show_score_t {
	struct screen_t* ms_screen;
} ms_ocall_show_score_t;

typedef struct ms_ocall_setup_playing_board_t {
	struct screen_t* ms_screen;
} ms_ocall_setup_playing_board_t;

typedef struct ms_ocall_print_string_t {
	char* ms_str;
} ms_ocall_print_string_t;

typedef struct ms_ocall_rand_t {
	int* ms_ret;
} ms_ocall_rand_t;

typedef struct ms_sgx_oc_cpuidex_t {
	int* ms_cpuinfo;
	int ms_leaf;
	int ms_subleaf;
} ms_sgx_oc_cpuidex_t;

typedef struct ms_sgx_thread_wait_untrusted_event_ocall_t {
	int ms_retval;
	void* ms_self;
} ms_sgx_thread_wait_untrusted_event_ocall_t;

typedef struct ms_sgx_thread_set_untrusted_event_ocall_t {
	int ms_retval;
	void* ms_waiter;
} ms_sgx_thread_set_untrusted_event_ocall_t;

typedef struct ms_sgx_thread_setwait_untrusted_events_ocall_t {
	int ms_retval;
	void* ms_waiter;
	void* ms_self;
} ms_sgx_thread_setwait_untrusted_events_ocall_t;

typedef struct ms_sgx_thread_set_multiple_untrusted_events_ocall_t {
	int ms_retval;
	void** ms_waiters;
	size_t ms_total;
} ms_sgx_thread_set_multiple_untrusted_events_ocall_t;

static sgx_status_t SGX_CDECL snake_ocall_get_char(void* pms)
{
	ms_ocall_get_char_t* ms = SGX_CAST(ms_ocall_get_char_t*, pms);
	ocall_get_char(ms->ms_key);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_goto(void* pms)
{
	ms_ocall_goto_t* ms = SGX_CAST(ms_ocall_goto_t*, pms);
	ocall_goto(ms->ms_x, ms->ms_y);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_set_usec_delay(void* pms)
{
	ms_ocall_set_usec_delay_t* ms = SGX_CAST(ms_ocall_set_usec_delay_t*, pms);
	ocall_set_usec_delay(ms->ms_delay);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_print_game_over(void* pms)
{
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ocall_print_game_over();
	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_print_another_game(void* pms)
{
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ocall_print_another_game();
	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_blank_coordinates(void* pms)
{
	ms_ocall_blank_coordinates_t* ms = SGX_CAST(ms_ocall_blank_coordinates_t*, pms);
	ocall_blank_coordinates(ms->ms_x, ms->ms_y);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_display_snake(void* pms)
{
	ms_ocall_display_snake_t* ms = SGX_CAST(ms_ocall_display_snake_t*, pms);
	ocall_display_snake(ms->ms_snake);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_show_score(void* pms)
{
	ms_ocall_show_score_t* ms = SGX_CAST(ms_ocall_show_score_t*, pms);
	ocall_show_score(ms->ms_screen);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_setup_playing_board(void* pms)
{
	ms_ocall_setup_playing_board_t* ms = SGX_CAST(ms_ocall_setup_playing_board_t*, pms);
	ocall_setup_playing_board(ms->ms_screen);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_print_string(void* pms)
{
	ms_ocall_print_string_t* ms = SGX_CAST(ms_ocall_print_string_t*, pms);
	ocall_print_string((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_ocall_rand(void* pms)
{
	ms_ocall_rand_t* ms = SGX_CAST(ms_ocall_rand_t*, pms);
	ocall_rand(ms->ms_ret);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_sgx_oc_cpuidex(void* pms)
{
	ms_sgx_oc_cpuidex_t* ms = SGX_CAST(ms_sgx_oc_cpuidex_t*, pms);
	sgx_oc_cpuidex(ms->ms_cpuinfo, ms->ms_leaf, ms->ms_subleaf);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_sgx_thread_wait_untrusted_event_ocall(void* pms)
{
	ms_sgx_thread_wait_untrusted_event_ocall_t* ms = SGX_CAST(ms_sgx_thread_wait_untrusted_event_ocall_t*, pms);
	ms->ms_retval = sgx_thread_wait_untrusted_event_ocall((const void*)ms->ms_self);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_sgx_thread_set_untrusted_event_ocall(void* pms)
{
	ms_sgx_thread_set_untrusted_event_ocall_t* ms = SGX_CAST(ms_sgx_thread_set_untrusted_event_ocall_t*, pms);
	ms->ms_retval = sgx_thread_set_untrusted_event_ocall((const void*)ms->ms_waiter);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_sgx_thread_setwait_untrusted_events_ocall(void* pms)
{
	ms_sgx_thread_setwait_untrusted_events_ocall_t* ms = SGX_CAST(ms_sgx_thread_setwait_untrusted_events_ocall_t*, pms);
	ms->ms_retval = sgx_thread_setwait_untrusted_events_ocall((const void*)ms->ms_waiter, (const void*)ms->ms_self);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL snake_sgx_thread_set_multiple_untrusted_events_ocall(void* pms)
{
	ms_sgx_thread_set_multiple_untrusted_events_ocall_t* ms = SGX_CAST(ms_sgx_thread_set_multiple_untrusted_events_ocall_t*, pms);
	ms->ms_retval = sgx_thread_set_multiple_untrusted_events_ocall((const void**)ms->ms_waiters, ms->ms_total);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[16];
} ocall_table_snake = {
	16,
	{
		(void*)snake_ocall_get_char,
		(void*)snake_ocall_goto,
		(void*)snake_ocall_set_usec_delay,
		(void*)snake_ocall_print_game_over,
		(void*)snake_ocall_print_another_game,
		(void*)snake_ocall_blank_coordinates,
		(void*)snake_ocall_display_snake,
		(void*)snake_ocall_show_score,
		(void*)snake_ocall_setup_playing_board,
		(void*)snake_ocall_print_string,
		(void*)snake_ocall_rand,
		(void*)snake_sgx_oc_cpuidex,
		(void*)snake_sgx_thread_wait_untrusted_event_ocall,
		(void*)snake_sgx_thread_set_untrusted_event_ocall,
		(void*)snake_sgx_thread_setwait_untrusted_events_ocall,
		(void*)snake_sgx_thread_set_multiple_untrusted_events_ocall,
	}
};
sgx_status_t ecall_start_game(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 0, &ocall_table_snake, NULL);
	return status;
}

sgx_status_t ecall_speed_up(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake)
{
	sgx_status_t status;
	ms_ecall_speed_up_t ms;
	ms.ms_screen = screen;
	ms.ms_snake = snake;
	status = sgx_ecall(eid, 1, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_setup_level_on_start(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake)
{
	sgx_status_t status;
	ms_ecall_setup_level_on_start_t ms;
	ms.ms_screen = screen;
	ms.ms_snake = snake;
	status = sgx_ecall(eid, 2, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_level_up(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake)
{
	sgx_status_t status;
	ms_ecall_level_up_t ms;
	ms.ms_screen = screen;
	ms.ms_snake = snake;
	status = sgx_ecall(eid, 3, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_update_screen_for_level(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake, int level)
{
	sgx_status_t status;
	ms_ecall_update_screen_for_level_t ms;
	ms.ms_screen = screen;
	ms.ms_snake = snake;
	ms.ms_level = level;
	status = sgx_ecall(eid, 4, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_fill_grid_blanks(sgx_enclave_id_t eid, struct screen_t* screen)
{
	sgx_status_t status;
	ms_ecall_fill_grid_blanks_t ms;
	ms.ms_screen = screen;
	status = sgx_ecall(eid, 5, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_fill_grid_objects(sgx_enclave_id_t eid, struct screen_t* screen)
{
	sgx_status_t status;
	ms_ecall_fill_grid_objects_t ms;
	ms.ms_screen = screen;
	status = sgx_ecall(eid, 6, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_setup_snake_body(sgx_enclave_id_t eid, struct snake_t* snake, int level)
{
	sgx_status_t status;
	ms_ecall_setup_snake_body_t ms;
	ms.ms_snake = snake;
	ms.ms_level = level;
	status = sgx_ecall(eid, 7, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_setup_level(sgx_enclave_id_t eid, struct screen_t* screen, struct snake_t* snake, int level)
{
	sgx_status_t status;
	ms_ecall_setup_level_t ms;
	ms.ms_screen = screen;
	ms.ms_snake = snake;
	ms.ms_level = level;
	status = sgx_ecall(eid, 8, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_determine_snake_direction(sgx_enclave_id_t eid, struct snake_t* snake, char* keys, char key)
{
	sgx_status_t status;
	ms_ecall_determine_snake_direction_t ms;
	ms.ms_snake = snake;
	ms.ms_keys = keys;
	ms.ms_key = key;
	status = sgx_ecall(eid, 9, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_do_move(sgx_enclave_id_t eid, struct snake_t* snake)
{
	sgx_status_t status;
	ms_ecall_do_move_t ms;
	ms.ms_snake = snake;
	status = sgx_ecall(eid, 10, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_move(sgx_enclave_id_t eid, struct snake_t* snake, char* keys, char key)
{
	sgx_status_t status;
	ms_ecall_move_t ms;
	ms.ms_snake = snake;
	ms.ms_keys = keys;
	ms.ms_key = key;
	status = sgx_ecall(eid, 11, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_collide_walls(sgx_enclave_id_t eid, int* retval, struct snake_t* snake)
{
	sgx_status_t status;
	ms_ecall_collide_walls_t ms;
	ms.ms_snake = snake;
	status = sgx_ecall(eid, 12, &ocall_table_snake, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_collide_object(sgx_enclave_id_t eid, int* retval, struct snake_t* snake, struct screen_t* screen, char object)
{
	sgx_status_t status;
	ms_ecall_collide_object_t ms;
	ms.ms_snake = snake;
	ms.ms_screen = screen;
	ms.ms_object = object;
	status = sgx_ecall(eid, 13, &ocall_table_snake, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_collide_self(sgx_enclave_id_t eid, int* retval, struct snake_t* snake)
{
	sgx_status_t status;
	ms_ecall_collide_self_t ms;
	ms.ms_snake = snake;
	status = sgx_ecall(eid, 14, &ocall_table_snake, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_collision(sgx_enclave_id_t eid, int* retval, struct snake_t* snake, struct screen_t* screen)
{
	sgx_status_t status;
	ms_ecall_collision_t ms;
	ms.ms_snake = snake;
	ms.ms_screen = screen;
	status = sgx_ecall(eid, 15, &ocall_table_snake, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_eat_gold(sgx_enclave_id_t eid, int* retval, struct snake_t* snake, struct screen_t* screen)
{
	sgx_status_t status;
	ms_ecall_eat_gold_t ms;
	ms.ms_snake = snake;
	ms.ms_screen = screen;
	status = sgx_ecall(eid, 16, &ocall_table_snake, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_malloc_free(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 17, &ocall_table_snake, NULL);
	return status;
}

sgx_status_t ecall_sgx_cpuid(sgx_enclave_id_t eid, int cpuinfo[4], int leaf)
{
	sgx_status_t status;
	ms_ecall_sgx_cpuid_t ms;
	ms.ms_cpuinfo = (int*)cpuinfo;
	ms.ms_leaf = leaf;
	status = sgx_ecall(eid, 18, &ocall_table_snake, &ms);
	return status;
}

sgx_status_t ecall_exception(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 19, &ocall_table_snake, NULL);
	return status;
}

sgx_status_t ecall_map(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 20, &ocall_table_snake, NULL);
	return status;
}

sgx_status_t ecall_increase_counter(sgx_enclave_id_t eid, size_t* retval)
{
	sgx_status_t status;
	ms_ecall_increase_counter_t ms;
	status = sgx_ecall(eid, 21, &ocall_table_snake, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t ecall_producer(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 22, &ocall_table_snake, NULL);
	return status;
}

sgx_status_t ecall_consumer(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 23, &ocall_table_snake, NULL);
	return status;
}

