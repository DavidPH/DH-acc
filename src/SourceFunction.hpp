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
// Source-level function handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceFunction_
#define HPP_SourceFunction_

#include "Counter.hpp"

#include <cstddef>
#include <map>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourceExpression;
class SourcePosition;
class SourceVariable;
class VariableType;

//
// SourceFunction
//
class SourceFunction : public PlainCounter
{
   CounterPreambleNoVirtual(SourceFunction, PlainCounter);

public:
   typedef std::vector<CounterPointer<SourceExpression> > ArgVec;
   typedef std::map<std::string, Reference> FuncMap;
   typedef std::vector<CounterPointer<VariableType> > TypeVec;


   ~SourceFunction();

   // Sets the function's body, issuing an error if already set.
   void setBody(SourceExpression *expr, TypeVec const &argTypes, SourcePosition const &pos);

   CounterPointer<SourceExpression> body;
   CounterReference<SourceVariable> var;
   ArgVec args;
   TypeVec argTypes;

   size_t argsMin, argsMax;
   // For overload detection. If there are fewer args than argsMax, but at least
   // as many as argsMin, use types[argc - argsMin] to resolve.
   TypeVec *types;


   // Finds an already added function, or returns null.
   static Pointer FindFunction(std::string const &name);

   // Finds an already added function, or creates one.
   static Reference FindFunction(SourceVariable *var);
   static Reference FindFunction(SourceVariable *var, ArgVec const &args);

   static FuncMap FunctionTable;

private:
   SourceFunction(SourceVariable *var, ArgVec const &args);
};

#endif//HPP_SourceFunction_

