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
// Array type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Array_H__
#define Type__Array_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_ArrayBase
//
class Type_ArrayBase : public Type
{
   CounterPreambleAbstract(Type_ArrayBase, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual TypeCR  getBase() const {return base;}

   // Type classification.
   virtual bool isAggregateType() const {return true;}
   virtual bool isArrayType() const     {return true;}
   virtual bool isObjectType() const    {return true;}

protected:
   Type_ArrayBase(Type const *base);

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   TypeCR const base;
};

//
// Type_Array
//
class Type_Array final : public Type_ArrayBase
{
   CounterPreamble(Type_Array, Type_ArrayBase);

public:
   // Type information.
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return true;}
   virtual bool isSizeVM() const;
   virtual bool isVM() const;


   friend TypeCR Type::getArrayType(bigsint size) const;

protected:
   Type_Array(Type_Array const &type);
   Type_Array(Type const *base, bigsint size);
   virtual ~Type_Array();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   bigsint const size;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   mutable Type_Array const *arrNext, *arrPrev;
};

//
// Type_Array0
//
class Type_Array0 final : public Type_ArrayBase
{
   CounterPreamble(Type_Array0, Type_ArrayBase);

public:
   // Type information.
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isSizeVM() const;
   virtual bool isVM() const;


   friend TypeCR Type::getArrayType() const;

protected:
   explicit Type_Array0(Type const *base);
   virtual ~Type_Array0();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

//
// Type_ArrayVLA
//
class Type_ArrayVLA final : public Type_ArrayBase
{
   CounterPreamble(Type_ArrayVLA, Type_ArrayBase);

public:
   // Type information.
   virtual bigsint getSizeBytes() const;
   virtual SrcExpR getSizeBytesVM() const;
   virtual bigsint getSizePtr() const;
   virtual SrcExpR getSizePtrVM() const;
   virtual bigsint getSizeWords() const;
   virtual SrcExpR getSizeWordsVM() const;

   virtual bool isComplete() const {return true;}
   virtual bool isSizeVM() const   {return true;}
   virtual bool isVM() const       {return true;}


   friend TypeCR Type::getArrayType(SrcExp *size) const;

protected:
   Type_ArrayVLA(Type_ArrayVLA const &type);
   Type_ArrayVLA(Type const *base, SrcExp *size);
   virtual ~Type_ArrayVLA();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   SrcExpR const size;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   mutable Type_ArrayVLA const *arrNext, *arrPrev;
};

//
// Type_ArrayVLA0
//
class Type_ArrayVLA0 final : public Type_ArrayBase
{
   CounterPreamble(Type_ArrayVLA0, Type_ArrayBase);

public:
   // Type information.
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isSizeVM() const {return true;}
   virtual bool isVM() const     {return true;}


   friend TypeCR Type::getArrayType(SrcExp *size) const;

protected:
   explicit Type_ArrayVLA0(Type const *base);
   virtual ~Type_ArrayVLA0();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

#endif//Type__Array_H__

