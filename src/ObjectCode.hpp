//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// Object Code handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ObjectCode_
#define HPP_ObjectCode_

#include <string>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectLoad;
class ObjectSave;
class SourcePosition;

//
// ObjectCode
//
enum ObjectCode
{
   #define DO_INIT(NAME) OCODE_##NAME,

   #include "OCODE.hpp"

   OCODE_NONE

   #undef DO_INIT
};

//
// ObjectCodeSet
//
struct ObjectCodeSet
{
   ObjectCodeSet() : ocode(OCODE_NONE), ocode_imm(OCODE_NONE) {}

   ObjectCode ocode;
   ObjectCode ocode_imm;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

ObjectCode ocode_get_code
(std::string const &data, SourcePosition const &position);

// Returns true if the passed ocode delays further execution.
bool ocode_is_delay(ObjectCode ocode);

// Returns true if the passed ocode pushes something to the stack without
// taking anything.
bool ocode_is_push_noarg(ObjectCode ocode);

char const *make_string(ObjectCode ocode);

ObjectSave &operator << (ObjectSave &arc, ObjectCode    const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectCodeSet const &data);

ObjectLoad &operator >> (ObjectLoad &arc, ObjectCode    &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectCodeSet &data);

#endif//HPP_ObjectCode_

