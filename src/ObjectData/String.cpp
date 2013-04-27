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
// Object-level string data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectArchive.hpp"
#include "../ObjectExpression.hpp"
#include "../option.hpp"

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

   data.string = string;

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
      data.string = string;

   data.names.push_back(name);

   ObjectExpression::add_symbol(name, ObjectExpression::ET_UNS);
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

      expr = ObjectExpression::CreateValueUNS(i++, SourcePosition::none());

      for (name = s.names.begin(); name != s.names.end(); ++name)
         ObjectExpression::add_symbol(*name, expr);
   }
}

//
// ObjectData::String::Iterate
//
void String::Iterate(IterFunc iterFunc, std::ostream *out)
{
   static String nullstring = {std::vector<std::string>(), std::string("\0", 1)};
   iterFunc(out, nullstring);

   for(StringIter itr = Table.begin(), end = Table.end(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

//
// ObjectData::String::Load
//
ObjectLoad &String::Load(ObjectLoad &arc)
{
   return arc >> Table;
}

//
// ObjectData::String::Save
//
ObjectSave &String::Save(ObjectSave &arc)
{
   return arc << Table;
}

}

//
// OA_Override<ObjectData::String>
//
void OA_Override(ObjectData::String &out, ObjectData::String const &in)
{
   // This function only gets called when the relevant key is the same. For
   // strings that inherently means override, which means adding more symbols
   // for the data.

   for(auto const &itr : in.names)
      out.names.push_back(itr);
}

//
// operator ObjectSave << ObjectData::String
//
ObjectSave &operator << (ObjectSave &arc, ObjectData::String const &data)
{
   return arc << data.names << data.string;
}

//
// operator ObjectLoad >> ObjectData::String
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::String &data)
{
   return arc >> data.names >> data.string;
}

// EOF

