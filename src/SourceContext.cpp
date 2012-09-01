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
#include "SourceFunction.hpp"
#include "SourceTokenC.hpp"
#include "SourceVariable.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"

#include <cstring>
#include <sstream>


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#ifdef __WIN32__
#define PRI_LL "%I64i"
#else
#define PRI_LL "%lli"
#endif


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static ObjectExpression::Pointer obj_0 =
   ObjectExpression::create_value_int(0, SourcePosition::builtin());
static ObjectExpression::Pointer obj_1 =
   ObjectExpression::create_value_int(1, SourcePosition::builtin());

static SourceVariable::Pointer var_false = SourceVariable::create_constant(
   "false", VariableType::get_bt_bit_hrd(), obj_0, SourcePosition::builtin());
static SourceVariable::Pointer var_true = SourceVariable::create_constant(
   "true", VariableType::get_bt_bit_hrd(), obj_1, SourcePosition::builtin());

static SourceVariable::Pointer var_nullptr = SourceVariable::create_constant(
   "nullptr", VariableType::get_bt_ptr_nul(), obj_0, SourcePosition::builtin());


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
   typeContext(CT_NAMESPACE),
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
SourceContext::SourceContext(SourceContext *_parent, std::string const &_label,
                             ContextType _typeContext)
 :
   label(_label),
   parent(_parent),
   countAuto(0),
   countRegister(0),
   labelCount(0),
   limitAuto(0),
   limitRegister(0),
   typeContext(_typeContext),
   caseDefault(false),
   inheritLocals(false)
{
   if(parent) parent->children.insert(this);

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

   case CT_NAMESPACE:
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
   if(parent) parent->children.erase(this);
}

//
// SourceContext::addCount
//
void SourceContext::addCount(int count, StoreType store)
{
   switch (store)
   {
   case STORE_NONE:
   case STORE_FAR:
   case STORE_STATIC:
   case STORE_CONST:
   case STORE_MAPREGISTER:
   case STORE_WORLDREGISTER:
   case STORE_GLOBALREGISTER:
   case STORE_MAPARRAY:
   case STORE_WORLDARRAY:
   case STORE_GLOBALARRAY:
   case STORE_STRING:
      break;

   case STORE_AUTO:
      countAuto += count;
      addLimit(getCount(store), store);
      break;

   case STORE_REGISTER:
      countRegister += count;
      addLimit(getCount(store), store);
      break;
   }
}

//
// SourceContext::addFunction
//
void SourceContext::addFunction(SourceFunction *func)
{
   for(std::vector<SourceFunction::Reference>::iterator itr = funcs.begin(),
       end = funcs.end(); itr != end; ++itr)
   {
      if(*itr == func) return;
   }

   funcs.push_back(static_cast<SourceFunction::Reference>(func));
}

//
// SourceContext::addLabelCase
//
std::string SourceContext::
addLabelCase(bigsint value, SourcePosition const &pos)
{
   // Pretend it's a 32-bit signed number.
   if((value &= 0xFFFFFFFF) > 0x7FFFFFFF) value = value - 0x100000000;

   if (typeContext == CT_SWITCH)
   {
      if (cases.find(value) == cases.end() || !cases[value])
         cases[value] = true;
      else
         Error_NP("case redefined: "PRI_LL, static_cast<long long int>(value));

      return getLabelCase(value, pos);
   }

   if (inheritLocals && parent)
      return parent->addLabelCase(value, pos);

   Error_NP("not CT_SWITCH");
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
         Error_NP("case default redefined");

      return getLabelCaseDefault(pos);
   }

   if (inheritLocals && parent)
      return parent->addLabelCaseDefault(pos);

   Error_NP("not CT_SWITCH");
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
void SourceContext::addLimit(int limit, StoreType store)
{
   switch (store)
   {
   default:
      break;

   case STORE_AUTO:
      if (limit > limitAuto)
         limitAuto = limit;

      if (inheritLocals && parent)
         parent->addLimit(limit, store);

      break;

   case STORE_REGISTER:
      if (limit > limitRegister)
         limitRegister = limit;

      if (inheritLocals && parent)
         parent->addLimit(limit, store);

      break;
   }
}

//
// SourceContext::addVar
//
void SourceContext::addVar(SourceVariable *var, bool externDef, bool externVis)
{
   #define PARM nameObj, type, externDef, externVis

   std::string const &nameObj = var->getNameObject();
   SourcePosition const &pos = var->getPosition();
   StoreType store = var->getStoreType();
   VariableType::Reference type = var->getType();

   varVars.push_back(var);
   varNames.push_back(var->getNameSource());

   switch (store)
   {
   case STORE_NONE:
   case STORE_FAR:
   case STORE_CONST:
   case STORE_STRING:
      break;

   case STORE_STATIC:
      ObjectData_Static::add(PARM);
      break;

   case STORE_AUTO:
      ObjectData_Auto::add(PARM, getCount(store));
      addCount(type->getSize(pos), store);
      break;

   case STORE_REGISTER:
      ObjectData_Register::add(PARM, getCount(store));
      addCount(type->getSize(pos), store);
      break;

   case STORE_MAPREGISTER:
      ObjectData_Register::add_map(PARM);
      break;

   case STORE_WORLDREGISTER:
      ObjectData_Register::add_world(PARM);
      break;

   case STORE_GLOBALREGISTER:
      ObjectData_Register::add_global(PARM);
      break;

   case STORE_MAPARRAY:
      ObjectData_Array::add_map(PARM);
      break;

   case STORE_WORLDARRAY:
      ObjectData_Array::add_world(PARM);
      break;

   case STORE_GLOBALARRAY:
      ObjectData_Array::add_global(PARM);
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

   std::string const &nameObj = var->getNameObject();
   SourcePosition const &pos = var->getPosition();
   StoreType store = var->getStoreType();
   VariableType::Reference type = var->getType();

   varVars.push_back(var);
   varNames.push_back(var->getNameSource());

   switch (store)
   {
   case STORE_NONE:
   case STORE_FAR:
   case STORE_CONST:
   case STORE_STRING:
      break;

   case STORE_STATIC:
      ObjectData_Static::add(PARM);
      break;

   case STORE_AUTO:
      ObjectData_Auto::add(PARM);
      addCount(type->getSize(pos), store);
      break;

   case STORE_REGISTER:
      ObjectData_Register::add(PARM);
      addCount(type->getSize(pos), store);
      break;

   case STORE_MAPREGISTER:
      ObjectData_Register::add_map(PARM);
      break;

   case STORE_WORLDREGISTER:
      ObjectData_Register::add_world(PARM);
      break;

   case STORE_GLOBALREGISTER:
      ObjectData_Register::add_global(PARM);
      break;

   case STORE_MAPARRAY:
      ObjectData_Array::add_map(PARM);
      break;

   case STORE_WORLDARRAY:
      ObjectData_Array::add_world(PARM);
      break;

   case STORE_GLOBALARRAY:
      ObjectData_Array::add_global(PARM);
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
   return Reference(new SourceContext(parent, parent->makeLabelShort(), typeContext));
}

//
// SourceContext::create
//
// Finds or creates a namespace.
//
SourceContext::Reference SourceContext::
create(SourceContext *_parent, std::string const &name)
{
   SourceContext::Pointer parent = _parent;

   while(parent->typeContext != CT_NAMESPACE) parent = parent->parent;

   for(std::set<SourceContext *>::const_iterator itr = parent->children.begin(),
       end = parent->children.end(); itr != end; ++itr)
   {
      if((*itr)->label == name) return static_cast<Reference>(*itr);
   }

   return static_cast<Reference>(new SourceContext(parent, name, CT_NAMESPACE));
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
            Error_NP("case undefined: "PRI_LL, static_cast<long long int>(iter->first));

         casev[i++] = iter->first;
      }

      // No need for sort since std::map will have done that.
      return casev;
   }

   if (inheritLocals && parent)
      return parent->getCases(pos);

   Error_NP("not CT_SWITCH");
}

//
// SourceContext::getContext
//
SourceContext::Reference SourceContext::getContext(std::string const &name,
                                                   SourcePosition const &pos) const
{
   SourceContext::Pointer context = getContextNull(name);

   if(!context) Error_NP("no such context '%s'", name.c_str());

   return static_cast<Reference>(context);
}

//
// SourceContext::getContextNull
//
SourceContext::Pointer SourceContext::getContextNull(std::string const &name) const
{
   for(std::set<SourceContext *>::const_iterator itr = children.begin(),
       end = children.end(); itr != end; ++itr)
   {
      if((*itr)->label == name) return static_cast<Reference>(*itr);
   }

   if(parent) return parent->getContextNull(name);

   return NULL;
}

//
// SourceContext::getCount
//
int SourceContext::getCount(StoreType store) const
{
   switch (store)
   {
   default:
      return 0;

   case STORE_AUTO:
      if (inheritLocals && parent)
         return parent->getCount(store) + countAuto;
      else
         return countAuto;

   case STORE_REGISTER:
      if (inheritLocals && parent)
         return parent->getCount(store) + countRegister;
      else
         return countRegister;
   }

   Error_Np("invalid store");
}

//
// SourceContext::getFunction
//
SourceFunction::Reference SourceContext::getFunction
(std::string const &name, SourcePosition const &pos)
{
   for (size_t i = 0; i < funcs.size(); ++i)
      if (funcs[i]->var->getNameSource() == name) return funcs[i];

   if (parent) return parent->getFunction(name, pos);

   Error_NP("no suitable function");
}

//
// SourceContext::getFunction
//
SourceFunction::Reference SourceContext::getFunction
(std::string const &name, SourcePosition const &pos,
 VariableType::Vector const &types)
{
   VariableType::CastType cast, castBest = VariableType::CAST_NEVER;
   unsigned funcCount = 0;
   std::vector<SourceFunction::Reference>::iterator func, funcItr, funcEnd = funcs.end();

   for(funcItr = funcs.begin(); funcItr != funcEnd; ++funcItr)
   {
      if((*funcItr)->var->getNameSource() != name) continue;

      if(types.size() < (*funcItr)->argsMin) continue;
      if(types.size() > (*funcItr)->argsMax) continue;

      if(types.size() == (*funcItr)->argsMax)
         cast = VariableType::get_cast((*funcItr)->var->getType()->getTypes(), types);
      else
         cast = VariableType::get_cast((*funcItr)->types[types.size() - (*funcItr)->argsMin], types);

      if(cast > castBest) continue;

      if(cast < castBest)
      {
         castBest = cast;
         funcCount = 0;
      }

      func = funcItr;
      ++funcCount;
   }

   if(castBest < VariableType::CAST_FORCE)
   {
      if(funcCount == 1)
         return *func;

      if(funcCount > 1)
         Error_NP("ambiguous overload: %s", name.c_str());
   }

   if(parent) return parent->getFunction(name, pos, types);

   Error_NP("no suitable overload: %s", name.c_str());
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

   Error_NP("not break CT");
}

//
// SourceContext::getLabelBreak
//
std::string SourceContext::getLabelBreak(std::string const &name,
                                         SourcePosition const &pos) const
{
   if((typeContext == CT_LOOP || typeContext == CT_SWITCH) && label == name)
      return getLabel() + "_break";

   if(inheritLocals)
      return parent->getLabelBreak(name, pos);

   Error_NP("cannot find break target '%s'", name.c_str());
}

//
// SourceContext::getLabelCase
//
std::string SourceContext::
getLabelCase(bigsint value, SourcePosition const &pos)
{
   // Pretend it's a 32-bit signed number.
   if((value &= 0xFFFFFFFF) > 0x7FFFFFFF) value = value - 0x100000000;

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

   Error_NP("not CT_SWITCH");
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

   Error_NP("not CT_SWITCH");
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

   Error_NP("not CT_LOOP");
}

//
// SourceContext::getLabelContinue
//
std::string SourceContext::getLabelContinue(std::string const &name,
                                            SourcePosition const &pos) const
{
   if(typeContext == CT_LOOP && label == name)
      return getLabel() + "_continue";

   if(inheritLocals)
      return parent->getLabelContinue(name, pos);

   Error_NP("cannot find continue target '%s'", name.c_str());
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
// SourceContext::getLabelNamespace
//
std::string SourceContext::getLabelNamespace() const
{
   if(!parent) return label + "::";

   if(typeContext == CT_NAMESPACE)
      return parent->getLabelNamespace() + label + "::";
   else
      return parent->getLabelNamespace();
}

//
// SourceContext::getLimit
//
int SourceContext::getLimit(StoreType store) const
{
   switch (store)
   {
   default:
      return 0;

   case STORE_AUTO:
      return limitAuto;

   case STORE_REGISTER:
      return limitRegister;
   }

   Error_Np("invalid store");
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
   static StoreType const store = store_autoregister();
   static VariableType::Reference const type = VariableType::get_bt_int();

   static char const *const name[] =
      {"__temp0__", "__temp1__", "__temp2__", "__temp3__", "__temp4__",
       "__temp5__", "__temp6__", "__temp7__", "__temp8__", "__temp9__"};

   if(inheritLocals)
      return parent->getTempVar(i);

   SourceVariable::Pointer var = findTempVar(i);

   if (!var)
   {
      if (i > 9) Error_Np("temp var out of range: %u", i);

      if (i >= tempVars.size())
         tempVars.resize(i+1);

      std::string nameSrc = name[i];
      std::string nameObj = getLabel() + nameSrc;

      var = SourceVariable::create_variable(nameSrc, type, nameObj, store, pos);

      tempVars[i] = var;
      addVar(var, false, false, getLimit(store));
      addLimit(getLimit(store) + type->getSize(pos), store);
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
   case CT_NAMESPACE:
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
         switch (varVars[i]->getStoreType())
         {
         default:
            return varVars[i];

         case STORE_AUTO:
         case STORE_REGISTER:
            if (canLocal)
               return varVars[i];

            break;
         }
      }
   }

   if (parent)
      return parent->getVariable(name, pos, canLocal && inheritLocals);

   Error_NP("no such variable: %s", name.c_str());
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
   for (size_t i = 0; i < funcs.size(); ++i)
      if (funcs[i]->var->getNameSource() == name) ++count;

   if (!count && parent) return parent->isFunction(name);

   return count;
}

//
// SourceContext::isVariable
//
bool SourceContext::isVariable(std::string const &name) const
{
   return isVariable(name, true);
}

//
// SourceContext::isVariable
//
bool SourceContext::isVariable(std::string const &name, bool canLocal) const
{
   for(size_t i = varNames.size(); i--;)
   {
      if(varNames[i] == name)
      {
         switch(varVars[i]->getStoreType())
         {
         default:
            return true;

         case STORE_AUTO:
         case STORE_REGISTER:
            if(canLocal)
               return true;

            break;
         }
      }
   }

   if(parent)
      return parent->isVariable(name, canLocal && inheritLocals);

   return false;
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
// SourceContext::makeNameObj
//
std::string SourceContext::makeNameObj(std::string const &nameSrc,
                                       LinkageSpecifier linkage)
{
   std::string nameObj;

   switch(linkage)
   {
   case LINKAGE_INTERN:
      nameObj = getLabel() + nameSrc;
      break;

   case LINKAGE_ACS:
      nameObj = nameSrc;
      break;

   case LINKAGE_C:
      nameObj = "_" + nameSrc;
      break;

   case LINKAGE_CPP:
   case LINKAGE_DS:
      nameObj = getLabelNamespace() + nameSrc;
      break;
   }

   return nameObj;
}

//
// SourceContext::makeNameObj
//
std::string SourceContext::makeNameObj(std::string const &nameSrc,
   LinkageSpecifier linkage, VariableType::Vector const &types)
{
   std::string nameObj;

   switch(linkage)
   {
   case LINKAGE_INTERN:
      nameObj = nameSrc.empty() ? makeLabel() : getLabel() + nameSrc;
      mangleNameObj(nameObj, types);
      break;

   case LINKAGE_ACS:
      nameObj = nameSrc;
      break;

   case LINKAGE_C:
      nameObj = "_" + nameSrc;
      break;

   case LINKAGE_CPP:
   case LINKAGE_DS:
      nameObj = getLabelNamespace() + nameSrc;
      mangleNameObj(nameObj, types);
      break;
   }

   return nameObj;
}

//
// SourceContext::mangleNameObj
//
void SourceContext::mangleNameObj(std::string &nameObj, VariableType::Vector const &types)
{
   nameObj += "$";

   for(VariableType::Vector::const_iterator type = types.begin(),
       end = types.end(); type != end; ++type)
   {
      nameObj += '$';

      if(*type)
         (*type)->getUnqualified()->getNameMangled(nameObj);
      else
         nameObj += "...";
   }
}

//
// SourceContext::setReturnType
//
void SourceContext::setReturnType(VariableType *type)
{
   typeReturn = type;
}

// EOF

