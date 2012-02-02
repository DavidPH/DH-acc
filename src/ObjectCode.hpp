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
// Object Code handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ObjectCode_
#define HPP_ObjectCode_

#include <ostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourcePosition;
class SourceTokenC;

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
   ObjectCodeSet();

   ObjectCode ocode;
   ObjectCode ocode_imm;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

ObjectCode ocode_get_code(SourceTokenC const &token);
ObjectCode ocode_get_code(std::string const &data,
                          SourcePosition const &position);

// Returns true if the passed ocode pushes something to the stack without
// taking anything.
bool ocode_is_push_noarg(ObjectCode ocode);

char const *make_string(ObjectCode ocode);

void read_object(std::istream *in, ObjectCode *out);
void read_object(std::istream *in, ObjectCodeSet *out);

void write_object(std::ostream *out, ObjectCode const &in);
void write_object(std::ostream *out, ObjectCodeSet const &in);

#endif//HPP_ObjectCode_

