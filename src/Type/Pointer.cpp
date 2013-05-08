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
// Pointer and reference type handling.
//
//-----------------------------------------------------------------------------

#include "Pointer.hpp"

#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Nullptr const Type_Nullptr::NullptrObj;

Type::TypeCR const Type::Nullptr{&Type_Nullptr::NullptrObj};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type::getLValueReferenceType
//
auto Type::getLValueReferenceType() const -> TypeCR
{
   if(!lvrType)
      lvrType = new Type_LValueReference(this);

   return static_cast<TypeCR>(lvrType);
}

//
// Type::getPointerType
//
auto Type::getPointerType() const -> TypeCR
{
   if(!ptrType)
      ptrType = new Type_Pointer(this);

   return static_cast<TypeCR>(ptrType);
}

//
// Type::getRValueReferenceType
//
auto Type::getRValueReferenceType() const -> TypeCR
{
   if(!rvrType)
      rvrType = new Type_RValueReference(this);

   return static_cast<TypeCR>(rvrType);
}

//
// Type_Nullptr::Type_Nullptr
//
Type_Nullptr::Type_Nullptr()
{
   ++refCount;
}

//
// Type_Nullptr::getAlignment
//
bigsint Type_Nullptr::getAlignment() const
{
   return GetAlignmentWord();
}

//
// Type_Nullptr::getNameMangleBase
//
void Type_Nullptr::getNameMangleBase(std::ostream &out, NameMangleStyle) const
{
   out << "BN";
}

//
// Type_Nullptr::getSizeBytes
//
bigsint Type_Nullptr::getSizeBytes() const
{
   return getSizeWords() * GetSizeWord();
}

//
// Type_Nullptr::getSizePtr
//
bigsint Type_Nullptr::getSizePtr() const
{
   return getSizeWords();
}

//
// Type_Nullptr::getSizeWords
//
bigsint Type_Nullptr::getSizeWords() const
{
   return 1;
}

//
// Type_PointerBase::Type_PointerBase
//
Type_PointerBase::Type_PointerBase(Type const *base_) : base{base_}
{
}

//
// Type_PointerBase::getAlignment
//
bigsint Type_PointerBase::getAlignment() const
{
   return GetAlignmentWord();
}

//
// Type_PointerBase::getSizeBytes
//
bigsint Type_PointerBase::getSizeBytes() const
{
   return getSizeWords() * GetSizeWord();
}

//
// Type_PointerBase::getSizePtr
//
bigsint Type_PointerBase::getSizePtr() const
{
   // Pointers to pointers are word-oriented pointers.
   return getSizeWords();
}

//
// Type_PointerBase::getSizeWords
//
bigsint Type_PointerBase::getSizeWords() const
{
   if(base->isFunctionType()) switch(base->getCallConv())
   {
   case CALL_AsmFunc:
      Error_Np("internal error: CALL_AsmFunc");

   default:
      return 1;
   }

   if(base->isLabel())
      return 1;

   if(base->isObjectType()) switch(base->getQual().addressSpace.base)
   {
   case ADDR_Far:
   case ADDR_GlobalArrays:
   case ADDR_MapArrays:
   case ADDR_StringArrays:
   case ADDR_WorldArrays:
      return 2;

   default:
      return 1;
   }

   return 1;
}

//
// Type_PointerBase::isVM
//
bool Type_PointerBase::isVM() const
{
   // Pointers are a variably-modified type if they point to a VLA.
   return base->isArrayType() && base->isSizeVM();
}

//
// Type_PointerBase::saveObject
//
ObjectSave &Type_PointerBase::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << base);
}

//
// Type_LValueReference::Type_LValueReference
//
Type_LValueReference::Type_LValueReference(Type const *base_) : Super{base_}
{
}

//
// Type_LValueReference::~Type_LValueReference
//
Type_LValueReference::~Type_LValueReference()
{
   if(base->lvrType == this)
      base->lvrType = nullptr;
}

//
// Type_LValueReference::getNameMangleBase
//
void Type_LValueReference::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'L'; GetNameMangle(base, out, mangle);
}

//
// Type_LValueReference::saveObject
//
ObjectSave &Type_LValueReference::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_lvref);
}

//
// Type_Pointer::Type_Pointer
//
Type_Pointer::Type_Pointer(Type const *base_) : Super{base_}
{
}

//
// Type_Pointer::~Type_Pointer
//
Type_Pointer::~Type_Pointer()
{
   if(base->ptrType == this)
      base->ptrType = nullptr;
}

//
// Type_Pointer::getNameMangleBase
//
void Type_Pointer::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'P'; GetNameMangle(base, out, mangle);
}

//
// Type_Pointer::saveObject
//
ObjectSave &Type_Pointer::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_pointer);
}

//
// Type_RValueReference::Type_RValueReference
//
Type_RValueReference::Type_RValueReference(Type const *base_) : Super{base_}
{
}

//
// Type_RValueReference::~Type_RValueReference
//
Type_RValueReference::~Type_RValueReference()
{
   if(base->rvrType == this)
      base->rvrType = nullptr;
}

//
// Type_RValueReference::getNameMangleBase
//
void Type_RValueReference::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'R'; GetNameMangle(base, out, mangle);
}

//
// Type_RValueReference::saveObject
//
ObjectSave &Type_RValueReference::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_rvref);
}

// EOF

