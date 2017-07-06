#include "snake_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */

#include <errno.h>
#include <string.h> /* for memcpy etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)



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

static sgx_status_t SGX_CDECL sgx_ecall_start_game(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_start_game();
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_speed_up(void* pms)
{
	ms_ecall_speed_up_t* ms = SGX_CAST(ms_ecall_speed_up_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_speed_up_t));
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ecall_speed_up(_in_screen, _in_snake);
err:
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_setup_level_on_start(void* pms)
{
	ms_ecall_setup_level_on_start_t* ms = SGX_CAST(ms_ecall_setup_level_on_start_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_setup_level_on_start_t));
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ecall_setup_level_on_start(_in_screen, _in_snake);
err:
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_level_up(void* pms)
{
	ms_ecall_level_up_t* ms = SGX_CAST(ms_ecall_level_up_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_level_up_t));
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ecall_level_up(_in_screen, _in_snake);
err:
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_update_screen_for_level(void* pms)
{
	ms_ecall_update_screen_for_level_t* ms = SGX_CAST(ms_ecall_update_screen_for_level_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_update_screen_for_level_t));
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ecall_update_screen_for_level(_in_screen, _in_snake, ms->ms_level);
err:
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_fill_grid_blanks(void* pms)
{
	ms_ecall_fill_grid_blanks_t* ms = SGX_CAST(ms_ecall_fill_grid_blanks_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_fill_grid_blanks_t));
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);

	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	ecall_fill_grid_blanks(_in_screen);
err:
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_fill_grid_objects(void* pms)
{
	ms_ecall_fill_grid_objects_t* ms = SGX_CAST(ms_ecall_fill_grid_objects_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_fill_grid_objects_t));
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);

	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	ecall_fill_grid_objects(_in_screen);
err:
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_setup_snake_body(void* pms)
{
	ms_ecall_setup_snake_body_t* ms = SGX_CAST(ms_ecall_setup_snake_body_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_setup_snake_body_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ecall_setup_snake_body(_in_snake, ms->ms_level);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_setup_level(void* pms)
{
	ms_ecall_setup_level_t* ms = SGX_CAST(ms_ecall_setup_level_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_setup_level_t));
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ecall_setup_level(_in_screen, _in_snake, ms->ms_level);
err:
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_determine_snake_direction(void* pms)
{
	ms_ecall_determine_snake_direction_t* ms = SGX_CAST(ms_ecall_determine_snake_direction_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;
	char* _tmp_keys = ms->ms_keys;
	size_t _len_keys = sizeof(*_tmp_keys);
	char* _in_keys = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_determine_snake_direction_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);
	CHECK_UNIQUE_POINTER(_tmp_keys, _len_keys);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	if (_tmp_keys != NULL) {
		_in_keys = (char*)malloc(_len_keys);
		if (_in_keys == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_keys, _tmp_keys, _len_keys);
	}
	ecall_determine_snake_direction(_in_snake, _in_keys, ms->ms_key);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}
	if (_in_keys) {
		memcpy(_tmp_keys, _in_keys, _len_keys);
		free(_in_keys);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_do_move(void* pms)
{
	ms_ecall_do_move_t* ms = SGX_CAST(ms_ecall_do_move_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_do_move_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ecall_do_move(_in_snake);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_move(void* pms)
{
	ms_ecall_move_t* ms = SGX_CAST(ms_ecall_move_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;
	char* _tmp_keys = ms->ms_keys;
	size_t _len_keys = sizeof(*_tmp_keys);
	char* _in_keys = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_move_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);
	CHECK_UNIQUE_POINTER(_tmp_keys, _len_keys);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	if (_tmp_keys != NULL) {
		_in_keys = (char*)malloc(_len_keys);
		if (_in_keys == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_keys, _tmp_keys, _len_keys);
	}
	ecall_move(_in_snake, _in_keys, ms->ms_key);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}
	if (_in_keys) {
		memcpy(_tmp_keys, _in_keys, _len_keys);
		free(_in_keys);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_collide_walls(void* pms)
{
	ms_ecall_collide_walls_t* ms = SGX_CAST(ms_ecall_collide_walls_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_collide_walls_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ms->ms_retval = ecall_collide_walls(_in_snake);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_collide_object(void* pms)
{
	ms_ecall_collide_object_t* ms = SGX_CAST(ms_ecall_collide_object_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_collide_object_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	ms->ms_retval = ecall_collide_object(_in_snake, _in_screen, ms->ms_object);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_collide_self(void* pms)
{
	ms_ecall_collide_self_t* ms = SGX_CAST(ms_ecall_collide_self_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_collide_self_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	ms->ms_retval = ecall_collide_self(_in_snake);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_collision(void* pms)
{
	ms_ecall_collision_t* ms = SGX_CAST(ms_ecall_collision_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_collision_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	ms->ms_retval = ecall_collision(_in_snake, _in_screen);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_eat_gold(void* pms)
{
	ms_ecall_eat_gold_t* ms = SGX_CAST(ms_ecall_eat_gold_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	struct snake_t* _tmp_snake = ms->ms_snake;
	size_t _len_snake = sizeof(*_tmp_snake);
	struct snake_t* _in_snake = NULL;
	struct screen_t* _tmp_screen = ms->ms_screen;
	size_t _len_screen = sizeof(*_tmp_screen);
	struct screen_t* _in_screen = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_eat_gold_t));
	CHECK_UNIQUE_POINTER(_tmp_snake, _len_snake);
	CHECK_UNIQUE_POINTER(_tmp_screen, _len_screen);

	if (_tmp_snake != NULL) {
		_in_snake = (struct snake_t*)malloc(_len_snake);
		if (_in_snake == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_snake, _tmp_snake, _len_snake);
	}
	if (_tmp_screen != NULL) {
		_in_screen = (struct screen_t*)malloc(_len_screen);
		if (_in_screen == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_screen, _tmp_screen, _len_screen);
	}
	ms->ms_retval = ecall_eat_gold(_in_snake, _in_screen);
err:
	if (_in_snake) {
		memcpy(_tmp_snake, _in_snake, _len_snake);
		free(_in_snake);
	}
	if (_in_screen) {
		memcpy(_tmp_screen, _in_screen, _len_screen);
		free(_in_screen);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_malloc_free(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_malloc_free();
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_sgx_cpuid(void* pms)
{
	ms_ecall_sgx_cpuid_t* ms = SGX_CAST(ms_ecall_sgx_cpuid_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	int* _tmp_cpuinfo = ms->ms_cpuinfo;
	size_t _len_cpuinfo = 4 * sizeof(*_tmp_cpuinfo);
	int* _in_cpuinfo = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_sgx_cpuid_t));
	CHECK_UNIQUE_POINTER(_tmp_cpuinfo, _len_cpuinfo);

	if (_tmp_cpuinfo != NULL) {
		_in_cpuinfo = (int*)malloc(_len_cpuinfo);
		if (_in_cpuinfo == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_cpuinfo, _tmp_cpuinfo, _len_cpuinfo);
	}
	ecall_sgx_cpuid(_in_cpuinfo, ms->ms_leaf);
err:
	if (_in_cpuinfo) {
		memcpy(_tmp_cpuinfo, _in_cpuinfo, _len_cpuinfo);
		free(_in_cpuinfo);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_exception(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_exception();
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_map(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_map();
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_increase_counter(void* pms)
{
	ms_ecall_increase_counter_t* ms = SGX_CAST(ms_ecall_increase_counter_t*, pms);
	sgx_status_t status = SGX_SUCCESS;

	CHECK_REF_POINTER(pms, sizeof(ms_ecall_increase_counter_t));

	ms->ms_retval = ecall_increase_counter();


	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_producer(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_producer();
	return status;
}

static sgx_status_t SGX_CDECL sgx_ecall_consumer(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	ecall_consumer();
	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[24];
} g_ecall_table = {
	24,
	{
		{(void*)(uintptr_t)sgx_ecall_start_game, 0},
		{(void*)(uintptr_t)sgx_ecall_speed_up, 1},
		{(void*)(uintptr_t)sgx_ecall_setup_level_on_start, 1},
		{(void*)(uintptr_t)sgx_ecall_level_up, 1},
		{(void*)(uintptr_t)sgx_ecall_update_screen_for_level, 1},
		{(void*)(uintptr_t)sgx_ecall_fill_grid_blanks, 1},
		{(void*)(uintptr_t)sgx_ecall_fill_grid_objects, 1},
		{(void*)(uintptr_t)sgx_ecall_setup_snake_body, 1},
		{(void*)(uintptr_t)sgx_ecall_setup_level, 1},
		{(void*)(uintptr_t)sgx_ecall_determine_snake_direction, 1},
		{(void*)(uintptr_t)sgx_ecall_do_move, 1},
		{(void*)(uintptr_t)sgx_ecall_move, 1},
		{(void*)(uintptr_t)sgx_ecall_collide_walls, 1},
		{(void*)(uintptr_t)sgx_ecall_collide_object, 1},
		{(void*)(uintptr_t)sgx_ecall_collide_self, 1},
		{(void*)(uintptr_t)sgx_ecall_collision, 1},
		{(void*)(uintptr_t)sgx_ecall_eat_gold, 1},
		{(void*)(uintptr_t)sgx_ecall_malloc_free, 0},
		{(void*)(uintptr_t)sgx_ecall_sgx_cpuid, 0},
		{(void*)(uintptr_t)sgx_ecall_exception, 0},
		{(void*)(uintptr_t)sgx_ecall_map, 0},
		{(void*)(uintptr_t)sgx_ecall_increase_counter, 0},
		{(void*)(uintptr_t)sgx_ecall_producer, 0},
		{(void*)(uintptr_t)sgx_ecall_consumer, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[16][24];
} g_dyn_entry_table = {
	16,
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_get_char(char* key)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_key = sizeof(*key);

	ms_ocall_get_char_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_get_char_t);
	void *__tmp = NULL;

	ocalloc_size += (key != NULL && sgx_is_within_enclave(key, _len_key)) ? _len_key : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_get_char_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_get_char_t));

	if (key != NULL && sgx_is_within_enclave(key, _len_key)) {
		ms->ms_key = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_key);
		memset(ms->ms_key, 0, _len_key);
	} else if (key == NULL) {
		ms->ms_key = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(0, ms);

	if (key) memcpy((void*)key, ms->ms_key, _len_key);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_goto(int x, int y)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_goto_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_goto_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_goto_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_goto_t));

	ms->ms_x = x;
	ms->ms_y = y;
	status = sgx_ocall(1, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_set_usec_delay(unsigned int delay)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_set_usec_delay_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_set_usec_delay_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_set_usec_delay_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_set_usec_delay_t));

	ms->ms_delay = delay;
	status = sgx_ocall(2, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_print_game_over()
{
	sgx_status_t status = SGX_SUCCESS;
	status = sgx_ocall(3, NULL);

	return status;
}

sgx_status_t SGX_CDECL ocall_print_another_game()
{
	sgx_status_t status = SGX_SUCCESS;
	status = sgx_ocall(4, NULL);

	return status;
}

sgx_status_t SGX_CDECL ocall_blank_coordinates(int x, int y)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_blank_coordinates_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_blank_coordinates_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_blank_coordinates_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_blank_coordinates_t));

	ms->ms_x = x;
	ms->ms_y = y;
	status = sgx_ocall(5, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_display_snake(struct snake_t* snake)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_display_snake_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_display_snake_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_display_snake_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_display_snake_t));

	ms->ms_snake = SGX_CAST(struct snake_t*, snake);
	status = sgx_ocall(6, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_show_score(struct screen_t* screen)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_show_score_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_show_score_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_show_score_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_show_score_t));

	ms->ms_screen = SGX_CAST(struct screen_t*, screen);
	status = sgx_ocall(7, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_setup_playing_board(struct screen_t* screen)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_ocall_setup_playing_board_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_setup_playing_board_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_setup_playing_board_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_setup_playing_board_t));

	ms->ms_screen = SGX_CAST(struct screen_t*, screen);
	status = sgx_ocall(8, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_print_string(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_ocall_print_string_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_print_string_t);
	void *__tmp = NULL;

	ocalloc_size += (str != NULL && sgx_is_within_enclave(str, _len_str)) ? _len_str : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_print_string_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_print_string_t));

	if (str != NULL && sgx_is_within_enclave(str, _len_str)) {
		ms->ms_str = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_str);
		memcpy((void*)ms->ms_str, str, _len_str);
	} else if (str == NULL) {
		ms->ms_str = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(9, ms);


	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_rand(int* ret)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_ret = sizeof(*ret);

	ms_ocall_rand_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_rand_t);
	void *__tmp = NULL;

	ocalloc_size += (ret != NULL && sgx_is_within_enclave(ret, _len_ret)) ? _len_ret : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_rand_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_rand_t));

	if (ret != NULL && sgx_is_within_enclave(ret, _len_ret)) {
		ms->ms_ret = (int*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_ret);
		memset(ms->ms_ret, 0, _len_ret);
	} else if (ret == NULL) {
		ms->ms_ret = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(10, ms);

	if (ret) memcpy((void*)ret, ms->ms_ret, _len_ret);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_oc_cpuidex(int cpuinfo[4], int leaf, int subleaf)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_cpuinfo = 4 * sizeof(*cpuinfo);

	ms_sgx_oc_cpuidex_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_oc_cpuidex_t);
	void *__tmp = NULL;

	ocalloc_size += (cpuinfo != NULL && sgx_is_within_enclave(cpuinfo, _len_cpuinfo)) ? _len_cpuinfo : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_oc_cpuidex_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_oc_cpuidex_t));

	if (cpuinfo != NULL && sgx_is_within_enclave(cpuinfo, _len_cpuinfo)) {
		ms->ms_cpuinfo = (int*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_cpuinfo);
		memcpy(ms->ms_cpuinfo, cpuinfo, _len_cpuinfo);
	} else if (cpuinfo == NULL) {
		ms->ms_cpuinfo = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_leaf = leaf;
	ms->ms_subleaf = subleaf;
	status = sgx_ocall(11, ms);

	if (cpuinfo) memcpy((void*)cpuinfo, ms->ms_cpuinfo, _len_cpuinfo);

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_wait_untrusted_event_ocall(int* retval, const void* self)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_wait_untrusted_event_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_wait_untrusted_event_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_wait_untrusted_event_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_wait_untrusted_event_ocall_t));

	ms->ms_self = SGX_CAST(void*, self);
	status = sgx_ocall(12, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_set_untrusted_event_ocall(int* retval, const void* waiter)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_set_untrusted_event_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_set_untrusted_event_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_set_untrusted_event_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_set_untrusted_event_ocall_t));

	ms->ms_waiter = SGX_CAST(void*, waiter);
	status = sgx_ocall(13, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_setwait_untrusted_events_ocall(int* retval, const void* waiter, const void* self)
{
	sgx_status_t status = SGX_SUCCESS;

	ms_sgx_thread_setwait_untrusted_events_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_setwait_untrusted_events_ocall_t);
	void *__tmp = NULL;


	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_setwait_untrusted_events_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_setwait_untrusted_events_ocall_t));

	ms->ms_waiter = SGX_CAST(void*, waiter);
	ms->ms_self = SGX_CAST(void*, self);
	status = sgx_ocall(14, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL sgx_thread_set_multiple_untrusted_events_ocall(int* retval, const void** waiters, size_t total)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_waiters = total * sizeof(*waiters);

	ms_sgx_thread_set_multiple_untrusted_events_ocall_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_sgx_thread_set_multiple_untrusted_events_ocall_t);
	void *__tmp = NULL;

	ocalloc_size += (waiters != NULL && sgx_is_within_enclave(waiters, _len_waiters)) ? _len_waiters : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_sgx_thread_set_multiple_untrusted_events_ocall_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_sgx_thread_set_multiple_untrusted_events_ocall_t));

	if (waiters != NULL && sgx_is_within_enclave(waiters, _len_waiters)) {
		ms->ms_waiters = (void**)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_waiters);
		memcpy((void*)ms->ms_waiters, waiters, _len_waiters);
	} else if (waiters == NULL) {
		ms->ms_waiters = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	ms->ms_total = total;
	status = sgx_ocall(15, ms);

	if (retval) *retval = ms->ms_retval;

	sgx_ocfree();
	return status;
}

