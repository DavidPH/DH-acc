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

typedef std::map<std::string, ObjectData_Script> ScriptTable;
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

static ScriptTable script_table;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

bool option_named_scripts = false;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

static bigsint get_number()
{
   ScriptIter iter;
   bigsint number = option_script_start;

   for (iter = script_table.begin(); iter != script_table.end(); ++iter)
   {
      if (iter->second.number == number)
      {
         ++number;
         iter = script_table.begin();
      }
   }

   return number;
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectData_Script::add
//
bool ObjectData_Script::add
(std::string const &name, std::string const &label, ScriptType stype,
 bigsint flags, bigsint argCount, SourceContext *context, bool externVis)
{
   return add(name, label, stype, flags, argCount, context, externVis, -1);
}

//
// ObjectData_Script::add
//
bool ObjectData_Script::add
(std::string const &name, std::string const &label, ScriptType stype,
 bigsint flags, bigsint argCount, SourceContext *context, bool externVis,
 bigsint number)
{
   ObjectData_Script &data = script_table[name];

   if (data.name != name)
   {
      data.label     = label;
      data.name      = name;
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
      data.number    = number;
      data.externDef = false;
   }

   return false;
}

//
// ObjectData_Script::generate_symbols
//
void ObjectData_Script::generate_symbols()
{
   ObjectExpression::Pointer obj;
   ScriptIter iter;
   bigsint number = 0;

   // Generate numbers.
   for (iter = script_table.begin(); iter != script_table.end(); ++iter)
   {
      if (iter->second.number >= 0 || iter->second.externDef) continue;

      if (option_named_scripts)
         iter->second.number = --number;
      else
         iter->second.number = get_number();
   }

   // Generate symbols.
   for (iter = script_table.begin(); iter != script_table.end(); ++iter)
   {
      if (iter->second.number < 0)
         obj = ObjectExpression::create_value_symbol
         (ObjectData_String::add(iter->second.label), SourcePosition::none());
      else
         obj = ObjectExpression::create_value_int
         (iter->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(iter->second.name, obj);
   }
}

//
// ObjectData_Script::get_flag
//
ObjectData_Script::ScriptFlag ObjectData_Script::get_flag
(std::string const &data, SourcePosition const &pos)
{
   if (data == "__net")
      return ObjectData_Script::SF_NET;

   if (data == "__clientside")
      return ObjectData_Script::SF_CLIENTSIDE;

   ERROR_P("invalid script-flag: %s", data.c_str());
}

//
// ObjectData_Script::get_type
//
ObjectData_Script::ScriptType ObjectData_Script::get_type
(std::string const &data, SourcePosition const &pos)
{
   if (data == "__closed")
      return ObjectData_Script::ST_CLOSED;

   if (data == "__open")
      return ObjectData_Script::ST_OPEN;

   if (data == "__respawn")
      return ObjectData_Script::ST_RESPAWN;

   if (data == "__death")
      return ObjectData_Script::ST_DEATH;

   if (data == "__enter")
      return ObjectData_Script::ST_ENTER;

   if (data == "__lightning")
      return ObjectData_Script::ST_LIGHTNING;

   if (data == "__unloading")
      return ObjectData_Script::ST_UNLOADING;

   if (data == "__disconnect")
      return ObjectData_Script::ST_DISCONNECT;

   if (data == "__return")
      return ObjectData_Script::ST_RETURN;

   ERROR_P("invalid script-type: %s", data.c_str());
}

//
// ObjectData_Script::iterate
//
void ObjectData_Script::iterate(IterFunc iterFunc, std::ostream *out)
{
   ScriptIter iter;

   for (iter = script_table.begin(); iter != script_table.end(); ++iter)
   {
      if (iter->second.context)
         iter->second.varCount =
            iter->second.context->getLimit(SourceVariable::SC_REGISTER);

      iterFunc(out, iter->second);
   }
}

//
// ObjectData_Script::read_objects
//
void ObjectData_Script::read_objects(std::istream *in)
{
   read_object(in, &script_table);
}

//
// ObjectData_Script::write_objects
//
void ObjectData_Script::write_objects(std::ostream *out)
{
   write_object(out, &script_table);
}

//
// override_object<ObjectData_Script>
//
void override_object(ObjectData_Script *out, ObjectData_Script const *in)
{
   if (out->externDef && !in->externDef)
   {
      out->number    = in->number;
      out->varCount  = in->varCount;
      out->externDef = false;
   }
}

//
// read_object<ObjectData_Script>
//
void read_object(std::istream *in, ObjectData_Script *out)
{
   read_object(in, &out->label);
   read_object(in, &out->name);
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
// read_object<ObjectData_Script::ScriptType>
//
void read_object(std::istream *in, ObjectData_Script::ScriptType *out)
{
   *out = static_cast<ObjectData_Script::ScriptType>(read_object_int(in));

   if (*out > ObjectData_Script::ST_NONE)
      *out = ObjectData_Script::ST_NONE;
}

//
// write_object<ObjectData_Script>
//
void write_object(std::ostream *out, ObjectData_Script const *in)
{
   if (in->context)
   {
      const_cast<ObjectData_Script *>(in)->varCount =
         in->context->getLimit(SourceVariable::SC_REGISTER);
   }

   write_object(out, &in->label);
   write_object(out, &in->name);
   write_object(out, &in->stype);
   write_object(out, &in->argCount);
   write_object(out, &in->flags);
   write_object(out, &in->number);
   write_object(out, &in->varCount);
   write_object(out, &in->externDef);
   write_object(out, &in->externVis);
}

//
// write_object<ObjectData_Script::ScriptType>
//
void write_object(std::ostream *out, ObjectData_Script::ScriptType const *in)
{
   write_object_int(out, static_cast<bigsint>(*in));
}

// EOF

