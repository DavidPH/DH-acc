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
#include "../option.hpp"
#include "../VariableType.hpp"

#include <map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData_Static> StaticTable;
typedef StaticTable::iterator StaticIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

extern int option_static_offset;
static option::option_dptr<int> option_static_offset_handler
('\0', "static-offset", "features",
 "Sets the allocation offset for static variables. 8192 by default.",
 NULL, &option_static_offset);

static StaticTable static_table;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int option_static_offset = 8192;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// set_number
//
void set_number(ObjectData_Static &data)
{
   StaticIter itr = static_table.begin(), end = static_table.end();
   bigsint index = option_static_offset, limit, iterI, iterL;

   while (itr != end)
   {
      if (itr->second.number < 0) {++itr; continue;}

      limit = index + data.size;

      iterI = itr->second.number;
      iterL = iterI + itr->second.size;
      if (!itr->second.size) ++iterL;

      // If either end of the iter's range is in range, try again.
      if ((iterI >= index && limit > iterI) ||
          (iterL >  index && limit > iterL))
      {
         index = iterL;
         itr = static_table.begin();
         continue;
      }

      ++itr;
   }

   data.number = index;
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectData_Static::add
//
void ObjectData_Static::add
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis)
{
   add(name, type, externDef, externVis, -1);
}

//
// ObjectData_Static::add
//
void ObjectData_Static::add
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   ObjectData_Static &data = static_table[name];

   if (data.name != name)
   {
      data.name      = name;
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.init      = NULL;
      data.externDef = externDef;
      data.externVis = externVis;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_UNS);
   }
   else if (data.externDef && externDef)
   {
      data.externDef = false;
   }
}

//
// ObjectData_Static::generate_symbols
//
void ObjectData_Static::generate_symbols()
{
   ObjectExpression::Pointer obj;
   StaticIter itr, end = static_table.end();

   for (itr = static_table.begin(); itr != end; ++itr)
      if (itr->second.number == -1) set_number(itr->second);

   for (itr = static_table.begin(); itr != end; ++itr)
   {
      obj = ObjectExpression::create_value_uns
      (itr->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(itr->second.name, obj);
   }
}

//
// ObjectData_Static::iterate
//
void ObjectData_Static::iterate(IterFunc iterFunc, std::ostream *out)
{
   StaticIter itr, end = static_table.end();

   for (itr = static_table.begin(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

//
// ObjectData_Static::read_objects
//
void ObjectData_Static::read_objects(std::istream *in)
{
   read_object(in, &static_table);
}

//
// ObjectData_Static::write_objects
//
void ObjectData_Static::write_objects(std::ostream *out)
{
   write_object(out, &static_table);
}

//
// override_object<ObjectData_Static>
//
void override_object(ObjectData_Static *out, ObjectData_Static const *in)
{
   if (out->number == -1)
      *out = *in;
}

//
// read_object<ObjectData_Static>
//
void read_object(std::istream *in, ObjectData_Static *out)
{
   read_object(in, &out->name);
   read_object(in, &out->number);
   read_object(in, &out->size);
   read_object(in, &out->init);
   read_object(in, &out->externDef);
   read_object(in, &out->externVis);
}

//
// write_object<ObjectData_Static>
//
void write_object(std::ostream *out, ObjectData_Static const *in)
{
   write_object(out, &in->name);
   write_object(out, &in->number);
   write_object(out, &in->size);
   write_object(out, &in->init);
   write_object(out, &in->externDef);
   write_object(out, &in->externVis);
}

// EOF

