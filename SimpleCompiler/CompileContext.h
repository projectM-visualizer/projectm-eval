#pragma once

#include "CompilerTypes.h"

/**
 * @brief Creates an empty compile context.
 * @param global_memory An optional pointer to a memory buffer to use as global memory (gmegabuf). If NULL, uses the built-in buffer.
 * @return A pointer to the newly created context.
 */
prjm_eel_compiler_context_t* prjm_eel_create_compile_context(prjm_eel_mem_buffer global_memory);

/**
 * @brief Destroys a compile context.
 * Do not use the pointer afterwards.
 * @param cctx The context to destroy.
 */
void prjm_eel_destroy_compile_context(prjm_eel_compiler_context_t* cctx);

/**
 * @brief Resets all internal variable values to 0.
 * Externally registered variables are not changed.
 * @param cctx The compile context containing the variables.
 */
void prjm_eel_reset_context_vars(prjm_eel_compiler_context_t* cctx);
