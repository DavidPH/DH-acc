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

#include "Tuple.hpp"

#include "ost_type.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

Type_Tuple const *Type_Tuple::TplHead = nullptr;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Tuple::TupleData constructor
//
Type_Tuple::TupleData::TupleData(Parm const *parm_) : parm{parm_}, complete{false}
{
   if(Target == TARGET_MageCraft)
      alignment = 4;
   else
      alignment = 1;
}

//
// Type_Tuple::TupleData::isComplete
//
bool Type_Tuple::TupleData::isComplete()
{
   if(complete) return true;

   sizeWords = 0;
   for(auto const &type : *parm)
   {
      if(!type->isComplete()) return false;
      sizeWords += type->getSizeWords();
   }

   sizeBytes = sizeWords * GetSizeWord();
   sizePtr   = sizeWords;

   return complete = true;
}

//
// Type_Tuple constructor
//
Type_Tuple::Type_Tuple(Parm const *parm) : data{new TupleData(parm)}
{
   if(TplHead)
   {
      (tplNext = TplHead->tplNext)->tplPrev = this;
      (tplPrev = TplHead         )->tplNext = this;
   }
   else
      TplHead = tplNext = tplPrev = this;
}

//
// Type_Tuple destructor
//
Type_Tuple::~Type_Tuple()
{
   if(!isQual()) delete data;

   if(TplHead == this)
   {
      if(tplNext == this)
      {
         TplHead = nullptr;
      }
      else
      {
         TplHead = tplNext;

         tplNext->tplPrev = tplPrev;
         tplPrev->tplNext = tplNext;
      }
   }
   else
   {
      tplNext->tplPrev = tplPrev;
      tplPrev->tplNext = tplNext;
   }
}

//
// Type_Tuple::getAlignment
//
bigsint Type_Tuple::getAlignment() const
{
   if(!data->isComplete()) Error_Np("internal error: incomplete");

   return data->alignment;
}

//
// Type_Tuple::getNameMangleBase
//
void Type_Tuple::getNameMangleBase(std::ostream &out, NameMangleStyle style) const
{
   out << 'T';

   data->parm->getNameMangle(out, style);
}

//
// Type_Tuple::getSizeBytes
//
bigsint Type_Tuple::getSizeBytes() const
{
   if(!data->isComplete()) Error_Np("internal error: incomplete");

   return data->sizeBytes;
}

//
// Type_Tuple::getSizePtr
//
bigsint Type_Tuple::getSizePtr() const
{
   if(!data->isComplete()) Error_Np("internal error: incomplete");

   return data->sizePtr;
}

//
// Type_Tuple::getSizeWords
//
bigsint Type_Tuple::getSizeWords() const
{
   if(!data->isComplete()) Error_Np("internal error: incomplete");

   return data->sizeWords;
}

//
// Type_Tuple::saveObject
//
ObjectSave &Type_Tuple::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_tuple << data->parm);
}

//
// Type_Tuple::Get
//
auto Type_Tuple::Get(Parm const *parm) -> TuplCR
{
   if(parm->varia) Error_p("internal error: variadic tuple");

   if(Tupl const *type = TplHead) do
   {
      if(type->data->parm == parm)
         return static_cast<TuplCR>(type);

      type = type->tplNext;
   }
   while(type != TplHead);

   return static_cast<TuplCR>(new Type_Tuple(parm));
}

//
// Type_Tuple::Get
//
auto Type_Tuple::Get(TypeCR const *types, std::size_t typeC) -> TuplCR
{
   return Get(Parm::Get(types, typeC, false));
}

// EOF

