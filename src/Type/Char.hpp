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
// Character type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Char_H__
#define Type__Char_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Char
//
// "char", "signed char", and "unsigned char"
//
class Type_Char final : public Type
{
   CounterPreamble(Type_Char, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const {return 1;}
   virtual bigsint getSizeBits() const;
   virtual bigsint getSizeBytes() const {return 1;}
   virtual bigsint getSizePtr() const   {return 1;}
   virtual bigsint getSizeWords() const {return 1;}

   virtual bool isComplete() const {return true;}
   virtual bool isSigned() const;

   // Type classification.
   virtual bool isArithmeticType() const      {return true;}
   virtual bool isCharacterType() const       {return true;}
   virtual bool isIntegerType() const         {return true;}
   virtual bool isObjectType() const          {return true;}
   virtual bool isRealType() const            {return true;}
   virtual bool isScalarType() const          {return true;}
   virtual bool isSignedIntegerType() const   {return sign;}
   virtual bool isUnsignedIntegerType() const {return unsi;}


   static Type_Char const CharObj;
   static Type_Char const SignedCharObj;
   static Type_Char const UnsignedCharObj;

protected:
   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   Type_Char(bool unsi, bool sign);

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   bool sign : 1;
   bool unsi : 1;
};

//
// Type_WChar
//
// "char16_t", "char32_t", and "wchar_t"
// In C, these are typedefs and not (necessarily) exposed as distinct types.
//
class Type_WChar final : public Type
{
   CounterPreamble(Type_WChar, Type);

public:
   // Type inspection.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBits() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const {return 1;}

   virtual bool isComplete() const {return true;}
   virtual bool isSigned() const;

   // Type classification.
   virtual bool isArithmetic() const {return true;}
   virtual bool isInteger() const    {return true;}
   virtual bool isObject() const     {return true;}
   virtual bool isRealType() const   {return true;}
   virtual bool isScalar() const     {return true;}


   static Type_WChar const Char16Obj;
   static Type_WChar const Char32Obj;
   static Type_WChar const WCharObj;

protected:
   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   Type_WChar(bool wide, bool sign);

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   bool sign : 1;
   bool wide : 1;
};

#endif//Type__Char_H__

