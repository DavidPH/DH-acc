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
#include "../VariableType.hpp"


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
   LinkageSpecifier linkage, bool externDef, bigsint number)
{
   Auto &data = Table[name];

   if(data.name != name)
   {
      data.name      = name;
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.linkage   = linkage;
      data.externDef = externDef;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_INT);
   }
   else if(data.externDef && !externDef)
   {
      data.number    = number;
      data.size      = type->getSize(SourcePosition::none());
      data.externDef = false;
   }
}

//
// ObjectData::Auto::Archive
//
ObjectArchive &Auto::Archive(ObjectArchive &arc)
{
   return arc << Table;
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

}

//
// OA_Override<ObjectData::Auto>
//
void OA_Override(ObjectData::Auto &out, ObjectData::Auto const &in)
{
   if(out.externDef && !in.externDef)
      out = in;
}

//
// operator ObjectArchive << ObjectData::Auto
//
ObjectArchive &operator << (ObjectArchive &arc, ObjectData::Auto &data)
{
   return arc << data.name << data.number << data.size << data.linkage << data.externDef;
}

// EOF

