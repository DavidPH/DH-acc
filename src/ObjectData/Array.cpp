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
#include "../SourceExpression.hpp"
#include "../VariableType.hpp"

#include <algorithm>
#include <map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData_Array> ArrayTable;
typedef ArrayTable::iterator ArrayIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<int> option_addr_array_handler
('\0', "addr-array", "features",
 "Selects which global array to use for addressable variables. 0 by default.",
 NULL, &option_addr_array);

static ArrayTable MapTable, WorldTable, GlobalTable;

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
// Add
//
static void Add(ArrayTable &table, std::string const &name,
                LinkageSpecifier linkage, bool externDef, bigsint number)
{
   ObjectData_Array &data = table[name];

   if(data.name != name)
   {
      data.name      = name;
      data.number    = number;
      data.size      = 0;
      data.linkage   = linkage;
      data.externDef = externDef;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_UNS);
   }
   else if(data.externDef && !externDef)
   {
      data.number    = number;
      data.externDef = externDef;
   }
}

//
// CountRegister
//
static void CountRegister(std::ostream *, ObjectData_Register const &r)
{
   for(bigsint i = r.size; i--;)
      used.insert(r.number + i);
}

//
// GenerateInit
//
static void GenerateInit(ArrayTable &table, ObjectData_ArrayVar const &v)
{
   ObjectData_Array &a = table[v.array];

   // If no init data, then never mind this var.
   if(v.init.empty() || v.strings.empty())
   {
      // But just to be safe, mark its contents as non-strings.
      // Um, but as a sort of GROSS HACK, don't do so for "::$init" objects.
      if(v.name.size() < 7 || !v.name.compare(v.name.size() - 7, 7, "::$init"))
         std::fill_n(a.strings.begin() + v.number, v.size, 0);

      return;
   }

   std::copy(v.init.begin(), v.init.end(), a.init.begin() + v.number);
   std::copy(v.strings.begin(), v.strings.end(), a.strings.begin() + v.number);

   // TODO: Add init for status byte when needed.
}

//
// GenerateInitMap
//
static void GenerateInitMap(std::ostream *, ObjectData_ArrayVar const &v)
{
   GenerateInit(MapTable, v);
}

//
// GenerateNumber
//
static void GenerateNumber(ArrayTable &table, ObjectData_Array &data)
{
   ArrayIter itr, end = table.end();
   bigsint index = 0;

begin:
   for(itr = table.begin(); itr != end; ++itr)
   {
      if(used.count(index) || index == itr->second.number)
      {
         ++index;
         goto begin;
      }
   }

   data.number = index;
}

//
// GenerateSize
//
static void GenerateSize(ArrayTable &table, ObjectData_ArrayVar const &v)
{
   ObjectData_Array &data = table[v.array];
   bigsint size = v.number + v.size;

   if(data.size < size)
   {
      data.size = size;

      // Preload init tables. Preload the strings table with true so it's easier to
      // detect all-strings. (Otherwise unused and init bytes would be unmarked.)
      // Possible alternative: 0 means Don't Care, 1 means String, 2 means Integer.
      // Although, at that point it should probably be an enumeration to allow for
      // other types of translated-index values. Such as functions.
      data.init.resize(data.size, NULL);
      data.strings.resize(data.size, 1);
   }
}

//
// GenerateSizeMap
//
static void GenerateSizeMap(std::ostream *, ObjectData_ArrayVar const &v)
{
   GenerateSize(MapTable, v);
}

//
// GenerateSizeWorld
//
static void GenerateSizeWorld(std::ostream *, ObjectData_ArrayVar const &v)
{
   GenerateSize(WorldTable, v);
}

//
// GenerateSizeGlobal
//
static void GenerateSizeGlobal(std::ostream *, ObjectData_ArrayVar const &v)
{
   GenerateSize(GlobalTable, v);
}

//
// GenerateSymbols
//
static void GenerateSymbols(ArrayTable &table)
{
   ArrayIter itr, end = table.end();
   ObjectExpression::Pointer obj;

   for(itr = table.begin(); itr != end; ++itr)
      if(itr->second.number == -1) GenerateNumber(table, itr->second);

   for(itr = table.begin(); itr != end; ++itr)
   {
      obj = ObjectExpression::create_value_uns(itr->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(itr->second.name, obj);
   }
}

//
// Iterate
//
static void Iterate(ArrayTable &table, ObjectData_Array::IterFunc iterFunc,
                    std::ostream *out)
{
   for(ArrayIter itr = table.begin(), end = table.end(); itr != end; ++itr)
      iterFunc(out, itr->second);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectData_Array::AddMap
//
void ObjectData_Array::AddMap(std::string const &name,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   Add(MapTable, name, linkage, externDef, number);
}

//
// ObjectData_Array::AddWorld
//
void ObjectData_Array::AddWorld(std::string const &name,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   Add(WorldTable, name, linkage, externDef, number);
}

//
// ObjectData_Array::AddGlobal
//
void ObjectData_Array::AddGlobal(std::string const &name,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   Add(GlobalTable, name, linkage, externDef, number);
}

//
// ObjectData_Array::GenerateSymbols
//
void ObjectData_Array::GenerateSymbols()
{
   ObjectData_Register::iterate_map(CountRegister, NULL);
   ::GenerateSymbols(MapTable);

   used.clear(); used.insert(option_auto_array);
   ::GenerateSymbols(WorldTable);

   used.clear(); used.insert(option_addr_array);
   ::GenerateSymbols(GlobalTable);

   used.clear();

   // Set sizes.
   ObjectData_ArrayVar::IterateMap(GenerateSizeMap, NULL);
   ObjectData_ArrayVar::IterateMap(GenerateSizeWorld, NULL);
   ObjectData_ArrayVar::IterateMap(GenerateSizeGlobal, NULL);

   // Build init table.
   ObjectData_ArrayVar::IterateMap(GenerateInitMap, NULL);
}

//
// ObjectData_Array::IterateMap
//
void ObjectData_Array::IterateMap(IterFunc iterFunc, std::ostream *out)
{
   Iterate(MapTable, iterFunc, out);
}

//
// ObjectData_Array::IterateWorld
//
void ObjectData_Array::IterateWorld(IterFunc iterFunc, std::ostream *out)
{
   Iterate(WorldTable, iterFunc, out);
}

//
// ObjectData_Array::IterateGlobal
//
void ObjectData_Array::IterateGlobal(IterFunc iterFunc, std::ostream *out)
{
   Iterate(GlobalTable, iterFunc, out);
}

//
// ObjectData_Array::ReadObjects
//
void ObjectData_Array::ReadObjects(std::istream *in)
{
   read_object(in, &MapTable);
   read_object(in, &WorldTable);
   read_object(in, &GlobalTable);
}

//
// ObjectData_Array::WriteObjects
//
void ObjectData_Array::WriteObjects(std::ostream *out)
{
   write_object(out, &MapTable);
   write_object(out, &WorldTable);
   write_object(out, &GlobalTable);
}

//
// override_object<ObjectData_Array>
//
void override_object(ObjectData_Array *out, ObjectData_Array const *in)
{
   if (out->externDef && !in->externDef)
   {
      out->number    = in->number;
      out->size      = in->size;
      out->externDef = in->externDef;
   }
}

//
// read_object<ObjectData_Array>
//
void read_object(std::istream *in, ObjectData_Array *out)
{
   read_object(in, &out->init);
   read_object(in, &out->strings);
   read_object(in, &out->name);
   read_object(in, &out->number);
   read_object(in, &out->size);
   read_object(in, &out->linkage);
   read_object(in, &out->externDef);
}

//
// write_object<ObjectData_Array>
//
void write_object(std::ostream *out, ObjectData_Array const *in)
{
   write_object(out, &in->init);
   write_object(out, &in->strings);
   write_object(out, &in->name);
   write_object(out, &in->number);
   write_object(out, &in->size);
   write_object(out, &in->linkage);
   write_object(out, &in->externDef);
}

// EOF

