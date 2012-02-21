//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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

#include "bignum.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// get_bigreal
//
bigreal get_bigreal(std::string const &value, SourcePosition const &)
{
   bigreal f;
   std::istringstream iss(value);
   iss >> f;
   return f;
}

//
// get_bigsint
//
bigsint get_bigsint(std::string const &value, SourcePosition const &)
{
   bigsint i;
   std::istringstream iss(value);
   iss >> i;
   return i;
}

// EOF

