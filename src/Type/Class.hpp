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

#include <set>
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

public:
   //
   // BaseClass
   //
   struct BaseClass
   {
      explicit BaseClass(ObjectLoad &load);
      BaseClass(Clas const *base, Access cont, bool virt);

      bool operator == (BaseClass const &m) const;

      ClasCR const base;
      Access const cont;
      bool   const virt : 1;

      friend ObjectSave &operator << (ObjectSave &save, BaseClass const &data);

      friend ObjectLoad &operator >> (ObjectLoad &load, std::vector<BaseClass> &data);
   };

   //
   // DataMember
   //
   struct DataMember
   {
      explicit DataMember(ObjectLoad &load);
      DataMember(bigsint offs, Type const *type, Keyword name, Access cont, bool muta);

      bool operator == (DataMember const &m) const;

      bigsint const offs;
      TypeCR  const type;
      Keyword const name;
      Access  const cont;
      bool    const muta : 1;

      friend ObjectSave &operator << (ObjectSave &save, DataMember const &data);

      friend ObjectLoad &operator >> (ObjectLoad &load, std::vector<DataMember> &data);
   };

   //
   // FuncMember
   //
   struct FuncMember
   {
      explicit FuncMember(ObjectLoad &load);
      FuncMember(bigsint offs, MFun const *type, Keyword name, Access cont, bool pure);

      bool operator == (FuncMember const &m) const;

      bigsint const offs;
      MFunCR  const type;
      Keyword const name;
      Access  const cont;
      bool    const pure : 1;

      friend ObjectSave &operator << (ObjectSave &save, FuncMember const &data);

      friend ObjectLoad &operator >> (ObjectLoad &load, std::vector<FuncMember> &data);
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

   static Pointer Find(ContextKey name);

   static ClasCR Get(ContextKey name);

   static ObjectLoad &Load(ObjectLoad &load);
   static void LoadFinish();

   static ClasCR LoadType(ObjectLoad &load);

   static ObjectSave &Save(ObjectSave &save);

protected:
   //
   // ClassData
   //
   struct ClassData
   {
      ClassData(Clas *type, ContextKey name, bool structure);
      ~ClassData();

      std::vector<BaseClass> baseClass;

      std::vector<DataMember> dataMember;
      std::vector<FuncMember> funcMember;

      bigsint sizeBytes, sizePtr, sizeWords;

      Type_MemberFunction *mfnType;
      Type_MemberPointer *ptmType;

      Clas *type;

      ContextKey const name;
      ContextKey nameTypedef;

      bool       complete  : 1;
      bool const structure : 1;


      friend ObjectSave &operator << (ObjectSave &save, ClassData const &data);

      static std::set<ClassData *> ClassDataSet;
   };


   Type_Class(ContextKey name, bool structure);
   virtual ~Type_Class();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   ClassData *const data;


   friend ObjectSave &operator << (ObjectSave &save, ClassData const &data);

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
   virtual void getNameMangleName(std::ostream &out, NameMangleStyle mangle) const;


   static std::set<ClasCR> LoadSet;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

ObjectSave &operator << (ObjectSave &save, AccessControl const &data);

ObjectLoad &operator >> (ObjectLoad &load, AccessControl &data);

#endif//Type__Class_H__

