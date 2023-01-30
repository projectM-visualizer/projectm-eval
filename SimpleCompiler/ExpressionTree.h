#pragma once

#include "CompilerTypes.h"

/**
 * @brief Recursively frees the memory of the given node.
 * @param expr The node to free.
 */
void prjm_eel_destroy_exptreenode(prjm_eel_exptreenode_t* expr);
