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
// Object-level function data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectExpression.hpp"
#include "../object_io.hpp"
#include "../SourceContext.hpp"

#include <map>


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
   bigsint argCount, bigsint retCount, SourceContext *context)
{
   Function &data = Table[name];

   if (data.name != name)
   {
      data.label     = label;
      data.name      = name;
      data.argCount  = argCount;
      data.number    = -1;
      data.retCount  = retCount;
      data.varCount  = -1;
      data.context   = context;
      data.externDef = !context;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_INT);

      return true;
   }
   else if (data.externDef && context)
   {
      data.externDef = false;
   }

   return false;
}

//
// ObjectData::Function::Add
//
bool Function::Add(std::string const &name, std::string const &label,
   bigsint argCount, bigsint retCount, bigsint varCount)
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
      data.externDef = varCount == -1;

      ObjectExpression::add_symbol(name, ObjectExpression::ET_INT);

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

   for(iter = Table.begin(); iter != Table.end(); ++iter)
   {
      iter->second.number = number++;

      obj = ObjectExpression::create_value_int
      (iter->second.number, SourcePosition::none());

      ObjectExpression::add_symbol(iter->second.name, obj);
   }
}

//
// ObjectData::Function::iterate
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
// ObjectData::Function::ReadObjects
//
void Function::ReadObjects(std::istream *in)
{
   read_object(in, &Table);
}

//
// ObjectData::Function::WriteObjects
//
void Function::WriteObjects(std::ostream *out)
{
   write_object(out, &Table);
}

}

//
// override_object<ObjectData::Function>
//
void override_object(ObjectData::Function *out, ObjectData::Function const *in)
{
   if (out->externDef && !in->externDef)
   {
      out->varCount = in->varCount;
      out->externDef = false;
   }
}

//
// read_object<ObjectData::Function>
//
void read_object(std::istream *in, ObjectData::Function *out)
{
   read_object(in, &out->label);
   read_object(in, &out->name);
   read_object(in, &out->argCount);
   read_object(in, &out->number);
   read_object(in, &out->retCount);
   read_object(in, &out->varCount);
   read_object(in, &out->externDef);

   out->context = NULL;
}

//
// write_object<ObjectData::Function>
//
void write_object(std::ostream *out, ObjectData::Function const *in)
{
   if (in->context)
   {
      const_cast<ObjectData::Function *>(in)->varCount =
         in->context->getLimit(STORE_REGISTER);
   }

   write_object(out, &in->label);
   write_object(out, &in->name);
   write_object(out, &in->argCount);
   write_object(out, &in->number);
   write_object(out, &in->retCount);
   write_object(out, &in->varCount);
   write_object(out, &in->externDef);
}

// EOF

