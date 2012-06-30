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

#include "SourceExpression.hpp"
#include "SourceVariable.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceFunction::SourceFunction
//
SourceFunction::SourceFunction(SourceVariable *_var, SourceExpression::Vector const &_args)
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

// EOF

