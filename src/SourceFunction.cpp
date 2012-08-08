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
// Source-level function handling.
//
//-----------------------------------------------------------------------------

#include "SourceFunction.hpp"

#include "SourceException.hpp"
#include "SourceExpression.hpp"
#include "SourceVariable.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceFunction::FuncMap SourceFunction::FunctionTable;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceFunction::SourceFunction
//
SourceFunction::SourceFunction(SourceVariable *_var, ArgVec const &_args)
 : var(_var), args(_args)
{
   VariableType::Vector const &varTypes = var->getType()->getTypes();
   size_t i, j;

   argsMin = 0;
   argsMax = varTypes.size();

   for(i = 0; i < args.size(); ++i)
      if(!args[i]) argsMin = i + 1;

   if(argsMin == argsMax)
      types = NULL;
   else
   {
      types = new VariableType::Vector[argsMax - argsMin];
      for(i = argsMin; i != argsMax; ++i)
      {
         VariableType::Vector &type = types[i - argsMin];

         type.resize(i);
         for(j = 0; j != i; ++j)
            type[j] = varTypes[j];
      }
   }
}

//
// SourceFunction::~SourceFunction
//
SourceFunction::~SourceFunction()
{
   delete[] types;
}

//
// setBody
//
void SourceFunction::setBody(SourceExpression *expr,
   VariableType::Vector const &_argTypes, SourcePosition const &pos)
{
   if(body) ERROR_P("redefined function");

   body = expr;
   argTypes = _argTypes;
}

//
// SourceFunction::FindFunction
//
SourceFunction::Pointer SourceFunction::FindFunction(std::string const &name)
{
   FuncMap::iterator funcp = FunctionTable.find(name);

   if(funcp == FunctionTable.end())
      return NULL;
   else
      return funcp->second;
}

//
// SourceFunction::FindFunction
//
SourceFunction::Reference SourceFunction::FindFunction(SourceVariable *var, ArgVec const &args)
{
   std::string const &name = var->getNameObject();

   if(name.empty())
      return static_cast<Reference>(new SourceFunction(var, args));

   FuncMap::iterator funcp = FunctionTable.find(var->getNameObject());

   if(funcp == FunctionTable.end())
   {
      // Just make a new SourceFunction, add it, and return it.
      Reference func(new SourceFunction(var, args));
      FuncMap::value_type pair(var->getNameObject(), func);
      FunctionTable.insert(pair);
      return func;
   }
   else
   {
      // Check for redefined defaults.
      for(ArgVec::const_iterator itr = args.begin(), end = args.end(); itr != end; ++itr)
         if(*itr) ERROR(var->getPosition(), "redefined defaults");

      Reference func = funcp->second;

      // Check type.
      //if(func->var->getType() != var->getType())
      //   ERROR(var->getPosition(), "wrong type");

      return func;
   }
}

// EOF

