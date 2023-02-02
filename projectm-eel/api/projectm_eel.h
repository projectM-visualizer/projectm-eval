/**
 * @file projectm_eel.h
 * @brief projectM Expression Evaluation Library Public API Header
 *
 * Defines all macros, stubs and methods to interface with the projectM Expression Evaluation Library.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Default floating-point number size in bytes (4 = float, 8 = double) */
#ifndef PRJM_F_SIZE
#define PRJM_F_SIZE 8
#endif

/* On 32-bit platforms, it may be more performant to use floats. */
#if PRJM_F_SIZE == 4
typedef float PRJM_EEL_F;
#else
typedef double PRJM_EEL_F;
#endif

/**
 * @brief Opaque context type which holds a variable/memory execution context.
 * The context stores all registered variables (internal and external) and the pointers
 * to the global and context-specific memory blocks used via megabuf and gmegabuf.
 */
typedef struct prjm_eel_context_t;

/**
 * @brief Opaque type for compiled programs.
 * This code handle refers to a single compiled program. A program is always tied to a prjm_eel_context_t
 * and will become invalid once the context has been destroyed. A context can contain any number of code
 * handles, and code handles can be destroyed and recompiled at any time.
 */
typedef struct prjm_eel_code_t;

/**
 * @brief Buffer pointer for megabuf/gmegabuf memory.
 */
typedef PRJM_EEL_F** prjm_eel_mem_buffer_t;


/**
 * @brief Host-defined lock function.
 * Used to prevent race conditions with memory access. Only required if multiple expressions using the same
 * global/local memory blocks will run in separate threads at the same time. Can be an empty function otherwise.
 * The function is not required to use a recursive mutex.
 */
void prjm_eel_memory_host_lock_mutex();

/**
 * @brief Host-defined unlock function.
 * Used to prevent race conditions with memory access. Only required if multiple expressions using the same
 * global/local memory blocks will run in separate threads at the same time. Can be an empty function otherwise.
 */
void prjm_eel_memory_host_unlock_mutex();

/**
 * @brief Creates a new execution context.
 * @param global_mem A pointer to a prjm_eel_mem_buffer_t which will be used as the global memory buffer (gmegabuf).
 * @return A handle to the new execution context, or NULL if the context could not be created.
 */
prjm_eel_context_t* pjrm_eel_create_context(prjm_eel_mem_buffer_t global_mem);

void pjrm_eel_destroy_context(prjm_eel_context_t* ctx);

prjm_eel_code_t* pjrm_eel_compile_code(prjm_eel_context_t* ctx, const char* code);

void pjrm_eel_destroy_code(const char* code);

const char* prjm_eel_get_error(prjm_eel_context_t* ctx);

#ifdef __cplusplus
};
#endif
