/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* bignum.hpp
**
** Declares types and functions for big numbers.
*/

#ifndef HPP_bignum_
#define HPP_bignum_

class SourceTokenC;



// A long double should service all requirements of bigreal.
typedef long double bigreal;

// 64 bits services all known requirements, so for now a typedef here will be
// sufficient.
typedef   signed long long int bigsint;
typedef unsigned long long int biguint;



bigreal get_bigreal(SourceTokenC const & token);
bigsint get_bigsint(SourceTokenC const & token);



#endif /* HPP_bignum_ */



