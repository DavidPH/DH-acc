//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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

#include "../ObjectArchive.hpp"
#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../SourceExpression.hpp"
#include "../VariableType.hpp"

#include <algorithm>
#include <map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData::Array> ArrayTable;
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


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ObjectData
{

//
// Add
//
static void Add(ArrayTable &table, std::string const &name,
                LinkageSpecifier linkage, bool externDef, bigsint number)
{
   Array &data = table[name];

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
static void CountRegister(std::ostream *, Register const &r)
{
   for(bigsint i = r.size; i--;)
      used.insert(r.number + i);
}

//
// GenerateInit
//
static void GenerateInit(ArrayTable &table, ArrayVar const &v)
{
   Array &a = table[v.array];

   std::copy(v.init.data.begin(), v.init.data.end(), a.init.data.begin() + v.number);
   std::copy(v.init.type.begin(), v.init.type.end(), a.init.type.begin() + v.number);

   if(a.init.typeAll == IT_UNKNOWN)
      a.init.typeAll = v.init.typeAll;
   else if(v.init.typeAll != IT_UNKNOWN && v.init.typeAll != a.init.typeAll)
      a.init.typeAll = IT_MIXED;

   // TODO: Add init for status byte when needed.
}

//
// GenerateInitMap
//
static void GenerateInitMap(std::ostream *, ArrayVar const &v)
{
   GenerateInit(MapTable, v);
}

//
// GenerateNumber
//
static void GenerateNumber(ArrayTable &table, Array &data)
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
static void GenerateSize(ArrayTable &table, ArrayVar const &v)
{
   Array &data = table[v.array];
   bigsint size = v.number + v.size;

   if(data.size < size)
   {
      data.size = size;

      // Preload init tables.
      data.init.data.resize(data.size, NULL);
      data.init.type.resize(data.size, IT_UNKNOWN);
   }
}

//
// GenerateSizeMap
//
static void GenerateSizeMap(std::ostream *, ArrayVar const &v)
{
   GenerateSize(MapTable, v);
}

//
// GenerateSizeWorld
//
static void GenerateSizeWorld(std::ostream *, ArrayVar const &v)
{
   GenerateSize(WorldTable, v);
}

//
// GenerateSizeGlobal
//
static void GenerateSizeGlobal(std::ostream *, ArrayVar const &v)
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
      obj = ObjectExpression::CreateValueUNS(itr->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(itr->second.name, obj);
   }
}

//
// Iterate
//
static void Iterate(ArrayTable &table, Array::IterFunc iterFunc, std::ostream *out)
{
   for(ArrayIter itr = table.begin(), end = table.end(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::Array::AddMap
//
void Array::AddMap(std::string const &name, LinkageSpecifier linkage,
   bool externDef, bigsint number)
{
   Add(MapTable, name, linkage, externDef, number);
}

//
// ObjectData::Array::AddWorld
//
void Array::AddWorld(std::string const &name, LinkageSpecifier linkage,
   bool externDef, bigsint number)
{
   Add(WorldTable, name, linkage, externDef, number);
}

//
// ObjectData::Array::AddGlobal
//
void Array::AddGlobal(std::string const &name, LinkageSpecifier linkage,
   bool externDef, bigsint number)
{
   Add(GlobalTable, name, linkage, externDef, number);
}

//
// ObjectData::Array::Archive
//
ObjectArchive &Array::Archive(ObjectArchive &arc)
{
   return arc << MapTable << WorldTable << GlobalTable;
}

//
// ObjectData::Array::GenerateSymbols
//
void Array::GenerateSymbols()
{
   used.clear(); Register::IterateMap(CountRegister, NULL);
   ObjectData::GenerateSymbols(MapTable);

   used.clear(); used.insert(option_auto_array);
   ObjectData::GenerateSymbols(WorldTable);

   used.clear(); used.insert(option_addr_array);
   ObjectData::GenerateSymbols(GlobalTable);

   // Set sizes.
   ArrayVar::IterateMap(GenerateSizeMap, NULL);
   ArrayVar::IterateMap(GenerateSizeWorld, NULL);
   ArrayVar::IterateMap(GenerateSizeGlobal, NULL);

   // Build init table.
   ArrayVar::IterateMap(GenerateInitMap, NULL);
}

//
// ObjectData::Array::IterateMap
//
void Array::IterateMap(IterFunc iterFunc, std::ostream *out)
{
   Iterate(MapTable, iterFunc, out);
}

//
// ObjectData::Array::IterateWorld
//
void Array::IterateWorld(IterFunc iterFunc, std::ostream *out)
{
   Iterate(WorldTable, iterFunc, out);
}

//
// ObjectData::Array::IterateGlobal
//
void Array::IterateGlobal(IterFunc iterFunc, std::ostream *out)
{
   Iterate(GlobalTable, iterFunc, out);
}

}

//
// OA_Override<ObjectData::Array>
//
void OA_Override(ObjectData::Array &out, ObjectData::Array const &in)
{
   if(out.externDef && !in.externDef)
      out = in;
}

//
// operator ObjectArchive << ObjectData::Array
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectData::Array &data)
{
   return arc << data.init << data.name << data.number << data.size
              << data.linkage << data.externDef;
}

// EOF

