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

#include <istream>
#include <ostream>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class SourceContext;
class SourcePosition;
class VariableType;

//
// ObjectData_Array
//
struct ObjectData_Array
{
   typedef void (*IterFunc)(std::ostream *, ObjectData_Array const &);


   std::vector<int> strings;
   std::string name;
   bigsint number;
   bigsint size;
   CounterPointer<ObjectExpression> init;
   bool externDef;
   bool externVis;


   static void add_map
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add_map
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void add_world
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add_world
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void add_global
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add_global
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void generate_symbols();

   static bool ini_map(std::string const &name, ObjectExpression *ini);

   static void iterate_map(IterFunc iterFunc, std::ostream *out);
   static void iterate_world(IterFunc iterFunc, std::ostream *out);
   static void iterate_global(IterFunc iterFunc, std::ostream *out);

   static void read_objects(std::istream *in);

   static void write_objects(std::ostream *out);
};

//
// ObjectData_Auto
//
struct ObjectData_Auto
{
   typedef void (*IterFunc)(std::ostream *, ObjectData_Auto const &);


   std::string name;
   bigsint number;
   bigsint size;
   CounterPointer<ObjectExpression> init;
   bool externDef;
   bool externVis;


   static void add
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void generate_symbols();

   static void iterate(IterFunc iterFunc, std::ostream *out);

   static void read_objects(std::istream *in);

   static void write_objects(std::ostream *out);
};

//
// ObjectData_Function
//
struct ObjectData_Function
{
   typedef void (*IterFunc)(std::ostream *, ObjectData_Function const &);


   std::string label;
   std::string name;
   bigsint argCount;
   bigsint number;
   bigsint retCount;
   bigsint varCount;
   CounterPointer<SourceContext> context;
   bool externDef;


   static bool add
   (std::string const &name, std::string const &label, bigsint argCount,
    bigsint retCount, SourceContext *context);

   static void generate_symbols();

   static void iterate(IterFunc iterFunc, std::ostream *out);

   static void read_objects(std::istream *in);

   static void write_objects(std::ostream *out);
};

//
// ObjectData_Register
//
struct ObjectData_Register
{
   typedef void (*IterFunc)(std::ostream *, ObjectData_Register const &);


   std::vector<int> strings;
   std::string name;
   bigsint number;
   bigsint size;
   CounterPointer<ObjectExpression> init;
   bool externDef;
   bool externVis;


   static void add
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void add_map
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add_map
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void add_world
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add_world
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void add_global
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add_global
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void generate_symbols();

   static void ini_map(std::string const &name, ObjectExpression *ini);

   static void iterate(IterFunc iterFunc, std::ostream *out);
   static void iterate_map(IterFunc iterFunc, std::ostream *out);
   static void iterate_world(IterFunc iterFunc, std::ostream *out);
   static void iterate_global(IterFunc iterFunc, std::ostream *out);

   static void read_objects(std::istream *in);

   static void write_objects(std::ostream *out);
};

//
// ObjectData_Script
//
struct ObjectData_Script
{
   typedef void (*IterFunc)(std::ostream *, ObjectData_Script const &);

   enum ScriptFlag
   {
      SF_NET        = 1,
      SF_CLIENTSIDE = 2
   };

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


   std::string label;
   std::string name;
   ScriptType stype;
   bigsint argCount;
   bigsint flags;
   bigsint number;
   bigsint varCount;
   CounterPointer<SourceContext> context;
   bool externDef;
   bool externVis;


   static bool add
   (std::string const &name, std::string const &label, ScriptType stype,
    bigsint flags, bigsint argCount, SourceContext *context, bool externVis);

   static bool add
   (std::string const &name, std::string const &label, ScriptType stype,
    bigsint flags, bigsint argCount, SourceContext *context, bool externVis,
    bigsint number);

   static void generate_symbols();

   static ScriptFlag get_flag
   (std::string const &data, SourcePosition const &position);

   static ScriptType get_type
   (std::string const &data, SourcePosition const &position);

   static void iterate(IterFunc iterFunc, std::ostream *out);

   static void read_objects(std::istream *in);

   static void write_objects(std::ostream *out);
};

//
// ObjectData_Static
//
struct ObjectData_Static
{
   typedef void (*IterFunc)(std::ostream *, ObjectData_Static const &);


   std::string name;
   bigsint number;
   bigsint size;
   CounterPointer<ObjectExpression> init;
   bool externDef;
   bool externVis;


   static void add
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis);

   static void add
   (std::string const &name, VariableType const *type, bool externDef,
    bool externVis, bigsint number);

   static void generate_symbols();

   static void ini(std::string const &name, ObjectExpression *ini);

   static void iterate(IterFunc iterFunc, std::ostream *out);

   static void read_objects(std::istream *in);

   static void write_objects(std::ostream *out);
};

//
// ObjectData_String
//
struct ObjectData_String
{
   typedef void (*IterFunc)(std::ostream *, ObjectData_String const &);


   std::vector<std::string> names;
   std::string string;


   // Returns the new string's name.
   static std::string const &add(std::string const &string);

   static void generate_symbols();

   static void iterate(IterFunc iterFunc, std::ostream *out);

   static void read_objects(std::istream *in);

   static void write_objects(std::ostream *out);
};


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

extern bool option_named_scripts;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

extern void override_object(ObjectData_Array    *out, ObjectData_Array    const *in);
extern void override_object(ObjectData_Auto     *out, ObjectData_Auto     const *in);
extern void override_object(ObjectData_Function *out, ObjectData_Function const *in);
extern void override_object(ObjectData_Register *out, ObjectData_Register const *in);
extern void override_object(ObjectData_Script   *out, ObjectData_Script   const *in);
extern void override_object(ObjectData_Static   *out, ObjectData_Static   const *in);
extern void override_object(ObjectData_String   *out, ObjectData_String   const *in);

extern void read_object(std::istream *in, ObjectData_Array    *out);
extern void read_object(std::istream *in, ObjectData_Auto     *out);
extern void read_object(std::istream *in, ObjectData_Function *out);
extern void read_object(std::istream *in, ObjectData_Register *out);
extern void read_object(std::istream *in, ObjectData_Script   *out);
extern void read_object(std::istream *in, ObjectData_Static   *out);
extern void read_object(std::istream *in, ObjectData_String   *out);
extern void read_object(std::istream *in, ObjectData_Script::ScriptType *out);

extern void write_object(std::ostream *out, ObjectData_Array    const *in);
extern void write_object(std::ostream *out, ObjectData_Auto     const *in);
extern void write_object(std::ostream *out, ObjectData_Function const *in);
extern void write_object(std::ostream *out, ObjectData_Register const *in);
extern void write_object(std::ostream *out, ObjectData_Script   const *in);
extern void write_object(std::ostream *out, ObjectData_Static   const *in);
extern void write_object(std::ostream *out, ObjectData_String   const *in);
extern void write_object(std::ostream *out, ObjectData_Script::ScriptType const *in);

#endif//HPP_ObjectData_

