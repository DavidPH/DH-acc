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
// Tuple (multi-type) type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Tuple_H__
#define Type__Tuple_H__

#include "Function.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Tuple
//
class Type_Tuple final : public Type
{
   CounterPreamble(Type_Tuple, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return data->isComplete();}

   // Type classification.
   virtual bool isAggregateType() const {return true;}
   virtual bool isObjectType() const    {return true;}
   virtual bool isTupleType() const     {return true;}


   static TuplCR Get(Parm const *parm);
   static TuplCR Get(TypeCR const *types, std::size_t typeC);

protected:
   //
   // TupleData
   //
   struct TupleData
   {
      explicit TupleData(Parm const *parm);

      bool isComplete();

      bigsint alignment;
      bigsint sizeBytes;
      bigsint sizePtr;
      bigsint sizeWords;

      ParmCR const parm;

      bool complete : 1;
   };

   explicit Type_Tuple(Parm const *parm);
   virtual ~Type_Tuple();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   TupleData *const data;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   mutable Type_Tuple const *tplNext, *tplPrev;


   static Type_Tuple const *TplHead;
};

#endif//Type__Tuple_H__

