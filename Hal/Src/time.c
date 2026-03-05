#include "time.h"
#include "cmsis_os.h"

uint32_t halGetTicks()
{
	return osKernelGetTickCount();
}
