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
// Object-level auto data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectExpression.hpp"
#include "../object_io.hpp"
#include "../VariableType.hpp"

#include <map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData_Auto> AutoTable;
typedef AutoTable::iterator AutoIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static AutoTable auto_table;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectData_Auto::add
//
void ObjectData_Auto::add
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   ObjectData_Auto &data = auto_table[name];

   if (data.name != name)
   {
      data.name      = name;
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.init      = NULL;
      data.externDef = externDef;
      data.externVis = externVis;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_INT);
   }
   else if (data.externDef && externDef)
   {
      data.externDef = false;
   }
}

//
// ObjectData_Auto::generate_symbols
//
void ObjectData_Auto::generate_symbols()
{
   ObjectExpression::Pointer obj;
   AutoIter itr, end = auto_table.end();

   for (itr = auto_table.begin(); itr != end; ++itr)
   {
      obj = ObjectExpression::create_value_int
      (itr->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(itr->second.name, obj);
   }
}

//
// ObjectData_Auto::iterate
//
void ObjectData_Auto::iterate(IterFunc iterFunc, std::ostream *out)
{
   AutoIter itr, end = auto_table.end();

   for (itr = auto_table.begin(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

//
// ObjectData_Auto::read_objects
//
void ObjectData_Auto::read_objects(std::istream *in)
{
   read_object(in, &auto_table);
}

//
// ObjectData_Auto::write_objects
//
void ObjectData_Auto::write_objects(std::ostream *out)
{
   write_object(out, &auto_table);
}

//
// override_object<ObjectData_Auto>
//
void override_object(ObjectData_Auto *, ObjectData_Auto const *)
{
}

//
// read_object<ObjectData_Auto>
//
void read_object(std::istream *in, ObjectData_Auto *out)
{
   read_object(in, &out->name);
   read_object(in, &out->number);
   read_object(in, &out->size);
   read_object(in, &out->init);
   read_object(in, &out->externDef);
   read_object(in, &out->externVis);
}

//
// write_object<ObjectData_Auto>
//
void write_object(std::ostream *out, ObjectData_Auto const *in)
{
   write_object(out, &in->name);
   write_object(out, &in->number);
   write_object(out, &in->size);
   write_object(out, &in->init);
   write_object(out, &in->externDef);
   write_object(out, &in->externVis);
}

// EOF

