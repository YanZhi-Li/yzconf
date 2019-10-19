
#include "util.h"
#include <stdio.h>
#include <stdint.h>

namespace yzconf
{

pthread_t GetThreadId(){
    return  pthread_self();
} 
uint32_t GetFiberId(){
    return 0;
}  

}