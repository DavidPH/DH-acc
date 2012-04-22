//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Large number handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_bignum_
#define HPP_bignum_

#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourcePosition;

// A long double should service all requirements of bigreal.
typedef long double bigreal;

// Will want something bigger eventually.
typedef   signed long long int bigsint;
typedef unsigned long long int biguint;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

bigreal get_bigreal(std::string const &value, SourcePosition const &position);

bigsint get_bigsint(std::string const &value, SourcePosition const &position);



#endif//HPP_bignum_

