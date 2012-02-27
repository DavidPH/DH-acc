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
// ACSE writing.
//
//-----------------------------------------------------------------------------

#include "../BinaryTokenZDACS.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../ost_type.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

extern bool option_fake_ACS0;
static option::option_dptr<bool> option_fake_ACS0_handler
('\0', "fake-ACS0", "output",
 "For ACSE, generate a fake ACS0 header. On by default.", NULL,
 &option_fake_ACS0);

static bigsint counter_temp = 0;
static std::vector<std::string> strings_temp;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool option_fake_ACS0 = true;

extern int option_script_regargs;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenZDACS::write_ACSE_chunk
//
void BinaryTokenZDACS::
write_ACSE_chunk(std::ostream *out, std::ostringstream *chunkout,
                 char const *chunkname)
{
   std::string chunk(chunkout->str());

   if (chunk.size())
   {
      *out << chunkname;
      BinaryTokenACS::write_ACS0_32(out, chunk.size());
      *out << chunk;
   }

   chunkout->str("");
}

//
// BinaryTokenZDACS::write_ACSE_counter
//
void BinaryTokenZDACS::write_ACSE_counter(std::ostream *out)
{
   if (counter_temp)
      BinaryTokenACS::write_ACS0_32(out, counter_temp);

   counter_temp = 0;
}

//
// BinaryTokenZDACS::write_ACSE_function_FUNC
//
void BinaryTokenZDACS::
write_ACSE_function_FUNC(std::ostream *out, ObjectData_Function const &f)
{
   static ObjectExpression::Pointer const addrExternal =
      ObjectExpression::create_value_int(0, SourcePosition::none());

   ObjectExpression::Pointer addr;

   if (f.externDef)
      addr = addrExternal;
   else
      addr = ObjectExpression::get_symbol(f.label, SourcePosition::none());

   BinaryTokenACS::write_ACS0_8 (out, f.argCount);
   BinaryTokenACS::write_ACS0_8 (out, f.varCount);
   BinaryTokenACS::write_ACS0_8 (out, !!f.retCount);
   BinaryTokenACS::write_ACS0_8 (out, 0);
   BinaryTokenACS::write_ACS0_32(out, *addr);
}

//
// BinaryTokenZDACS::write_ACSE_function_FNAM
//
void BinaryTokenZDACS::
write_ACSE_function_FNAM(std::ostream *, ObjectData_Function const &f)
{
   size_t index = static_cast<size_t>(f.number);

   if (strings_temp.size() <= index)
      strings_temp.resize(index+1);

   strings_temp[index] = f.name;
}

//
// BinaryTokenZDACS::write_ACSE_library
//
void BinaryTokenZDACS::
write_ACSE_library(std::ostream *out, std::string const &lib)
{
   *out << lib << '\0';
}

//
// BinaryTokenZDACS::write_ACSE_register_MIMP
//
void BinaryTokenZDACS::
write_ACSE_register_MIMP(std::ostream *out, ObjectData_Register const &r)
{
   if (!r.externDef) return;

   // A bit of a hack, really.
   for (int i = 0; i < r.size; i++)
   {
      BinaryTokenACS::write_ACS0_32(out, r.number+i);
      *out << r.name << (char)(i+1) << '\0';
   }
}

//
// BinaryTokenZDACS::write_ACSE_register_MEXP
//
void BinaryTokenZDACS::
write_ACSE_register_MEXP(std::ostream *, ObjectData_Register const &r)
{
   if (!r.externVis) return;

   std::string s(r.name);
   s += '\0';

   if (strings_temp.size() < static_cast<size_t>(r.number + r.size))
      strings_temp.resize(r.number + r.size);

   // A bit of a hack, really.
   // More specifically, this is how multi-byte variable names get translated.
   for (int i = 0; i < r.size; i++)
   {
      s[s.size()-1] = (char)(i+1);
      strings_temp[r.number+i] = s;
   }
}

//
// BinaryTokenZDACS::write_ACSE_registerarray_ARAY
//
void BinaryTokenZDACS::
write_ACSE_registerarray_ARAY(std::ostream *out,
                              ObjectData_RegisterArray const &r)
{
   if (r.externDef) return;

   BinaryTokenACS::write_ACS0_32(out, r.number);
   BinaryTokenACS::write_ACS0_32(out, r.size);
}

//
// BinaryTokenZDACS::write_ACSE_registerarray_AIMP
//
void BinaryTokenZDACS::
write_ACSE_registerarray_AIMP(std::ostream *out,
                              ObjectData_RegisterArray const &r)
{
   if (!r.externDef) return;

   BinaryTokenACS::write_ACS0_32(out, r.number);
   BinaryTokenACS::write_ACS0_32(out, r.size);
   *out << r.name << '\0';
}

//
// BinaryTokenZDACS::write_ACSE_registerarray_AIMP_counter
//
void BinaryTokenZDACS::
write_ACSE_registerarray_AIMP_counter(std::ostream *,
                                      ObjectData_RegisterArray const &r)
{
   if (!r.externDef) return;

   ++counter_temp;
}

//
// BinaryTokenZDACS::write_ACSE_registerarray_MEXP
//
void BinaryTokenZDACS::
write_ACSE_registerarray_MEXP(std::ostream *, ObjectData_RegisterArray const &r)
{
   if (!r.externDef) return;

   if (strings_temp.size() < static_cast<size_t>(r.number + 1))
      strings_temp.resize(r.number + 1);

   strings_temp[r.number] = r.name;
}

//
// BinaryTokenZDACS::write_ACSE_script
//
void BinaryTokenZDACS::
write_ACSE_script(std::ostream *out, ObjectData_Script const &s)
{
   if (s.externDef) return;

   ObjectExpression::Pointer addr =
      ObjectExpression::get_symbol(s.label, SourcePosition::none());

   bigsint args =
      s.argCount <= option_script_regargs ? s.argCount : option_script_regargs;

   if (option_fake_ACS0)
   {
      BinaryTokenACS::write_ACS0_16(out, s.number);
      BinaryTokenACS::write_ACS0_8 (out, s.stype);
      BinaryTokenACS::write_ACS0_8 (out, args);
      BinaryTokenACS::write_ACS0_32(out, *addr);
   }
   else
   {
      BinaryTokenACS::write_ACS0_16(out, s.number);
      BinaryTokenACS::write_ACS0_16(out, s.stype);
      BinaryTokenACS::write_ACS0_32(out, *addr);
      BinaryTokenACS::write_ACS0_32(out, args);
   }
}

//
// BinaryTokenZDACS::write_ACSE_script_SFLG
//
void BinaryTokenZDACS::
write_ACSE_script_SFLG(std::ostream *out, ObjectData_Script const &s)
{
   if (s.externDef) return;
   if (!s.flags) return;

   BinaryTokenACS::write_ACS0_16(out, s.number);
   BinaryTokenACS::write_ACS0_16(out, s.flags);
}

//
// BinaryTokenZDACS::write_ACSE_script_SNAM
//
void BinaryTokenZDACS::
write_ACSE_script_SNAM(std::ostream *, ObjectData_Script const &s)
{
   if (s.externDef) return;
   // ACSE named scripts must be negative.
   if (s.number >= 0) return;

   size_t index = static_cast<size_t>(-s.number - 1);

   if (strings_temp.size() <= index)
      strings_temp.resize(index+1);

   strings_temp[index] = s.label;
}

//
// BinaryTokenZDACS::write_ACSE_script_SVCT
//
void BinaryTokenZDACS::
write_ACSE_script_SVCT(std::ostream *out, ObjectData_Script const &s)
{
   if (s.externDef) return;
   if (s.varCount <= 10) return;
   if (s.varCount <= 20 && target_type == TARGET_ZDoom) return;

   BinaryTokenACS::write_ACS0_16(out, s.number);
   BinaryTokenACS::write_ACS0_16(out, s.varCount);
}

//
// BinaryTokenZDACS::write_ACSE_string_STRL
//
void BinaryTokenZDACS::
write_ACSE_string_STRL(std::ostream *, ObjectData_String const &s)
{
   strings_temp.push_back(s.string);
}

//
// BinaryTokenZDACS::write_ACSE_stringtable
//
void BinaryTokenZDACS::write_ACSE_stringtable(std::ostream *out, bool junk)
{
   std::vector<std::string>::const_iterator it;
   bigsint stringCount = strings_temp.size();
   bigsint offset = (junk ? 12 : 4) + stringCount*4;

   // Header (if writing any strings)
   if (stringCount)
   {
      if (junk) BinaryTokenACS::write_ACS0_32(out, 0);
      BinaryTokenACS::write_ACS0_32(out, stringCount);
      if (junk) BinaryTokenACS::write_ACS0_32(out, 0);
   }

   // Offsets
   for (it = strings_temp.begin(); it != strings_temp.end(); ++it)
   {
      BinaryTokenACS::write_ACS0_32(out, offset);
      offset += it->size();

      // Ensure null termination.
      if (it->size() == 0 || (*it)[it->size()-1])
         ++offset;
   }

   // Strings
   for (it = strings_temp.begin(); it != strings_temp.end(); ++it)
   {
      *out << *it;

      // Ensure null termination.
      if (it->size() == 0 || (*it)[it->size()-1])
         *out << '\0';
   }

   strings_temp.clear();
}

// EOF

