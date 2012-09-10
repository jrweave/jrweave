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

#ifndef __TIMING_H__
#define __TIMING_H__

#include <cstdio>
#include "timing_choices.h"

#ifndef TIMING_USE
#define TIMING_USE TIMING_TIME
#endif

#if TIMING_USE == TIMING_NONE
#define TIME_T(...)
#define TIME()
#define DIFFTIME(b,a)
#define PRINTTIME
#define TIMEUNITS
#define TIMEOUTPUT(t) "[no time]"
#define TIMESTREAM(s, t)
#elif TIMING_USE == TIMING_TIME
#include <ctime>
#define TIME_T(...) time_t __VA_ARGS__
#define TIME() time(NULL)
#define DIFFTIME(b,a) difftime(b,a)
#define PRINTTIME "%u"
#define TIMEUNITS "seconds"
#define TIMEOUTPUT(t) t << " " << TIMEUNITS
#define TIMESTREAM(s, t) (s << TIMEOUTPUT(t))
#elif TIMING_USE == TIMING_CLOCK
#include <ctime>
#define TIME_T(...) clock_t __VA_ARGS__
#define TIME() clock()
#define DIFFTIME(b,a) (b-a)
#define PRINTTIME "%u"
#define TIMEUNITS "seconds"
#define TIMEOUTPUT(t) t << " " << TIMEUNITS
#define TIMESTREAM(s, t) (s << TIMEOUTPUT(t))
#elif TIMING_USE == TIMING_RDTSC
#include "rdtsc.h"
#define TIME_T(...) unsigned long long __VA_ARGS__
#define TIME() rdtsc()
#define PRINTTIME "%llu"
#define DIFFTIME(b,a) (b-a)
#ifdef TIMING_CPU_FREQUENCY
#define TIMEUNITS "seconds"
#define TIMEOUTPUT(t) (t/TIMING_CPU_FEQUENCY) << " " << TIMEUNITS
#else
#define TIMEUNITS "cycles"
#define TIMEOUTPUT(t) t << " " << TIMEUNITS
#endif
#define TIMESTREAM(s, t) (s << TIMEOUTPUT(t))
#else
#error "Invalid value defined for TIMING_USE."
#endif

#endif /* __TIMING_H__ */
