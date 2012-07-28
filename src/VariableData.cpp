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

#include "VariableData.hpp"

#include "ObjectExpression.hpp"
#include "SourceExpression.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static ObjectExpression::Reference address0 =
   ObjectExpression::create_value_uns(0, SourcePosition::none());


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// VariableData::VariableData
//
VariableData::
VariableData(MemoryType _type, bigsint _size, ObjectExpression *_address,
             SourceExpression *offset)
             : type(_type), size(_size), address(_address), offsetExpr(offset)
{
   if (!address)
      address = address0;
}

//
// VariableData::VariableData
//
VariableData::
VariableData(MemoryType _type, bigsint _size, SectionR section,
             ObjectExpression *_address, SourceExpression *offset)
             : type(_type), size(_size), sectionR(section), address(_address),
               offsetExpr(offset)
{
   if (!address)
      address = address0;
}

//
// VariableData::VariableData
//
VariableData::
VariableData(MemoryType _type, bigsint _size, SectionA section,
             ObjectExpression *_address, SourceExpression *offset)
 : type(_type), size(_size), sectionA(section), address(_address), offsetExpr(offset)
{
   if (!address)
      address = address0;
}

//
// VariableData::~VariableData
//
VariableData::~VariableData()
{
}

//
// VariableData::create_array
//
VariableData::Pointer VariableData::create_array(bigsint size, SectionA section,
   ObjectExpression *address, SourceExpression *offset)
{
   return new VariableData(MT_ARRAY, size, section, address, offset);
}

//
// VariableData::create_auto
//
VariableData::Pointer VariableData::
create_auto(bigsint size, ObjectExpression *address)
{
   return new VariableData(MT_AUTO, size, address, NULL);
}

//
// VariableData::create_farptr
//
VariableData::Pointer VariableData::create_farptr(bigsint size, SourceExpression *offset)
{
   return new VariableData(MT_FARPTR, size, NULL, offset);
}

//
// VariableData::create_literal
//
VariableData::Pointer VariableData::
create_literal(bigsint size, ObjectExpression *value)
{
   return new VariableData(MT_LITERAL, size, value, NULL);
}

//
// VariableData::create_pointer
//
VariableData::Pointer VariableData::
create_pointer(bigsint size, ObjectExpression *address,
               SourceExpression *offset)
{
   return new VariableData(MT_POINTER, size, address, offset);
}

//
// VariableData::create_register
//
VariableData::Pointer VariableData::
create_register(bigsint size, SectionR section,
                ObjectExpression *address)
{
   return new VariableData(MT_REGISTER, size, section, address, NULL);
}

//
// VariableData::create_stack
//
VariableData::Pointer VariableData::create_stack(bigsint size)
{
   Pointer stack1 = new VariableData(MT_STACK, 1, NULL, NULL);

   if (size == 1) return stack1;

   return new VariableData(MT_STACK, size, NULL, NULL);
}

//
// VariableData::create_static
//
VariableData::Pointer VariableData::
create_static(bigsint size, ObjectExpression *address)
{
   return new VariableData(MT_STATIC, size, address, NULL);
}

//
// VariableData::create_string
//
VariableData::Pointer VariableData::create_string(bigsint size, SourceExpression *offset)
{
   return new VariableData(MT_STRING, size, NULL, offset);
}

//
// VariableData::create_void
//
// Why have a sized void? So that the sink always has the same size as the
// source.
//
VariableData::Pointer VariableData::create_void(bigsint size)
{
   Pointer void1 = new VariableData(MT_VOID, 1, NULL, NULL);

   if (size == 1) return void1;

   return new VariableData(MT_VOID, size, NULL, NULL);
}



// EOF

