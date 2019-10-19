#ifndef __YZCONF_H_
#define __YZCONF_H_

#include <pthread.h>
#include <stdint.h>

namespace yzconf 
{
pthread_t GetThreadId();
uint32_t GetFiberId();


}

#endif //__YZCONF_H_