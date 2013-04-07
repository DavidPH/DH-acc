//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Array type handling.
//
//-----------------------------------------------------------------------------

#include "Array.hpp"

#include "SourceException.hpp"
#include "SourceExpression.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type::getArrayType
//
auto Type::getArrayType() const -> TypeCR
{
   if(!arrType0)
      arrType0 = new Type_Array0(this);

   return static_cast<TypeCR>(arrType0);
}

//
// Type::getArrayType
//
auto Type::getArrayType(bigsint size) const -> TypeCR
{
   // Search for existing array type.
   if(Type_Array const *type = arrType) do
   {
      if(type->size == size)
         return static_cast<TypeCR>(type);

      type = type->arrNext;
   }
   while(type != arrType);

   // Otherwise, create one and return it.
   return static_cast<TypeCR>(new Type_Array(this, size));
}

//
// Type::getArrayType
//
auto Type::getArrayType(SrcExp *size) const -> TypeCR
{
   if(!size)
   {
      if(!arrTypeVLA0)
         arrTypeVLA0 = new Type_ArrayVLA0(this);

      return static_cast<TypeCR>(arrTypeVLA0);
   }

   // A check for size being a constant expression could go here. However, that
   // is really a higher level question that cannot be answered here.

   // Search for existing array type.
   // This is unlikely to succeed. Do it anyway.
   if(Type_ArrayVLA const *type = arrTypeVLA) do
   {
      if(type->size == size)
         return static_cast<TypeCR>(type);

      type = type->arrNext;
   }
   while(type != arrTypeVLA);

   // Otherwise, create one and return it.
   return static_cast<TypeCR>(new Type_ArrayVLA(this, size));
}

//
// Type_ArrayBase::Type_ArrayBase
//
Type_ArrayBase::Type_ArrayBase(Type const *base_) : base{base_}
{
}

//
// Type_ArrayBase::getAlignment
//
bigsint Type_ArrayBase::getAlignment() const
{
   return base->getAlignment();
}

//
// Type_Array::Type_Array
//
Type_Array::Type_Array(Type_Array const &type) : Super{type}, size{type.size},
   arrNext{this}, arrPrev{this}
{
}

//
// Type_Array::Type_Array
//
Type_Array::Type_Array(Type const *base_, bigsint size_) : Super{base_}, size{size_}
{
   if(base->arrType)
   {
      arrNext = base->arrType;
      arrPrev = base->arrType->arrPrev;

      arrNext->arrPrev = this;
      arrPrev->arrNext = this;
   }
   else
   {
      base->arrType = this;

      arrNext = this;
      arrPrev = this;
   }
}

//
// Type_Array::~Type_Array
//
Type_Array::~Type_Array()
{
   if(base->arrType == this)
   {
      if(arrNext == this)
      {
         base->arrType = nullptr;
      }
      else
      {
         base->arrType = arrNext;

         arrNext->arrPrev = arrPrev;
         arrPrev->arrNext = arrNext;
      }
   }
   else
   {
      arrNext->arrPrev = arrPrev;
      arrPrev->arrNext = arrNext;
   }
}

//
// Type_Array::getNameMangleBase
//
void Type_Array::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'A';
   GetNameMangle(size, out, mangle);
   GetNameMangle(base, out, mangle);
}

//
// Type_Array::getSizeBytes
//
bigsint Type_Array::getSizeBytes() const
{
   return base->getSizeBytes() * size;
}

//
// Type_Array::getSizePtr
//
bigsint Type_Array::getSizePtr() const
{
   return base->getSizePtr() * size;
}

//
// Type_Array::getSizeWords
//
bigsint Type_Array::getSizeWords() const
{
   return base->getSizeWords() * size;
}

//
// Type_Array::isSizeVM
//
bool Type_Array::isSizeVM() const
{
   return base->isSizeVM();
}

//
// Type_Array::isVM
//
bool Type_Array::isVM() const
{
   return base->isSizeVM();
}

//
// Type_Array0::Type_Array0
//
Type_Array0::Type_Array0(Type const *base_) : Super{base_}
{
}

//
// Type_Array0::~Type_Array0
//
Type_Array0::~Type_Array0()
{
   if(base->arrType0 == this)
      base->arrType0 = nullptr;
}

//
// Type_Array0::getNameMangleBase
//
void Type_Array0::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << "A[]"; GetNameMangle(base, out, mangle);
}

//
// Type_Array0::getSizeBytes
//
bigsint Type_Array0::getSizeBytes() const
{
   Error_Np("internal error: incomplete");
}

//
// Type_Array0::getSizePtr
//
bigsint Type_Array0::getSizePtr() const
{
   Error_Np("internal error: incomplete");
}

//
// Type_Array0::getSizeWords
//
bigsint Type_Array0::getSizeWords() const
{
   Error_Np("internal error: incomplete");
}

//
// Type_Array0::isSizeVM
//
bool Type_Array0::isSizeVM() const
{
   return base->isSizeVM();
}

//
// Type_Array0::isVM
//
bool Type_Array0::isVM() const
{
   return base->isSizeVM();
}

//
// Type_ArrayVLA::Type_ArrayVLA
//
Type_ArrayVLA::Type_ArrayVLA(Type_ArrayVLA const &type) : Super{type},
   size{type.size}, arrNext{this}, arrPrev{this}
{
}

//
// Type_ArrayVLA::Type_ArrayVLA
//
Type_ArrayVLA::Type_ArrayVLA(Type const *base_, SrcExp *size_) : Super{base_}, size{size_}
{
   if(base->arrTypeVLA)
   {
      arrNext = base->arrTypeVLA;
      arrPrev = base->arrTypeVLA->arrPrev;

      arrNext->arrPrev = this;
      arrPrev->arrNext = this;
   }
   else
   {
      base->arrTypeVLA = this;

      arrNext = this;
      arrPrev = this;
   }
}

//
// Type_ArrayVLA::~Type_ArrayVLA
//
Type_ArrayVLA::~Type_ArrayVLA()
{
   if(base->arrTypeVLA == this)
   {
      if(arrNext == this)
      {
         base->arrTypeVLA = nullptr;
      }
      else
      {
         base->arrTypeVLA = arrNext;

         arrNext->arrPrev = arrPrev;
         arrPrev->arrNext = arrNext;
      }
   }
   else
   {
      arrNext->arrPrev = arrPrev;
      arrPrev->arrNext = arrNext;
   }
}

//
// Type_ArrayVLA::getNameMangleBase
//
void Type_ArrayVLA::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << "A[*]"; GetNameMangle(base, out, mangle);
}

//
// Type_ArrayVLA::getSizeBytes
//
bigsint Type_ArrayVLA::getSizeBytes() const
{
   Error_Np("internal error: VLA");
}

//
// Type_ArrayVLA::getSizeBytesVM
//
auto Type_ArrayVLA::getSizeBytesVM() const -> SrcExpR
{
   Error_Np("stub");
 //return SourceExpression::CreateBinaryMul(base->getSizeBytesVM(), size);
}

//
// Type_ArrayVLA::getSizePtr
//
bigsint Type_ArrayVLA::getSizePtr() const
{
   Error_Np("internal error: VLA");
}

//
// Type_ArrayVLA::getSizePtrVM
//
auto Type_ArrayVLA::getSizePtrVM() const -> SrcExpR
{
   Error_Np("stub");
 //return SourceExpression::CreateBinaryMul(base->getSizePtrVM(), size);
}

//
// Type_ArrayVLA::getSizeWords
//
bigsint Type_ArrayVLA::getSizeWords() const
{
   Error_Np("internal error: VLA");
}

//
// Type_ArrayVLA::getSizeWordsVM
//
auto Type_ArrayVLA::getSizeWordsVM() const -> SrcExpR
{
   Error_Np("stub");
 //return SourceExpression::CreateBinaryMul(base->getSizeWordsVM(), sizeVM);
}

//
// Type_ArrayVLA0::Type_ArrayVLA0
//
Type_ArrayVLA0::Type_ArrayVLA0(Type const *base_) : Super{base_}
{
}

//
// Type_ArrayVLA0::~Type_ArrayVLA0
//
Type_ArrayVLA0::~Type_ArrayVLA0()
{
   if(base->arrTypeVLA0 == this)
      base->arrTypeVLA0 = nullptr;
}

//
// Type_ArrayVLA0::getNameMangleBase
//
void Type_ArrayVLA0::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << "A[*]"; GetNameMangle(base, out, mangle);
}

//
// Type_ArrayVLA0::getSizeBytes
//
bigsint Type_ArrayVLA0::getSizeBytes() const
{
   Error_Np("internal error: incomplete VLA");
}

//
// Type_ArrayVLA0::getSizePtr
//
bigsint Type_ArrayVLA0::getSizePtr() const
{
   Error_Np("internal error: incomplete VLA");
}

//
// Type_ArrayVLA0::getSizeWords
//
bigsint Type_ArrayVLA0::getSizeWords() const
{
   Error_Np("internal error: incomplete VLA");
}

// EOF

