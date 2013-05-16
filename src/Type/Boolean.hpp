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
// Boolean type handling.
//
//-----------------------------------------------------------------------------

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Boolean
//
class Type_Boolean final : public Type
{
   CounterPreamble(Type_Boolean, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBitsF() const {return 0;}
   virtual bigsint getSizeBitsI() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const   {return 1;}
   virtual bigsint getSizeWords() const {return 1;}

   virtual bool isComplete() const {return true;}
   virtual bool isBoolean() const  {return true;}

   // Type classification.
   virtual bool isArithmeticType() const      {return hard;}
   virtual bool isIntegerType() const         {return hard;}
   virtual bool isObjectType() const          {return true;}
   virtual bool isRealType() const            {return true;}
   virtual bool isScalarType() const          {return true;}
   virtual bool isUnsignedIntegerType() const {return unsi;}


   static Type_Boolean const BoolCObj;
   static Type_Boolean const BoolCXXObj;
   static Type_Boolean const FastBoolObj;

protected:
   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   Type_Boolean(bool hard, bool unsi);

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   bool const hard : 1;
   bool const unsi : 1;
};

// EOF

