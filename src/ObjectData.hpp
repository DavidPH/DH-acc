//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
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

#include <istream>
#include <ostream>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
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

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
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

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
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
   CounterPointer<ObjectExpression> init;
   bool externDef;
   bool externVis;


   static void Add(std::string const &name, VariableType const *type,
      bool externDef, bool externVis, bigsint number);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
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
   bool externDef;


   static bool Add(std::string const &name, std::string const &label,
      bigsint argCount, bigsint retCount, SourceContext *context);
   static bool Add(std::string const &name, std::string const &label,
      bigsint argCount, bigsint retCount, bigsint varCount);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
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
   bool externDef;
   bool externVis;


   static void Add(std::string const &name, VariableType const *type,
      bool externDef, bool externVis, bigsint number);
   static void AddMap(std::string const &name, VariableType const *type,
      bool externDef, bool externVis, bigsint number = -1);
   static void AddWorld(std::string const &name, VariableType const *type,
      bool externDef, bool externVis, bigsint number = -1);
   static void AddGlobal(std::string const &name, VariableType const *type,
      bool externDef, bool externVis, bigsint number = -1);

   static void GenerateSymbols();

   static bool InitMap(std::string const &name, VariableType const *type,
                       ObjectExpression *init);

   static void Iterate(IterFunc iterFunc, std::ostream *out);
   static void IterateMap(IterFunc iterFunc, std::ostream *out);
   static void IterateWorld(IterFunc iterFunc, std::ostream *out);
   static void IterateGlobal(IterFunc iterFunc, std::ostream *out);

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
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
   bigsint flags;
   bigsint number;
   bigsint varCount;
   CounterPointer<SourceContext> context;
   bool externDef;
   bool externVis;


   static bool Add(std::string const &name, std::string const &label,
      ScriptType stype, bigsint flags, bigsint argCount, SourceContext *context,
      bool externVis, bigsint number, std::string const &string);
   static bool Add(std::string const &name, std::string const &label,
      ScriptType stype, bigsint flags, bigsint argCount, bigsint varCount,
      bool externVis, bigsint number, std::string const &string);

   static void GenerateSymbols();

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
};

//
// ObjectData::Static
//
struct Static
{
   typedef void (*IterFunc)(std::ostream *, Static const &);


   std::string name;
   bigsint number;
   bigsint size;
   CounterPointer<ObjectExpression> init;
   bool externDef;
   bool externVis;


   static void Add(std::string const &name, VariableType const *type,
      bool externDef, bool externVis, bigsint number = -1);

   static void GenerateSymbols();

   static void Init(std::string const &name, ObjectExpression *init);

   static void Iterate(IterFunc iterFunc, std::ostream *out);

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
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

   static void ReadObjects(std::istream *in);

   static void WriteObjects(std::ostream *out);
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

extern void override_object(ObjectData::Array    *out, ObjectData::Array    const *in);
extern void override_object(ObjectData::ArrayVar *out, ObjectData::ArrayVar const *in);
extern void override_object(ObjectData::Auto     *out, ObjectData::Auto     const *in);
extern void override_object(ObjectData::Function *out, ObjectData::Function const *in);
extern void override_object(ObjectData::Register *out, ObjectData::Register const *in);
extern void override_object(ObjectData::Script   *out, ObjectData::Script   const *in);
extern void override_object(ObjectData::Static   *out, ObjectData::Static   const *in);
extern void override_object(ObjectData::String   *out, ObjectData::String   const *in);

extern void read_object(std::istream *in, ObjectData::Array      *out);
extern void read_object(std::istream *in, ObjectData::ArrayVar   *out);
extern void read_object(std::istream *in, ObjectData::Auto       *out);
extern void read_object(std::istream *in, ObjectData::Function   *out);
extern void read_object(std::istream *in, ObjectData::Init       *out);
extern void read_object(std::istream *in, ObjectData::InitType   *out);
extern void read_object(std::istream *in, ObjectData::Register   *out);
extern void read_object(std::istream *in, ObjectData::Script     *out);
extern void read_object(std::istream *in, ObjectData::Static     *out);
extern void read_object(std::istream *in, ObjectData::String     *out);
extern void read_object(std::istream *in, ObjectData::ScriptType *out);

extern void write_object(std::ostream *out, ObjectData::Array      const *in);
extern void write_object(std::ostream *out, ObjectData::ArrayVar   const *in);
extern void write_object(std::ostream *out, ObjectData::Auto       const *in);
extern void write_object(std::ostream *out, ObjectData::Function   const *in);
extern void write_object(std::ostream *out, ObjectData::Init       const *in);
extern void write_object(std::ostream *out, ObjectData::InitType   const *in);
extern void write_object(std::ostream *out, ObjectData::Register   const *in);
extern void write_object(std::ostream *out, ObjectData::Script     const *in);
extern void write_object(std::ostream *out, ObjectData::Static     const *in);
extern void write_object(std::ostream *out, ObjectData::String     const *in);
extern void write_object(std::ostream *out, ObjectData::ScriptType const *in);

//namespace ObjectData { using ::override_object; }

#endif//HPP_ObjectData_

