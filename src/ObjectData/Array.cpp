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
// Object-level registerarray data.
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

typedef std::map<std::string, ObjectData_Array> ArrayTable;
typedef ArrayTable::iterator ArrayIter;


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static void set_number(ArrayTable &table, ObjectData_Array &data);
static void set_strings(std::vector<int> &strings, VariableType const *type);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<int> option_addr_array_handler
('\0', "addr-array", "features",
 "Selects which global array to use for addressable variables. 0 by default.",
 NULL, &option_addr_array);

static ArrayTable map_table;
static ArrayTable world_table;
static ArrayTable global_table;

static std::set<bigsint> used;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int option_addr_array = 0;
extern bool option_named_scripts;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// add
//
static void add
(ArrayTable &table, std::string const &name, VariableType const *type,
 bool externDef, bool externVis, bigsint number)
{
   ObjectData_Array &data = table[name];

   if (data.name != name)
   {
      set_strings(data.strings, type);
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
// count_register
//
static void count_register(std::ostream *, ObjectData_Register const &r)
{
   for (bigsint i = r.size; i--;)
      used.insert(r.number+i);
}

//
// generate_symbols
//
static void generate_symbols(ArrayTable &table)
{
   ObjectExpression::Pointer obj;
   ArrayIter iter;

   for (iter = table.begin(); iter != table.end(); ++iter)
      if (iter->second.number == -1) set_number(table, iter->second);

   for (iter = table.begin(); iter != table.end(); ++iter)
   {
      obj = ObjectExpression::create_value_int
      (iter->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(iter->second.name, obj);
   }
}

//
// iterate
//
static void iterate
(ArrayTable &table, ObjectData_Array::IterFunc iterFunc, std::ostream *out)
{
   for (ArrayIter iter = table.begin(); iter != table.end(); ++iter)
      iterFunc(out, iter->second);
}

//
// set_number
//
static void set_number(ArrayTable &table, ObjectData_Array &data)
{
   ArrayIter iter;
   bigsint index = 0;

   for (iter = table.begin(); iter != table.end(); ++iter)
   {
      if (used.count(index) || index == iter->second.number)
         {++index; iter = table.begin(); continue;}
   }

   data.number = index;
}

//
// set_strings
//
static void set_strings(std::vector<int> &strings, VariableType const *type)
{
   switch (type->getBasicType())
   {
   case VariableType::BT_ARRAY:
   {
      VariableType::Reference types = type->getReturn();
      bigsint width = type->getWidth();
      while (width--)
         set_strings(strings, types);
   }
      break;

   case VariableType::BT_ASMFUNC:
   case VariableType::BT_BLOCK:
   case VariableType::BT_BOOLHARD:
   case VariableType::BT_BOOLSOFT:
   case VariableType::BT_CHAR:
   case VariableType::BT_ENUM:
   case VariableType::BT_FIXED:
   case VariableType::BT_FLOAT:
   case VariableType::BT_FUNCTION:
   case VariableType::BT_INT:
   case VariableType::BT_LABEL:
   case VariableType::BT_LINESPEC:
   case VariableType::BT_LONG:
   case VariableType::BT_NATIVE:
   case VariableType::BT_NULLPTR:
   case VariableType::BT_POINTER:
   case VariableType::BT_REAL:
   case VariableType::BT_SCHAR:
   case VariableType::BT_SHORT:
   case VariableType::BT_UCHAR:
   case VariableType::BT_UINT:
   case VariableType::BT_ULLONG:
   case VariableType::BT_ULONG:
   case VariableType::BT_USHORT:
      strings.push_back(false);
      break;

   case VariableType::BT_LFLOAT:
   case VariableType::BT_LLFLOAT:
   case VariableType::BT_LLONG:
      strings.push_back(false);
      strings.push_back(false);
      break;

   case VariableType::BT_SCRIPT:
      strings.push_back(option_named_scripts);
      break;

   case VariableType::BT_STRING:
      strings.push_back(true);
      break;

   case VariableType::BT_STRUCT:
   {
      VariableType::Vector const &types = type->getTypes();
      VariableType::Vector::const_iterator iter;
      for (iter = types.begin(); iter != types.end(); ++iter)
         set_strings(strings, *iter);
   }

   case VariableType::BT_UNION:
   {
      bigsint types = type->getSize(SourcePosition::none());
      while (types--)
         strings.push_back(false);
   }
      break;

   case VariableType::BT_VOID:
      break;
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectData_Array::add_map
//
void ObjectData_Array::add_map
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis)
{
   add(map_table, name, type, externDef, externVis, -1);
}

//
// ObjectData_Array::add_map
//
void ObjectData_Array::add_map
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   add(map_table, name, type, externDef, externVis, number);
}

//
// ObjectData_Array::add_world
//
void ObjectData_Array::add_world
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis)
{
   add(world_table, name, type, externDef, externVis, -1);
}

//
// ObjectData_Array::add_world
//
void ObjectData_Array::add_world
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   add(world_table, name, type, externDef, externVis, number);
}

//
// ObjectData_Array::add_global
//
void ObjectData_Array::add_global
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis)
{
   add(global_table, name, type, externDef, externVis, -1);
}

//
// ObjectData_Array::add_global
//
void ObjectData_Array::add_global
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   add(global_table, name, type, externDef, externVis, number);
}

//
// ObjectData_Array::generate_symbols
//
void ObjectData_Array::generate_symbols()
{
   ObjectData_Register::iterate_map(count_register, NULL);
   ::generate_symbols(map_table);

   used.clear();
   ::generate_symbols(world_table);

   used.clear(); used.insert(option_addr_array);
   ::generate_symbols(global_table);

   used.clear();
}

//
// ObjectData_Array::ini_map
//
bool ObjectData_Array::ini_map
(std::string const &name, ObjectExpression *ini)
{
   ArrayIter a = map_table.find(name);
   if (a == map_table.end()) return false;

   // Only allow if either all strings or all not strings.
   std::vector<int>::iterator iter, end = a->second.strings.end(), begin;
   for (iter = begin = a->second.strings.begin(); iter != end; ++iter)
      if (*iter != *begin)
         return false;

   a->second.init = ini;
   return true;
}

//
// ObjectData_Array::iterate_map
//
void ObjectData_Array::iterate_map(IterFunc iterFunc, std::ostream *out)
{
   iterate(map_table, iterFunc, out);
}

//
// ObjectData_Array::iterate_world
//
void ObjectData_Array::iterate_world(IterFunc iterFunc, std::ostream *out)
{
   iterate(world_table, iterFunc, out);
}

//
// ObjectData_Array::iterate_global
//
void ObjectData_Array::iterate_global(IterFunc iterFunc, std::ostream *out)
{
   iterate(global_table, iterFunc, out);
}

//
// ObjectData_Array::read_objects
//
void ObjectData_Array::read_objects(std::istream *in)
{
   read_object(in, &map_table);
   read_object(in, &world_table);
   read_object(in, &global_table);
}

//
// ObjectData_Array::write_objects
//
void ObjectData_Array::write_objects(std::ostream *out)
{
   write_object(out, &map_table);
   write_object(out, &world_table);
   write_object(out, &global_table);
}

//
// override_object<ObjectData_Array>
//
void override_object(ObjectData_Array *out, ObjectData_Array const *in)
{
   if (out->externDef && !in->externDef)
   {
      out->number    = in->number;
      out->externDef = false;
   }
}

//
// read_object<ObjectData_Array>
//
void read_object(std::istream *in, ObjectData_Array *out)
{
   read_object(in, &out->strings);
   read_object(in, &out->name);
   read_object(in, &out->number);
   read_object(in, &out->size);
   read_object(in, &out->init);
   read_object(in, &out->externDef);
   read_object(in, &out->externVis);
}

//
// write_object<ObjectData_Array>
//
void write_object(std::ostream *out, ObjectData_Array const *in)
{
   write_object(out, &in->strings);
   write_object(out, &in->name);
   write_object(out, &in->number);
   write_object(out, &in->size);
   write_object(out, &in->init);
   write_object(out, &in->externDef);
   write_object(out, &in->externVis);
}

// EOF

