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
// Object-level register data.
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

typedef std::map<std::string, ObjectData_Register> RegisterTable;
typedef RegisterTable::iterator RegisterIter;


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static void set_number(RegisterTable &table, ObjectData_Register &data);
static void set_strings(std::vector<int> &strings, VariableType const *type);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static RegisterTable map_table;
static RegisterTable world_table;
static RegisterTable global_table;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_named_scripts;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// add
//
static void add
(RegisterTable &table, std::string const &name, VariableType const *type,
 bool externDef, bool externVis, bigsint number)
{
   ObjectData_Register &data = table[name];

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
// generate_symbols
//
static void generate_symbols(RegisterTable &table)
{
   ObjectExpression::Pointer obj;
   RegisterIter iter;

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
(RegisterTable &table, ObjectData_Register::IterFunc iterFunc, std::ostream *out)
{
   for (RegisterIter iter = table.begin(); iter != table.end(); ++iter)
      iterFunc(out, iter->second);
}

//
// set_number
//
static void set_number(RegisterTable &table, ObjectData_Register &data)
{
   RegisterIter iter = table.begin();
   bigsint index = 0, limit, iterI, iterL;

   while (iter != table.end())
   {
      if (iter->second.number < 0) {++iter; continue;}

      limit = index + data.size;

      // If this is the world table and stack-pointer is in range, try again.
      if (&table == &world_table &&
          option_addr_stack >= index && limit > option_addr_stack)
      {
         index = option_addr_stack + 1;
         iter = table.begin();
         continue;
      }

      iterI = iter->second.number;
      iterL = iterI + iter->second.size;
      if (!iter->second.size) ++iterL;

      // If either end of the iter's range is in range, try again.
      if ((iterI >= index && limit > iterI) ||
          (iterL >  index && limit > iterL))
      {
         index = iterL;
         iter = table.begin();
         continue;
      }

      ++iter;
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
// ObjectData_Register::add_map
//
void ObjectData_Register::add_map
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis)
{
   add(map_table, name, type, externDef, externVis, -1);
}

//
// ObjectData_Register::add_map
//
void ObjectData_Register::add_map
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   add(map_table, name, type, externDef, externVis, number);
}

//
// ObjectData_Register::add_world
//
void ObjectData_Register::add_world
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis)
{
   add(world_table, name, type, externDef, externVis, -1);
}

//
// ObjectData_Register::add_world
//
void ObjectData_Register::add_world
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   add(world_table, name, type, externDef, externVis, number);
}

//
// ObjectData_Register::add_global
//
void ObjectData_Register::add_global
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis)
{
   add(global_table, name, type, externDef, externVis, -1);
}

//
// ObjectData_Register::add_global
//
void ObjectData_Register::add_global
(std::string const &name, VariableType const *type, bool externDef,
 bool externVis, bigsint number)
{
   add(global_table, name, type, externDef, externVis, number);
}

//
// ObjectData_Register::generate_symbols
//
void ObjectData_Register::generate_symbols()
{
   ::generate_symbols(map_table);
   ::generate_symbols(world_table);
   ::generate_symbols(global_table);
}

//
// ObjectData_Register::ini_map
//
void ObjectData_Register::ini_map
(std::string const &name, ObjectExpression *ini)
{
   RegisterIter r = map_table.find(name);
   if (r == map_table.end()) return;
   r->second.init = ini;
}

//
// ObjectData_Register::iterate_map
//
void ObjectData_Register::iterate_map(IterFunc iterFunc, std::ostream *out)
{
   iterate(map_table, iterFunc, out);
}

//
// ObjectData_Register::iterate_world
//
void ObjectData_Register::iterate_world(IterFunc iterFunc, std::ostream *out)
{
   iterate(world_table, iterFunc, out);
}

//
// ObjectData_Register::iterate_global
//
void ObjectData_Register::iterate_global(IterFunc iterFunc, std::ostream *out)
{
   iterate(global_table, iterFunc, out);
}

//
// ObjectData_Register::read_objects
//
void ObjectData_Register::read_objects(std::istream *in)
{
   read_object(in, &map_table);
   read_object(in, &world_table);
   read_object(in, &global_table);
}

//
// ObjectData_Register::write_objects
//
void ObjectData_Register::write_objects(std::ostream *out)
{
   write_object(out, map_table);
   write_object(out, world_table);
   write_object(out, global_table);
}

//
// override_object<ObjectData_Register>
//
bool override_object(ObjectData_Register *out, ObjectData_Register const &in)
{
   if (out->name != in.name)
      return false;

   if (out->externDef && !in.externDef)
   {
      out->number    = in.number;
      out->externDef = false;
   }

   return true;
}

//
// read_object<ObjectData_Register>
//
void read_object(std::istream *in, ObjectData_Register *out)
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
// write_object<ObjectData_Register>
//
void write_object(std::ostream * out, ObjectData_Register const & in)
{
   write_object(out, in.strings);
   write_object(out, in.name);
   write_object(out, in.number);
   write_object(out, in.size);
   write_object(out, in.init);
   write_object(out, in.externDef);
   write_object(out, in.externVis);
}

// EOF

