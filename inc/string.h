//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// String handling.
//
//-----------------------------------------------------------------------------

#ifndef __HEADER__STRING_H__
#define __HEADER__STRING_H__

#ifndef __LIBDS_NOLIB
# ifdef __LIBDS_PARTS
__library("string");
# else
__library("libDS");
# endif
#endif


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// NULL
//
#ifndef NULL
#define NULL nullptr
#endif


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// size_t
//
#ifndef __HAVE_SIZE_T
#define __HAVE_SIZE_T 1
typedef unsigned size_t;
#endif


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Copying functions.
//

// Copies a chunk of memory.
extern __function memcpy
(void * restrict s1, void const *restrict s2, size_t n) -> void *;

// Copies a chunk of memory.
extern __function memmove(void *s1, void const *s2, size_t n) -> void *;

// Swaps two chunks of memory.
extern __function __memswp
(void *restrict s1, void *restrict s2, size_t n) -> void *;

// Copies a null-terminated string.
extern __function strcpy(char *restrict s1, char const *restrict s2) -> char *;

// Copies a null-terminated string.
extern __function strncpy
(char *restrict s1, char const *restrict s2, size_t n) -> char *;

//
// Concatenation functions.
//

// Concatenates two null-terminated strings.
extern __function strcat(char *restrict s1, char const *restrict s2) -> char *;

// Concatenates two null-terminated strings.
extern __function char *strncat(char *restrict s1, char const *restrict s2, size_t n);

//
// Comparison functions.
//

// Compares two chunks of memory.
extern __function memcmp(void const *s1, void const *s2, size_t n) -> int;

// Compares two null-terminated strings.
extern __function strcmp(char const *s1, char const *s2) -> int;

// Compares two null-terminated strings.
extern __function strcoll(char const *s1, char const *s2) -> int;

// Compares two null-terminated strings.
extern __function strncmp(char const *s1, char const *s2, size_t n) -> int;

//
extern __function strxfrm
(char *restrict s1, char const *restrict s2, size_t n) -> size_t;

//
// Search functions
//

// Finds a value in a chunk of memory.
extern __function memchr(void const *s, int c, size_t n) -> void *;

// Finds a value in a null-terminated string.
extern __function strchr(char const *s, int c) -> char *;

//
extern __function strcspn(char const *s1, char const *s2) -> size_t;

//
extern __function strpbrk(char const *s1, char const *s2) -> char *;

// Finds a value in a null-terminated string.
extern __function strrchr(char const *s, int c) -> char *;

//
extern __function strspn(char const *s1, char const *s2) -> size_t;

// Finds a value in a null-terminated string.
extern __function strstr(char const *s1, char const *s2) -> char *;

// Tokenizes a null-terminated string.
extern __function strtok(char *restrict s1, char const *restrict s2) -> char *;

//
// Miscellaneous functions.
//

// Sets a chunk of memory.
extern __function memset(void *s, int c, size_t n) -> void *;

//
extern __function strerror(int errnum) -> char *;

// Finds the length of a null-terminated string.
extern __function strlen(char const *s) -> size_t;

#endif//__HEADER__STRING_H__
