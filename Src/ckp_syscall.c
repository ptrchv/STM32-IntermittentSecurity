#include "ckp_syscall.h"


void checkpoint(void)
{
	__asm ("SVC 0xf4");
}


void restore(void)
{
	__asm ("SVC 0xf5");
}

