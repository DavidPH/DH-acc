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
// Void and other odd type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Void_H__
#define Type__Void_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Label
//
class Type_Label final : public Type
{
   CounterPreamble(Type_Label, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;


   static Type_Label const LabelObj;

private:
   Type_Label();

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

//
// Type_String
//
class Type_String final : public Type
{
   CounterPreamble(Type_String, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return true;}

   // Type classification.
   virtual bool isObjectType() const {return true;}
   virtual bool isScalarType() const {return true;}
   virtual bool isString() const     {return true;}


   static Type_String const StringObj;

private:
   Type_String();

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

//
// Type_Void
//
class Type_Void final : public Type
{
   CounterPreamble(Type_Void, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   // Type classification.
   virtual bool isObjectType() const {return true;}
   virtual bool isVoid() const       {return true;}


   static Type_Void const &GetVoidObj();

   static Type_Void const &VoidObj;

private:
   Type_Void();

   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
};

#endif//Type__Void_H__

