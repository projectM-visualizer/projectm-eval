#include "ns-eel2.h"

#include "projectm-eval/api/projectm-eval.h"
#include "projectm-eval/CompilerTypes.h"

void prjm_eel_memory_host_lock_mutex()
{
    NSEEL_HOSTSTUB_EnterMutex();
}

void prjm_eel_memory_host_unlock_mutex()
{
    NSEEL_HOSTSTUB_LeaveMutex();
}

void NSEEL_quit()
{
    prjm_eel_memory_global_destroy();
}

NSEEL_VMCTX NSEEL_VM_alloc()
{
    return (NSEEL_VMCTX)pjrm_eel_context_create(NULL);
}

void NSEEL_VM_free(NSEEL_VMCTX ctx)
{
    pjrm_eel_context_destroy((struct prjm_eel_context*)ctx);
}

void NSEEL_VM_resetvars(NSEEL_VMCTX ctx)
{
    pjrm_eel_context_reset_variables((struct prjm_eel_context*)ctx);
}

EEL_F *NSEEL_VM_regvar(NSEEL_VMCTX ctx, const char *name)
{
    return pjrm_eel_context_register_variable((struct prjm_eel_context*)ctx, name);
}

void NSEEL_VM_freeRAM(NSEEL_VMCTX ctx)
{
    pjrm_eel_context_free_memory((struct prjm_eel_context*)ctx);
}

void NSEEL_VM_SetGRAM(NSEEL_VMCTX ctx, void **gram)
{
    struct prjm_eel_context* prjm_ctx = (struct prjm_eel_context*)ctx;
    prjm_ctx->global_memory = (prjm_eel_mem_buffer)gram;
}

void NSEEL_VM_FreeGRAM(void **ufd)
{
    prjm_eel_memory_buffer_destroy((prjm_eel_mem_buffer)ufd);
}

NSEEL_CODEHANDLE NSEEL_code_compile(NSEEL_VMCTX ctx, char *code, int lineoffs)
{
    // ToDo: Forward passed pointer to skip "lineoffs" \n chars for compatibility
    return (NSEEL_CODEHANDLE)pjrm_eel_code_compile((struct prjm_eel_context*)ctx, code);
}

char *NSEEL_code_getcodeerror(NSEEL_VMCTX ctx)
{
    return ((struct prjm_eel_context*)ctx)->error.error;
}

void NSEEL_code_execute(NSEEL_CODEHANDLE code)
{
    pjrm_eel_code_execute((struct prjm_eel_code*)code);
}

void NSEEL_code_free(NSEEL_CODEHANDLE code)
{
    pjrm_eel_code_destroy((struct prjm_eel_code*)code);
}
