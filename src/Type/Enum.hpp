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
// Enumerated type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__Enum_H__
#define Type__Enum_H__

#include "Type.hpp"

#include <set>
#include <unordered_map>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_Enumerated
//
class Type_Enumerated final : public Type
{
   CounterPreamble(Type_Enumerated, Type);

public:
   // Type augmentation.
   void addComplete(Type const *baseType);
   void addValue(Keyword name, bigsint value);

   // Type information.
   virtual bigsint getAlignment() const;
   virtual TypeCR  getBase() const;
   virtual Keyword getName() const {return data->name.getKeyword();}
   virtual bigsint getSizeBits() const;
   virtual bigsint getSizeBytes() const;
   virtual bigsint getSizePtr() const;
   virtual bigsint getSizeWords() const;
           bigsint getValue(Keyword name) const;
           bigsint getValueMax() const;
           bigsint getValueMin() const;

   virtual bool isComplete() const {return data->complete;}
   virtual bool isSigned() const;
           bool isValue(Keyword name) const;

   // Type classification.
   virtual bool isArithmeticType() const          {return !data->cxx;}
   virtual bool isEnumeratedType() const          {return true;}
   virtual bool isIntegerType() const             {return !data->cxx;}
   virtual bool isObjectType() const              {return true;}
   virtual bool isRealType() const                {return !data->cxx;}
   virtual bool isScalarType() const              {return true;}
   virtual bool isScopedEnumerationType() const   {return data->scoped;}
   virtual bool isUnscopedEnumerationType() const {return data->unscoped;}


   static Reference CreateC(ContextKey name);
   static Reference CreateCXX(ContextKey name, bool scoped);

   static Pointer Find(ContextKey name);

   static EnumCR Get(ContextKey name);

   static ObjectLoad &Load(ObjectLoad &load);
   static void LoadFinish();

   static ObjectSave &Save(ObjectSave &save);

protected:
   //
   // EnumData
   //
   struct EnumData
   {
      EnumData(Enum *type, ContextKey name, bool cxx, bool scoped);
      ~EnumData();

      std::unordered_map<Keyword, bigsint> values;

      ContextKey const name;

      bigsint valueMax;
      bigsint valueMin;

      TypeCR baseType;

      Enum *type;

      bool       complete : 1;
      bool const cxx      : 1;
      bool const scoped   : 1;
      bool const unscoped : 1;


      friend ObjectSave &operator << (ObjectSave &save, EnumData const &data);

      static std::set<EnumData *> EnumDataSet;
   };

   Type_Enumerated(ContextKey name, bool cxx, bool scoped);
   virtual ~Type_Enumerated();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   EnumData *const data;


   friend ObjectSave &operator << (ObjectSave &save, EnumData const &data);

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;
   virtual void getNameMangleName(std::ostream &out, NameMangleStyle mangle) const;


   static std::set<EnumCR> LoadSet;
};

#endif//Type__Enum_H__

