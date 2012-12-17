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
// Object-level script data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectExpression.hpp"
#include "../object_io.hpp"
#include "../option.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"

#include <map>


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
   ScriptType stype, bigsint flags, bigsint argCount, SourceContext *context,
   bool externVis, bigsint number, std::string const &string)
{
   Script &data = Table[name];

   if (data.name != name)
   {
      data.label     = label;
      data.name      = name;
      data.string    = string;
      data.stype     = stype;
      data.argCount  = argCount;
      data.flags     = flags;
      data.number    = number;
      data.varCount  = -1;
      data.context   = context;
      data.externDef = !context;
      data.externVis = externVis;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_INT);

      return true;
   }
   else if (data.externDef && context)
   {
      data.string    = string;
      data.number    = number;
      data.externDef = false;
   }

   return false;
}

//
// ObjectData::Script::Add
//
bool Script::Add(std::string const &name, std::string const &label,
   ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount,
   bool externVis, bigsint number, std::string const &string)
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
      data.varCount  = varCount;
      data.context   = NULL;
      data.externDef = varCount == -1;
      data.externVis = externVis;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_INT);

      return true;
   }
   else if(data.externDef && varCount != -1)
   {
      data.string    = string;
      data.number    = number;
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
         obj = ObjectExpression::create_value_symbol(
            String::Add(iter->second.string), SourcePosition::none());
      else
         obj = ObjectExpression::create_value_int
         (iter->second.number, SourcePosition::none());

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
// ObjectData::Script::ReadObjects
//
void Script::ReadObjects(std::istream *in)
{
   read_object(in, &Table);
}

//
// ObjectData::Script::WriteObjects
//
void Script::WriteObjects(std::ostream *out)
{
   write_object(out, &Table);
}

}

//
// override_object<ObjectData::Script>
//
void override_object(ObjectData::Script *out, ObjectData::Script const *in)
{
   if (out->externDef && !in->externDef)
   {
      out->number    = in->number;
      out->varCount  = in->varCount;
      out->externDef = false;
   }
}

//
// read_object<ObjectData::Script>
//
void read_object(std::istream *in, ObjectData::Script *out)
{
   read_object(in, &out->label);
   read_object(in, &out->name);
   read_object(in, &out->string);
   read_object(in, &out->stype);
   read_object(in, &out->argCount);
   read_object(in, &out->flags);
   read_object(in, &out->number);
   read_object(in, &out->varCount);
   read_object(in, &out->externDef);
   read_object(in, &out->externVis);

   out->context = NULL;
}

//
// read_object<ObjectData::ScriptType>
//
void read_object(std::istream *in, ObjectData::ScriptType *out)
{
   *out = static_cast<ObjectData::ScriptType>(read_object_int(in));

   if(*out > ObjectData::ST_NONE)
      *out = ObjectData::ST_NONE;
}

//
// write_object<ObjectData::Script>
//
void write_object(std::ostream *out, ObjectData::Script const *in)
{
   if (in->context)
   {
      const_cast<ObjectData::Script *>(in)->varCount =
         in->context->getLimit(STORE_REGISTER);
   }

   write_object(out, &in->label);
   write_object(out, &in->name);
   write_object(out, &in->string);
   write_object(out, &in->stype);
   write_object(out, &in->argCount);
   write_object(out, &in->flags);
   write_object(out, &in->number);
   write_object(out, &in->varCount);
   write_object(out, &in->externDef);
   write_object(out, &in->externVis);
}

//
// write_object<ObjectData::ScriptType>
//
void write_object(std::ostream *out, ObjectData::ScriptType const *in)
{
   write_object_int(out, static_cast<bigsint>(*in));
}

// EOF

