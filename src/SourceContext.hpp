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
// Source-level context handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_SourceContext_
#define HPP_SourceContext_

#include "bignum.hpp"
#include "Counter.hpp"
#include "LinkSpec.hpp"
#include "StoreType.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;
class SourceFunction;
class SourcePosition;
class SourceVariable;
class VariableType;

//
// SourceContext
//
class SourceContext : public PlainCounter
{
   CounterPreambleNoVirtual(SourceContext, PlainCounter);

public:
   enum ContextType
   {
      CT_BLOCK,
      CT_FUNCTION,
      CT_LOOP,
      CT_NAMESPACE,
      CT_SCRIPT,
      CT_SWITCH
   };

   //
   // AddressSpace
   //
   struct AddressSpace
   {
      std::string array;
      StoreType   store;
   };


   void addAddressSpace(std::string const &name, AddressSpace const &addr);

   void addFunction(SourceFunction *func);

   std::string addLabelCase(bigsint value, SourcePosition const & position);
   std::string addLabelCaseDefault(SourcePosition const & position);
   std::string addLabelGoto(std::string const &name, SourcePosition const &position);

   void addVar(SourceVariable *var, LinkageSpecifier linkSpec, bool externDef);
   void addVar(SourceVariable *var, LinkageSpecifier linkSpec, bool externDef, bigsint address);

   void addVariableType_struct(std::string const &name);

   void addVariableType_union(std::string const &name);

   AddressSpace const &getAddressSpace(std::string const &name, SourcePosition const &pos) const;

   std::vector<bigsint> getCases(SourcePosition const & position) const;

   Reference getContext(std::string const &name, SourcePosition const &pos) const;
   Pointer getContextNull(std::string const &name) const;

   CounterReference<SourceFunction> getFunction
   (std::string const &name, SourcePosition const &pos);
   CounterReference<SourceFunction> getFunction
   (std::string const &name, SourcePosition const &pos,
    std::vector<CounterPointer<VariableType> > const &types);
   CounterReference<SourceFunction> getFunction(std::string const &name,
      SourcePosition const &pos,
      std::vector<CounterPointer<VariableType> > const &types,
      std::vector<CounterPointer<ObjectExpression> > const &objs);

   std::string getLabel() const;
   std::string getLabelBreak(SourcePosition const & position) const;
   std::string getLabelBreak(std::string const &name, SourcePosition const &pos) const;
   std::string getLabelCase(bigsint value, SourcePosition const & position);
   std::string getLabelCaseDefault(SourcePosition const & position) const;
   std::string getLabelContinue(SourcePosition const & position) const;
   std::string getLabelContinue(std::string const &name, SourcePosition const &pos) const;
   std::string getLabelGoto(std::string const &name, SourcePosition const &position) const;
   std::string getLabelNamespace() const;

   int getLimit(StoreType store) const;

   CounterReference<VariableType> getReturnType() const;

   CounterPointer<ObjectExpression> getTempVar(unsigned i);

   ContextType getType() const {return typeContext;}
   ContextType getTypeRoot() const;

   CounterPointer<SourceVariable> getVariable(std::string const &name, SourcePosition const &position) const;

   // Named type.
   CounterReference<VariableType> getVariableType
   (std::string const &name, SourcePosition const &position);

   // enum
   CounterReference<VariableType> getVariableType_enum
   (std::string const &name, bool block, SourcePosition const &position);

   // struct
   CounterReference<VariableType> getVariableType_struct
   (std::string const &name, SourcePosition const &position);

   // struct {...}
   CounterReference<VariableType> getVariableType_struct
   (std::string const &name, std::vector<std::string> const &names,
    std::vector<CounterPointer<VariableType> > const &types,
    SourcePosition const &position);

   // typedef-name
   CounterReference<VariableType> getVariableType_typedef(
      std::string const &name, SourcePosition const &pos);

   // typedef
   CounterReference<VariableType> getVariableType_typedef
   (std::string const &name, VariableType *type, SourcePosition const &position);

   // union
   CounterReference<VariableType> getVariableType_union
   (std::string const &name, SourcePosition const & position);

   // union {...}
   CounterReference<VariableType> getVariableType_union
   (std::string const &name, std::vector<std::string> const &names,
    std::vector<CounterPointer<VariableType> > const &types,
    SourcePosition const & position);

   CounterPointer<VariableType> getVariableTypeNull(std::string const &name);

   bool hasLabelCaseDefault() const;


   bool isAddressSpace(std::string const &name) const;
   int isFunction(std::string const &name) const;
   bool isVariable(std::string const &name) const;
   bool isVariable(std::string const &name, bool canLocal) const;
   bool isVariableType_typedef(std::string const &name) const;

   std::string makeLabel();

   // Does name mangling for variable identifiers.
   std::string makeNameObj(std::string const &nameSrc, LinkageSpecifier linkage);

   // Does name mangling for function identifiers.
   std::string makeNameObj(std::string const &nameSrc, LinkageSpecifier linkage,
                           std::vector<CounterPointer<VariableType> > const &types);

   void setLabel(std::string const &_label) {label = _label;}

   void setReturnType(VariableType *type);


   static Reference create(SourceContext *parent, ContextType type);
   static Reference create(SourceContext *parent, std::string const &name);

   static void init();

   static Pointer global_context;

private:
   SourceContext(SourceContext *parent, std::string const &label, ContextType type);
   SourceContext();
   ~SourceContext();

   void addCount(int count, StoreType store);
   void addLimit(int limit, StoreType store);

   CounterPointer<SourceVariable> findTempVar(unsigned i);

   int getCount(StoreType store) const;

   CounterPointer<SourceVariable> getVariable(std::string const & name, SourcePosition const & position, bool canLocal) const;

   CounterPointer<VariableType> getVariableType_enum(std::string const & name);
   CounterPointer<VariableType> getVariableType_struct(std::string const & name);
   CounterPointer<VariableType> getVariableType_union(std::string const & name);

   std::string makeLabelShort();

   void mangleNameObj(std::string &nameObj, std::vector<CounterPointer<VariableType> > const &types);

   std::map<bigsint, bool> cases;

   std::set<SourceContext *> children;

   std::vector<std::string> addrNames;
   std::vector<AddressSpace> addrAddrs;

   std::vector<std::string> enumNames;
   std::vector<CounterReference<VariableType> > enumTypes;

   std::vector<CounterReference<SourceFunction> > funcs;

   std::vector<std::string> structNames;
   std::vector<CounterReference<VariableType> > structTypes;

   std::vector<CounterPointer<SourceVariable> > tempVars;

   std::vector<std::string> typedefNames;
   std::vector<CounterReference<VariableType> > typedefTypes;

   std::vector<std::string> unionNames;
   std::vector<CounterReference<VariableType> > unionTypes;

   std::vector<std::string> varNames;
   std::vector<CounterPointer<SourceVariable> > varVars;

   std::string label;

   SourceContext::Pointer parent;
   CounterPointer<VariableType> typeReturn;

   bigsint countAuto;
   bigsint countRegister;
   bigsint labelCount;
   bigsint limitAuto;
   bigsint limitRegister;

   ContextType typeContext;

   bool caseDefault   : 1;
   bool inheritLocals : 1;
};

#endif//HPP_SourceContext_

