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

extern "C"
{

//
// Numeric conversion functions.
//

// Converts a null-terminated string to a double.
__function double atof(char const *nptr);

// Converts a null-terminated string to an int.
__function int atoi(char const *nptr);
__function long atol(char const *nptr);
__function long long atoll(char const *nptr);

__function double strtod(char const *restrict nptr, char **restrict endptr);
__function float strtof(char const *restrict nptr, char **restrict endptr);
__function long double strtold(char const *restrict nptr, char **restrict endptr);

__function long strtol(char const *restrict nptr, char **restrict endptr);
__function long long strtoll(char const *restrict nptr, char **restrict endptr);
__function unsigned long strtoul(char const *restrict nptr, char **restrict endptr);
__function unsigned long long strtoull(char const *restrict nptr, char **restrict endptr);

//
// Pseudo-random sequence generation functions.
//

__function int rand();
__function void srand(unsigned int);

//
// Memory management functions.
//

__function void *calloc(size_t nmemb, size_t size);
__function void free(void *ptr);
__function void *malloc(size_t size);
__function void *realloc(void *ptr, size_t size);

//
// Communication with the environment.
//

__function void abort();
__function int atexit(__func_t() -> void func);
__function void exit(int status);
__function void _Exit(int status);
__function char *getenv(char const *name);
__function int system(char const *string);

//
// Searching and sorting utilities.
//

__function void *bsearch(void const *key, void const *base, size_t nmemb, size_t size,
                         __func_t(void const *, void const *) -> int compar);

__function void qsort(void *base, size_t nmemb, size_t size,
                      __func_t(void const *, void const *) -> int compar);

//
// Integer arithmetic functions.
//

__function int abs(int j);
__function long labs(long int j);
__function long long llabs(long long int j);

__function div_t div(int numer, int denom);
__function ldiv_t ldiv(long int numer, long int denom);
__function lldiv_t lldiv(long long int numer, long long int denom);

//
// Multibyte/wide character conversion functions.
//

__function int mblen(char const *s, size_t n);
__function int mbtowc(wchar_t *restrict pwc, char const *restrict s, size_t n);
__function int wctomb(char *s, wchar_t wc);

//
// Multibyte/wide string conversion functions.
//

__function size_t mbstowcs(wchar_t *restrict pwcs, char const *restrict s, size_t n);
__function size_t wcstombs(char *restrict s, wchar_t const *restrict pwcs, size_t n);

//
// Implementation extensions.
//

__function int _Getptr(void const __far *p);
__function void _Setptr(int v, void __far *p);

__function unsigned _Ursh(unsigned u, int v);

};

#endif//__HEADER__STDLIB_H__

