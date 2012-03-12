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
// Attribute macros.
//
//-----------------------------------------------------------------------------

#ifndef HPP_attr_
#define HPP_attr_


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// ATTR_FORMAT
//
#if defined(__GNUC__)
#  define ATTR_FORMAT(A,B,C) __attribute__((__format__(A,B,C)))
#elif defined(__clang__)
#  if __has_attribute(__format__)
#     define ATTR_FORMAT(A,B,C) __attribute__((__format__(A,B,C)))
#  else
#     define ATTR_FORMAT(A,B,C)
#  endif
#else
#  define ATTR_FORMAT(A,B,C)
#endif

//
// ATTR_NORETURN
//
#if defined(__GNUC__)
#  define ATTR_NORETURN __attribute__((__noreturn__))
#elif defined(__clang__)
#  if __has_attribute(__noreturn__)
#     define ATTR_NORETURN __attribute__((__noreturn__))
#  else
#     define ATTR_NORETURN
#  endif
#else
#  define ATTR_NORETURN
#endif

#endif//HPP_attr_

