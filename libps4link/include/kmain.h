#ifndef KMainH
#define KMainH

#include <sys/sysent.h>

int path_self_mmap_check_function(struct thread *td, void *uap);
int unpath_self_mmap_check_function(struct thread *td, void *uap);

#endif
