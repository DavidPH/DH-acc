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
// Source-level error exception.
//
//-----------------------------------------------------------------------------

#include "SourceException.hpp"

#include "SourcePosition.hpp"

#include <cstdarg>
#include <cstdio>


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

void SourceException::error(char const *file, int line, char const *func,
   char const *name, SourcePosition const &pos, char const *fmt, ...)
{
   char *whatstr;
   size_t whatlen, whatprt;
   va_list whatarg;

   if (name)
      whatlen = std::snprintf(NULL, 0, "%s:%li:%li (%s:%i %s::%s): ",
         pos.filename.c_str(), pos.line, pos.column, file, line, name, func);
   else
      whatlen = std::snprintf(NULL, 0, "%s:%li:%li (%s:%i %s): ",
         pos.filename.c_str(), pos.line, pos.column, file, line, func);

   va_start(whatarg, fmt);

   whatlen += std::vsnprintf(NULL, 0, fmt, whatarg);

   va_end(whatarg);

   whatstr = new char[++whatlen];

   if (name)
      whatprt = std::snprintf(whatstr, whatlen, "%s:%li:%li (%s:%i %s::%s): ",
         pos.filename.c_str(), pos.line, pos.column, file, line, name, func);
   else
      whatprt = std::snprintf(whatstr, whatlen, "%s:%li:%li (%s:%i %s): ",
         pos.filename.c_str(), pos.line, pos.column, file, line, func);

   va_start(whatarg, fmt);

   whatprt += std::vsnprintf(whatstr+whatprt, whatlen-whatprt, fmt, whatarg);

   va_end(whatarg);

   throw SourceException(whatstr);
}

// EOF

