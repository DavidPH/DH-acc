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
// Handling for types that inherit classification from a base type.
//
//-----------------------------------------------------------------------------

#ifndef Type__Wrapper_H__
#define Type__Wrapper_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Wrapper
//
class Type_Wrapper : public Type
{
   CounterPreambleAbstract(Type_Wrapper, Type);

public:
   // Type information.
   virtual bool isBoolean() const    {return base->isBoolean();}
   virtual bool isComplete() const   {return base->isComplete();}
   virtual bool isComplex() const    {return base->isComplex();}
   virtual bool isImaginary() const  {return base->isImaginary();}
   virtual bool isSaturating() const {return base->isSaturating();}
   virtual bool isSigned() const     {return base->isSigned();}
   virtual bool isSizeVM() const     {return base->isSizeVM();}
   virtual bool isVM() const         {return base->isVM();}

   // Type classification.
   #define DoClassify(C) virtual bool C() const {return base->C();}
   DoClassify(isAccumType);
   DoClassify(isAggregateType);
   DoClassify(isArithmeticType);
   DoClassify(isArrayType);
   DoClassify(isBitfieldType);
   DoClassify(isCharacterType);
   DoClassify(isComplexType);
   DoClassify(isEnumeratedType);
   DoClassify(isFixedPointType);
   DoClassify(isFloatingType);
   DoClassify(isFractType);
   DoClassify(isFunctionType);
   DoClassify(isImaginaryType);
   DoClassify(isIntegerType);
   DoClassify(isLabel);
   DoClassify(isLValueReferenceType);
   DoClassify(isMemberFunctionType);
   DoClassify(isMemberPointerType);
   DoClassify(isNullptr);
   DoClassify(isObjectType);
   DoClassify(isPointerType);
   DoClassify(isPrimaryFixedPointType);
   DoClassify(isPrimarySignedFixedPointType);
   DoClassify(isPrimaryUnsignedFixedPointType);
   DoClassify(isRealType);
   DoClassify(isRealFloatingType);
   DoClassify(isReferenceType);
   DoClassify(isRValueReferenceType);
   DoClassify(isSaturatingFixedPointType);
   DoClassify(isSaturatingSignedFixedPointType);
   DoClassify(isSaturatingUnsignedFixedPointType);
   DoClassify(isScalarType);
   DoClassify(isScopedEnumerationType);
   DoClassify(isSignedAccumType);
   DoClassify(isSignedFractType);
   DoClassify(isSignedIntegerType);
   DoClassify(isStaticFunctionType);
   DoClassify(isString);
   DoClassify(isStructureType);
   DoClassify(isUnionType);
   DoClassify(isUnscopedEnumerationType);
   DoClassify(isUnsignedAccumType);
   DoClassify(isUnsignedFractType);
   DoClassify(isUnsignedIntegerType);
   DoClassify(isVoid);
   #undef DoClassify

protected:
   Type_Wrapper(Type const *base_) : base{base_} {}

   virtual ObjectSave &saveObject(ObjectSave &save) const
      {return Super::saveObject(save << base);}

   TypeCR const base;
};

#endif//Type__Wrapper_H__

