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

#ifndef Type__Function_H__
#define Type__Function_H__

#include "Type.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class Type_MemberFunction;
class Type_StaticFunction;

//
// ParameterSet
//
class ParameterSet : public PlainCounter
{
   CounterPreambleNoVirtual(ParameterSet, PlainCounter);

protected:
   typedef ParameterSet Parm;
   typedef ConstPointer   ParmCP;
   typedef ConstReference ParmCR;

   typedef Type::ConstReference TypeCR;

public:
   TypeCR const *begin() const {return types;}
   TypeCR const *end() const {return types + typeC;}

   void getNameMangle(std::ostream &out, NameMangleStyle mangle) const;

   ObjectSave &saveObject(ObjectSave &save) const;

   TypeCR const *const types;
   std::size_t   const typeC;
   bool          const varia : 1;


   friend ObjectSave &operator << (ObjectSave &load, ParmCR const &data);

   friend ObjectLoad &operator >> (ObjectLoad &load, ParmCP &data);
   friend ObjectLoad &operator >> (ObjectLoad &load, ParmCR &data);

   static ParmCR Get();
   static ParmCR Get(TypeCR const *types, std::size_t typeC, bool varia);

   static ParmCR LoadParm(ObjectLoad &load);

private:
   explicit ParameterSet(bool varia);
   ParameterSet(ParameterSet const &parm) = delete;
   ParameterSet(Parm *parm, TypeCR const *types, std::size_t typeC, bool varia);
   ~ParameterSet();

   ParameterSet *next, *prev;


   static ParameterSet Head;
   static ParameterSet HeadV;
};

//
// Type_Function
//
class Type_Function : public Type
{
   CounterPreambleAbstract(Type_Function, Type);

public:
   // Type information.
   virtual bigsint getAlignment() const;
   virtual TypeCR  getBase() const {return retn;}
   virtual CallCon getCallConv() const {return conv;}
   virtual bigsint getCallWords() const = 0;
           ParmCR  getParameterSet() const {return parm;}
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

           bool isVariadic() const {return parm->varia;}

   // Type classification.
   virtual bool isFunctionType() const {return true;}

protected:
   Type_Function(Type const *retn, Parm const *parm, CallCon conv);

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   TypeCR const retn;
   ParmCR const parm;

   CallCon const conv;
};

//
// Type_StaticFunction
//
class Type_StaticFunction final : public Type_Function
{
   CounterPreamble(Type_StaticFunction, Type_Function);

public:
   // Type information.
   virtual bigsint getCallWords() const;

   // Type classification.
   virtual bool isStaticFunctionType() const {return true;}


   friend TypeCR Type::getFunctionType(Parm const *parm, CallCon conv) const;

protected:
   Type_StaticFunction(Type_StaticFunction const &type);
   Type_StaticFunction(Type const *retn, Parm const *parm, CallCon conv);
   virtual ~Type_StaticFunction();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   mutable Type_StaticFunction const *sfnNext, *sfnPrev;
};

#endif//Type__Function_H__

