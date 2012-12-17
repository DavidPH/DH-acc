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

#include "option.hpp"
#include "SourcePosition.hpp"

#include <cstdarg>
#include <cstdio>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define POS pos.filename.c_str(), pos.line, pos.column
#define POSSTR "%s:%li:%li"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_data<bool> option_debug_error_pos
('\0', "debug-error-pos", "debugging",
 "If enabled, will print internal source of errors and warnings.", NULL, false);

static option::option_data<bool> option_warnings('\0', "warnings", "warnings",
   "Enables warnings. Use --no-warnings to disable. On by default.", NULL, true);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceException::error
//
void SourceException::error(char const *file, int line, char const *func,
   char const *name, SourcePosition const &pos, char const *fmt, ...)
{
   char *whatstr;
   size_t whatlen, whatprt;
   va_list whatarg;

   if(!option_debug_error_pos.data)
      whatlen = std::snprintf(NULL, 0, POSSTR ": ", POS);
   else if(name)
      whatlen = std::snprintf(NULL, 0, POSSTR " (%s:%i %s::%s): ", POS, file, line, name, func);
   else
      whatlen = std::snprintf(NULL, 0, POSSTR " (%s:%i %s): ", POS, file, line, func);

   va_start(whatarg, fmt);

   whatlen += std::vsnprintf(NULL, 0, fmt, whatarg);

   va_end(whatarg);

   whatstr = new char[++whatlen];

   if(!option_debug_error_pos.data)
      whatprt = std::snprintf(whatstr, whatlen, POSSTR ": ", POS);
   else if(name)
      whatprt = std::snprintf(whatstr, whatlen, POSSTR " (%s:%i %s::%s): ", POS, file, line, name, func);
   else
      whatprt = std::snprintf(whatstr, whatlen, POSSTR " (%s:%i %s): ", POS, file, line, func);

   va_start(whatarg, fmt);

   std::vsnprintf(whatstr+whatprt, whatlen-whatprt, fmt, whatarg);

   va_end(whatarg);

   throw SourceException(whatstr);
}

//
// SourceException::warn
//
void SourceException::warn(char const *file, int line, char const *func,
   char const *name, SourcePosition const &pos, char const *fmt, ...)
{
   if(!option_warnings.data) return;

   va_list whatarg;

   if(!option_debug_error_pos.data)
      std::fprintf(stderr, "warning: " POSSTR ": ", POS);
   else if(name)
      std::fprintf(stderr, "warning: " POSSTR " (%s:%i %s::%s): ", POS, file, line, name, func);
   else
      std::fprintf(stderr, "warning: " POSSTR " (%s:%i %s): ", POS, file, line, func);

   va_start(whatarg, fmt);

   std::vfprintf(stderr, fmt, whatarg);

   va_end(whatarg);

   std::fprintf(stderr, "\n");
}

// EOF

