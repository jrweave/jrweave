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

#ifndef __SYS__CHAR_H__
#define __SYS__CHAR_H__

#include <cctype>
#include <cstring>
#include "sys/ints.h"

uint8_t to_ascii(char c);
char to_lchar(uint8_t c);
uint8_t *ascii_strcpy(uint8_t *ascii, const char *cstr);
uint8_t *ascii_strncpy(uint8_t *ascii, const char *cstr, const size_t n);
int ascii_strncmp(const uint8_t *ascii, const char *cstr, const size_t n);
bool is_alnum(uint32_t c);
bool is_alpha(uint32_t c);
bool is_cntrl(uint32_t c);
bool is_digit(uint32_t c);
bool is_graph(uint32_t c);
bool is_lower(uint32_t c);
bool is_print(uint32_t c);
bool is_punct(uint32_t c);
bool is_space(uint32_t c);
bool is_upper(uint32_t c);
bool is_xdigit(uint32_t c);
bool is_ascii(uint32_t c);
uint32_t to_lower(uint32_t c);
uint32_t to_upper(uint32_t c);

#include "char-inl.h"

#endif /* __SYS__CHAR_H__ */
