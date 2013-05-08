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

#include "Class.hpp"

#include "MemFunc.hpp"

#include "ObjectArchive.hpp"
#include "ost_type.hpp"
#include "SourceException.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

std::set<Type_Class::ClasCR> Type_Class::LoadSet;

std::set<Type_Class::ClassData *> Type_Class::ClassData::ClassDataSet;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// Type_Class::BaseClass::BaseClass
//
Type_Class::BaseClass::BaseClass(ObjectLoad &load) : base{LoadType(load)},
   cont{load.loadBits(cont)}, virt{load.loadBits(virt)}
{
}

//
// Type_Class::BaseClass::BaseClass
//
Type_Class::BaseClass::BaseClass(Clas const *base_, Access cont_, bool virt_) :
   base{base_}, cont{cont_}, virt{virt_}
{
}

//
// Type_Class::BaseClass::operator == Type_Class::BaseClass
//
bool Type_Class::BaseClass::operator == (BaseClass const &m) const
{
   return m.base == base && m.cont == cont && m.virt == virt;
}

//
// Type_Class::DataMember::DataMember
//
Type_Class::DataMember::DataMember(ObjectLoad &load) : offs{load.loadBits(offs)},
   type{Type::LoadType(load)}, name{load.loadBits(name)}, cont{load.loadBits(cont)},
   muta{load.loadBits(muta)}
{
}

//
// Type_Class::DataMember::DataMember
//
Type_Class::DataMember::DataMember(bigsint offs_, Type const *type_, Keyword name_,
   Access cont_, bool muta_) : offs{offs_}, type{type_}, name{name_}, cont{cont_},
   muta{muta_}
{
}

//
// Type_Class::DataMember::operator == Type_Class::DataMember
//
bool Type_Class::DataMember::operator == (DataMember const &m) const
{
   return m.offs == offs && m.type == type && m.name == name && m.cont == cont && m.muta == muta;
}

//
// Type_Class::FuncMember::FuncMember
//
Type_Class::FuncMember::FuncMember(ObjectLoad &load) : offs{load.loadBits(offs)},
   type{MFun::LoadType(load)}, name{load.loadBits(name)}, cont{load.loadBits(cont)},
   pure{load.loadBits(pure)}
{
}

//
// Type_Class::FuncMember::FuncMember
//
Type_Class::FuncMember::FuncMember(bigsint offs_, MFun const *type_, Keyword name_,
   Access cont_, bool pure_) : offs{offs_}, type{type_}, name{name_}, cont{cont_},
   pure{pure_}
{
}

//
// Type_Class::FuncMember::operator == Type_Class::FuncMember
//
bool Type_Class::FuncMember::operator == (FuncMember const &m) const
{
   return m.offs == offs && m.type == type && m.name == name && m.cont == cont && m.pure == pure;
}

//
// Type_Class::ClassData::ClassData
//
Type_Class::ClassData::ClassData(Clas *type_, ContextKey name_, bool structure_) :
   sizeBytes{0}, sizePtr{0}, sizeWords{0}, mfnType{nullptr}, ptmType{nullptr},
   type{type_}, name{name_}, complete{false}, structure{structure_}
{
   ClassDataSet.insert(this);
}

//
// Type_Class::ClassData::~ClassData
//
Type_Class::ClassData::~ClassData()
{
   ClassDataSet.erase(this);
}

//
// Type_Class::Type_Class
//
Type_Class::Type_Class(ContextKey name, bool structure) : data{new ClassData(this, name, structure)}
{
}

//
// Type_Class::~Type_Class
//
Type_Class::~Type_Class()
{
   if(!isQual()) delete data;
}

//
// Type_Class::addBaseClass
//
void Type_Class::addBaseClass(Clas const *base, Access cont, bool virt)
{
   if(data->complete) Error_Np("internal error: complete");

   data->baseClass.emplace_back(base, cont, virt);

   data->sizeBytes += base->getSizeBytes();
}

//
// Type_Class::addComplete
//
void Type_Class::addComplete()
{
   if(data->complete) Error_Np("internal error: complete");

   if(!data->sizeBytes) data->sizeBytes = 1;

   if(Target == TARGET_MageCraft)
   {
      data->sizePtr = data->sizeWords = (data->sizeBytes + 3) / 4;
      data->sizeBytes = data->sizeWords * 4;
   }
   else
   {
      data->sizePtr = data->sizeWords = data->sizeBytes;
   }

   data->complete = true;
}

//
// Type_Class::addDataMember
//
void Type_Class::addDataMember(bigsint offs, Type const *type, Keyword name,
   Access cont, bool muta)
{
   if(data->complete) Error_Np("internal error: complete");

   data->dataMember.emplace_back(offs, type, name, cont, muta);

   if(type->isComplete())
   {
      bigsint size = offs + type->getSizeBytes();

      if(data->sizeBytes < size)
         data->sizeBytes = size;
   }
}

//
// Type_Class::addFuncMember
//
void Type_Class::addFuncMember(bigsint offs, MFun const *type, Keyword name,
   Access cont, bool pure)
{
   if(data->complete) Error_Np("internal error: complete");

   data->funcMember.emplace_back(offs, type, name, cont, pure);
}

//
// Type_Class::addNameTypedef
//
void Type_Class::addNameTypedef(ContextKey name)
{
   if(!data->nameTypedef) data->nameTypedef = name;
}

//
// Type_Class::getAlignment
//
bigsint Type_Class::getAlignment() const
{
   if(Target == TARGET_MageCraft) return 4;

   return 1;
}

//
// Type_Class::getBaseClassBegin
//
auto Type_Class::getBaseClassBegin() const -> BaseCP
{
   if(data->baseClass.empty())
      return nullptr;
   else
      return &*data->baseClass.begin();
}

//
// Type_Class::getBaseClassCount
//
bigsint Type_Class::getBaseClassCount() const
{
   return data->baseClass.size();
}

//
// Type_Class::getBaseClassEnd
//
auto Type_Class::getBaseClassEnd() const -> BaseCP
{
   if(data->baseClass.empty())
      return nullptr;
   else
      return &*data->baseClass.end();
}

//
// Type_Class::getDataMember
//
auto Type_Class::getDataMember(Keyword name) const -> DMemCR
{
   for(auto const &mem : data->dataMember)
      if(mem.name == name) return mem;

   Error_Np("internal error: not data-member");
}

//
// Type_Class::getFuncMember
//
auto Type_Class::getFuncMember(Keyword name) const -> FMemCR
{
   for(auto const &mem : data->funcMember)
      if(mem.name == name) return mem;

   Error_Np("internal error: not func-member");
}

//
// Type_Class::getNameMangleBase
//
void Type_Class::getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const
{
   out << (data->structure ? 'S' : 'U');
   getNameMangleName(out, mangle);
}

//
// Type_Class::getNameMangleName
//
void Type_Class::getNameMangleName(std::ostream &out, NameMangleStyle mangle) const
{
   if(!data->name.getKeyword() && data->nameTypedef)
      GetNameMangle(data->nameTypedef, out, mangle);
   else
      GetNameMangle(data->name, out, mangle);
}

//
// Type_Class::getSizeBytes
//
bigsint Type_Class::getSizeBytes() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->sizeBytes;
}

//
// Type_Class::getSizePtr
//
bigsint Type_Class::getSizePtr() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->sizePtr;
}

//
// Type_Class::getSizeWords
//
bigsint Type_Class::getSizeWords() const
{
   if(!data->complete) Error_Np("internal error: incomplete");

   return data->sizeWords;
}

//
// Type_Class::isDataMember
//
bool Type_Class::isDataMember(Keyword name) const
{
   for(auto const &mem : data->dataMember)
      if(mem.name == name) return true;

   return false;
}

//
// Type_Class::isFuncMember
//
int Type_Class::isFuncMember(Keyword name) const
{
   int n = 0;

   for(auto const &mem : data->funcMember)
      if(mem.name == name) ++n;

   return n;
}

//
// Type_Class::saveObject
//
ObjectSave &Type_Class::saveObject(ObjectSave &save) const
{
   return Super::saveObject(save << KWRD_class << data->name);
}

//
// Type_Class::CreateStruct
//
auto Type_Class::CreateStruct(ContextKey name) -> Reference
{
   return static_cast<Reference>(new Type_Class(name, true));
}

//
// Type_Class::CreateUnion
//
auto Type_Class::CreateUnion(ContextKey name) -> Reference
{
   return static_cast<Reference>(new Type_Class(name, false));
}

//
// Type_Class::Find
//
auto Type_Class::Find(ContextKey name) -> Pointer
{
   for(auto const &data : ClassData::ClassDataSet)
      if(data->name == name) return data->type;

   return nullptr;
}

//
// Type_Class::Get
//
auto Type_Class::Get(ContextKey name) -> ClasCR
{
   if(auto type = Find(name))
      return static_cast<ClasCR>(type);

   Error_p("internal error");
}

//
// Type_Class::Load
//
ObjectLoad &Type_Class::Load(ObjectLoad &load)
{
   std::set<ClassData *>::size_type size;

   load >> size;

   while(size--)
   {
      std::vector<BaseClass> baseClass;
      std::vector<DataMember> dataMember;
      std::vector<FuncMember> funcMember;
      bigsint sizeBytes, sizePtr, sizeWords;
      ContextKey name, nameTypedef;
      bool complete, structure;

      load >> name >> structure
         >> baseClass >> dataMember >> funcMember
         >> sizeBytes >> sizePtr >> sizeWords >> nameTypedef >> complete;

      auto type = Find(name);

      if(type)
      {
         if(!complete) continue;

         if(type->data->complete)
         {
            if(type->data->baseClass != baseClass || type->data->dataMember != dataMember ||
               type->data->funcMember != funcMember || type->data->sizeBytes != sizeBytes ||
               type->data->sizePtr != sizePtr || type->data->sizeWords != sizeWords ||
               type->data->nameTypedef != type->data->nameTypedef)
            {
               Error_p("conflicting class definitions");
            }

            continue;
         }
      }
      else
      {
         type = structure ? CreateStruct(name) : CreateUnion(name);

         LoadSet.insert(static_cast<ClasCR>(type));
      }

      type->data->baseClass .swap(baseClass);
      type->data->dataMember.swap(dataMember);
      type->data->funcMember.swap(funcMember);
      type->data->sizeBytes   = sizeBytes;
      type->data->sizePtr     = sizePtr;
      type->data->sizeWords   = sizeWords;
      type->data->nameTypedef = nameTypedef;
      type->data->complete    = complete;
   }

   return load;
}

//
// Type_Class::LoadFinish
//
void Type_Class::LoadFinish()
{
   LoadSet.clear();
}

//
// Type_Class::LoadType
//
auto Type_Class::LoadType(ObjectLoad &load) -> ClasCR
{
   TypeCR baseType = Type::LoadType(load);

   if(Clas const *realType = dynamic_cast<Clas const *>(&*baseType))
      return static_cast<ClasCR>(realType);

   Error_p("internal error: expected Type_Class");
}

//
// Type_Class::Save
//
ObjectSave &Type_Class::Save(ObjectSave &save)
{
   save << ClassData::ClassDataSet.size();

   for(auto const &data : ClassData::ClassDataSet)
      save << *data;

   return save;
}

//
// operator ObjectSave << AccessControl
//
ObjectSave &operator << (ObjectSave &save, AccessControl const &data)
{
   return save.saveEnum(data);
}

//
// operator ObjectSave << Type::ClasCR
//
ObjectSave &operator << (ObjectSave &save, Type::ClasCR const &data)
{
   return save << static_cast<Type::TypeCR>(data);
}

//
// operator ObjectSave << Type_Class::BaseClass
//
ObjectSave &operator << (ObjectSave &save, Type_Class::BaseClass const &data)
{
   return save << data.base << data.cont << data.virt;
}

//
// operator ObjectSave << Type_Class::ClassData
//
ObjectSave &operator << (ObjectSave &save, Type_Class::ClassData const &data)
{
   return save << data.name << data.structure
      << data.baseClass << data.dataMember << data.funcMember << data.sizeBytes
      << data.sizePtr << data.sizeWords << data.nameTypedef << data.complete;
}

//
// operator ObjectSave << Type_Class::DataMember
//
ObjectSave &operator << (ObjectSave &save, Type_Class::DataMember const &data)
{
   return save << data.offs << data.type << data.name << data.cont << data.muta;
}

//
// operator ObjectSave << Type_Class::FuncMember
//
ObjectSave &operator << (ObjectSave &save, Type_Class::FuncMember const &data)
{
   return save << data.offs << data.type << data.name << data.cont << data.pure;
}

//
// operator ObjectLoad >> AccessControl
//
ObjectLoad &operator >> (ObjectLoad &load, AccessControl &data)
{
   return load.loadEnum(data, ACCESS_Internal);
}

//
// operator ObjectLoad >> std::vector<Type_Class::BaseClass>
//
ObjectLoad &operator >> (ObjectLoad &load, std::vector<Type_Class::BaseClass> &data)
{
   return OA_LoadEmplace(load, data);
}

//
// operator ObjectLoad >> std::vector<Type_Class::DataMember>
//
ObjectLoad &operator >> (ObjectLoad &load, std::vector<Type_Class::DataMember> &data)
{
   return OA_LoadEmplace(load, data);
}

//
// operator ObjectLoad >> std::vector<Type_Class::FuncMember>
//
ObjectLoad &operator >> (ObjectLoad &load, std::vector<Type_Class::FuncMember> &data)
{
   return OA_LoadEmplace(load, data);
}

// EOF

