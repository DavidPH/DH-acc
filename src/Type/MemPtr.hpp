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

#ifndef Type__MemPtr_H__
#define Type__MemPtr_H__

#include "Class.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_MemberPointer
//
class Type_MemberPointer final : public Type
{
   CounterPreamble(Type_MemberPointer, Type);

protected:
   typedef Type_Class Clas;
   typedef Clas::ConstReference ClasCR;

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual TypeCR  getBase() const {return base;}
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return true;}

   // Type classification.
   virtual bool isMemberPointerType() const {return true;}
   virtual bool isObjectType() const        {return true;}
   virtual bool isScalarType() const        {return true;}


   friend TypeCR Clas::getMemberPointerType(Type const *base) const;

protected:
   Type_MemberPointer(Type_MemberPointer const &type);
   Type_MemberPointer(Clas const *clas, Type const *base);
   virtual ~Type_MemberPointer();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   ClasCR const clas;
   TypeCR const base;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   Type_MemberPointer *ptmNext, *ptmPrev;
};

#endif//Type__MemPtr_H__

