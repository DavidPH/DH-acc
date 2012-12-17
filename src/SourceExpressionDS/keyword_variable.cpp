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
// DS handling of variable declaration.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../ost_type.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct StoreData
{
   StoreType type;
   ObjectExpression::Pointer area;
   std::string name;
};


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// add_var
//
static SourceExpression::Pointer add_var(SourceContext *context,
   LinkageSpecifier linkSpec, SourcePosition const &pos,
   std::string const &nameSrc, std::string &nameObj, VariableType *type,
   StoreData const &store, ObjectExpression *addr, bool externDef,
   SourceVariable::Pointer &var)
{
   if(store.name.empty() && (store.type == STORE_MAPARRAY ||
      store.type == STORE_WORLDARRAY || store.type == STORE_GLOBALARRAY))
   {
      std::string nameArr = nameObj; nameObj += "$var";

      bigsint number = store.area ? store.area->resolveINT() : -1;

      if(store.type == STORE_MAPARRAY)
         ObjectData::Array::AddMap(nameArr, linkSpec, externDef, number);

      else if(store.type == STORE_WORLDARRAY)
         ObjectData::Array::AddWorld(nameArr, linkSpec, externDef, number);

      else if(store.type == STORE_GLOBALARRAY)
         ObjectData::Array::AddGlobal(nameArr, linkSpec, externDef, number);

      var = SourceVariable::create_variable(nameSrc, type, nameObj, nameArr, store.type, pos);
   }
   else
      var = SourceVariable::create_variable(nameSrc, type, nameObj, store.name, store.type, pos);

   if(addr)
      context->addVar(var, linkSpec, externDef, addr->resolveUNS());
   else
      context->addVar(var, linkSpec, externDef);

   return SourceExpression::create_value_variable(var, context, pos);
}

//
// get_array_length
//
static void get_array_length(std::vector<bigsint> &len, size_t depth,
                             VariableType::Vector const &types)
{
   if (depth >= len.size()) return;

   for (VariableType::Vector::const_iterator end = types.end(),
        itr = types.begin(); itr != end; ++itr)
   {
      if ((*itr)->getBasicType() == VariableType::BT_BLOCK)
      {
         VariableType::Vector const &itrTypes = (*itr)->getTypes();
         bigsint itrLen = itrTypes.size();
         if (itrLen > len[depth]) len[depth] = itrLen;
         get_array_length(len, depth+1, (*itr)->getTypes());
      }
   }
}

//
// get_array_length
//
static VariableType::Reference get_array_length(std::vector<bigsint> const &len,
   size_t &depth, VariableType *type)
{
   if (type->getBasicType() != VariableType::BT_ARR)
      return static_cast<VariableType::Reference>(type);

   if (static_cast<biguint>(depth) >= len.size())
      Error_p("err");

   if (type->getWidth())
      return get_array_length(len, depth, type->getReturn())
         ->getArray((depth++, type->getWidth()));
   else
      return get_array_length(len, depth, type->getReturn())
         ->getArray(len[depth++]);
}

//
// get_array_length
//
static VariableType::Reference get_array_length(VariableType *exprType,
                                                VariableType *initType)
{
   size_t depth = 0;

   // Determine how many indexes to find.
   for (VariableType::Reference type(exprType);
        type->getBasicType() == VariableType::BT_ARR;
        type = type->getReturn())
   {
      ++depth;
   }

   // Not an array, so just return.
   if (!depth)
      return static_cast<VariableType::Reference>(exprType);

   std::vector<bigsint> len(depth, 0);
   len[0] = initType->getTypes().size();
   get_array_length(len, 1, initType->getTypes());

   depth = 0;
   return get_array_length(len, depth, exprType);
}

//
// is_array_length_zero
//
static bool is_array_length_zero(VariableType const *type)
{
   if (type->getBasicType() != VariableType::BT_ARR)
      return false;

   if (type->getWidth())
      return is_array_length_zero(type->getReturn());

   return true;
}

//
// is_autoptr
//
static bool is_autoptr(VariableType const *type)
{
   if(type->getBasicType() != VariableType::BT_PTR)
      return false;

   if(type->getReturn()->getStoreType() != STORE_AUTO)
      return false;

   return true;
}

//
// get_void
//
static VariableType::Reference get_void(VariableType *exprType,
                                        VariableType *initType)
{
   if (exprType->getBasicType() == VariableType::BT_VOID)
      return static_cast<VariableType::Reference>(initType);

   if (initType->getBasicType() == VariableType::BT_BLOCK)
   {
      if (!initType->getTypes().empty())
         return get_void(exprType->getReturn(), initType->getTypes()[0])
                   ->getArray(exprType->getWidth());
   }

   return get_void(exprType->getReturn(), VariableType::get_bt_void())
             ->getArray(exprType->getWidth());
}

//
// is_void
//
static bool is_void(VariableType const *type)
{
   if (type->getBasicType() == VariableType::BT_VOID)
      return true;

   if (type->getBasicType() == VariableType::BT_ARR)
      return is_void(type->getReturn());

   return false;
}

//
// MakeInit
//
static SourceExpression::Pointer MakeInit(SourceContext *context,
   SourcePosition const &pos, SourceVariable *var, std::string const &nameObj,
   StoreData const &store, bool externDef)
{
   static VariableType::Reference const initType = VariableType::get_bt_bit_hrd();

   // Generate source/object name
   std::string initNameObj = nameObj + "$init";

   // Generate variable.
   SourceVariable::Pointer initVar = SourceVariable::create_variable(
      initNameObj, initType, initNameObj, var->nameArr, store.type, pos);
   context->addVar(initVar, LINKAGE_INTERN, externDef);

   // Generate expression.
   return SourceExpression::create_value_variable(initVar, context, pos);
}

//
// make_var
//
static SourceExpression::Pointer make_var(SourceTokenizerC *in,
   SourceContext *context, LinkageSpecifier linkSpec, SourcePosition const &pos,
   std::string const &nameSrc, VariableType::Reference type,
   StoreData const &store, bool externDef)
{
   SourceExpression::Pointer initExpr;
   SourceVariable::Pointer var;

   ObjectExpression::Pointer addr;
   if (in->peekType(SourceTokenC::TT_AT))
   {
      in->get();
      addr = SourceExpressionDS::make_prefix(in, context)->makeObject();
   }

   // Generate object name.
   std::string nameObj;
   switch (linkSpec)
   {
   case LINKAGE_INTERN:
      nameObj = context->getLabel() + nameSrc;
      break;

   case LINKAGE_ACS:
      nameObj = nameSrc;
      break;

   case LINKAGE_C:
      nameObj = "_" + nameSrc;
      break;

   case LINKAGE_CPP:
   case LINKAGE_DS:
      nameObj = context->getLabelNamespace() + nameSrc;
      break;
   }

   // Generate expression (unless final type is determined later).
   SourceExpression::Pointer expr;
   if(!is_array_length_zero(type) && !is_void(type) && !is_autoptr(type))
      expr = add_var(context, linkSpec, pos, nameSrc, nameObj, type, store,
                     addr, externDef, var);

   // Variable initialization. (But not for external declaration.)
   if (!externDef && in->peekType(SourceTokenC::TT_EQUALS))
   {
      SourceExpression::Pointer initSrc;
      ObjectExpression::Pointer initObj;

      in->get(SourceTokenC::TT_EQUALS);
      initSrc = SourceExpressionDS::make_assignment(in, context);

      // Automatic type.
      if (is_void(type))
      {
         if (type->getBasicType() == VariableType::BT_VOID)
            type = initSrc->getType();
         else
         {
            VariableType::Reference initSrcType = initSrc->getType();

            if (initSrcType->getBasicType() != VariableType::BT_BLOCK)
               Error_P("expected BT_BLOCK");

            type = get_void(type, initSrcType);

            type = get_array_length(type, initSrcType);
         }

         expr = add_var(context, linkSpec, pos, nameSrc, nameObj, type, store,
                        addr, externDef, var);
      }
      // Initializer-determined array length.
      else if(is_array_length_zero(type))
      {
         VariableType::Reference initSrcType = initSrc->getType();

         if(initSrcType->getBasicType() == VariableType::BT_BLOCK)
         {
            type = get_array_length(type, initSrcType);
         }
         else if(initSrcType->getBasicType() == VariableType::BT_STR)
         {
            std::string initStr = initSrc->makeObject()->resolveString();
            type = type->getReturn()->getArray(initStr.size());

            initSrcType = initSrcType->getReturn()->getArray(initStr.size());
            initSrc = SourceExpression::
               create_value_cast_force(initSrc, initSrcType, context, pos);
         }
         else
            Error_P("expected block or string");

         expr = add_var(context, linkSpec, pos, nameSrc, nameObj, type, store,
                        addr, externDef, var);
      }
      // Initializer-determined storage.
      else if(is_autoptr(type))
      {
         VariableType::Reference initSrcType = initSrc->getType();

         if(initSrcType->getBasicType() == VariableType::BT_PTR)
         {
            initSrcType = initSrcType->getReturn();
            type = type->getReturn()
                       ->setStorage(initSrcType->getStoreType(),
                                    initSrcType->getStoreArea())
                       ->getPointer();
         }

         expr = add_var(context, linkSpec, pos, nameSrc, nameObj, type, store,
                        addr, externDef, var);
      }

      // Generate initObj here in case one of the above rules alters initSrc.
      if(initSrc->canMakeObject())
         initObj = initSrc->makeObject();

      // Generate expression to set variable.
      SourceExpression::Pointer exprSet = SourceExpression::
         create_binary_assign_const(expr, initSrc, context, pos);

      switch (store.type)
      {
      case STORE_MAPREGISTER:
         if(!initObj || Target == TARGET_Hexen)
            goto case_init;

         if(!ObjectData::Register::InitMap(nameObj, initObj))
            goto case_init;

         break;

      case STORE_MAPARRAY:
         if(!initObj)
            goto case_init;

         if(!ObjectData::ArrayVar::InitMap(nameObj, type, initObj))
            goto case_init;

         break;

      case STORE_WORLDARRAY:
      case STORE_GLOBALARRAY:
      case STORE_STATIC:
      case_init:
      {
         if(!initExpr)
            initExpr = MakeInit(context, pos, var, nameObj, store, false);

         // Generate expression of value if initialized.
         SourceExpression::Pointer initVal = SourceExpression::
            create_value_int(1, context, pos);

         // Generate expression to set flag.
         SourceExpression::Pointer initSet = SourceExpression::
            create_binary_assign(initExpr, initVal, context, pos);

         // Generate expression to check if initialized.
         SourceExpression::Pointer initChk = SourceExpression::
            create_branch_not(initExpr, context, pos);

         // Generate expression to set variable and flag.
         SourceExpression::Vector initBothVector;
         initBothVector.push_back(exprSet);
         initBothVector.push_back(initSet);
         SourceExpression::Pointer initBoth = SourceExpression::
            create_value_block(initBothVector, context, pos);

         // Generate expression to conditionally set variable and flag.
         expr = SourceExpression::
            create_branch_if(initChk, initBoth, context, pos);
      }
         break;

      default:
         expr = exprSet;
         break;
      }
   }

   // If not done yet, add var now.
   if (!expr)
      expr = add_var(context, linkSpec, pos, nameSrc, nameObj, type, store,
                     addr, externDef, var);

   // Need to always generate the init var for array storage.
   if((store.type == STORE_MAPARRAY || store.type == STORE_WORLDARRAY ||
      store.type == STORE_GLOBALARRAY) && !initExpr)
   {
      initExpr = MakeInit(context, pos, var, nameObj, store, externDef);
   }

   return expr;
}

//
// make_var
//
static SourceExpression::Pointer make_var(SourceTokenizerC *in,
   SourceContext *context, LinkageSpecifier linkSpec, SourcePosition const &pos,
   VariableType::Reference type, StoreData const &store, bool externDef)
{
   // If not followed by an identifier, then don't try to read any names.
   // (This is needed for standalone struct definitions.)
   if (!in->peekType(SourceTokenC::TT_NAM))
      return SourceExpression::create_value_data
         (VariableType::get_bt_void(), context, pos);

   SourceExpression::Vector vars;

   // Read source name.
   std::string nameSrc = in->get(SourceTokenC::TT_NAM)->data;
   vars.push_back(make_var(in, context, linkSpec, pos, nameSrc, type, store, externDef));

   // Read in any additional variables.
   while (in->peekType(SourceTokenC::TT_COMMA))
   {
      in->get(SourceTokenC::TT_COMMA);

      nameSrc = in->get(SourceTokenC::TT_NAM)->data;
      vars.push_back(make_var(in, context, linkSpec, pos, nameSrc, type, store, externDef));
   }

   if (vars.size() == 1)
      return vars[0];
   else
      return SourceExpression::create_value_block(vars, context, pos);
}

//
// make_var
//
static SourceExpression::Pointer make_var(SourceTokenizerC *in,
   SourceContext *context, LinkageSpecifier linkSpec, SourcePosition const &pos,
   StoreData &store, bool externDef)
{
   // Read variable type.
   VariableType::Reference type = SourceExpressionDS::make_type(in, context);

   // STORE_CONST is used to signal automatic storetype selection.
   if (store.type == STORE_CONST)
   {
      if(context->getType() == SourceContext::CT_NAMESPACE)
      {
         if (type->getBasicType() == VariableType::BT_ARR)
            store.type = store_staticarray();
         else
            store.type = store_staticregister();
      }
      else
      {
         if (type->getBasicType() == VariableType::BT_ARR)
            store.type = store_autoarray();
         else
            store.type = store_autoregister();
      }
   }

   return make_var(in, context, linkSpec, pos, type, store, externDef);
}

//
// make_var
//
static SourceExpression::Pointer make_var(SourceTokenizerC *in,
   SourceContext *context, LinkageSpecifier linkSpec, bool linkCheck,
   SourcePosition const &pos, bool externDef)
{
   // Read storage class.
   StoreData store;

   if(in->peekType(SourceTokenC::TT_NAM) && context->isAddressSpace(in->peek()->data))
   {
      SourceTokenC::Reference tok = in->get();
      SourceContext::AddressSpace const &addr = context->getAddressSpace(tok->data, tok->pos);
      store.type = addr.store;
      store.name = addr.array;
   }
   else
      store.type = SourceExpressionDS::make_store(in, context, &store.area);

   if (linkCheck && (store.type == STORE_AUTO || store.type == STORE_REGISTER))
      linkSpec = LINKAGE_INTERN;

   return make_var(in, context, linkSpec, pos, store, externDef);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::make_extern_variable
//
SRCEXPDS_EXTERN_DEFN(variable)
{
   return make_var(in, context, linkSpec, false, tok->pos, true);
}

//
// SourceExpressionDS::make_keyword_variable
//
SRCEXPDS_KEYWORD_DEFN(variable)
{
   bool linkCheck = false;
   LinkageSpecifier linkSpec;

   if (tok->data == "__variable")
   {
      if(context->getType() == SourceContext::CT_NAMESPACE)
      {
         linkCheck = true;
         linkSpec = LINKAGE_DS;
      }
      else
         linkSpec = LINKAGE_INTERN;
   }
   else if (tok->data == "__extvar")
   {
      if (in->peekType(SourceTokenC::TT_STR))
         linkSpec = make_linkspec(in);
      else
         linkSpec = LINKAGE_DS;
   }
   else
      linkSpec = LINKAGE_INTERN;

   return make_var(in, context, linkSpec, linkCheck, tok->pos, false);
}

//
// SourceExpressionDS::make_keyword_variable_store
//
SRCEXPDS_KEYWORD_DEFN(variable_store)
{
   bool linkCheck = false;
   LinkageSpecifier linkSpec;

   in->unget(tok);

   if(context->getType() == SourceContext::CT_NAMESPACE)
   {
      linkCheck = true;
      linkSpec = LINKAGE_DS;
   }
   else
      linkSpec = LINKAGE_INTERN;

   return make_var(in, context, linkSpec, linkCheck, tok->pos, false);
}

//
// SourceExpressionDS::make_keyword_variable_type
//
SRCEXPDS_KEYWORD_DEFN(variable_type)
{
   LinkageSpecifier linkSpec;
   StoreData store = {STORE_CONST, NULL, ""};

   in->unget(tok);

   if(context->getType() == SourceContext::CT_NAMESPACE)
      linkSpec = LINKAGE_DS;
   else
      linkSpec = LINKAGE_INTERN;

   return make_var(in, context, linkSpec, tok->pos, store, false);
}

// EOF

