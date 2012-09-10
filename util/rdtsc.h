/* Copyright 2012 Jesse Weaver
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 *    implied. See the License for the specific language governing
 *    permissions and limitations under the License.
 */

#ifndef __RDTSC_H_DEFINED__
#define __RDTSC_H_DEFINED__


#if defined(__i386__)

static __inline__ unsigned long long rdtsc(void)
{
 unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}
#elif defined(__x86_64__)

// typedef unsigned long long int unsigned long long;

static __inline__ unsigned long long rdtsc(void)
{
 unsigned hi, lo;
 __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
 return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

#elif defined(__powerpc__)

// typedef unsigned long long int unsigned long long;

static __inline__ unsigned long long rdtsc(void)
{
 unsigned long long int result=0;
 unsigned long int upper, lower,tmp;
 __asm__ volatile(
               "0:                  \n"
               "\tmftbu   %0           \n"
               "\tmftb    %1           \n"
               "\tmftbu   %2           \n"
               "\tcmpw    %2,%0        \n"
               "\tbne     0b         \n"
               : "=r"(upper),"=r"(lower),"=r"(tmp)
               );
 result = upper;
 result = result<<32;
 result = result|lower;

 return(result);
}

#else

#error "No tick counter is available!"

#endif

#endif
