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
class SourceVariable : Counter
{
   MAKE_NOCLONE_COUNTER_CLASS_BASE(SourceVariable, Counter);

public:
   enum StorageClass
   {
      SC_AUTO,
      SC_CONSTANT,
      SC_REGISTER,
      SC_REGISTER_GLOBAL,
      SC_REGISTER_MAP,
      SC_REGISTER_WORLD,
      SC_REGISTERARRAY_GLOBAL,
      SC_REGISTERARRAY_MAP,
      SC_REGISTERARRAY_WORLD,
      SC_STATIC
   };



   StorageClass getClass() const;

   std::string const &getNameObject() const;
   std::string const &getNameSource() const;

   CounterPointer<VariableData> getData() const;

   CounterReference<VariableType> getType() const;



   static Pointer create_constant
   (std::string const &nameSource, VariableType *type, ObjectExpression *expr,
    SourcePosition const &position);

   static Pointer create_constant
   (std::string const &nameSource, VariableType *type,
    std::string const &nameObject, SourcePosition const &position);

   static Pointer create_literal
   (VariableType *type, ObjectExpression *expr, SourcePosition const &position);

   static Pointer create_literal
   (VariableType *type, std::string const &nameObject,
    SourcePosition const &position);

   static Pointer create_variable
   (std::string const &nameSource, VariableType *type,
    std::string const &nameObject, StorageClass sc,
    SourcePosition const &position);

   static StorageClass get_StorageClass
   (std::string const &data, SourcePosition const &position);

private:
   SourceVariable
   (std::string const &nameSource, VariableType *type, ObjectExpression *expr,
    SourcePosition const &position);

   SourceVariable
   (std::string const &nameSource, VariableType *type,
    std::string const &nameObject, SourcePosition const &position);

   SourceVariable
   (std::string const &nameSource, VariableType *type,
    std::string const &nameObject, StorageClass sc,
    SourcePosition const &position);

   ~SourceVariable();

   SourcePosition position;
   std::string nameObject;
   std::string nameSource;
   CounterPointer<ObjectExpression> expr;
   CounterReference<VariableType> type;
   StorageClass sc;
};

#endif//HPP_SourceVariable_

