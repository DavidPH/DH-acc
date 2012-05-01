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

typedef std::map<std::string, ObjectData_String> StringTable;
typedef StringTable::iterator StringIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<bool> option_string_fold_handle
('\0', "string-fold", "optimization", "Removes duplicate strings.", NULL,
 &option_string_fold);

static StringTable string_table;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool option_string_fold = false;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectData_String::add
//
std::string const &ObjectData_String::add(std::string const &string)
{
   if (string_table.count(string))
      return string_table[string].names[0];

   ObjectData_String &data = string_table[string];

   data.string = string + '\0';

   std::ostringstream oss;
   oss << ObjectExpression::get_filename() << "::0s::" << string_table.size();
   data.names.push_back(oss.str());

   ObjectExpression::add_symbol(data.names[0], ObjectExpression::ET_INT);

   return data.names[0];
}

//
// ObjectData_String::generate_symbols
//
void ObjectData_String::generate_symbols()
{
   ObjectExpression::Pointer expr;
   StringIter it;
   std::vector<std::string>::iterator name;
   bigsint i = 0;

   for (it = string_table.begin(); it != string_table.end(); ++it)
   {
      ObjectData_String &s = it->second;

      expr = ObjectExpression::create_value_int(i++, SourcePosition::none());

      for (name = s.names.begin(); name != s.names.end(); ++name)
         ObjectExpression::add_symbol(*name, expr);
   }
}

//
// ObjectData_String::iterate
//
void ObjectData_String::iterate(IterFunc iterFunc, std::ostream *out)
{
   StringIter it;

   for (it = string_table.begin(); it != string_table.end(); ++it)
      iterFunc(out, it->second);
}

//
// ObjectData_String::read_objects
//
void ObjectData_String::read_objects(std::istream *in)
{
   read_object(in, &string_table);
}

//
// ObjectData_String::write_objects
//
void ObjectData_String::write_objects(std::ostream *out)
{
   write_object(out, &string_table);
}

//
// override_object<ObjectData_String>
//
void override_object(ObjectData_String *out, ObjectData_String const *in)
{
   // This function only gets called when the relevant key is the same. For
   // strings that inherently means override, which means adding more symbols
   // for the data.

   std::vector<std::string>::const_iterator it;
   for (it = in->names.begin(); it != in->names.end(); ++it)
      out->names.push_back(*it);
}

//
// read_object<ObjectData_String>
//
void read_object(std::istream *in, ObjectData_String *out)
{
   read_object(in, &out->names);
   read_object(in, &out->string);
}

//
// write_object<ObjectData_String>
//
void write_object(std::ostream *out, ObjectData_String const *in)
{
   write_object(out, &in->names);
   write_object(out, &in->string);
}

// EOF

