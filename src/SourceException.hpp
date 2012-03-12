//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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

#ifndef HPP_SourceException_
#define HPP_SourceException_

#include "attr.hpp"

#include <stdexcept>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// ERROR
//
#define ERROR(POS,...) \
SourceException::error \
(__FILE__, __LINE__, __func__, NULL, POS, __VA_ARGS__)
#define ERROR_N(POS,...) \
SourceException::error   \
(__FILE__, __LINE__, __func__, getClassName(), POS, __VA_ARGS__)
#define ERROR_NP(...)  \
SourceException::error \
(__FILE__, __LINE__, __func__, getClassName(), pos, __VA_ARGS__)
#define ERROR_Np(...)  \
SourceException::error \
(__FILE__, __LINE__, __func__, getClassName(), SourcePosition::none(), __VA_ARGS__)
#define ERROR_P(...)   \
SourceException::error \
(__FILE__, __LINE__, __func__, NULL, pos, __VA_ARGS__)
#define ERROR_p(...)   \
SourceException::error \
(__FILE__, __LINE__, __func__, NULL, SourcePosition::none(), __VA_ARGS__)

//
// WARN
//
#define WARN(POS,...) \
SourceException::warn(__FILE__, __LINE__, __func__, NULL, POS, __VA_ARGS__)
#define WARN_N(POS,...) \
SourceException::warn(__FILE__, __LINE__, __func__, getClassName(), POS, __VA_ARGS__)
#define WARN_NP(...) \
SourceException::warn(__FILE__, __LINE__, __func__, getClassName(), pos, __VA_ARGS__)
#define WARN_P(...) \
SourceException::warn(__FILE__, __LINE__, __func__, NULL, pos, __VA_ARGS__)


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourcePosition;

//
// SourceException
//
class SourceException : public std::exception
{
public:
   virtual ~SourceException() throw() {delete[] whatstr;}

   virtual char const *what() const throw() {return whatstr;}


   static void error
   (char const *file, int line, char const *func, char const *name,
    SourcePosition const &pos, char const *fmt, ...)
   ATTR_FORMAT(__printf__, 6, 7) ATTR_NORETURN;

   static void warn
   (char const *file, int line, char const *func, char const *name,
    SourcePosition const &pos, char const *fmt, ...)
   ATTR_FORMAT(__printf__, 6, 7);

private:
   SourceException(char *_whatstr) : whatstr(_whatstr) {}
   SourceException(SourceException const &e);

   char *whatstr;
};

#endif//HPP_SourcePosition_

