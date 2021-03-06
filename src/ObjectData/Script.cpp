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
// Object-level script data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectArchive.hpp"
#include "../ObjectExpression.hpp"
#include "../option.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::map<std::string, ObjectData::Script> ScriptTable;
typedef ScriptTable::iterator ScriptIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_dptr<bool> option_named_scripts_handler
('\0', "named-scripts", "features",
 "Does codegen for named scripts. When enabled, explicitly numbered scripts "
 "cannot be called normally.", NULL, &option_named_scripts);

static int option_script_start = 1;
static option::option_dptr<int> option_script_start_handler
('\0', "script-start", "features",
 "Sets the lowest number to use for automatically allocated script numbers.",
 NULL, &option_script_start);

static ScriptTable Table;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool option_named_scripts = false;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

namespace ObjectData
{

//
// GetNumber
//
static bigsint GetNumber()
{
   ScriptIter iter;
   bigsint number = option_script_start;

   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      if (iter->second.number == number)
      {
         ++number;
         iter = Table.begin();
      }
   }

   return number;
}

}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::Script::Add
//
bool Script::Add(std::string const &name, std::string const &label,
   bigsint argCount, bigsint retCount, SourceContext *context,
   LinkageSpecifier linkage, bigsint number, std::string const &string,
   ScriptType stype, biguint flags)
{
   Script &data = Table[name];

   if(data.name != name)
   {
      data.label     = label;
      data.name      = name;
      data.string    = string;
      data.stype     = stype;
      data.argCount  = argCount;
      data.flags     = flags;
      data.number    = number;
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
      data.string    = string;
      data.number    = number;
      data.context   = context;
      data.externDef = false;
   }

   return false;
}

//
// ObjectData::Script::Add
//
bool Script::Add(std::string const &name, std::string const &label,
   bigsint argCount, bigsint retCount, bigsint varCount,
   LinkageSpecifier linkage, bigsint number, std::string const &string,
   ScriptType stype, biguint flags)
{
   Script &data = Table[name];

   if(data.name != name)
   {
      data.label     = label;
      data.name      = name;
      data.string    = string;
      data.stype     = stype;
      data.argCount  = argCount;
      data.flags     = flags;
      data.number    = number;
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
      data.string    = string;
      data.number    = number;
      data.varCount  = varCount;
      data.externDef = false;
   }

   return false;
}

//
// ObjectData::Script::GenerateSymbols
//
void ObjectData::Script::GenerateSymbols()
{
   ObjectExpression::Pointer obj;
   ScriptIter iter;
   bigsint number = 0;

   // If not set yet, generate varCount.
   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      if(iter->second.context)
         iter->second.varCount = iter->second.context->getLimit(STORE_REGISTER);
   }

   // Generate numbers.
   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      if (iter->second.number >= 0 || iter->second.externDef) continue;

      if(iter->second.number == -2)
         iter->second.number = --number;
      else
         iter->second.number = GetNumber();
   }

   // Generate symbols.
   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      if (iter->second.number < 0)
         obj = ObjectExpression::CreateValueSymbol(
            String::Add(iter->second.string), SourcePosition::none());
      else
         obj = ObjectExpression::CreateValueUNS(iter->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(iter->second.name, obj);
   }
}

//
// ObjectData::Script::Iterate
//
void Script::Iterate(IterFunc iterFunc, std::ostream *out)
{
   ScriptIter iter;

   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      if (iter->second.context)
         iter->second.varCount = iter->second.context->getLimit(STORE_REGISTER);

      iterFunc(out, iter->second);
   }
}

//
// ObjectData::Script::Load
//
ObjectLoad &Script::Load(ObjectLoad &arc)
{
   return arc >> Table;
}

//
// ObjectData::Script::Save
//
ObjectSave &Script::Save(ObjectSave &arc)
{
   return arc << Table;
}

}

//
// OA_Override<ObjectData::Script>
//
void OA_Override(ObjectData::Script &out, ObjectData::Script const &in)
{
   if(out.externDef && !in.externDef)
      out = in;
}

//
// operator ObjectSave << ObjectData::Script
//
ObjectSave &operator << (ObjectSave &arc, ObjectData::Script const &data)
{
   auto varCount = data.context ? data.context->getLimit(STORE_REGISTER) : data.varCount;

   arc << data.label << data.name << data.string << data.stype << data.argCount
       << data.flags << data.number << data.retCount << varCount << data.linkage
       << data.externDef;

   return arc;
}

//
// operator ObjectSave << ObjectData::ScriptType
//
ObjectSave &operator << (ObjectSave &arc, ObjectData::ScriptType const &data)
{
   return arc.saveEnum(data);
}

//
// operator ObjectLoad >> ObjectData::Script
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Script &data)
{
   arc >> data.label >> data.name >> data.string >> data.stype >> data.argCount
       >> data.flags >> data.number >> data.retCount >> data.varCount >> data.linkage
       >> data.externDef;

   return arc;
}

//
// operator ObjectLoad >> ObjectData::ScriptType
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::ScriptType &data)
{
   return arc.loadEnum(data, ObjectData::ST_NONE);
}

// EOF

