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
// Object-level array-storage variable data.
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

typedef std::map<std::string, ObjectData::ArrayVar> ArrayVarTable;
typedef ArrayVarTable::iterator ArrayVarIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static ArrayVarTable MapTable, WorldTable, GlobalTable;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ObjectData
{

//
// Add
//
static void Add(ArrayVarTable &table, std::string const &array,
   std::string const &name, VariableType const *type, LinkageSpecifier linkage,
   bool externDef, bigsint number)
{
   ArrayVar &data = table[name];

   if(data.name != name)
   {
      SetInit(data.init, NULL, type);

      // A slight hack for isInit variables.
      if(name.size() < 5 || name.compare(name.size() - 5, 5, "$init"))
         data.init.typeAll = IT_UNKNOWN;
      if(name.size() < 7 || name.compare(name.size() - 7, 7, "$isInit"))
         data.init.typeAll = IT_UNKNOWN;

      data.array     = array;
      data.name      = name;
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.linkage   = linkage;
      data.externDef = externDef;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_UNS);
   }
   else if(data.externDef && !externDef)
   {
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.externDef = externDef;
   }
}

//
// GenerateNumber
//
static void GenerateNumber(ArrayVarTable &table, ArrayVar &data)
{
   ArrayVarIter itr, end = table.end();
   bigsint index = 1, limit, itrI, itrL;

begin:
   for(itr = table.begin(); itr != end; ++itr)
   {
      if(itr->second.number < 0) continue;

      // Only variables in the same array can collide.
      if(itr->second.array != data.array) continue;

      limit = index + data.size;

      itrI = itr->second.number;
      itrL = itrI + itr->second.size;
      if(itrI == itrL) ++itrL;

      // If either end of itr's range is in range, try again.
      if((itrI >= index && limit > itrI) ||
         (itrL >  index && limit > itrL))
      {
         index = itrL;
         goto begin;
      }
   }

   data.number = index;
}

//
// GenerateSymbols
//
static void GenerateSymbols(ArrayVarTable &table)
{
   ArrayVarIter itr, end = table.end();
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
// Init
//
static bool Init(ArrayVarTable &table, std::string const &name,
   VariableType const *type, ObjectExpression *init)
{
   ArrayVarIter tableItr = table.find(name);

   // Surprisingly, data cannot be applied to an object that does not exist.
   if(tableItr == table.end()) return false;

   ArrayVar &data = tableItr->second;

   // Set init data.
   SetInit(data.init, init, type);

   // Now to determine if the entire object can be statically initialized.

   // This test is as simple as it is obvious. Is it a constant?
   if(!data.init.dataAll)
      return false;

   // The second is simple, but less obvious. If the expression contains any
   // strings, return false. Technically, if all of the objects in the final
   // array are strings, then the static initialization can (and will) happen.
   // But we can't know what else might be put in the final array. However, if
   // there are no strings, then we know that this section can be initialized.
   //
   // Unless we're using the ATAG chunk.
   if(!Option_UseChunkATAG && data.init.typeAll != IT_INTEGER)
      return false;

   // At this point, we know that the object can be statically initialized.
   return true;
}

//
// Iterate
//
static void Iterate(ArrayVarTable &table, ArrayVar::IterFunc iterFunc, std::ostream *out)
{
   for(ArrayVarIter itr = table.begin(), end = table.end(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::ArrayVar::AddMap
//
void ArrayVar::AddMap(std::string const &array, std::string const &name,
   VariableType const *type, LinkageSpecifier linkage, bool externDef,
   bigsint number)
{
   Add(MapTable, array, name, type, linkage, externDef, number);
}

//
// ObjectData::ArrayVar::AddWorld
//
void ArrayVar::AddWorld(std::string const &array, std::string const &name,
   VariableType const *type, LinkageSpecifier linkage, bool externDef,
   bigsint number)
{
   Add(WorldTable, array, name, type, linkage, externDef, number);
}

//
// ObjectData::ArrayVar::AddGlobal
//
void ArrayVar::AddGlobal(std::string const &array, std::string const &name,
   VariableType const *type, LinkageSpecifier linkage, bool externDef,
   bigsint number)
{
   Add(GlobalTable, array, name, type, linkage, externDef, number);
}

//
// ObjectData::ArrayVar::ArrayMap
//
std::string const &ArrayVar::ArrayMap(std::string const &name)
{
   return MapTable[name].array;
}

//
// ObjectData::ArrayVar::ArrayWorld
//
std::string const &ArrayVar::ArrayWorld(std::string const &name)
{
   return WorldTable[name].array;
}

//
// ObjectData::ArrayVar::ArrayGlobal
//
std::string const &ArrayVar::ArrayGlobal(std::string const &name)
{
   return GlobalTable[name].array;
}

//
// ObjectData::ArrayVar::GenerateSymbols
//
void ArrayVar::GenerateSymbols()
{
   ObjectData::GenerateSymbols(MapTable);
   ObjectData::GenerateSymbols(WorldTable);
   ObjectData::GenerateSymbols(GlobalTable);
}

//
// ObjectData::ArrayVar::InitMap
//
bool ArrayVar::InitMap(std::string const &name, VariableType const *type,
   ObjectExpression *init)
{
   return Init(MapTable, name, type, init);
}

//
// ObjectData::ArrayVar::IterateMap
//
void ArrayVar::IterateMap(IterFunc iterFunc, std::ostream *out)
{
   Iterate(MapTable, iterFunc, out);
}

//
// ObjectData::ArrayVar::IterateWorld
//
void ArrayVar::IterateWorld(IterFunc iterFunc, std::ostream *out)
{
   Iterate(WorldTable, iterFunc, out);
}

//
// ObjectData::ArrayVar::IterateGlobal
//
void ArrayVar::IterateGlobal(IterFunc iterFunc, std::ostream *out)
{
   Iterate(GlobalTable, iterFunc, out);
}

//
// ObjectData::ArrayVar::ReadObjects
//
void ArrayVar::ReadObjects(std::istream *in)
{
   read_object(in, &MapTable);
   read_object(in, &WorldTable);
   read_object(in, &GlobalTable);
}

//
// ObjectData::ArrayVar::WriteObjects
//
void ArrayVar::WriteObjects(std::ostream *out)
{
   write_object(out, &MapTable);
   write_object(out, &WorldTable);
   write_object(out, &GlobalTable);
}

}

//
// override_object<ObjectData::ArrayVar>
//
void override_object(ObjectData::ArrayVar *out, ObjectData::ArrayVar const *in)
{
   if(out->externDef && !in->externDef)
   {
      out->number    = in->number;
      out->size      = in->size;
      out->externDef = in->externDef;
   }
}

//
// read_object<ObjectData::ArrayVar>
//
void read_object(std::istream *in, ObjectData::ArrayVar *out)
{
   read_object(in, &out->init);
   read_object(in, &out->array);
   read_object(in, &out->name);
   read_object(in, &out->number);
   read_object(in, &out->size);
   read_object(in, &out->linkage);
   read_object(in, &out->externDef);
}

//
// write_object<ObjectData::ArrayVar>
//
void write_object(std::ostream *out, ObjectData::ArrayVar const *in)
{
   write_object(out, &in->init);
   write_object(out, &in->array);
   write_object(out, &in->name);
   write_object(out, &in->number);
   write_object(out, &in->size);
   write_object(out, &in->linkage);
   write_object(out, &in->externDef);
}

// EOF

