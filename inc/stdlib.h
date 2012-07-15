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
// General utilities.
//
//-----------------------------------------------------------------------------

#ifndef __HEADER__STDLIB_H__
#define __HEADER__STDLIB_H__

#ifndef __LIBDS_NOLIB
# ifdef __LIBDS_PARTS
__library("stdlib");
# else
__library("libDS");
# endif
#endif


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// EXIT_FAILURE
//
#define EXIT_FAILURE 1

//
// EXIT_SUCCESS
//
#define EXIT_SUCCESS 0

//
// MB_CUR_MAX
//
#define MB_CUR_MAX (size_t)1

//
// NULL
//
#ifndef NULL
#define NULL nullptr
#endif

//
// RAND_MAX
//
#define RAND_MAX 32767


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// div_t
//
struct div_t
{
   int quot;
   int rem;
};

//
// ldiv_t
//
struct ldiv_t
{
   long int quot;
   long int rem;
};

//
// lldiv_t
//
struct lldiv_t
{
   long long int quot;
   long long int rem;
};

//
// size_t
//
#ifndef __HAVE_SIZE_T
#define __HAVE_SIZE_T 1
typedef unsigned int size_t;
#endif

//
// wchar_t
//
#ifndef __HAVE_WCHAR_T
#define __HAVE_WCHAR_T 1
typedef char wchar_t;
#endif


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Numeric conversion functions.
//

// Converts a null-terminated string to a real.
extern __function atof(char const *nptr) -> __real;

// Converts a null-terminated string to an int.
extern __function atoi(char const *nptr) -> int;
extern __function atol(char const *nptr) -> int;
extern __function atoll(char const *nptr) -> int;

extern __function strtod
(char const *restrict nptr, char **restrict endptr) -> double;
extern __function strtof
(char const *restrict nptr, char **restrict endptr) -> float;
extern __function strtold
(char const *restrict nptr, char **restrict endptr) -> long double;

extern __function strtol
(char const *restrict nptr, char **restrict endptr) -> long int;
extern __function strtoll
(char const *restrict nptr, char **restrict endptr) -> long long int;
extern __function strtoul
(char const *restrict nptr, char **restrict endptr) -> unsigned long int;
extern __function strtoull
(char const *restrict nptr, char **restrict endptr) -> unsigned long long int;

//
// Pseudo-random sequence generation functions.
//

extern __function rand() -> int;
extern __function srand(unsigned int) -> void;

//
// Memory management functions.
//

extern __function calloc(size_t nmemb, size_t size) -> void *;
extern __function free(void *ptr) -> void;
extern __function malloc(size_t size) -> void *;
extern __function realloc(void *ptr, size_t size) -> void *;

//
// Communication with the environment.
//

extern __function abort() -> void;
extern __function atexit(__func_t() -> void func) -> int;
extern __function exit(int status) -> void;
extern __function _Exit(int status) -> void;
extern __function getenv(char const *name) -> char *;
extern __function system(char const *string) -> int;

//
// Searching and sorting utilities.
//

extern __function bsearch
(void const *key, void const *base, size_t nmemb, size_t size,
 __func_t(void const *, void const *) -> int compar) -> void *;

extern __function qsort
(void *base, size_t nmemb, size_t size,
 __func_t(void const *, void const *) -> int compar) -> void;

//
// Integer arithmetic functions.
//

extern __function abs(int j) -> int;
extern __function labs(long int j) -> long int;
extern __function llabs(long long int j) -> long long int;

extern __function div(int numer, int denom) -> div_t;
extern __function ldiv(long int numer, long int denom) -> ldiv_t;
extern __function lldiv(long long int numer, long long int denom) -> lldiv_t;

//
// Multibyte/wide character conversion functions.
//

extern __function mblen(char const *s, size_t n) -> int;
extern __function mbtowc
(wchar_t *restrict pwc, char const *restrict s, size_t n) -> int;
extern __function wctomb(char *s, wchar_t wc) -> int;

//
// Multibyte/wide string conversion functions.
//

extern __function mbstowcs
(wchar_t *restrict pwcs, char const *restrict s, size_t n) -> size_t;
extern __function wcstombs
(char *restrict s, wchar_t const *restrict pwcs, size_t n) -> size_t;

#endif//__HEADER__STDLIB_H__
