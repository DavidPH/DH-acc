//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2013 David Hill
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
// Storage of source-code positions.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourcePosition_
#define HPP_SourcePosition_

#include <ostream>
#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectLoad;
class ObjectSave;

//
// SourcePosition
//
class SourcePosition
{
public:
   SourcePosition() : filename(), line(0), column(0) {}
   SourcePosition(std::string const &_filename, long _line, long _column)
    : filename(_filename), line(_line), column(_column) {}

   std::string filename;
   long line;
   long column;


   friend ObjectSave &operator << (ObjectSave &arc, SourcePosition const &data);
   friend std::ostream &operator << (std::ostream &out, SourcePosition const &in);

   friend ObjectLoad &operator >> (ObjectLoad &arc, SourcePosition &data);

   //
   // builtin
   //
   static SourcePosition const &builtin()
   {
      static SourcePosition const pos("__builtin__", 0, 0);
      return pos;
   }

   //
   // none
   //
   static SourcePosition const &none()
   {
      static SourcePosition const pos("(none)", 0, 0);
      return pos;
   }
};

#endif//HPP_SourcePosition_

