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
// Object-level auto data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectArchive.hpp"
#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../ost_type.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData::Static> StaticTable;
typedef StaticTable::iterator StaticIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

extern int option_static_offset;
static option::option_dptr<int> option_static_offset_handler
('\0', "static-offset", "features",
 "Sets the allocation offset for static variables. 8192 by default.",
 NULL, &option_static_offset);

static StaticTable Table;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

int option_static_offset = 8192;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ObjectData
{

//
// SetNumber
//
void SetNumber(Static &data)
{
   StaticIter itr = Table.begin(), end = Table.end();
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
         itr = Table.begin();
         continue;
      }

      ++itr;
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
// ObjectData::Static::Add
//
void Static::Add(std::string const &name, VariableType const *type,
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   Static &data = Table[name];

   if (data.name != name)
   {
      data.name      = name;
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.linkage   = linkage;
      data.externDef = externDef;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_UNS);
   }
   else if (data.externDef && externDef)
   {
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.externDef = false;
   }
}

//
// ObjectData::Static::Archive
//
ObjectArchive &Static::Archive(ObjectArchive &arc)
{
   return arc << Table;
}

//
// ObjectData::Static::GenerateSymbols
//
void Static::GenerateSymbols()
{
   ObjectExpression::Pointer obj;
   StaticIter itr, end = Table.end();

   for(itr = Table.begin(); itr != end; ++itr)
   {
      if(itr->second.number == -1)
         SetNumber(itr->second);
   }

   for(itr = Table.begin(); itr != end; ++itr)
   {
      obj = ObjectExpression::CreateValueUNS(itr->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(itr->second.name, obj);
   }
}

//
// ObjectData::Static::Init
//
bool Static::Init(std::string const &name, VariableType const *type, ObjectExpression *init)
{
   auto itr = Table.find(name);
   if(itr == Table.end()) return false;

   Static &data = itr->second;

   SetInit(data.init, init, type);

   if(!data.init.dataAll) return false;

   if(Target != TARGET_MageCraft) return false;

   return true;
}

//
// ObjectData::Static::Iterate
//
void Static::Iterate(IterFunc iterFunc, std::ostream *out)
{
   for(StaticIter itr = Table.begin(), end = Table.end(); itr != end; ++itr)
      iterFunc(out, itr->second);
}

}

//
// OA_Override<ObjectData::Static>
//
void OA_Override(ObjectData::Static &out, ObjectData::Static const &in)
{
   if(out.externDef && !in.externDef)
      out = in;
}

//
// operator ObjectArchive << ObjectData::Static
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectData::Static &data)
{
   return arc << data.name << data.number << data.size << data.linkage << data.externDef;
}

// EOF

