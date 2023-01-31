#include "MemoryBuffer.h"

#include <stdlib.h>
#include <string.h>

#define PRJM_EEL_MEM_BLOCKS 128
#define PRJM_EEL_MEM_ITEMSPERBLOCK 65536

static prjm_eel_mem_buffer static_global_memory;

void prjm_eel_memory_destroy_global()
{
    prjm_eel_memory_destroy_buffer(static_global_memory);
}

prjm_eel_mem_buffer prjm_eel_memory_global()
{
    if (!static_global_memory)
    {
        prjm_eel_memory_host_lock_mutex();

        static_global_memory = prjm_eel_memory_create_buffer();

        prjm_eel_memory_host_unlock_mutex();
    }

    return static_global_memory;
}

prjm_eel_mem_buffer prjm_eel_memory_create_buffer()
{
    return calloc(PRJM_EEL_MEM_BLOCKS, sizeof(float*));
}

void prjm_eel_memory_destroy_buffer(prjm_eel_mem_buffer buffer)
{
    prjm_eel_memory_free(buffer);

    free(buffer);
}

void prjm_eel_memory_free(prjm_eel_mem_buffer buffer)
{
    if (!buffer)
    {
        return;
    }

    prjm_eel_memory_host_lock_mutex();

    for (int block = 0; block < PRJM_EEL_MEM_BLOCKS; ++block)
    {
        if (buffer[block])
        {
            free(buffer[block]);
        }
    }

    memset(buffer, 0, PRJM_EEL_MEM_BLOCKS * sizeof(float*));

    prjm_eel_memory_host_unlock_mutex();
}

float* prjm_eel_memory_allocate(prjm_eel_mem_buffer buffer, int index)
{
    int block;
    if (!buffer)
    {
        return NULL;
    }

    if (index >= 0 && (block = index / PRJM_EEL_MEM_ITEMSPERBLOCK) < PRJM_EEL_MEM_BLOCKS)
    {
        float* cur_block = buffer[block];

        if (!cur_block)
        {
            prjm_eel_memory_host_lock_mutex();

            if (!(cur_block = buffer[block]))
            {
                cur_block = buffer[block] = calloc(sizeof(float), PRJM_EEL_MEM_ITEMSPERBLOCK);
            }
            if (!cur_block)
            {
                index = 0;
            }

            prjm_eel_memory_host_unlock_mutex();
        }

        return cur_block + (index & (PRJM_EEL_MEM_ITEMSPERBLOCK - 1));
    }

    return NULL;
}
