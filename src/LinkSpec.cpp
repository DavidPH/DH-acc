//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// Linkage specifier handling.
//
//-----------------------------------------------------------------------------

#include "LinkSpec.hpp"

#include "object_io.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
//
//
void read_object(std::istream *in, LinkageSpecifier *out)
{
   *out = static_cast<LinkageSpecifier>(read_object_int(in));

   if(*out > LINKAGE_DS)
      *out = LINKAGE_DS;
}

void write_object(std::ostream *out, LinkageSpecifier const *in)
{
   write_object_int(out, static_cast<bigsint>(*in));
}

//EOF

