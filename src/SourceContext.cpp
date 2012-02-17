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

#include "ObjectExpression.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"
#include "SourceVariable.hpp"
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
   SourcePosition const &pos = SourcePosition::builtin();

   getVariableType_typedef(  "bool",     VariableType::get_bt_boolhard(), pos);
   getVariableType_typedef("__softbool", VariableType::get_bt_boolsoft(), pos);
   getVariableType_typedef(  "char",     VariableType::get_bt_char(),     pos);
   getVariableType_typedef(  "int",      VariableType::get_bt_int(),      pos);
   getVariableType_typedef("__label",    VariableType::get_bt_label(),    pos);
   getVariableType_typedef("__real",     VariableType::get_bt_real(),     pos);
   getVariableType_typedef("__string",   VariableType::get_bt_string(),   pos);
   getVariableType_typedef(  "unsigned", VariableType::get_bt_uint(),     pos);
   getVariableType_typedef(  "void",     VariableType::get_bt_void(),     pos);


   addVariable(var_false);
   addVariable(var_true);
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
// SourceContext::addLabelCase
//
std::string SourceContext::
addLabelCase(bigsint value, SourcePosition const &position)
{
   if (typeContext == CT_SWITCH)
   {
      if (cases.find(value) == cases.end() || !cases[value])
         cases[value] = true;
      else
         throw SourceException("case redefined", position, __func__);

      return getLabelCase(value, position);
   }

   if (inheritLocals && parent)
      return parent->addLabelCase(value, position);

   throw SourceException("not CT_SWITCH", position, __func__);
}

//
// SourceContext::addLabelCaseDefault
//
std::string SourceContext::addLabelCaseDefault(SourcePosition const &position)
{
   if (typeContext == CT_SWITCH)
   {
      if (!caseDefault)
         caseDefault = true;
      else
         throw SourceException("case default redefined", position, __func__);

      return getLabelCaseDefault(position);
   }

   if (inheritLocals && parent)
      return parent->addLabelCaseDefault(position);

   throw SourceException("not CT_SWITCH", position, __func__);
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
// SourceContext::addVariable
//
void SourceContext::addVariable(SourceVariable *var)
{
   varVars.push_back(var);
   varNames.push_back(var->getNameSource());

   SourceVariable::StorageClass sc = var->getClass();
   switch (sc)
   {
   case SourceVariable::SC_AUTO:
   case SourceVariable::SC_CONSTANT:
   case SourceVariable::SC_REGISTER:
   case SourceVariable::SC_REGISTER_GLOBAL:
   case SourceVariable::SC_REGISTER_MAP:
   case SourceVariable::SC_REGISTER_WORLD:
   case SourceVariable::SC_STATIC:
      addCount(var->getType()->getSize(SourcePosition::none()), sc);
      break;

   case SourceVariable::SC_REGISTERARRAY_MAP:
   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
   case SourceVariable::SC_REGISTERARRAY_WORLD:
      // Register arrays only require a single allocation regardless of size.
      addCount(1, sc);
      break;
   }
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
std::vector<bigsint> SourceContext::
getCases(SourcePosition const & position) const
{
   if (typeContext == CT_SWITCH)
   {
      std::vector<bigsint> casev(cases.size());

      size_t i = 0;
      for (std::map<bigsint, bool>::const_iterator iter(cases.begin());
           iter != cases.end(); ++iter)
      {
         if (!iter->second)
            throw SourceException("case undefined", position, __func__);

         casev[i++] = iter->first;
      }

      // No need for sort since std::map will have done that.
      return casev;
   }

   if (inheritLocals && parent)
      return parent->getCases(position);

   throw SourceException("not CT_SWITCH", position, __func__);
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

   throw SourceException("invalid sc", SourcePosition::none(), __func__);
}

//
// SourceContext::getLabel
//
std::string SourceContext::getLabel() const
{
   if (parent)
      return parent->getLabel() + label;
   else
      return ObjectExpression::get_filename() + label;
}

//
// SourceContext::getLabelBreak
//
std::string SourceContext::getLabelBreak(SourcePosition const &position) const
{
   if (typeContext == CT_LOOP || typeContext == CT_SWITCH)
      return getLabel() + "_break";

   if (inheritLocals && parent)
      return parent->getLabelBreak(position);

   throw SourceException("not break CT", position, __func__);
}

//
// SourceContext::getLabelCase
//
std::string SourceContext::
getLabelCase(bigsint value, SourcePosition const &position)
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
      return parent->getLabelCase(value, position);

   throw SourceException("not CT_SWITCH", position, __func__);
}

//
// SourceContext::getLabelCaseDefault
//
std::string SourceContext::
getLabelCaseDefault(SourcePosition const &position) const
{
   if (typeContext == CT_SWITCH)
      return getLabel() + "_casedefault";

   if (inheritLocals && parent)
      return parent->getLabelCaseDefault(position);

   throw SourceException("not CT_SWITCH", position, __func__);
}

//
// SourceContext::getLabelContinue
//
std::string SourceContext::
getLabelContinue(SourcePosition const &position) const
{
   if (typeContext == CT_LOOP)
      return getLabel() + "_continue";

   if (inheritLocals && parent)
      return parent->getLabelContinue(position);

   throw SourceException("not CT_LOOP", position, __func__);
}

//
// SourceContext::getLabelGoto
//
std::string SourceContext::
getLabelGoto(std::string const &name, SourcePosition const &position) const
{
   if (parent && inheritLocals)
      return parent->getLabelGoto(name, position);

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

   throw SourceException("invalid sc", SourcePosition::none(), __func__);
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
SourceVariable::Pointer SourceContext::
getVariable(std::string const &name, SourcePosition const &position,
            bool canLocal) const
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
      return parent->getVariable(name, position, canLocal && inheritLocals);

   throw SourceException("no such variable '" + name + "'", position, __func__);
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

   oss << "block" << ++labelCount;

   return oss.str();
}

//
// SourceContext::makeNameObject
//
std::string SourceContext::makeNameObject
(NameType nameType, SourceVariable::StorageClass sc, VariableType *type,
 std::string const &nameSource, SourcePosition const &position) const
{
   #define PARM nameObject, typeSize, nameType == NT_EXTERN, \
                nameType != NT_LOCAL

   std::string nameObject;
   if (nameType == NT_LOCAL)
      nameObject += getLabel();
   nameObject += nameSource;

   bigsint typeSize = type->getSize(position);

   switch (sc)
   {
   case SourceVariable::SC_AUTO:
   case SourceVariable::SC_REGISTER:
      // TODO: Need a separate list for register at some point eventually, but
      // it's not that important.
      ObjectExpression::add_auto(nameObject, typeSize, getCount(sc));
      break;

   case SourceVariable::SC_CONSTANT:
      throw SourceException("SC_CONSTANT", position, __func__);

   case SourceVariable::SC_REGISTER_GLOBAL:
      ObjectExpression::add_register_global(PARM);
      break;

   case SourceVariable::SC_REGISTER_MAP:
      ObjectExpression::add_register_map(PARM);
      break;

   case SourceVariable::SC_REGISTER_WORLD:
      ObjectExpression::add_register_world(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
      ObjectExpression::add_registerarray_global(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_MAP:
      ObjectExpression::add_registerarray_map(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_WORLD:
      ObjectExpression::add_registerarray_world(PARM);
      break;

   case SourceVariable::SC_STATIC:
      ObjectExpression::add_static(nameObject, typeSize);
      break;
   }

   return nameObject;

   #undef PARM
}

//
// SourceContext::makeNameObject
//
std::string SourceContext::makeNameObject
(NameType nameType, SourceVariable::StorageClass sc, VariableType *type,
 std::string const &nameSource, bigsint address,
 SourcePosition const &position) const
{
   #define PARM nameObject, typeSize, address, nameType == NT_EXTERN, \
                nameType != NT_LOCAL

   std::string nameObject;
   if (nameType == NT_LOCAL)
      nameObject += getLabel();
   nameObject += nameSource;

   bigsint typeSize = type->getSize(position);

   switch (sc)
   {
   case SourceVariable::SC_AUTO:
   case SourceVariable::SC_REGISTER:
      // TODO: Need a separate list for register at some point eventually, but
      // it's not that important.
      ObjectExpression::add_auto(nameObject, typeSize, address);
      break;

   case SourceVariable::SC_CONSTANT:
      throw SourceException("makeNameObject on SC_CONSTANT", position, "SourceContext");

   case SourceVariable::SC_REGISTER_GLOBAL:
      ObjectExpression::add_register_global(PARM);
      break;

   case SourceVariable::SC_REGISTER_MAP:
      ObjectExpression::add_register_map(PARM);
      break;

   case SourceVariable::SC_REGISTER_WORLD:
      ObjectExpression::add_register_world(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_GLOBAL:
      ObjectExpression::add_registerarray_global(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_MAP:
      ObjectExpression::add_registerarray_map(PARM);
      break;

   case SourceVariable::SC_REGISTERARRAY_WORLD:
      ObjectExpression::add_registerarray_world(PARM);
      break;

   case SourceVariable::SC_STATIC:
      ObjectExpression::add_static(nameObject, typeSize, address);
      break;
   }

   return nameObject;

   #undef PARM
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

