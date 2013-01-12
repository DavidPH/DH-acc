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
// ACS0 writing.
//
//-----------------------------------------------------------------------------

#include "../BinaryTokenACS.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static bigsint script_count = 0;
static bigsint string_count = 0;
static bigsint string_offset = 0;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern int option_script_regargs;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenACS::write_ACS0_8
//
void BinaryTokenACS::write_ACS0_8(std::ostream *out, bigsint i)
{
   out->put(char((i >> 0) & 0xFF));
}

//
// BinaryTokenACS::write_ACS0_16
//
void BinaryTokenACS::write_ACS0_16(std::ostream *out, bigsint i)
{
   out->put(char((i >> 0) & 0xFF));
   out->put(char((i >> 8) & 0xFF));
}

//
// BinaryTokenACS::write_ACS0_32
//
void BinaryTokenACS::write_ACS0_32(std::ostream *out, ObjectExpression const &expr)
{
   write_ACS0_32(out, expr.resolveBinary(0));
}

//
// BinaryTokenACS::write_ACS0_32
//
void BinaryTokenACS::write_ACS0_32(std::ostream *out, bigsint i)
{
   out->put(char((i >>  0) & 0xFF));
   out->put(char((i >>  8) & 0xFF));
   out->put(char((i >> 16) & 0xFF));
   out->put(char((i >> 24) & 0xFF));
}

//
// BinaryTokenACS::write_ACS0_script
//
void BinaryTokenACS::write_ACS0_script(std::ostream *out, ObjectData::Script const &s)
{
   ObjectExpression::Pointer addr =
      ObjectExpression::get_symbol(s.label, SourcePosition::none());

   bigsint args =
      s.argCount <= option_script_regargs ? s.argCount : option_script_regargs;

   write_ACS0_32(out, (s.stype * 1000) + s.number);
   write_ACS0_32(out, *addr);
   write_ACS0_32(out, args);
}

//
// BinaryTokenACS::write_ACS0_script_count
//
void BinaryTokenACS::write_ACS0_script_count(std::ostream *out)
{
   write_ACS0_32(out, script_count);
}

//
// BinaryTokenACS::write_ACS0_script_counter
//
void BinaryTokenACS::write_ACS0_script_counter(std::ostream *, ObjectData::Script const &)
{
   ++script_count;
}

//
// BinaryTokenACS::write_ACS0_string
//
void BinaryTokenACS::write_ACS0_string(std::ostream *out, ObjectData::String const &s)
{
   *out << s.string;
}

//
// BinaryTokenACS::write_ACS0_string_count
//
void BinaryTokenACS::write_ACS0_string_count(std::ostream *out)
{
   write_ACS0_32(out, string_count);
}

//
// BinaryTokenACS::write_ACS0_string_counter
//
void BinaryTokenACS::write_ACS0_string_counter(std::ostream *, ObjectData::String const &)
{
   ++string_count;
}

//
// BinaryTokenACS::write_ACS0_string_offset
//
void BinaryTokenACS::write_ACS0_string_offset(std::ostream *out,
   ObjectData::String const &s)
{
   bigsint base_offset =
      ObjectExpression::get_address_count() +
      4 + (script_count * 12) +
      4 + (string_count *  4);

   write_ACS0_32(out, base_offset + string_offset);

   string_offset += s.string.size();
}

// EOF

