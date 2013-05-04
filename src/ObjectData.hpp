//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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
// Object-level data.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ObjectData_
#define HPP_ObjectData_

#include "bignum.hpp"
#include "Counter.hpp"
#include "LinkSpec.hpp"

#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class ObjectLoad;
class ObjectSave;
class SourceContext;
class VariableType;

namespace ObjectData
{

typedef CounterPointer<ObjectExpression> InitData;
typedef std::vector<InitData> InitDataVector;

//
// ObjectData::InitType
//
enum InitType
{
   IT_UNKNOWN,
   IT_MIXED,
   IT_INTEGER,
   IT_STRING,
   IT_FUNCTION
};

typedef std::vector<InitType> InitTypeVector;

//
// ObjectData::ScriptFlag
//
enum ScriptFlag
{
   SF_NET        = 1,
   SF_CLIENTSIDE = 2
};

//
// ObjectData::ScriptType
//
enum ScriptType
{
   ST_CLOSED     =  0,
   ST_OPEN       =  1,
   ST_RESPAWN    =  2,
   ST_DEATH      =  3,
   ST_ENTER      =  4,
   ST_LIGHTNING  = 12,
   ST_UNLOADING  = 13,
   ST_DISCONNECT = 14,
   ST_RETURN     = 15,

   ST_NONE
};

//
// ObjectData::Init
//
struct Init
{
   InitDataVector data;
   InitTypeVector type;

   InitType typeAll;
   bool dataAll;
};

//
// ObjectData::Array
//
struct Array
{
   typedef void (*IterFunc)(std::ostream *, Array const &);


   Init init;
   std::string name;
   bigsint number;
   bigsint size;
   LinkageSpecifier linkage;
   bool externDef;


   static void AddMap(std::string const &name, LinkageSpecifier linkage,
                      bool externDef, bigsint number = -1);
   static void AddWorld(std::string const &name, LinkageSpecifier linkage,
                        bool externDef, bigsint number = -1);
   static void AddGlobal(std::string const &name, LinkageSpecifier linkage,
                         bool externDef, bigsint number = -1);

   static void GenerateSymbols();

   static void IterateMap(IterFunc iterFunc, std::ostream *out);
   static void IterateWorld(IterFunc iterFunc, std::ostream *out);
   static void IterateGlobal(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::ArrayVar
//
struct ArrayVar
{
   typedef void (*IterFunc)(std::ostream *, ArrayVar const &);


   Init init;
   std::string array;
   std::string name;
   bigsint number;
   bigsint size;
   LinkageSpecifier linkage;
   bool externDef;


   static void AddMap(std::string const &array, std::string const &name,
      VariableType const *type, LinkageSpecifier linkage, bool externDef,
      bigsint number = -1);
   static void AddWorld(std::string const &array, std::string const &name,
      VariableType const *type, LinkageSpecifier linkage, bool externDef,
      bigsint number = -1);
   static void AddGlobal(std::string const &array, std::string const &name,
      VariableType const *type, LinkageSpecifier linkage, bool externDef,
      bigsint number = -1);

   // Gets the name of the associated Array.
   static std::string const &ArrayMap(std::string const &name);
   static std::string const &ArrayWorld(std::string const &name);
   static std::string const &ArrayGlobal(std::string const &name);

   static void GenerateSymbols();

   static bool InitMap(std::string const &name, VariableType const *type,
                       ObjectExpression *init);

   static void IterateMap(IterFunc iterFunc, std::ostream *out);
   static void IterateWorld(IterFunc iterFunc, std::ostream *out);
   static void IterateGlobal(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::Auto
//
struct Auto
{
   typedef void (*IterFunc)(std::ostream *, Auto const &);


   std::string name;
   bigsint number;
   bigsint size;
   LinkageSpecifier linkage;
   bool externDef;


   static void Add(std::string const &name, VariableType const *type,
      LinkageSpecifier linkage, bool externDef, bigsint number);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::Function
//
struct Function
{
   typedef void (*IterFunc)(std::ostream *, Function const &);


   std::string label;
   std::string name;
   bigsint argCount;
   bigsint number;
   bigsint retCount;
   bigsint varCount;
   CounterPointer<SourceContext> context;
   LinkageSpecifier linkage;
   bool externDef;


   static bool Add(std::string const &name, std::string const &label,
      bigsint argCount, bigsint retCount, SourceContext *context,
      LinkageSpecifier linkage);
   static bool Add(std::string const &name, std::string const &label,
      bigsint argCount, bigsint retCount, bigsint varCount,
      LinkageSpecifier linkage);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::Label
//
struct Label
{
   typedef void (*IterFunc)(std::ostream *, Label const &);


   std::string label;
   std::string name;


   // Returns the new label's name.
   static std::string const &Add(std::string const &label);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::Register
//
struct Register
{
   typedef void (*IterFunc)(std::ostream *, Register const &);


   Init init;
   std::string name;
   bigsint number;
   bigsint size;
   LinkageSpecifier linkage;
   bool externDef;


   static void Add(std::string const &name, VariableType const *type,
      LinkageSpecifier linkage, bool externDef, bigsint number);
   static void AddMap(std::string const &name, VariableType const *type,
      LinkageSpecifier linkage, bool externDef, bigsint number = -1);
   static void AddWorld(std::string const &name, VariableType const *type,
      LinkageSpecifier linkage, bool externDef, bigsint number = -1);
   static void AddGlobal(std::string const &name, VariableType const *type,
      LinkageSpecifier linkage, bool externDef, bigsint number = -1);

   static void GenerateSymbols();

   static bool InitMap(std::string const &name, VariableType const *type,
                       ObjectExpression *init);

   static void Iterate(IterFunc iterFunc, std::ostream *out);
   static void IterateMap(IterFunc iterFunc, std::ostream *out);
   static void IterateWorld(IterFunc iterFunc, std::ostream *out);
   static void IterateGlobal(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::Script
//
struct Script
{
   typedef void (*IterFunc)(std::ostream *, Script const &);


   std::string label;
   std::string name;
   std::string string;
   ScriptType stype;
   bigsint argCount;
   biguint flags;
   bigsint number;
   bigsint retCount;
   bigsint varCount;
   CounterPointer<SourceContext> context;
   LinkageSpecifier linkage;
   bool externDef;


   static bool Add(std::string const &name, std::string const &label,
      bigsint argCount, bigsint retCount, SourceContext *context,
      LinkageSpecifier linkage, bigsint number, std::string const &string,
      ScriptType stype = ST_CLOSED, biguint flags = 0);
   static bool Add(std::string const &name, std::string const &label,
      bigsint argCount, bigsint retCount, bigsint varCount,
      LinkageSpecifier linkage, bigsint number, std::string const &string,
      ScriptType stype = ST_CLOSED, biguint flags = 0);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::Static
//
struct Static
{
   typedef void (*IterFunc)(std::ostream *, Static const &);


   ObjectData::Init init;
   std::string name;
   bigsint number;
   bigsint size;
   LinkageSpecifier linkage;
   bool externDef;


   static void Add(std::string const &name, VariableType const *type,
      LinkageSpecifier linkage, bool externDef, bigsint number = -1);

   static void GenerateSymbols();

   static bool Init(std::string const &name, VariableType const *type,
                    ObjectExpression *init);

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

//
// ObjectData::String
//
struct String
{
   typedef void (*IterFunc)(std::ostream *, String const &);


   std::vector<std::string> names;
   std::string string;


   // Returns the new string's name.
   static std::string const &Add(std::string const &string);
   static void Add(std::string const &name, std::string const &string);

   static String const *Find(std::string const &symbol);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static ObjectLoad &Load(ObjectLoad &arc);

   static ObjectSave &Save(ObjectSave &arc);
};

}


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_named_scripts;
extern bool Option_UseChunkATAG;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::SetInit
//
void SetInit(Init &init, ObjectExpression *data, VariableType const *type);

}

void OA_Override(ObjectData::Array    &out, ObjectData::Array    const &in);
void OA_Override(ObjectData::ArrayVar &out, ObjectData::ArrayVar const &in);
void OA_Override(ObjectData::Auto     &out, ObjectData::Auto     const &in);
void OA_Override(ObjectData::Function &out, ObjectData::Function const &in);
void OA_Override(ObjectData::Register &out, ObjectData::Register const &in);
void OA_Override(ObjectData::Script   &out, ObjectData::Script   const &in);
void OA_Override(ObjectData::Static   &out, ObjectData::Static   const &in);
void OA_Override(ObjectData::String   &out, ObjectData::String   const &in);

ObjectSave &operator << (ObjectSave &arc, ObjectData::Array      const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::ArrayVar   const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::Auto       const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::Function   const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::Init       const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::InitType   const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::Label      const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::Register   const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::Script     const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::Static     const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::String     const &data);
ObjectSave &operator << (ObjectSave &arc, ObjectData::ScriptType const &data);

ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Array      &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::ArrayVar   &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Auto       &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Function   &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Init       &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::InitType   &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Label      &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Register   &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Script     &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Static     &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::String     &data);
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::ScriptType &data);

#endif//HPP_ObjectData_

