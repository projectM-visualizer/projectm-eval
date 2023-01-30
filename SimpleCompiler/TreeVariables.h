#pragma once

#include "CompilerTypes.h"

prjm_eel_variable_def_t* prjm_eel_register_variable(prjm_eel_compiler_context_t* cctx,
                                                    const char* name,
                                                    float* value_ptr);

void prjm_eel_unregister_variable(prjm_eel_compiler_context_t* cctx,
                                  const char* name);
