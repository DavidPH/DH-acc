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
// Structure and union type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Class_H__
#define Type__Class_H__

#include "Type.hpp"

#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class Type_MemberFunction;
class Type_MemberPointer;

enum AccessControl
{
   ACCESS_Public,
   ACCESS_Protected,
   ACCESS_Private,
   ACCESS_Internal,
};

//
// Type_Class
//
class Type_Class final : public Type
{
   CounterPreamble(Type_Class, Type);

protected:
   typedef AccessControl Access;

   typedef Type_Class Clas;
   typedef ConstReference ClasCR;

public:
   //
   // BaseClass
   //
   struct BaseClass
   {
      BaseClass(Clas const *base, Access cont, bool virt);

      ClasCR const base;
      Access const cont;
      bool   const virt : 1;
   };

   //
   // DataMember
   //
   struct DataMember
   {
      DataMember(bigsint offs, Type const *type, Keyword name, Access cont, bool muta);

      bigsint const offs;
      TypeCR  const type;
      Keyword const name;
      Access  const cont;
      bool    const muta : 1;
   };

   //
   // FuncMember
   //
   struct FuncMember
   {
      FuncMember(bigsint offs, MFun const *type, Keyword name, Access cont, bool pure);

      bigsint const offs;
      MFunCR  const type;
      Keyword const name;
      Access  const cont;
      bool    const pure : 1;
   };

protected:
   typedef BaseClass const *BaseCP;
   typedef DataMember const &DMemCR;
   typedef FuncMember const &FMemCR;

public:
   // Type augmentation.
   void addBaseClass(Clas const *base, Access cont, bool virt);
   void addComplete();
   void addDataMember(bigsint offs, Type const *type, Keyword name, Access cont, bool muta);
   void addFuncMember(bigsint offs, MFun const *type, Keyword name, Access cont, bool pure);
   void addNameTypedef(ContextKey name);

   // Type creation.
   TypeCR getMemberFunctionType(Type const *retn,       // Type/MemFunc.cpp
      Parm const *parm, CallCon conv) const;
   TypeCR getMemberPointerType(Type const *base) const; // Type/MemPtr.cpp

   // Type information.
   virtual bigsint getAlignment() const;
           BaseCP  getBaseClassBegin() const;
           bigsint getBaseClassCount() const;
           BaseCP  getBaseClassEnd() const;
           DMemCR  getDataMember(Keyword name) const;
           FMemCR  getFuncMember(Keyword name) const;
   virtual Keyword getName() const {return data->name.getKeyword();}
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;

   virtual bool isComplete() const {return data->complete;}
           bool isDataMember(Keyword name) const;
           int  isFuncMember(Keyword name) const;

   // Type classification.
   virtual bool isAggregateType() const {return data->structure;}
   virtual bool isObjectType() const    {return true;}
   virtual bool isStructureType() const {return data->structure;}
   virtual bool isUnionType() const     {return !data->structure;}


   friend class Type_MemberFunction;
   friend class Type_MemberPointer;

   static Reference CreateStruct(ContextKey name);
   static Reference CreateUnion(ContextKey name);

protected:
   //
   // ClassData
   //
   struct ClassData
   {
      ClassData(ContextKey name, bool structure);

      std::vector<BaseClass> baseClass;

      std::vector<DataMember> dataMember;
      std::vector<FuncMember> funcMember;

      bigsint sizeBytes, sizePtr, sizeWords;

      Type_MemberFunction *mfnType;
      Type_MemberPointer *ptmType;

      ContextKey const name;
      ContextKey nameTypedef;

      bool       complete  : 1;
      bool const structure : 1;
   };


   Type_Class(ContextKey name, bool structure);
   virtual ~Type_Class();

   ClassData *const data;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
   virtual void getNameMangleName(std::ostream &out, NameMangleStyle mangle) const;
};

#endif//Type__Class_H__

