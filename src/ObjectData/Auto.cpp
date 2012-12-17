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

typedef std::map<std::string, ObjectData::Auto> AutoTable;
typedef AutoTable::iterator AutoIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static AutoTable Table;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::Auto::Add
//
void Auto::Add(std::string const &name, VariableType const *type,
   bool externDef, bool externVis, bigsint number)
{
   Auto &data = Table[name];

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
// ObjectData::Auto::GenerateSymbols
//
void Auto::GenerateSymbols()
{
   ObjectExpression::Pointer obj;
   AutoIter itr, end = Table.end();

   for (itr = Table.begin(); itr != end; ++itr)
   {
      obj = ObjectExpression::create_value_int
      (itr->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(itr->second.name, obj);
   }
}

//
// ObjectData::Auto::Iterate
//
void Auto::Iterate(IterFunc iterFunc, std::ostream *out)
{
   AutoIter itr, end = Table.end();

   for (itr = Table.begin(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

//
// ObjectData::Auto::ReadObjects
//
void Auto::ReadObjects(std::istream *in)
{
   read_object(in, &Table);
}

//
// ObjectData::Auto::WriteObjects
//
void Auto::WriteObjects(std::ostream *out)
{
   write_object(out, &Table);
}

}

//
// override_object<ObjectData::Auto>
//
void override_object(ObjectData::Auto *, ObjectData::Auto const *)
{
}

//
// read_object<ObjectData::Auto>
//
void read_object(std::istream *in, ObjectData::Auto *out)
{
   read_object(in, &out->name);
   read_object(in, &out->number);
   read_object(in, &out->size);
   read_object(in, &out->init);
   read_object(in, &out->externDef);
   read_object(in, &out->externVis);
}

//
// write_object<ObjectData::Auto>
//
void write_object(std::ostream *out, ObjectData::Auto const *in)
{
   write_object(out, &in->name);
   write_object(out, &in->number);
   write_object(out, &in->size);
   write_object(out, &in->init);
   write_object(out, &in->externDef);
   write_object(out, &in->externVis);
}

// EOF

