#undef _SYS_CDEFS_H_
#undef _SYS_TYPES_H_
#undef _SYS_PARAM_H_
#undef _SYS_MALLOC_H_

#define _XOPEN_SOURCE 700
#define __BSD_VISIBLE 1
#define _KERNEL
#define _WANT_UCRED

#include <sys/cdefs.h>
#include <sys/types.h>
// #include <sys/param.h>
// #include <sys/kernel.h>
// #include <sys/systm.h>
// #include <sys/lock.h>
// #include <sys/mutex.h>
// #include <sys/proc.h>
// #include <sys/malloc.h>
// #include <sys/uio.h>
// #include <sys/filedesc.h>
// #include <sys/file.h>

#include <ps4/kernel.h>

#include "kmain.h"

int myhook1(struct thread *td, Ps4KernelFunctionHookArgument *arg) 
{
	arg->returns->rax = 1;
	return PS4_KERNEL_FUNCTION_HOOK_CONTROL_CONTINUE;
}

int myhook2(struct thread *td, Ps4KernelFunctionHookArgument *arg) 
{
	arg->returns->rax = 0;
	return PS4_KERNEL_FUNCTION_HOOK_CONTROL_CONTINUE;
}

int path_self_mmap_check_function(struct thread *td, void *uap) 
{
	void *func1 = ps4KernelDlSym("sceSblACMgrIsAllowedToMmapSelf");
	void *func2 = ps4KernelDlSym("sceSblAuthMgrIsLoadable");
	ps4KernelFunctionPosthook(func1, myhook1);
	ps4KernelFunctionPosthook(func2, myhook2);
	return 0;
}

int unpath_self_mmap_check_function(struct thread *td, void *uap) 
{
	void *func1 = ps4KernelDlSym("sceSblACMgrIsAllowedToMmapSelf");
	void *func2 = ps4KernelDlSym("sceSblAuthMgrIsLoadable");
	ps4KernelFunctionUnhook(func1);
	ps4KernelFunctionUnhook(func2);
	return 0;
}




