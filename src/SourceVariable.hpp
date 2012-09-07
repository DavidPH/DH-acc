//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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
// Source-level variable handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceVariable_
#define HPP_SourceVariable_

#include "Counter.hpp"
#include "SourcePosition.hpp"
#include "StoreType.hpp"

#include <ostream>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class ObjectVector;
class SourceExpression;
class VariableData;
class VariableType;

//
// SourceVariable
//
class SourceVariable : PlainCounter
{
   MAKE_NOVIRTUAL_COUNTER_CLASS_BASE(SourceVariable, PlainCounter);

public:
   StoreType getStoreType() const {return store;}

   std::string const &getNameObject() const {return nameObj;}
   std::string const &getNameSource() const {return nameSrc;}

   CounterPointer<VariableData> getData() const;

   SourcePosition const &getPosition() const {return pos;}

   CounterReference<VariableType> const &getType() const {return type;}

   void setNameArr(std::string const &nameArr);

   std::string nameArr;


   static Pointer create_constant(std::string const &nameSrc,
      VariableType *type, ObjectExpression *expr, SourcePosition const &pos)
   {
      return new SourceVariable(nameSrc, type, "", expr, pos);
   }

   static Pointer create_constant(std::string const &nameSrc,
      VariableType *type, std::string const &nameObj, SourcePosition const &pos)
   {
      return new SourceVariable(nameSrc, type, nameObj, NULL, pos);
   }

   static Pointer create_literal(VariableType *type, ObjectExpression *expr,
      SourcePosition const &pos)
   {
      return new SourceVariable("", type, "", expr, pos);
   }

   static Pointer create_literal(VariableType *type,
      std::string const &nameObj, SourcePosition const &pos)
   {
      return new SourceVariable("", type, nameObj, NULL, pos);
   }

   static Pointer create_variable(std::string const &nameSrc,
      VariableType *type, std::string const &nameObj, StoreType store,
      SourcePosition const &pos)
   {
      return new SourceVariable(nameSrc, type, nameObj, NULL, store, pos);
   }

   static Pointer create_variable(VariableType *type, ObjectExpression *expr,
      StoreType store, SourcePosition const &pos)
   {
      return new SourceVariable("", type, "", expr, store, pos);
   }

private:
   SourceVariable(std::string const &nameSrc, VariableType *type,
      std::string const &nameObj, ObjectExpression *expr,
      SourcePosition const &pos);

   SourceVariable(std::string const &nameSrc, VariableType *type,
      std::string const &nameObj, ObjectExpression *expr, StoreType store,
      SourcePosition const &pos);

   ~SourceVariable();

   SourcePosition pos;
   std::string nameObj;
   std::string nameSrc;
   CounterPointer<ObjectExpression> expr;
   CounterReference<VariableType> type;
   StoreType store;
};

#endif//HPP_SourceVariable_

