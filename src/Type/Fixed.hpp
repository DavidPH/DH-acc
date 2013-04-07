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
// Fixed-point type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Fixed_H__
#define Type__Fixed_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Fixed
//
class Type_Fixed final : public Type
{
   CounterPreamble(Type_Fixed, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual TypeCR  getBase() const;
   virtual bigsint getSizeBits() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const   {return true;}
   virtual bool isSaturating() const {return satu;}
   virtual bool isSigned() const     {return sign;}

   // Type classification. (See Type.cpp for chart.)
   virtual bool isAccumType() const                        {return accu;}
   virtual bool isArithmeticType() const                   {return true;}
   virtual bool isFixedPointType() const                   {return true;}
   virtual bool isFractType() const                        {return frac;}
   virtual bool isObjectType() const                       {return true;}
   virtual bool isPrimaryFixedPointType() const            {return prim;}
   virtual bool isPrimarySignedFixedPointType() const      {return psig;}
   virtual bool isPrimaryUnsignedFixedPointType() const    {return puns;}
   virtual bool isRealType() const                         {return true;}
   virtual bool isSaturatingFixedPointType() const         {return satu;}
   virtual bool isSaturatingSignedFixedPointType() const   {return ssig;}
   virtual bool isSaturatingUnsignedFixedPointType() const {return suns;}
   virtual bool isScalarType() const                       {return true;}
   virtual bool isSignedAccumType() const                  {return sacc;}
   virtual bool isSignedFractType() const                  {return sfra;}
   virtual bool isUnsignedAccumType() const                {return uacc;}
   virtual bool isUnsignedFractType() const                {return ufra;}


   static Type_Fixed const ShortFractObj;
   static Type_Fixed const FractObj;
   static Type_Fixed const LongFractObj;
   static Type_Fixed const LongLongFractObj;
   static Type_Fixed const ShortAccumObj;
   static Type_Fixed const AccumObj;
   static Type_Fixed const LongAccumObj;
   static Type_Fixed const LongLongAccumObj;

   static Type_Fixed const UnsignedShortFractObj;
   static Type_Fixed const UnsignedFractObj;
   static Type_Fixed const UnsignedLongFractObj;
   static Type_Fixed const UnsignedLongLongFractObj;
   static Type_Fixed const UnsignedShortAccumObj;
   static Type_Fixed const UnsignedAccumObj;
   static Type_Fixed const UnsignedLongAccumObj;
   static Type_Fixed const UnsignedLongLongAccumObj;

   static Type_Fixed const SatShortFractObj;
   static Type_Fixed const SatFractObj;
   static Type_Fixed const SatLongFractObj;
   static Type_Fixed const SatLongLongFractObj;
   static Type_Fixed const SatShortAccumObj;
   static Type_Fixed const SatAccumObj;
   static Type_Fixed const SatLongAccumObj;
   static Type_Fixed const SatLongLongAccumObj;

   static Type_Fixed const SatUnsignedShortFractObj;
   static Type_Fixed const SatUnsignedFractObj;
   static Type_Fixed const SatUnsignedLongFractObj;
   static Type_Fixed const SatUnsignedLongLongFractObj;
   static Type_Fixed const SatUnsignedShortAccumObj;
   static Type_Fixed const SatUnsignedAccumObj;
   static Type_Fixed const SatUnsignedLongAccumObj;
   static Type_Fixed const SatUnsignedLongLongAccumObj;

private:
   Type_Fixed(unsigned size, bool satu, bool sign, bool frac);

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   unsigned const size : 2;
   bool     const accu : 1;
   bool     const frac : 1;
   bool     const prim : 1;
   bool     const psig : 1;
   bool     const puns : 1;
   bool     const sacc : 1;
   bool     const satu : 1;
   bool     const sfra : 1;
   bool     const sign : 1;
   bool     const ssig : 1;
   bool     const suns : 1;
   bool     const uacc : 1;
   bool     const ufra : 1;
   bool     const unsi : 1;
};

#endif//Type__Fixed_H__

