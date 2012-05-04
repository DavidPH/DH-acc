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
// Source-level context handling.
//
//-----------------------------------------------------------------------------

#include "SourceContext.hpp"

#include "ObjectData.hpp"
#include "ObjectExpression.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "SourceVariable.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"

#include <cstring>
#include <sstream>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static ObjectExpression::Pointer obj_0 =
   ObjectExpression::create_value_int(0, SourcePosition::builtin());
static ObjectExpression::Pointer obj_1 =
   ObjectExpression::create_value_int(1, SourcePosition::builtin());

static SourceVariable::Pointer var_false =
   SourceVariable::create_constant("false", VariableType::get_bt_boolhard(),
                                   obj_0, SourcePosition::builtin());
static SourceVariable::Pointer var_true =
   SourceVariable::create_constant("true", VariableType::get_bt_boolhard(),
                                   obj_1, SourcePosition::builtin());

static SourceVariable::Pointer var_nullptr = SourceVariable::create_constant
("nullptr", VariableType::get_bt_nullptr(), obj_0, SourcePosition::builtin());


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceContext::Pointer SourceContext::global_context;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceContext::SourceContext
//
SourceContext::SourceContext() :
   typeReturn(VariableType::get_bt_void()),
   countAuto(0),
   countRegister(0),
   labelCount(0),
   limitAuto(0),
   limitRegister(0),
   typeContext(CT_BLOCK),
   allowLabel(true),
   caseDefault(false),
   inheritLocals(false)
{
   addVar(var_false, false, true);
   addVar(var_true, false, true);
   addVar(var_nullptr, false, true);
}

//
// SourceContext::SourceContext
//
SourceContext::SourceContext(SourceContext *_parent, ContextType _typeContext) :
   label(_parent->makeLabelShort()),
   parent(_parent),
   countAuto(0),
   countRegister(0),
   labelCount(0),
   limitAuto(0),
   limitRegister(0),
   typeContext(_typeContext),
   allowLabel(true),
   caseDefault(false),
   inheritLocals(false)
{
   switch (typeContext)
   {
   case CT_BLOCK:
      inheritLocals = true;
      break;

   case CT_FUNCTION:
      break;

   case CT_LOOP:
      inheritLocals = true;
      break;

   case CT_SCRIPT:
      break;

   case CT_SWITCH:
      inheritLocals = true;
      break;
   }

   if (inheritLocals)
   {
      limitAuto     = parent->limitAuto;
      limitRegister = parent->limitRegister;
   }
}

//
// SourceContext::~SourceContext
//
SourceContext::~SourceContext()
{
}

//
// SourceContext::addCount
//
void SourceContext::addCount(int count, SourceVariable::StorageClass sc)
{
   switch (sc)
   {
   case SourceVariable::SC_AUTO:
      countAuto += count;
      addLimit(getCount(sc), sc);
      break;

   case SourceVariable::SC_REGISTER:
      countRegister += count;
      addLimit(getCount(sc), sc);
      break;

   case SourceVariable::SC_CONSTANT:
   case SourceVariable::SC_REGISTER_GLOBAL:
   case SourceVariable::SC_REGISTER_MAP:
   case SourceVariable::SC_REGISTER_WORLD:
   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
   case SourceVariable::SC_REGISTERARRAY_MAP:
   case SourceVariable::SC_REGISTERARRAY_WORLD:
   case SourceVariable::SC_STATIC:
      break;
   }
}

//
// SourceContext::addFunction
//
void SourceContext::addFunction(SourceVariable *func)
{
   funcNames.push_back(func->getNameSource());
   funcTypes.push_back(func->getType());
   funcVars .push_back(func);
}

//
// SourceContext::addLabelCase
//
std::string SourceContext::
addLabelCase(bigsint value, SourcePosition const &pos)
{
   if (typeContext == CT_SWITCH)
   {
      if (cases.find(value) == cases.end() || !cases[value])
         cases[value] = true;
      else
         ERROR_NP("case redefined: %lli", static_cast<long long int>(value));

      return getLabelCase(value, pos);
   }

   if (inheritLocals && parent)
      return parent->addLabelCase(value, pos);

   ERROR_NP("not CT_SWITCH");
}

//
// SourceContext::addLabelCaseDefault
//
std::string SourceContext::addLabelCaseDefault(SourcePosition const &pos)
{
   if (typeContext == CT_SWITCH)
   {
      if (!caseDefault)
         caseDefault = true;
      else
         ERROR_NP("case default redefined");

      return getLabelCaseDefault(pos);
   }

   if (inheritLocals && parent)
      return parent->addLabelCaseDefault(pos);

   ERROR_NP("not CT_SWITCH");
}

//
// SourceContext::addLabelGoto
//
std::string SourceContext::
addLabelGoto(std::string const &name, SourcePosition const &position)
{
   return getLabelGoto(name, position);
}

//
// SourceContext::addLimit
//
void SourceContext::addLimit(int limit, SourceVariable::StorageClass sc)
{
   switch (sc)
   {
   case SourceVariable::SC_AUTO:
      if (limit > limitAuto)
         limitAuto = limit;

      if (inheritLocals && parent)
         parent->addLimit(limit, sc);

      break;

   case SourceVariable::SC_REGISTER:
      if (limit > limitRegister)
         limitRegister = limit;

      if (inheritLocals && parent)
         parent->addLimit(limit, sc);

      break;

   case SourceVariable::SC_CONSTANT:
   case SourceVariable::SC_REGISTER_GLOBAL:
   case SourceVariable::SC_REGISTER_MAP:
   case SourceVariable::SC_REGISTER_WORLD:
   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
   case SourceVariable::SC_REGISTERARRAY_MAP:
   case SourceVariable::SC_REGISTERARRAY_WORLD:
   case SourceVariable::SC_STATIC:
      break;
   }
}

//
// SourceContext::addVar
//
void SourceContext::addVar(SourceVariable *var, bool externDef, bool externVis)
{
   #define PARM nameObj, type, externDef, externVis

   SourceVariable::StorageClass sc = var->getClass();
   std::string const &nameObj = var->getNameObject();
   VariableType::Reference type = var->getType();

   varVars.push_back(var);
   varNames.push_back(var->getNameSource());

   switch (sc)
   {
   case SourceVariable::SC_AUTO:
      ObjectData_Auto::add(PARM, getCount(sc));
      addCount(type->getSize(SourcePosition::none()), sc);
      break;

   case SourceVariable::SC_CONSTANT:
      break;

   case SourceVariable::SC_REGISTER:
      ObjectData_Register::add(PARM, getCount(sc));
      addCount(type->getSize(SourcePosition::none()), sc);
      break;

   case SourceVariable::SC_REGISTER_GLOBAL:
      ObjectData_Register::add_global(PARM);
      break;

   case SourceVariable::SC_REGISTER_MAP:
      ObjectData_Register::add_map(PARM);
      break;

   case SourceVariable::SC_REGISTER_WORLD:
      ObjectData_Register::add_world(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
      ObjectData_Array::add_global(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_MAP:
      ObjectData_Array::add_map(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_WORLD:
      ObjectData_Array::add_world(PARM);
      break;

   case SourceVariable::SC_STATIC:
      ObjectData_Static::add(PARM);
      break;
   }

   #undef PARM
}

//
// SourceContext::addVar
//
void SourceContext::addVar(SourceVariable *var, bool externDef, bool externVis,
                           bigsint address)
{
   #define PARM nameObj, type, externDef, externVis, address

   SourceVariable::StorageClass sc = var->getClass();
   std::string const &nameObj = var->getNameObject();
   VariableType::Reference type = var->getType();

   varVars.push_back(var);
   varNames.push_back(var->getNameSource());

   switch (sc)
   {
   case SourceVariable::SC_AUTO:
      ObjectData_Auto::add(PARM);
      addCount(type->getSize(SourcePosition::none()), sc);
      break;

   case SourceVariable::SC_CONSTANT:
      break;

   case SourceVariable::SC_REGISTER:
      ObjectData_Register::add(PARM);
      addCount(type->getSize(SourcePosition::none()), sc);
      break;

   case SourceVariable::SC_REGISTER_GLOBAL:
      ObjectData_Register::add_global(PARM);
      break;

   case SourceVariable::SC_REGISTER_MAP:
      ObjectData_Register::add_map(PARM);
      break;

   case SourceVariable::SC_REGISTER_WORLD:
      ObjectData_Register::add_world(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
      ObjectData_Array::add_global(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_MAP:
      ObjectData_Array::add_map(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_WORLD:
      ObjectData_Array::add_world(PARM);
      break;

   case SourceVariable::SC_STATIC:
      ObjectData_Static::add(PARM);
      break;
   }

   #undef PARM
}

//
// SourceContext::create
//
SourceContext::Reference SourceContext::
create(SourceContext *parent, ContextType typeContext)
{
   return Reference(new SourceContext(parent, typeContext));
}

//
// SourceContext::findTempVar
//
SourceVariable::Pointer SourceContext::findTempVar(unsigned i)
{
   if (i < tempVars.size() && tempVars[i])
      return tempVars[i];

   if (inheritLocals) return parent->findTempVar(i);

   return NULL;
}

//
// SourceContext::getAllowLabel
//
bool SourceContext::getAllowLabel() const
{
   // Should this be inherited?
   if (inheritLocals && parent)
      return allowLabel && parent->getAllowLabel();
   else
      return allowLabel;
}

//
// SourceContext::getCases
//
std::vector<bigsint> SourceContext::getCases(SourcePosition const &pos) const
{
   if (typeContext == CT_SWITCH)
   {
      std::vector<bigsint> casev(cases.size());

      size_t i = 0;
      for (std::map<bigsint, bool>::const_iterator iter(cases.begin());
           iter != cases.end(); ++iter)
      {
         if (!iter->second)
            ERROR_NP
            ("case undefined: %lli", static_cast<long long int>(iter->first));

         casev[i++] = iter->first;
      }

      // No need for sort since std::map will have done that.
      return casev;
   }

   if (inheritLocals && parent)
      return parent->getCases(pos);

   ERROR_NP("not CT_SWITCH");
}

//
// SourceContext::getCount
//
int SourceContext::getCount(SourceVariable::StorageClass sc) const
{
   switch (sc)
   {
   case SourceVariable::SC_AUTO:
      if (inheritLocals && parent)
         return parent->getCount(sc) + countAuto;
      else
         return countAuto;

   case SourceVariable::SC_REGISTER:
      if (inheritLocals && parent)
         return parent->getCount(sc) + countRegister;
      else
         return countRegister;

   case SourceVariable::SC_CONSTANT:
   case SourceVariable::SC_REGISTER_GLOBAL:
   case SourceVariable::SC_REGISTER_MAP:
   case SourceVariable::SC_REGISTER_WORLD:
   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
   case SourceVariable::SC_REGISTERARRAY_MAP:
   case SourceVariable::SC_REGISTERARRAY_WORLD:
   case SourceVariable::SC_STATIC:
      return 0;
   }

   ERROR_Np("invalid SC");
}

//
// SourceContext::getFunction
//
SourceVariable::Pointer SourceContext::getFunction
(std::string const &name, SourcePosition const &pos)
{
   for (size_t i = 0; i < funcVars.size(); ++i)
      if (funcNames[i] == name) return funcVars[i];

   if (parent) return parent->getFunction(name, pos);

   ERROR_NP("no suitable function");
}

//
// SourceContext::getFunction
//
SourceVariable::Pointer SourceContext::getFunction
(std::string const &name, SourcePosition const &pos,
 VariableType::Vector const &types)
{
   unsigned cast, funcCount = 0;
   SourceVariable *func;

   for (size_t i = 0; i < funcVars.size(); ++i)
   {
      if (funcNames[i] != name) continue;

      func = funcVars[i];
      cast = VariableType::get_cast(funcTypes[i]->getTypes(), types);

      if (cast & VariableType::CAST_NONE)
         return func;

      if (cast & VariableType::CAST_IMPLICIT)
         ++funcCount;
   }

   if (funcCount == 1)
      return func;

   if (funcCount > 1)
      ERROR_NP("ambiguous overload: %s", name.c_str());

   if (parent) return parent->getFunction(name, pos, types);

   ERROR_NP("no suitable overload: %s", name.c_str());
}

//
// SourceContext::getLabel
//
std::string SourceContext::getLabel() const
{
   if (parent)
      return parent->getLabel() + label;
   else
      return ObjectExpression::get_filename() + "::" + label;
}

//
// SourceContext::getLabelBreak
//
std::string SourceContext::getLabelBreak(SourcePosition const &pos) const
{
   if (typeContext == CT_LOOP || typeContext == CT_SWITCH)
      return getLabel() + "_break";

   if (inheritLocals && parent)
      return parent->getLabelBreak(pos);

   ERROR_NP("not break CT");
}

//
// SourceContext::getLabelCase
//
std::string SourceContext::
getLabelCase(bigsint value, SourcePosition const &pos)
{
   if (typeContext == CT_SWITCH)
   {
      if (cases.find(value) == cases.end())
         cases[value] = false;

      std::ostringstream out;
      out << getLabel() << "_case" << value;
      return out.str();
   }

   if (inheritLocals && parent)
      return parent->getLabelCase(value, pos);

   ERROR_NP("not CT_SWITCH");
}

//
// SourceContext::getLabelCaseDefault
//
std::string SourceContext::
getLabelCaseDefault(SourcePosition const &pos) const
{
   if (typeContext == CT_SWITCH)
      return getLabel() + "_casedefault";

   if (inheritLocals && parent)
      return parent->getLabelCaseDefault(pos);

   ERROR_NP("not CT_SWITCH");
}

//
// SourceContext::getLabelContinue
//
std::string SourceContext::
getLabelContinue(SourcePosition const &pos) const
{
   if (typeContext == CT_LOOP)
      return getLabel() + "_continue";

   if (inheritLocals && parent)
      return parent->getLabelContinue(pos);

   ERROR_NP("not CT_LOOP");
}

//
// SourceContext::getLabelGoto
//
std::string SourceContext::
getLabelGoto(std::string const &name, SourcePosition const &pos) const
{
   if (parent && inheritLocals)
      return parent->getLabelGoto(name, pos);

   return getLabel() + "_goto" + name;
}

//
// SourceContext::getLimit
//
int SourceContext::getLimit(SourceVariable::StorageClass sc) const
{
   switch (sc)
   {
   case SourceVariable::SC_AUTO:
      return limitAuto;

   case SourceVariable::SC_REGISTER:
      return limitRegister;

   case SourceVariable::SC_CONSTANT:
   case SourceVariable::SC_REGISTER_GLOBAL:
   case SourceVariable::SC_REGISTER_MAP:
   case SourceVariable::SC_REGISTER_WORLD:
   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
   case SourceVariable::SC_REGISTERARRAY_MAP:
   case SourceVariable::SC_REGISTERARRAY_WORLD:
   case SourceVariable::SC_STATIC:
      return 0;
   }

   ERROR_Np("invalid SC");
}

//
// SourceContext::getReturnType
//
VariableType::Reference SourceContext::getReturnType() const
{
   return typeReturn ? VariableType::Reference(typeReturn)
                     : parent->getReturnType();
}

//
// SourceContext::getTempVar
//
ObjectExpression::Pointer SourceContext::getTempVar(unsigned i)
{
   static SourcePosition const &pos = SourcePosition::builtin();
   static SourceVariable::StorageClass const sc =
      SourceVariable::get_sc_autoreg();
   static VariableType::Reference const type = VariableType::get_bt_int();

   static char const *const name[] =
      {"__temp0__", "__temp1__", "__temp2__", "__temp3__", "__temp4__",
       "__temp5__", "__temp6__", "__temp7__", "__temp8__", "__temp9__"};

   SourceVariable::Pointer var = findTempVar(i);

   if (!var)
   {
      if (i > 9) ERROR_Np("temp var out of range: %u", i);

      if (i >= tempVars.size())
         tempVars.resize(i+1);

      std::string nameSrc = name[i];
      std::string nameObj = getLabel() + nameSrc;

      var = SourceVariable::create_variable(nameSrc, type, nameObj, sc, pos);

      tempVars[i] = var;
      addVar(var, false, false);
   }

   return var->getData()->address;
}

//
// SourceContext::getTypeRoot
//
SourceContext::ContextType SourceContext::getTypeRoot() const
{
   switch (typeContext)
   {
   case CT_BLOCK:
   case CT_LOOP:
   case CT_SWITCH:
      if (parent)
         return parent->getTypeRoot();
      break;

   case CT_FUNCTION:
   case CT_SCRIPT:
      return typeContext;
   }

   return typeContext;
}

//
// SourceContext::getVariable
//
SourceVariable::Pointer SourceContext::
getVariable(std::string const &name, SourcePosition const &position) const
{
   return getVariable(name, position, true);
}

//
// SourceContext::getVariable
//
SourceVariable::Pointer SourceContext::getVariable
(std::string const &name, SourcePosition const &pos, bool canLocal) const
{
   for (size_t i = varNames.size(); i--;)
   {
      if (varNames[i] == name)
      {
         switch (varVars[i]->getClass())
         {
         case SourceVariable::SC_AUTO:
         case SourceVariable::SC_REGISTER:
            if (canLocal)
               return varVars[i];

            break;

         case SourceVariable::SC_CONSTANT:
         case SourceVariable::SC_REGISTER_GLOBAL:
         case SourceVariable::SC_REGISTER_MAP:
         case SourceVariable::SC_REGISTER_WORLD:
         case SourceVariable::SC_REGISTERARRAY_GLOBAL:
         case SourceVariable::SC_REGISTERARRAY_MAP:
         case SourceVariable::SC_REGISTERARRAY_WORLD:
         case SourceVariable::SC_STATIC:
            return varVars[i];
         }
      }
   }

   if (parent)
      return parent->getVariable(name, pos, canLocal && inheritLocals);

   ERROR_NP("no such variable: %s", name.c_str());
}

//
// SourceContext::hasLabelCaseDefault
//
bool SourceContext::hasLabelCaseDefault() const
{
   return caseDefault;
}

//
// SourceContext::init
//
void SourceContext::init()
{
   global_context = new SourceContext;
}

//
// SourceContext::isFunction
//
int SourceContext::isFunction(std::string const &name) const
{
   int count = 0;
   for (size_t i = 0; i < funcVars.size(); ++i)
      if (funcNames[i] == name) ++count;

   if (!count && parent) return parent->isFunction(name);

   return count;
}

//
// SourceContext::makeLabel
//
std::string SourceContext::makeLabel()
{
   return getLabel() + makeLabelShort();
}

//
// SourceContext::makeLabelShort
//
std::string SourceContext::makeLabelShort()
{
   std::ostringstream oss;

   oss << ++labelCount << "::";

   return oss.str();
}

//
// SourceContext::setAllowLabel
//
void SourceContext::setAllowLabel(bool allow)
{
   allowLabel = allow;
}

//
// SourceContext::setReturnType
//
void SourceContext::setReturnType(VariableType *type)
{
   typeReturn = type;
}

// EOF

