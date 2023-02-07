/**
 * @file Memory handling functions for megabuf/gmegabuf.
 * Provides functions to allocate, free and access memory buffers, both global and context-local ones.
 * See docs/Memory-Handling.md for details about thread safety, differences to ns-eel2 and other considerations.
 */
#pragma once

#include "CompilerTypes.h"

/**
 * @brief Destroys the global memory buffer and its contents.
 * Only to be used after all context objects are destroyed. Will cause segfaults otherwise.
 */
void prjm_eel_memory_destroy_global();

/**
 * @brief Returns the global memory buffer.
 * Can be used if only one global buffer is required. Call @a prjm_eel_memory_destroy_global()
 * to free this buffer.
 */
projectm_eval_mem_buffer prjm_eel_memory_global();

/**
 * @brief Creates a memory buffer which can hold the required amount of blocks.
 * @return A pointer to the empty buffer.
 */
projectm_eval_mem_buffer prjm_eel_memory_create_buffer();

/**
 * @brief Destroys a memory buffer and any blocks stored within.
 * @param buffer A pointer to the buffer which should be destroyed.
 */
void prjm_eel_memory_destroy_buffer(projectm_eval_mem_buffer buffer);

/**
 * @brief Frees the data stored in the buffer.
 * The buffer itself will not be destroyed. Call @a prjm_eel_memory_destroy_buffer() if this is needed.
 * @param buffer The buffer to clear.
 */
void prjm_eel_memory_free(projectm_eval_mem_buffer buffer);

/**
 * @brief Frees the data stored in a single block of the buffer.
 * The buffer itself will not be destroyed. Call @a prjm_eel_memory_destroy_buffer() if this is needed.
 * @param buffer The buffer to clear.
 * @param block The block to clear.
 */
void prjm_eel_memory_free_block(projectm_eval_mem_buffer buffer, int block);

/**
 * @brief Allocates the appropriate memory block and returns a pointer to the data address for the given index.
 * @param buffer A pointer to the buffer to use.
 * @param index The memory index (offset) to allocate and return a pointer to.
 */
PRJM_EVAL_F* prjm_eel_memory_allocate(projectm_eval_mem_buffer buffer, int index);
