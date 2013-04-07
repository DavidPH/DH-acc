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
// Function type handling.
//
//-----------------------------------------------------------------------------

#include "Function.hpp"

#include "SourceException.hpp"

#include "Type/Void.hpp"

#include <algorithm>


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

ParameterSet ParameterSet::Head{false};
ParameterSet ParameterSet::HeadV{true};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ParameterSet::ParameterSet
//
ParameterSet::ParameterSet(bool varia_) : types{nullptr}, typeC{0}, varia{varia_},
   next{this}, prev{this}
{
   ++refCount;
}

//
// ParameterSet::ParameterSet
//
ParameterSet::ParameterSet(Parm *head, TypeCR const *types_, std::size_t typeC_,
   bool varia_) : types{types_}, typeC{typeC_}, varia{varia_}, next{head}, prev{head->prev}
{
   prev->next = this;
   next->prev = this;
}

//
// ParameterSet::~ParameterSet
//
ParameterSet::~ParameterSet()
{
   for(auto &type : *this) type.~TypeCR();
   operator delete(const_cast<TypeCR *>(types));

   prev->next = next;
   next->prev = prev;
}

//
// ParameterSet::getNameMangle
//
void ParameterSet::getNameMangle(std::ostream &out, NameMangleStyle mangle) const
{
   if(typeC)
   {
      out << '(';

      TypeCR const *type = begin(), *tend = end();

      (*type)->getNameMangle(out, mangle);

      while(++type != tend) (*type)->getNameMangle(out << ',', mangle);

      if(varia) out << ",...";

      out << ')';
   }
   else if(varia)
      out << "(...)";
   else
      out << "()";
}

//
// ParameterSet::Get
//
auto ParameterSet::Get() -> ParmCR
{
   return static_cast<ParmCR>(&Head);
}

//
// ParameterSet::Get
//
auto ParameterSet::Get(TypeCR const *types, std::size_t typeC, bool varia) -> ParmCR
{
   Parm *head = varia ? &HeadV : &Head;

   if(!typeC) return static_cast<ParmCR>(head);

   for(ParameterSet *parm = head->next; parm != head; parm = parm->next)
   {
      if(parm->typeC == typeC && std::equal(parm->begin(), parm->end(), types))
         return static_cast<ParmCR>(parm);
   }

   TypeCR *typesNew = static_cast<TypeCR *>(operator new(typeC * sizeof(TypeCR)));
   std::uninitialized_copy_n(types, typeC, typesNew);
   return static_cast<ParmCR>(new ParameterSet(head, typesNew, typeC, varia));
}

//
// Type::getFunctionType
//
auto Type::getFunctionType(Parm const *parm, CallCon conv) const -> TypeCR
{
   // Search for existing function type.
   if(SFun const *type = sfnType) do
   {
      if(type->parm == parm && type->conv == conv)
         return static_cast<TypeCR>(type);

      type = type->sfnNext;
   }
   while(type != sfnType);

   // Otherwise, create one and return it.
   return static_cast<TypeCR>(new SFun(this, parm, conv));
}

//
// Type_Function::Type_Function
//
Type_Function::Type_Function(Type const *retn_, Parm const *parm_, CallCon conv_) :
   retn{retn_}, parm{parm_}, conv{conv_}
{
}

//
// Type_Function::getAlignment
//
bigsint Type_Function::getAlignment() const
{
   Error_Np("internal error: function");
}

//
// Type_Function::getSizeBytes
//
bigsint Type_Function::getSizeBytes() const
{
   Error_Np("internal error: function");
}

//
// Type_Function::getSizePtr
//
bigsint Type_Function::getSizePtr() const
{
   Error_Np("internal error: function");
}

//
// Type_Function::getSizeWords
//
bigsint Type_Function::getSizeWords() const
{
   Error_Np("internal error: function");
}

//
// Type_StaticFunction::Type_StaticFunction
//
Type_StaticFunction::Type_StaticFunction(Type_StaticFunction const &type) :
   Super{type}, sfnNext{this}, sfnPrev{this}
{
}

//
// Type_StaticFunction::Type_StaticFunction
//
Type_StaticFunction::Type_StaticFunction(Type const *retn_, Parm const *parm_,
   CallCon conv_) : Super{retn_, parm_, conv_}
{
   if(retn->sfnType)
   {
      sfnNext = retn->sfnType;
      sfnPrev = retn->sfnType->sfnPrev;

      sfnNext->sfnPrev = this;
      sfnPrev->sfnNext = this;
   }
   else
   {
      retn->sfnType = this;

      sfnNext = this;
      sfnPrev = this;
   }
}

//
// Type_StaticFunction::~Type_StaticFunction
//
Type_StaticFunction::~Type_StaticFunction()
{
   if(retn->sfnType == this)
   {
      if(sfnNext == this)
      {
         retn->sfnType = nullptr;
      }
      else
      {
         retn->sfnType = sfnNext;

         sfnNext->sfnPrev = sfnPrev;
         sfnPrev->sfnNext = sfnNext;
      }
   }
   else
   {
      sfnNext->sfnPrev = sfnPrev;
      sfnPrev->sfnNext = sfnNext;
   }
}

//
// Type_StaticFunction::getCallWords
//
bigsint Type_StaticFunction::getCallWords() const
{
   bigsint size = 0;

   for(auto const &type : *parm)
      size += type->getSizeWords();

   return size;
}

//
// Type_StaticFunction::getNameMangleBase
//
void Type_StaticFunction::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << 'F';

   GetNameMangle(conv, out, mangle);

   parm->getNameMangle(out, mangle);

   GetNameMangle(retn, out, mangle);
}

// EOF

