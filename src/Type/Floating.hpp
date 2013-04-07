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
// Floating type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Float_H__
#define Type__Float_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Floating
//
class Type_Floating final : public Type
{
   CounterPreamble(Type_Floating, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual TypeCR  getBase() const;
   virtual bigsint getSizeBits() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const  {return true;}
   virtual bool isComplex() const   {return comp;}
   virtual bool isImaginary() const {return imag;}
   virtual bool isSigned() const    {return true;}

   // Type classification. (See Type.cpp for chart.)
   virtual bool isArithmeticType() const   {return true;}
   virtual bool isComplexType() const      {return comp;}
   virtual bool isFloatingType() const     {return true;}
   virtual bool isObjectType() const       {return true;}
   virtual bool isRealType() const         {return real;}
   virtual bool isRealFloatingType() const {return real;}
   virtual bool isScalarType() const       {return true;}


   static Type_Floating const ShortFloatComplexObj;
   static Type_Floating const FloatComplexObj;
   static Type_Floating const LongFloatComplexObj;
   static Type_Floating const LongLongFloatComplexObj;

   static Type_Floating const ShortFloatImaginaryObj;
   static Type_Floating const FloatImaginaryObj;
   static Type_Floating const LongFloatImaginaryObj;
   static Type_Floating const LongLongFloatImaginaryObj;

   static Type_Floating const ShortFloatObj;
   static Type_Floating const FloatObj;
   static Type_Floating const LongFloatObj;
   static Type_Floating const LongLongFloatObj;

private:
   Type_Floating(unsigned size, bool comp, bool imag);

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   unsigned const size : 2;
   bool     const comp : 1;
   bool     const imag : 1;
   bool     const real : 1;
};

#endif//Type__Float_H__

