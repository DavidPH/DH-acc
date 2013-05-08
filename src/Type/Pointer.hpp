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
// Pointer and reference type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Pointer_H__
#define Type__Pointer_H__

#include "Type.hpp"

#include "ost_type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Nullptr
//
class Type_Nullptr final : public Type
{
   CounterPreamble(Type_Nullptr, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return true;}

   // Type classification.
   virtual bool isNullptr() const    {return true;}
   virtual bool isObjectType() const {return true;}
   virtual bool isScalarType() const {return true;}


   static Type_Nullptr const NullptrObj;

protected:
   virtual ObjectSave &saveObject(ObjectSave &save) const
      {return Super::saveObject(save << KWRD_Nullptr);}

private:
   Type_Nullptr();

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

//
// Type_PointerBase
//
class Type_PointerBase : public Type
{
   CounterPreambleAbstract(Type_PointerBase, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual TypeCR  getBase() const {return base;}
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return true;}
   virtual bool isVM() const;

protected:
   Type_PointerBase(Type const *base);

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   TypeCR const base;
};

//
// Type_LValueReference
//
class Type_LValueReference final : public Type_PointerBase
{
   CounterPreamble(Type_LValueReference, Type_PointerBase);

public:
   // Type classification.
   virtual bool isLValueReferenceType() const {return true;}
   virtual bool isReferenceType()       const {return true;}


   friend TypeCR Type::getLValueReferenceType() const;

protected:
   explicit Type_LValueReference(Type const *base);
   virtual ~Type_LValueReference();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

//
// Type_Pointer
//
class Type_Pointer final : public Type_PointerBase
{
   CounterPreamble(Type_Pointer, Type_PointerBase);

public:
   // Type classification.
   virtual bool isObjectType() const  {return true;}
   virtual bool isPointerType() const {return true;}
   virtual bool isScalarType() const  {return true;}


   friend TypeCR Type::getPointerType() const;

protected:
   explicit Type_Pointer(Type const *base);
   virtual ~Type_Pointer();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

//
// Type_RValueReference
//
class Type_RValueReference final : public Type_PointerBase
{
   CounterPreamble(Type_RValueReference, Type_PointerBase);

public:
   // Type classification.
   virtual bool isReferenceType()       const {return true;}
   virtual bool isRValueReferenceType() const {return true;}


   friend TypeCR Type::getRValueReferenceType() const;

protected:
   explicit Type_RValueReference(Type const *base);
   virtual ~Type_RValueReference();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

#endif//Type_Pointer_H__

