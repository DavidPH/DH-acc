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
// Object-level register data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectArchive.hpp"
#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData::Register> RegisterTable;
typedef RegisterTable::iterator RegisterIter;


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

namespace ObjectData
{

static void SetNumber(RegisterTable &table, ObjectData::Register &data);

}


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<int> option_addr_stack_handler
('\0', "addr-stack", "features",
 "Selects which world register to use for the addressable stack. 0 by default.",
 NULL, &option_addr_stack);

static RegisterTable Table;
static RegisterTable MapTable;
static RegisterTable WorldTable;
static RegisterTable GlobalTable;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int option_addr_stack = 0;
extern bool option_named_scripts;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ObjectData
{

//
// Add
//
static void Add(RegisterTable &table, std::string const &name,
   VariableType const *type, LinkageSpecifier linkage, bool externDef,
   bigsint number)
{
   Register &data = table[name];

   if(data.name != name)
   {
      SetInit(data.init, NULL, type);

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
      data.externDef = false;
   }
}

//
// GenerateSymbols
//
static void GenerateSymbols(RegisterTable &table)
{
   ObjectExpression::Pointer obj;
   RegisterIter iter;

   for (iter = table.begin(); iter != table.end(); ++iter)
      if (iter->second.number == -1) SetNumber(table, iter->second);

   for (iter = table.begin(); iter != table.end(); ++iter)
   {
      obj = ObjectExpression::CreateValueUNS(iter->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(iter->second.name, obj);
   }
}

//
// Iterate
//
static void Iterate(RegisterTable &table, Register::IterFunc iterFunc, std::ostream *out)
{
   for (RegisterIter iter = table.begin(); iter != table.end(); ++iter)
      iterFunc(out, iter->second);
}

//
// SetNumber
//
static void SetNumber(RegisterTable &table, Register &data)
{
   RegisterIter iter = table.begin();
   bigsint index = 0, limit, iterI, iterL;

   while (iter != table.end())
   {
      if (iter->second.number < 0) {++iter; continue;}

      limit = index + data.size;

      // If this is the world table and stack-pointer is in range, try again.
      if (&table == &WorldTable &&
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

}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::Register::Add
//
void Register::Add(std::string const &name, VariableType const *type,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   ObjectData::Add(Table, name, type, linkage, externDef, number);
}

//
// ObjectData::Register::AddMap
//
void Register::AddMap(std::string const &name, VariableType const *type,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   ObjectData::Add(MapTable, name, type, linkage, externDef, number);
}

//
// ObjectData::Register::AddWorld
//
void Register::AddWorld(std::string const &name, VariableType const *type,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   ObjectData::Add(WorldTable, name, type, linkage, externDef, number);
}

//
// ObjectData::Register::AddGlobal
//
void Register::AddGlobal(std::string const &name, VariableType const *type,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   ObjectData::Add(GlobalTable, name, type, linkage, externDef, number);
}

//
// ObjectData::Register::Archive
//
ObjectArchive &Register::Archive(ObjectArchive &arc)
{
   return arc << Table << MapTable << WorldTable << GlobalTable;
}

//
// ObjectData::Register::GenerateSymbols
//
void Register::GenerateSymbols()
{
   ObjectData::GenerateSymbols(Table);
   ObjectData::GenerateSymbols(MapTable);
   ObjectData::GenerateSymbols(WorldTable);
   ObjectData::GenerateSymbols(GlobalTable);
}

//
// ObjectData::Register::InitMap
//
bool Register::InitMap(std::string const &name, VariableType const *type,
                       ObjectExpression *init)
{
   RegisterIter r = MapTable.find(name);
   if(r == MapTable.end()) return false;

   Register &data = r->second;

   SetInit(data.init, init, type);

   if(!data.init.dataAll) return false;

   return true;
}

//
// ObjectData::Register::Iterate
//
void Register::Iterate(IterFunc iterFunc, std::ostream *out)
{
   ObjectData::Iterate(Table, iterFunc, out);
}

//
// ObjectData::Register::IterateMap
//
void Register::IterateMap(IterFunc iterFunc, std::ostream *out)
{
   ObjectData::Iterate(MapTable, iterFunc, out);
}

//
// ObjectData::Register::IterateWorld
//
void Register::IterateWorld(IterFunc iterFunc, std::ostream *out)
{
   ObjectData::Iterate(WorldTable, iterFunc, out);
}

//
// ObjectData::Register::IterateGlobal
//
void Register::IterateGlobal(IterFunc iterFunc, std::ostream *out)
{
   ObjectData::Iterate(GlobalTable, iterFunc, out);
}

}

//
// OA_Override<ObjectData::Register>
//
void OA_Override(ObjectData::Register &out, ObjectData::Register const &in)
{
   if(out.externDef && !in.externDef)
      out = in;
}

//
// operator ObjectArchive << ObjectData::Register
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectData::Register &data)
{
   return arc << data.init << data.name << data.number << data.size
              << data.linkage << data.externDef;
}

// EOF

