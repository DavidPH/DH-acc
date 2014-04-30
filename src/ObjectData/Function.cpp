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
// Object-level function data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectArchive.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData::Function> FunctionTable;
typedef FunctionTable::iterator FunctionIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static FunctionTable Table;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::Function::Add
//
bool Function::Add(std::string const &name, std::string const &label,
   bigsint argCount, bigsint retCount, SourceContext *context,
   LinkageSpecifier linkage)
{
   Function &data = Table[name];

   if(data.name != name)
   {
      data.label     = label;
      data.name      = name;
      data.argCount  = argCount;
      data.number    = -1;
      data.retCount  = retCount;
      data.varCount  = -1;
      data.context   = context;
      data.linkage   = linkage;
      data.externDef = !context;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_UNS);

      return true;
   }
   else if(data.externDef && context)
   {
      data.context   = context;
      data.externDef = false;
   }

   return false;
}

//
// ObjectData::Function::Add
//
bool Function::Add(std::string const &name, std::string const &label,
   bigsint argCount, bigsint retCount, bigsint varCount,
   LinkageSpecifier linkage)
{
   Function &data = Table[name];

   if(data.name != name)
   {
      data.label     = label;
      data.name      = name;
      data.argCount  = argCount;
      data.number    = -1;
      data.retCount  = retCount;
      data.varCount  = varCount;
      data.context   = NULL;
      data.linkage   = linkage;
      data.externDef = varCount == -1;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_UNS);

      return true;
   }
   else if(data.externDef && varCount != -1)
   {
      data.varCount  = varCount;
      data.externDef = false;
   }

   return false;
}

//
// ObjectData::Function::GenerateSymbols
//
void Function::GenerateSymbols()
{
   ObjectExpression::Pointer obj;
   FunctionIter iter;
   bigsint number = 0;

   // If not set yet, generate varCount.
   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      if(iter->second.context)
         iter->second.varCount = iter->second.context->getLimit(STORE_REGISTER);
   }

   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      iter->second.number = ++number;

      obj = ObjectExpression::CreateValueUNS(iter->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(iter->second.name, obj);
   }
}

//
// ObjectData::Function::Iterate
//
void Function::Iterate(IterFunc iterFunc, std::ostream *out)
{
   FunctionIter iter;

   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      if (iter->second.context)
         iter->second.varCount = iter->second.context->getLimit(STORE_REGISTER);

      iterFunc(out, iter->second);
   }
}

//
// ObjectData::Function::Load
//
ObjectLoad &Function::Load(ObjectLoad &arc)
{
   return arc >> Table;
}

//
// ObjectData::Function::Save
//
ObjectSave &Function::Save(ObjectSave &arc)
{
   return arc << Table;
}

}

//
// OA_Override<ObjectData::Function>
//
void OA_Override(ObjectData::Function &out, ObjectData::Function const &in)
{
   if(out.externDef && !in.externDef)
      out = in;
}

//
// operator ObjectSave << ObjectData::Function
//
ObjectSave &operator << (ObjectSave &arc, ObjectData::Function const &data)
{
   auto varCount = data.context ? data.context->getLimit(STORE_REGISTER) : data.varCount;

   arc << data.label << data.name << data.argCount << data.number
       << data.retCount << varCount << data.externDef;

   return arc;
}

//
// operator ObjectLoad >> ObjectData::Function
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Function &data)
{
   arc >> data.label >> data.name >> data.argCount >> data.number
       >> data.retCount >> data.varCount >> data.externDef;

   return arc;
}

// EOF

