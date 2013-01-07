//-----------------------------------------------------------------------------
//
// Copyright(C) 2011 David Hill
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
// Low-level variable access information.
//
//-----------------------------------------------------------------------------

#ifndef VARIABLEDATA_H__
#define VARIABLEDATA_H__

#include "bignum.hpp"
#include "Counter.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class SourceExpression;

//
// VariableData
//
class VariableData : public PlainCounter
{
   CounterPreambleNoVirtual(VariableData, PlainCounter);

public:
   enum MemoryType
   {
      MT_ARRAY,
      MT_AUTO,
      MT_FARPTR,
      MT_LITERAL,
      MT_POINTER,
      MT_REGISTER,
      MT_STACK,
      MT_STATIC,
      MT_STRING,
      MT_VOID,

      MT_NONE
   };

   enum SectionA // Array
   {
      SA_MAP,
      SA_WORLD,
      SA_GLOBAL
   };

   enum SectionR // Register
   {
      SR_LOCAL,
      SR_MAP,
      SR_WORLD,
      SR_GLOBAL
   };


   MemoryType type;

   bigsint size;

   union
   {
      SectionA sectionA;
      SectionR sectionR;
   };

   CounterPointer<ObjectExpression> address;

   CounterPointer<SourceExpression> offsetExpr;
   Pointer offsetTemp;


   static Pointer create_array(bigsint size, SectionA section,
                               ObjectExpression *address, SourceExpression *offset);

   static Pointer create_auto(bigsint size, ObjectExpression *address);

   static Pointer create_farptr(bigsint size, ObjectExpression *address,
                                SourceExpression *offset);

   static Pointer create_literal(bigsint size, ObjectExpression *value);

   static Pointer create_pointer(bigsint size, ObjectExpression *address,
                                 SourceExpression *offset);

   static Pointer create_register(bigsint size, SectionR section,
                                  ObjectExpression *address);

   static Pointer create_stack(bigsint size);

   static Pointer create_static(bigsint size, ObjectExpression *address);

   static Pointer create_string(bigsint size, SourceExpression *offset);

   static Pointer create_void(bigsint size);

private:
   VariableData(MemoryType type, bigsint size, ObjectExpression *address,
                SourceExpression *offset);

   VariableData(MemoryType type, bigsint size, SectionR section,
                ObjectExpression *address, SourceExpression *offset);

   VariableData(MemoryType type, bigsint size, SectionA section,
                ObjectExpression *address, SourceExpression *offset);

   ~VariableData();
};

#endif//VARIABLEDATA_H__

