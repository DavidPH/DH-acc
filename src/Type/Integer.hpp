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
// Integer type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Integer_H__
#define Type__Integer_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Integer
//
class Type_Integer final : public Type
{
   CounterPreamble(Type_Integer, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBits() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return true;}
   virtual bool isSigned() const {return sign;}

   // Type classification.
   virtual bool isArithmeticType() const      {return true;}
   virtual bool isIntegerType() const         {return true;}
   virtual bool isObjectType() const          {return true;}
   virtual bool isRealType() const            {return true;}
   virtual bool isScalarType() const          {return true;}
   virtual bool isSignedIntegerType() const   {return sign;}
   virtual bool isUnsignedIntegerType() const {return unsi;}


   static Type_Integer const ShortIntObj;
   static Type_Integer const IntObj;
   static Type_Integer const LongIntObj;
   static Type_Integer const LongLongIntObj;

   static Type_Integer const UnsignedShortIntObj;
   static Type_Integer const UnsignedIntObj;
   static Type_Integer const UnsignedLongIntObj;
   static Type_Integer const UnsignedLongLongIntObj;

private:
   Type_Integer(unsigned size, bool sign);

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   unsigned const size : 2;
   bool     const sign : 1;
   bool     const unsi : 1;
};

#endif//Type__Integer_H__

