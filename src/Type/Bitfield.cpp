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
// Bitfield type handling.
//
//-----------------------------------------------------------------------------

#include "Bitfield.hpp"

#include "ObjectArchive.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type::getBitfieldType
//
auto Type::getBitfieldType(bigsint bits, bigsint offs) const -> TypeCR
{
   // Search for existing bitfield type.
   if(Type_Bitfield const *type = bitType) do
   {
      if(type->bits == bits && type->offs == offs)
         return static_cast<TypeCR>(type);

      type = type->bitNext;
   }
   while(type != bitType);

   // Otherwise, create one and return it.
   return static_cast<TypeCR>(new Type_Bitfield(this, bits, offs));
}

//
// Type_Bitfield::Type_Bitfield
//
Type_Bitfield::Type_Bitfield(Type_Bitfield const &type) : Super{type},
   bits{type.bits}, offs{type.offs}, sizeBits{type.sizeBits}, sizeBytes{type.sizeBytes},
   sizePtr{type.sizePtr}, sizeWords{type.sizeWords}, bitNext{this}, bitPrev{this}
{
}

//
// Type_Bitfield::Type_Bitfield
//
Type_Bitfield::Type_Bitfield(Type const *base_, bigsint bits_, bigsint offs_) :
   Super{base_}, bits{bits_}, offs{offs_}, sizeBits{calcSizeBits()},
   sizeBytes{calcSizeBytes()}, sizePtr{calcSizePtr()}, sizeWords{calcSizeWords()}
{
   if(base->bitType)
   {
      bitNext = base->bitType;
      bitPrev = base->bitType->bitPrev;

      bitNext->bitPrev = this;
      bitPrev->bitNext = this;
   }
   else
   {
      base->bitType = this;

      bitNext = this;
      bitPrev = this;
   }
}

//
// Type_Bitfield::~Type_Bitfield
//
Type_Bitfield::~Type_Bitfield()
{
   if(base->bitType == this)
   {
      if(bitNext == this)
      {
         base->bitType = nullptr;
      }
      else
      {
         base->bitType = bitNext;

         bitNext->bitPrev = bitPrev;
         bitPrev->bitNext = bitNext;
      }
   }
   else
   {
      bitNext->bitPrev = bitPrev;
      bitPrev->bitNext = bitNext;
   }
}

//
// Type_Bitfield::calcSizeBits
//
bigsint Type_Bitfield::calcSizeBits() const
{
   bigsint baseBits = base->getSizeBits();
   return baseBits < bits ? baseBits : bits;
}

//
// Type_Bitfield::calcSizeBytes
//
bigsint Type_Bitfield::calcSizeBytes() const
{
   bigsint baseBytes = base->getSizeBytes();
   bigsint thisBytes = (bits + GetSizeByte() - 1) / GetSizeByte();
   return (thisBytes + baseBytes - 1) / baseBytes * baseBytes;
}

//
// Type_Bitfield::calcSizePtr
//
bigsint Type_Bitfield::calcSizePtr() const
{
   return base->getSizePtr() * sizeBytes / base->getSizeBytes();
}

//
// Type_Bitfield::calcSizeWords
//
bigsint Type_Bitfield::calcSizeWords() const
{
   return (sizeBytes + GetSizeWord() - 1) / GetSizeWord();
}

//
// Type_Bitfield::getNameMangleBase
//
void Type_Bitfield::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'W'; // The W is for Wat.
   GetNameMangle(base, out, mangle);
   GetNameMangle(bits, out, mangle);
   GetNameMangle(offs, out, mangle);
}

//
// Type_Bitfield::saveObject
//
ObjectSave &Type_Bitfield::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_bitfield << bits << offs);
}

// EOF

