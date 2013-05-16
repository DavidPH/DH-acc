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

#ifndef Type__Bitfield_H__
#define Type__Bitfield_H__

#include "Wrapper.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Bitfield
//
class Type_Bitfield : public Type_Wrapper
{
   CounterPreamble(Type_Bitfield, Type_Wrapper);

public:
   // Type information.
   virtual bigsint getAlignment() const {return base->getAlignment();}
   virtual TypeCR  getBase() const      {return base;}
   virtual bigsint getSizeBitsF() const {return 0;}
   virtual bigsint getSizeBitsI() const {return sizeBitsI;}
   virtual bigsint getSizeBytes() const {return sizeBytes;}
   virtual bigsint getSizePtr() const   {return sizePtr;}
   virtual bigsint getSizeWords() const {return sizeWords;}

   // Type classification.
   virtual bool isBitfieldType() const {return true;}


   friend TypeCR Type::getBitfieldType(bigsint bits, bigsint offs) const;

protected:
   Type_Bitfield(Type_Bitfield const &type);
   Type_Bitfield(Type const *base, bigsint bits, bigsint offs);
   virtual ~Type_Bitfield();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   bigsint const bits;
   bigsint const offs;

   bigsint const sizeBitsI;
   bigsint const sizeBytes;
   bigsint const sizePtr;
   bigsint const sizeWords;

private:
   bigsint calcSizeBitsI() const;
   bigsint calcSizeBytes() const;
   bigsint calcSizePtr() const;
   bigsint calcSizeWords() const;

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   mutable Type_Bitfield const *bitNext, *bitPrev;
};

#endif//Type__Bitfield_H__

