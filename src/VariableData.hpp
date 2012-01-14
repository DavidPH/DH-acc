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
class VariableData : public Counter
{
   MAKE_COUNTER_CLASS_BASE(VariableData, Counter);

public:
   enum MemoryType
   {
      MT_AUTO,
      MT_LITERAL,
      MT_POINTER,
      MT_REGISTER,
      MT_REGISTERARRAY,
      MT_STACK,
      MT_STATIC,
      MT_VOID,

      MT_NONE
   };

   enum SectionR // Register
   {
      SR_LOCAL,
      SR_MAP,
      SR_WORLD,
      SR_GLOBAL
   };

   enum SectionRA // RegisterArray
   {
      SRA_MAP,
      SRA_WORLD,
      SRA_GLOBAL
   };


   MemoryType type;

   bigsint size;

   union
   {
      SectionR  sectionR;
      SectionRA sectionRA;
   };

   CounterPointer<ObjectExpression> address;

   CounterPointer<SourceExpression> offsetExpr;
   Pointer offsetTemp;


   static Pointer create_auto(bigsint size, ObjectExpression *address);

   static Pointer create_literal(bigsint size, ObjectExpression *value);

   static Pointer create_pointer(bigsint size, ObjectExpression *address,
                                 SourceExpression *offset);

   static Pointer create_register(bigsint size, SectionR section,
                                  ObjectExpression *address);

   static Pointer create_registerarray(bigsint size, SectionRA section,
                                       ObjectExpression *address,
                                       SourceExpression *offset);

   static Pointer create_stack(bigsint size);

   static Pointer create_static(bigsint size, ObjectExpression *address);

   static Pointer create_void(bigsint size);

private:
   VariableData(VariableData const &data);

   VariableData(MemoryType type, bigsint size, ObjectExpression *address,
                SourceExpression *offset);

   VariableData(MemoryType type, bigsint size, SectionR section,
                ObjectExpression *address, SourceExpression *offset);

   VariableData(MemoryType type, bigsint size, SectionRA section,
                ObjectExpression *address, SourceExpression *offset);

   ~VariableData();
};

#endif//VARIABLEDATA_H__

