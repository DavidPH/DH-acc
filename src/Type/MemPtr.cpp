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
// Pointer to member type handling.
//
//-----------------------------------------------------------------------------

#include "MemPtr.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Class::getMemberPointerType
//
auto Type_Class::getMemberPointerType(Type const *base) const -> TypeCR
{
   // Search for existing pointer to member type.
   if(Type_MemberPointer *type = data->ptmType) do
   {
      if(type->base == base)
         return static_cast<TypeCR>(type);

      type = type->ptmNext;
   }
   while(type != data->ptmType);

   // Otherwise, create one and return it.
   return static_cast<TypeCR>(new Type_MemberPointer(this, base));
}

//
// Type_MemberPointer::Type_MemberPointer
//
Type_MemberPointer::Type_MemberPointer(Type_MemberPointer const &type) : Super{type},
   clas{type.clas}, base{type.base}, ptmNext{this}, ptmPrev{this}
{
}

//
// Type_MemberPointer::Type_MemberPointer
//
Type_MemberPointer::Type_MemberPointer(Clas const *clas_, Type const *base_) :
   clas{clas_}, base{base_}
{
   if(clas->data->ptmType)
   {
      ptmNext = clas->data->ptmType;
      ptmPrev = clas->data->ptmType->ptmPrev;

      ptmNext->ptmPrev = this;
      ptmPrev->ptmNext = this;
   }
   else
   {
      clas->data->ptmType = this;

      ptmNext = this;
      ptmPrev = this;
   }
}

//
// Type_MemberPointer::~Type_MemberPointer
//
Type_MemberPointer::~Type_MemberPointer()
{
   if(clas->data->ptmType == this)
   {
      if(ptmNext == this)
      {
         clas->data->ptmType = nullptr;
      }
      else
      {
         clas->data->ptmType = ptmNext;

         ptmNext->ptmPrev = ptmPrev;
         ptmPrev->ptmNext = ptmNext;
      }
   }
   else
   {
      ptmNext->ptmPrev = ptmPrev;
      ptmPrev->ptmNext = ptmNext;
   }
}

//
// Type_MemberPointer::getAlignment
//
bigsint Type_MemberPointer::getAlignment() const
{
   return GetAlignmentWord();
}

//
// Type_MemberPointer::getNameMangleBase
//
void Type_MemberPointer::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'O'; clas->getNameMangleName(out, mangle); GetNameMangle(base, out, mangle);
}

//
// Type_MemberPointer::getSizeBytes
//
bigsint Type_MemberPointer::getSizeBytes() const
{
   return getSizeWords() * GetSizeWord();
}

//
// Type_MemberPointer::getSizePtr
//
bigsint Type_MemberPointer::getSizePtr() const
{
   return getSizeWords();
}

//
// Type_MemberPointer::getSizeWords
//
bigsint Type_MemberPointer::getSizeWords() const
{
   if(base->isMemberFunctionType()) return 2;

   return 1;
}

//
// Type_MemberPointer::saveObject
//
ObjectSave &Type_MemberPointer::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_memptr << clas << base);
}

// EOF

