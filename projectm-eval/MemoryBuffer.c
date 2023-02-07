#include "MemoryBuffer.h"

#include <stdlib.h>
#include <string.h>

#define PRJM_EEL_MEM_BLOCKS 128
#define PRJM_EEL_MEM_ITEMSPERBLOCK 65536

static projectm_eval_mem_buffer static_global_memory;

void prjm_eel_memory_destroy_global()
{
    prjm_eel_memory_destroy_buffer(static_global_memory);
}

projectm_eval_mem_buffer prjm_eel_memory_global()
{
    if (!static_global_memory)
    {
        projectm_eval_memory_host_lock_mutex();

        static_global_memory = prjm_eel_memory_create_buffer();

        projectm_eval_memory_host_unlock_mutex();
    }

    return static_global_memory;
}

projectm_eval_mem_buffer prjm_eel_memory_create_buffer()
{
    return calloc(PRJM_EEL_MEM_BLOCKS, sizeof(PRJM_EVAL_F*));
}

void prjm_eel_memory_destroy_buffer(projectm_eval_mem_buffer buffer)
{
    prjm_eel_memory_free(buffer);

    free(buffer);
}

void prjm_eel_memory_free(projectm_eval_mem_buffer buffer)
{
    if (!buffer)
    {
        return;
    }

    projectm_eval_memory_host_lock_mutex();

    for (int block = 0; block < PRJM_EEL_MEM_BLOCKS; ++block)
    {
        if (buffer[block])
        {
            free(buffer[block]);
        }
    }

    memset(buffer, 0, PRJM_EEL_MEM_BLOCKS * sizeof(PRJM_EVAL_F*));

    projectm_eval_memory_host_unlock_mutex();
}

void prjm_eel_memory_free_block(projectm_eval_mem_buffer buffer, int block)
{
    if (block < 0)
    {
        block = 0;
    }
    if (block < PRJM_EEL_MEM_BLOCKS * PRJM_EEL_MEM_ITEMSPERBLOCK)
    {
        // Set int following the block pointer to the starting index of the first index/block to be freed.
    }
}

PRJM_EVAL_F* prjm_eel_memory_allocate(projectm_eval_mem_buffer buffer, int index)
{
    int block;
    if (!buffer)
    {
        return NULL;
    }

    if (index >= 0 && (block = index / PRJM_EEL_MEM_ITEMSPERBLOCK) < PRJM_EEL_MEM_BLOCKS)
    {
        PRJM_EVAL_F* cur_block = buffer[block];

        if (!cur_block)
        {
            projectm_eval_memory_host_lock_mutex();

            if (!(cur_block = buffer[block]))
            {
                cur_block = buffer[block] = calloc(sizeof(PRJM_EVAL_F), PRJM_EEL_MEM_ITEMSPERBLOCK);
            }
            if (!cur_block)
            {
                index = 0;
            }

            projectm_eval_memory_host_unlock_mutex();
        }

        return cur_block + (index & (PRJM_EEL_MEM_ITEMSPERBLOCK - 1));
    }

    return NULL;
}
