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
// Static Functions                                                           |
//

//
// SetTempString
//
// Sets an index of strings_temp, automatically expanding as needed.
//
static void SetTempString(size_t i, std::string const &s)
{
   if(i >= strings_temp.size())
      strings_temp.resize(i+1);

   std::string &out = strings_temp[i];

   out.reserve(s.size());

   for(std::string::const_iterator itr = s.begin(), end = s.end(); itr != end; ++itr)
   {
      switch(*itr)
      {
      case '\0':
         out += "\\x00";
         break;

      case '\\':
         out += '\\';
      default:
         out += *itr;
      }
   }
}

//
// AddStringsTemp
//
static void AddStringsTemp(std::string const &s)
{
   SetTempString(strings_temp.size(), s);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// BinaryTokenZDACS::write_ACSE_array_ARAY
//
void BinaryTokenZDACS::write_ACSE_array_ARAY(std::ostream *out,
   ObjectData::Array const &a)
{
   if (a.externDef) return;

   BinaryTokenACS::write_ACS0_32(out, a.number);
   BinaryTokenACS::write_ACS0_32(out, a.size);
}

//
// BinaryTokenZDACS::write_ACSE_array_AIMP
//
void BinaryTokenZDACS::write_ACSE_array_AIMP(std::ostream *out,
   ObjectData::Array const &a)
{
   if (!a.externDef) return;

   BinaryTokenACS::write_ACS0_32(out, a.number);
   BinaryTokenACS::write_ACS0_32(out, a.size);
   *out << a.name << '\0';
}

//
// BinaryTokenZDACS::write_ACSE_array_AIMP_counter
//
void BinaryTokenZDACS::write_ACSE_array_AIMP_counter(std::ostream *,
   ObjectData::Array const &a)
{
   if (!a.externDef) return;

   ++counter_temp;
}

//
// BinaryTokenZDACS::write_ACSE_array_AINI
//
void BinaryTokenZDACS::write_ACSE_array_AINI(std::ostream *out, ObjectData::Array const &a)
{
   if (a.externDef) return;
   if(a.init.data.empty()) return;

   *out << 'A' << 'I' << 'N' << 'I';
   BinaryTokenACS::write_ACS0_32(out, (a.size + 1) * 4);
   BinaryTokenACS::write_ACS0_32(out, a.number);

   ObjectExpression::Vector const &init = a.init.data;

   for (size_t i = 0, end = a.size; i != end; ++i)
   {
      if(i < init.size() && init[i])
         BinaryTokenACS::write_ACS0_32(out, *init[i]);
      else
         BinaryTokenACS::write_ACS0_32(out, 0);
   }
}

//
// BinaryTokenZDACS::write_ACSE_array_ASTR
//
void BinaryTokenZDACS::write_ACSE_array_ASTR(std::ostream *out, ObjectData::Array const &a)
{
   if (a.externDef) return;

   // Mark as strings only if totally comprised of strings.
   if(a.init.typeAll != ObjectData::IT_STRING)
      return;

   BinaryTokenACS::write_ACS0_32(out, a.number);
}

//
// BinaryTokenZDACS::write_ACSE_array_ATAG
//
void BinaryTokenZDACS::write_ACSE_array_ATAG(std::ostream *out, ObjectData::Array const &a)
{
   if(!Option_UseChunkATAG) return;

   if(a.externDef) return;
   if(a.init.type.empty()) return;

   // If true, the array is covered by ASTR.
   if(a.init.typeAll == ObjectData::IT_STRING) return;

   std::vector<unsigned char> buf; buf.reserve(a.init.type.size());

   for(auto const &itr : a.init.type)
   {
      switch(itr)
      {
      default:
      case ObjectData::IT_INTEGER:  buf.push_back('\0'); break;
      case ObjectData::IT_STRING:   buf.push_back('\1'); break;
      case ObjectData::IT_FUNCTION: buf.push_back('\2'); break;
      }
   }

   // Strip trailing 0s.
   while(!buf.empty() && !buf.back())
      buf.pop_back();

   // If that was everything, then don't generate a chunk.
   if(buf.empty()) return;

   *out << 'A' << 'T' << 'A' << 'G';
   BinaryTokenACS::write_ACS0_32(out, buf.size() + 5);
   *out << '\0';
   BinaryTokenACS::write_ACS0_32(out, a.number);

   for(auto const &itr : buf)
      out->put(itr);
}

//
// BinaryTokenZDACS::write_ACSE_array_MEXP
//
void BinaryTokenZDACS::write_ACSE_array_MEXP(std::ostream *, ObjectData::Array const &a)
{
   if(a.externDef || a.linkage == LINKAGE_INTERN) return;

   SetTempString(a.number, a.name);
}

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
void BinaryTokenZDACS::write_ACSE_function_FUNC(std::ostream *out,
   ObjectData::Function const &f)
{
   static ObjectExpression::Pointer const addrExternal =
      ObjectExpression::CreateValueUNS(0, SourcePosition::none());

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
void BinaryTokenZDACS::write_ACSE_function_FNAM(std::ostream *,
   ObjectData::Function const &f)
{
   SetTempString(f.number, f.name);
}

//
// BinaryTokenZDACS::write_ACSE_label_JUMP
//
void BinaryTokenZDACS::write_ACSE_label_JUMP(std::ostream *out, ObjectData::Label const &l)
{
   ObjectExpression::Pointer addr;

   addr = ObjectExpression::get_symbol(l.label, SourcePosition::none());

   BinaryTokenACS::write_ACS0_32(out, *addr);
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
void BinaryTokenZDACS::write_ACSE_register_MIMP(std::ostream *out,
   ObjectData::Register const &r)
{
   if (!r.externDef) return;

   if (r.size > 1)
   {
      // Mangle the name for each byte.
      for (bigsint i = 0; i < r.size; ++i)
      {
         BinaryTokenACS::write_ACS0_32(out, r.number + i);
         *out << r.name << "::" << i << '\0';
      }
   }
   else
   {
      BinaryTokenACS::write_ACS0_32(out, r.number);
      *out << r.name << '\0';
   }
}

//
// BinaryTokenZDACS::write_ACSE_register_MEXP
//
void BinaryTokenZDACS::write_ACSE_register_MEXP(std::ostream *,
   ObjectData::Register const &r)
{
   if(r.externDef || r.linkage == LINKAGE_INTERN) return;

   if (r.size > 1)
   {
      std::ostringstream oss;

      // Mangle the name for each byte.
      for (bigsint i = 0; i < r.size; ++i)
      {
         oss.str(r.name);
         oss << "::" << i;
         SetTempString(r.number + i, oss.str());
      }
   }
   else
      SetTempString(r.number, r.name);
}

//
// BinaryTokenZDACS::write_ACSE_register_MINI
//
void BinaryTokenZDACS::write_ACSE_register_MINI(std::ostream *out,
   ObjectData::Register const &r)
{
   if (r.externDef) return;
   if(r.init.data.empty()) return;

   *out << 'M' << 'I' << 'N' << 'I';
   BinaryTokenACS::write_ACS0_32(out, r.size*4+4);
   BinaryTokenACS::write_ACS0_32(out, r.number);

   ObjectExpression::Vector const &init = r.init.data;

   for (size_t i = 0, end = r.size; i != end; ++i)
   {
      if(i < init.size() && init[i])
         BinaryTokenACS::write_ACS0_32(out, *init[i]);
      else
         BinaryTokenACS::write_ACS0_32(out, 0);
   }
}

//
// BinaryTokenZDACS::write_ACSE_register_MSTR
//
void BinaryTokenZDACS::write_ACSE_register_MSTR(std::ostream *out,
   ObjectData::Register const &r)
{
   if (r.externDef) return;
   if(!r.init.type.empty()) return;

   for(size_t i = r.init.type.size(); i--;)
      if(r.init.type[i] == ObjectData::IT_STRING)
         BinaryTokenACS::write_ACS0_32(out, r.number + i);
}

//
// BinaryTokenZDACS::write_ACSE_script
//
void BinaryTokenZDACS::write_ACSE_script(std::ostream *out, ObjectData::Script const &s)
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
void BinaryTokenZDACS::write_ACSE_script_SFLG(std::ostream *out,
   ObjectData::Script const &s)
{
   if (s.externDef) return;
   if (!s.flags) return;

   BinaryTokenACS::write_ACS0_16(out, s.number);
   BinaryTokenACS::write_ACS0_16(out, s.flags);
}

//
// BinaryTokenZDACS::write_ACSE_script_SNAM
//
void BinaryTokenZDACS::write_ACSE_script_SNAM(std::ostream *, ObjectData::Script const &s)
{
   if (s.externDef) return;
   // ACSE named scripts must be negative.
   if (s.number >= 0) return;

   SetTempString(-s.number - 1, s.string);
}

//
// BinaryTokenZDACS::write_ACSE_script_SVCT
//
void BinaryTokenZDACS::write_ACSE_script_SVCT(std::ostream *out,
   ObjectData::Script const &s)
{
   if(s.externDef) return;
   if(s.varCount <= 10) return;
   if(s.varCount <= 20 && Target == TARGET_ZDoom) return;

   BinaryTokenACS::write_ACS0_16(out, s.number);
   BinaryTokenACS::write_ACS0_16(out, s.varCount);
}

//
// BinaryTokenZDACS::write_ACSE_string_STRL
//
void BinaryTokenZDACS::write_ACSE_string_STRL(std::ostream *, ObjectData::String const &s)
{
   AddStringsTemp(s.string);
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

