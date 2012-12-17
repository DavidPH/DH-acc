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
// Object-level string data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectExpression.hpp"
#include "../object_io.hpp"
#include "../option.hpp"

#include <map>
#include <sstream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData::String> StringTable;
typedef StringTable::iterator StringIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<bool> option_string_fold_handle
('\0', "string-fold", "optimization", "Removes duplicate strings.", NULL,
 &option_string_fold);

static StringTable Table;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool option_string_fold = false;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::String::Add
//
std::string const &String::Add(std::string const &string)
{
   if(Table.count(string))
      return Table[string].names[0];

   String &data = Table[string];

   data.string = string + '\0';

   std::ostringstream oss;
   oss << ObjectExpression::get_filename() << "::0s::" << Table.size();
   data.names.push_back(oss.str());

   ObjectExpression::add_symbol(data.names[0], ObjectExpression::ET_INT);

   return data.names[0];
}

//
// ObjectData::String::Add
//
void String::Add(std::string const &name, std::string const &string)
{
   String &data = Table[string];

   if(data.string.empty())
      data.string = string + '\0';

   data.names.push_back(name);

   ObjectExpression::add_symbol(name, ObjectExpression::ET_INT);
}

//
// ObjectData::String::Find
//
String const *String::Find(std::string const &symbol)
{
   StringIter strItr, strEnd;
   std::vector<std::string>::iterator symItr, symEnd;

   for(strItr = Table.begin(), strEnd = Table.end(); strItr != strEnd; ++strItr)
   {
      for(symItr = strItr->second.names.begin(), symEnd = strItr->second.names.end();
          symItr != symEnd; ++symItr)
      {
         if(*symItr == symbol)
            return &strItr->second;
      }
   }

   return NULL;
}

//
// ObjectData::String::GenerateSymbols
//
void String::GenerateSymbols()
{
   ObjectExpression::Pointer expr;
   StringIter it;
   std::vector<std::string>::iterator name;
   bigsint i = 1;

   for(it = Table.begin(); it != Table.end(); ++it)
   {
      String &s = it->second;

      expr = ObjectExpression::create_value_int(i++, SourcePosition::none());

      for (name = s.names.begin(); name != s.names.end(); ++name)
         ObjectExpression::add_symbol(*name, expr);
   }
}

//
// ObjectData::String::Iterate
//
void String::Iterate(IterFunc iterFunc, std::ostream *out)
{
   static String nullstring; iterFunc(out, nullstring);

   for(StringIter itr = Table.begin(), end = Table.end(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

//
// ObjectData::String::ReadObjects
//
void String::ReadObjects(std::istream *in)
{
   read_object(in, &Table);
}

//
// ObjectData::String::WriteObjects
//
void String::WriteObjects(std::ostream *out)
{
   write_object(out, &Table);
}

}

//
// override_object<ObjectData::String>
//
void override_object(ObjectData::String *out, ObjectData::String const *in)
{
   // This function only gets called when the relevant key is the same. For
   // strings that inherently means override, which means adding more symbols
   // for the data.

   std::vector<std::string>::const_iterator it;
   for (it = in->names.begin(); it != in->names.end(); ++it)
      out->names.push_back(*it);
}

//
// read_object<ObjectData::String>
//
void read_object(std::istream *in, ObjectData::String *out)
{
   read_object(in, &out->names);
   read_object(in, &out->string);
}

//
// write_object<ObjectData::String>
//
void write_object(std::ostream *out, ObjectData::String const *in)
{
   write_object(out, &in->names);
   write_object(out, &in->string);
}

// EOF

