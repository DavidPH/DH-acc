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
// Member function type handling.
//
//-----------------------------------------------------------------------------

#include "MemFunc.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Class::getMemberFunctionType
//
auto Type_Class::getMemberFunctionType(Type const *retn, Parm const *parm, CallCon conv) const -> TypeCR
{
   // Search for existing function type.
   if(MFun *type = data->mfnType) do
   {
      if(type->retn == retn && type->parm == parm && type->conv == conv)
         return static_cast<TypeCR>(type);

      type = type->mfnNext;
   }
   while(type != data->mfnType);

   // Otherwise, create one and return it.
   return static_cast<TypeCR>(new MFun(this, retn, parm, conv));
}

//
// Type_MemberFunction::Type_MemberFunction
//
Type_MemberFunction::Type_MemberFunction(Type_MemberFunction const &type) :
   Super{type}, clas{type.clas}, mfnNext{this}, mfnPrev{this}
{
}

//
// Type_MemberFunction::Type_MemberFunction
//
Type_MemberFunction::Type_MemberFunction(Clas const *clas_, Type const *retn_,
   Parm const *parm_, CallCon conv_) : Super{retn_, parm_, conv_}, clas{clas_}
{
   if(clas->data->mfnType)
   {
      mfnNext = clas->data->mfnType;
      mfnPrev = clas->data->mfnType->mfnPrev;

      mfnNext->mfnPrev = this;
      mfnPrev->mfnNext = this;
   }
   else
   {
      clas->data->mfnType = this;

      mfnNext = this;
      mfnPrev = this;
   }
}

//
// Type_MemberFunction::~Type_MemberFunction
//
Type_MemberFunction::~Type_MemberFunction()
{
   if(clas->data->mfnType == this)
   {
      if(mfnNext == this)
      {
         clas->data->mfnType = nullptr;
      }
      else
      {
         clas->data->mfnType = mfnNext;

         mfnNext->mfnPrev = mfnPrev;
         mfnPrev->mfnNext = mfnNext;
      }
   }
   else
   {
      mfnNext->mfnPrev = mfnPrev;
      mfnPrev->mfnNext = mfnNext;
   }
}

//
// Type_MemberFunction::getCallWords
//
bigsint Type_MemberFunction::getCallWords() const
{
   bigsint size = clas->getQualType(getQual())->getPointerType()->getSizeWords();

   for(auto const &type : *parm)
      size += type->getSizeWords();

   return size;
}

//
// Type_MemberFunction::getNameMangleBase
//
void Type_MemberFunction::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'M';

   clas->getNameMangleName(out, mangle);

   GetNameMangle(conv, out, mangle);

   parm->getNameMangle(out, mangle);

   GetNameMangle(retn, out, mangle);
}

// EOF

