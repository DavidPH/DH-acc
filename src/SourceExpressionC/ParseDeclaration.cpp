//-----------------------------------------------------------------------------
//
// Copyright(C) 2012 David Hill
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
// C declaration parsing.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionC.hpp"

#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceFunction.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::IsDeclaration
//
bool SourceExpressionC::IsDeclaration(SRCEXPC_PARSE_ARG1)
{
   SourceTokenC::Reference tok = in->peek();

   if(tok->type != SourceTokenC::TT_NAM) return false;

   // static_assert-declaration
   if(tok->data == "_Static_assert") return true;

   // storage-class-specifier
   if(tok->data == "typedef")  return true;
   if(tok->data == "extern")   return true;
   if(tok->data == "static")   return true;
   if(tok->data == "auto")     return true;
   if(tok->data == "register") return true;

   // type-specifier
   // type-qualifier
   if(IsType(in, context)) return true;

   // function-specifier
   if(tok->data == "inline") return true;

   return false;
}

//
// SourceExpressionC::ParseInitializer
//
SRCEXPC_PARSE_DEFN_EXT(Initializer, VariableType::Pointer &type, bool root)
{
   SourcePosition pos = in->peek()->pos;

   if(in->dropType(SourceTokenC::TT_BRACE_O))
   {
      // Bracketed array initializer.
      if(type->getBasicType() == VariableType::BT_ARR)
      {
         VariableType::Pointer subType = type->getReturn();
         biguint index = 0;
         Vector  exprs;

         do
         {
            if(in->dropType(SourceTokenC::TT_BRACK_O))
            {
               index = ParseConditional(in, context)->makeObject()->resolveUNS();
               in->get(SourceTokenC::TT_BRACK_C);
               in->get(SourceTokenC::TT_EQUALS);
            }

            if(index >= exprs.size())
               exprs.resize(index+1, create_value_data(subType, context, pos));

            exprs[index++] = ParseInitializer(subType, false, in, context);
         }
         while(in->dropType(SourceTokenC::TT_COMMA) && !in->peekType(SourceTokenC::TT_BRACE_C));

         in->get(SourceTokenC::TT_BRACE_C);

         if(!type->getWidth()) type = type->getReturn()->getArray(exprs.size());

         return create_value_block(exprs, context, pos);
      }

      // Bracketed struct initializer.
      if(type->getBasicType() == VariableType::BT_STRUCT)
      {
         VariableType::Vector const &types = type->getTypes();
         biguint index = 0;
         Vector  exprs(types.size());

         // Preload the initializer with default values.
         for(biguint i = types.size(); i--;)
           exprs[i] = create_value_data(types[i], context, pos);

         do
         {
            if(in->dropType(SourceTokenC::TT_DOT))
            {
               SourceTokenC::Reference tok = in->get(SourceTokenC::TT_NAM);
               index = type->getIndex(tok->data, tok->pos);
               in->get(SourceTokenC::TT_EQUALS);
            }

            if(index < types.size())
            {
               VariableType::Pointer subType = types[index];
               exprs[index++] = ParseInitializer(subType, false, in, context);
            }
            else
            {
               VariableType::Pointer subType = VariableType::get_bt_void();
               exprs.push_back(ParseInitializer(subType, false, in, context));
            }
         }
         while(in->dropType(SourceTokenC::TT_COMMA) && !in->peekType(SourceTokenC::TT_BRACE_C));

         in->get(SourceTokenC::TT_BRACE_C);

         return create_value_block(exprs, context, pos);
      }

      // Bracketed union initializer.
      if(type->getBasicType() == VariableType::BT_UNION)
      {
         Error_P("stub");
      }

      // Bracketed scalar initializer.
      SourceExpression::Pointer expr = ParseAssignment(in, context);
      in->dropType(SourceTokenC::TT_COMMA);
      in->get(SourceTokenC::TT_BRACE_C);
      return expr;
   }
   else if(!root)
   {
      // Unbracketed array initializer.
      if(type->getBasicType() == VariableType::BT_ARR)
      {
         VariableType::Pointer subType = type->getReturn();
         bigsint width = type->getWidth();
         Vector  exprs;

         if(width) do
         {
            exprs.push_back(ParseInitializer(subType, false, in, context));
         }
         while(--width && in->dropType(SourceTokenC::TT_COMMA) &&
                         !in->peekType(SourceTokenC::TT_BRACE_C));

         return create_value_block(exprs, context, pos);
      }

      // Unbracketed struct initializer.
      if(type->getBasicType() == VariableType::BT_STRUCT)
      {
         VariableType::Vector const &types = type->getTypes();
         biguint index = 0;
         Vector  exprs(types.size());

         // Preload the initializer with default values.
         for(biguint i = types.size(); i--;)
           exprs[i] = create_value_data(types[i], context, pos);

         if(!types.empty()) do
         {
            VariableType::Pointer subType = types[index];
            exprs[index++] = ParseInitializer(subType, false, in, context);
         }
         while(index < types.size() && in->dropType(SourceTokenC::TT_COMMA) &&
                                      !in->peekType(SourceTokenC::TT_BRACE_C));

         return create_value_block(exprs, context, pos);
      }

      // Unbracketed union initializer.
      if(type->getBasicType() == VariableType::BT_UNION)
      {
         Error_P("stub");
      }

      // Unbracketed scalar initializer.
      return ParseAssignment(in, context);
   }
   else
      return ParseAssignment(in, context);
}

//
// SourceExpressionC::ParseVariable
//
SRCEXPC_PARSE_DEFN_EXT(Variable, DeclarationSpecifiers const &spec, Declarator &decl)
{
   SourcePosition pos = in->peek()->pos;

   // Determine linkage.
   LinkageSpecifier linkage;

   if(spec.storage == SC_EXTERN || (spec.storage == SC_NONE && spec.external))
      linkage = LINKAGE_C;
   else
      linkage = LINKAGE_INTERN;

   // Determine storage.
   StoreType store;
   if(spec.storage == SC_NONE)
      store = spec.external ? STORE_STATIC : STORE_AUTO;
   else if(spec.storage == SC_AUTO)
      store = STORE_AUTO;
   else if(spec.storage == SC_REGISTER)
      store = STORE_REGISTER;
   else
      store = STORE_STATIC;

   // Function prototype.
   if(VariableType::is_bt_function(decl.type->getBasicType()))
   {
      if(spec.storage == SC_AUTO || spec.storage == SC_REGISTER)
         Error_P("auto or register in function prototype");

      // Determine parameters.
      bigsint returnSize = decl.type->getReturn()->getSize(pos);
      bigsint paramSize = 0;
      VariableType::Vector const &paramTypes = decl.type->getTypes();
      for(VariableType::Vector::const_iterator param = paramTypes.begin(),
          end = paramTypes.end(); param != end && *param; ++param)
      {
         paramSize += (*param)->getSize(pos);
      }

      std::string nameObj = context->makeNameObj(decl.name, linkage, paramTypes);
      std::string label = nameObj + "::$label";

      if(decl.funcAttr.script)
      {
         if(decl.funcAttr.scriptName.empty()) decl.funcAttr.scriptName = nameObj;

         ObjectData_Script::add(nameObj, label, decl.funcAttr.scriptType,
            decl.funcAttr.scriptFlag, paramSize, NULL, linkage != LINKAGE_INTERN,
            decl.funcAttr.scriptAddr, decl.funcAttr.scriptName);
      }
      else
         ObjectData_Function::add(nameObj, label, paramSize, returnSize, NULL);

      SourceFunction::Reference func = SourceFunction::FindFunction(
         SourceVariable::create_constant(decl.name, decl.type, nameObj, pos));

      context->addFunction(func);

      return create_value_function(func, context, pos);
   }

   std::string nameObj = context->makeNameObj(decl.name, LINKAGE_INTERN);

   // Variable definition with initializer.
   if(in->dropType(SourceTokenC::TT_EQUALS))
   {
      SourceExpression::Pointer init = ParseInitializer(decl.type, true, in, context);

      SourceVariable::Pointer var = SourceVariable::create_variable(decl.name,
         decl.type, nameObj, store, pos);

      context->addVar(var, false, linkage != LINKAGE_INTERN);

      SourceExpression::Pointer expr = create_value_variable(var, context, pos);

      expr = create_binary_assign_const(expr, init, context, pos);

      if(store != STORE_AUTO && store != STORE_REGISTER)
      {
         // if(!isInit) isInit = true, expr = init;

         std::string             isInitName = nameObj + "::$isInit";
         VariableType::Reference isInitType = VariableType::get_bt_bit_hrd();

         SourceExpression::Pointer isInitExpr;

         if(store == STORE_STATIC        || store == STORE_MAPREGISTER ||
            store == STORE_WORLDREGISTER || store == STORE_GLOBALREGISTER)
         {
            // Create an extra variable to store isInit.

            SourceVariable::Pointer isInitVar  = SourceVariable::create_variable("",
               isInitType, isInitName, store, pos);

            context->addVar(isInitVar, false, false);

            isInitExpr = create_value_variable(isInitVar, context, pos);
         }
         else
         {
            // For array storages, use the null index for isInit.

            isInitType = isInitType->setStorage(store, nameObj)->getPointer();

            isInitExpr = create_value_int(0, context, pos);
            isInitExpr = create_value_cast_explicit(isInitExpr, isInitType, context, pos);
            isInitExpr = create_unary_dereference(isInitExpr, context, pos);
         }

         SourceExpression::Pointer isInitNot = create_branch_not(isInitExpr, context, pos);
         SourceExpression::Pointer isInitSet = create_binary_assign(isInitExpr,
            create_value_int(1, context, pos), context, pos);

         expr = create_binary_pair(isInitSet, expr, context, pos);
         expr = create_branch_if(isInitNot, expr, context, pos);
      }

      return expr;
   }
   // Variable declaration or definition.
   else
   {
      SourceVariable::Pointer var = SourceVariable::create_variable(decl.name,
         decl.type, nameObj, store, pos);

      context->addVar(var, spec.storage == SC_EXTERN, linkage != LINKAGE_INTERN);

      return create_value_variable(var, context, pos);
   }
}

//
// SourceExpressionC::ParseTypedef
//
SRCEXPC_PARSE_DEFN_EXT(Typedef, DeclarationSpecifiers const &spec)
{
   SourcePosition pos = in->peek()->pos;

   if(spec.hasFunctionSpecifier())
      Error_P("typedef with function-specifier");

   // A typedef must include at least one declarator.
   if(in->peekType(SourceTokenC::TT_SEMICOLON))
      Error_P("expected declarator");

   // Read a series of declarators as typedefs.
   do
   {
      Declarator decl = ParseDeclarator(spec.type, in, context);

      if(decl.name.empty()) Error_P("expected declarator");

      context->getVariableType_typedef(decl.name, decl.type, pos);
   }
   while(in->dropType(SourceTokenC::TT_COMMA));

   in->get(SourceTokenC::TT_SEMICOLON);

   return create_value_data(context, pos);
}

//
// SourceExpressionC::ParseStaticAssert
//
SRCEXPC_PARSE_DEFN_HALF(StaticAssert)
{
   // _Static_assert ( constant-expression , string-literal ) ;

   SourcePosition pos = in->get(SourceTokenC::TT_NAM, "_Static_assert")->pos;

   in->get(SourceTokenC::TT_PAREN_O);

   bigsint i = ParseConditional(in, context)->makeObject()->resolveINT();

   in->get(SourceTokenC::TT_COMMA);

   std::string s = in->get(SourceTokenC::TT_STR)->data;

   in->get(SourceTokenC::TT_PAREN_C);

   in->get(SourceTokenC::TT_SEMICOLON);

   if(!i) Error_P("static assertion failed: %s", s.c_str());

   return create_value_data(context, pos);
}

//
// SourceExpressionC::ParseDeclaration
//
SRCEXPC_PARSE_DEFN_HALF(Declaration)
{
   SourcePosition pos = in->peek()->pos;

   if(in->peekType(SourceTokenC::TT_NAM, "_Static_assert"))
      return ParseStaticAssert(in, context);

   DeclarationSpecifiers spec = ParseDeclarationSpecifiers(in, context);

   if(spec.storage == SC_NONE && in->dropType(SourceTokenC::TT_SEMICOLON))
      return create_value_data(context, pos);

   if(spec.storage == SC_TYPEDEF)
      return ParseTypedef(spec, in, context);

   // Parse variable declarations and function prototypes.

   Vector exprs;

   do
   {
      Declarator decl = ParseDeclarator(spec.type, in, context);

      exprs.push_back(ParseVariable(spec, decl, in, context));
   }
   while(in->dropType(SourceTokenC::TT_COMMA));

   in->get(SourceTokenC::TT_SEMICOLON);

   return create_value_block(exprs, context, pos);
}

//
// SourceExpressionC::ParseFunction
//
SRCEXPC_PARSE_DEFN_EXT(Function, DeclarationSpecifiers const &spec, Declarator &decl)
{
   SourcePosition pos = in->peek()->pos;

   if(spec.functionInline)
      Error_P("inline not yet supported");

   LinkageSpecifier linkage = spec.storage == SC_STATIC ? LINKAGE_INTERN : LINKAGE_C;

   SourceContext::Reference funcContext = SourceContext::create(context,
      decl.funcAttr.script ? SourceContext::CT_SCRIPT : SourceContext::CT_FUNCTION);

   // Set return type of context.
   funcContext->setReturnType(decl.type->getReturn());

   // Add parameters.
   bigsint returnSize = decl.type->getReturn()->getSize(pos);
   bigsint paramSize = 0;
   VariableType::Vector paramTypes;
   for(std::vector<Parameter>::const_iterator param = decl.param.begin(),
       end = decl.param.end(); param != end; ++param)
   {
      VariableType::Pointer type = param->decl.type;

      std::string const &nameSrc = param->decl.name;
      std::string nameObj = funcContext->makeNameObj(nameSrc, LINKAGE_INTERN);

      StoreType store = param->spec.storage == SC_REGISTER ? STORE_REGISTER : STORE_AUTO;

      funcContext->addVar(SourceVariable::create_variable(nameSrc, type, nameObj,
         store, pos), false, false);

      paramSize += type->getSize(pos);
      paramTypes.push_back(type->setStorage(store));
   }

   // nameObj
   std::string nameObj = context->makeNameObj(decl.name, linkage, paramTypes);
   std::string label = nameObj + "::$label";

   // __func__
   funcContext->addVar(SourceVariable::create_constant("__func__",
      VariableType::get_bt_str(), ObjectData_String::add(decl.name), pos), false, false);

   if(decl.funcAttr.script)
   {
      if(decl.funcAttr.scriptName.empty()) decl.funcAttr.scriptName = nameObj;

      ObjectData_Script::add(nameObj, label, decl.funcAttr.scriptType,
         decl.funcAttr.scriptFlag, paramSize, funcContext, linkage != LINKAGE_INTERN,
         decl.funcAttr.scriptAddr, decl.funcAttr.scriptName);
   }
   else
      ObjectData_Function::add(nameObj, label, paramSize, returnSize, funcContext);

   SourceFunction::Reference func = SourceFunction::FindFunction(
      SourceVariable::create_constant(decl.name, decl.type, nameObj, pos));

   context->addFunction(func);

   // funcExpr
   func->setBody(ParseStatement(in, funcContext), paramTypes, pos);

   return create_value_function(func, context, pos);
}

//
// SourceExpressionC::ParseExternalDeclaration
//
SRCEXPC_PARSE_DEFN_HALF(ExternalDeclaration)
{
   SourcePosition pos = in->peek()->pos;

   if(in->peekType(SourceTokenC::TT_NAM, "_Static_assert"))
      return ParseStaticAssert(in, context);

   // library-declaration:
   //   __library ( string-literal ) ;
   if(in->dropType(SourceTokenC::TT_NAM, "__library"))
   {
      in->get(SourceTokenC::TT_PAREN_O);
      ObjectExpression::set_library(in->get(SourceTokenC::TT_STR)->data);
      in->get(SourceTokenC::TT_PAREN_C);
      in->get(SourceTokenC::TT_SEMICOLON);
   }

   DeclarationSpecifiers spec = ParseDeclarationSpecifiers(in, context);
   spec.external = true;

   if(spec.storage == SC_NONE && in->dropType(SourceTokenC::TT_SEMICOLON))
      return create_value_data(context, pos);

   if(spec.storage == SC_TYPEDEF)
      return ParseTypedef(spec, in, context);

   if(spec.storage == SC_AUTO || spec.storage == SC_REGISTER)
      Error_P("auto or register in external-declaration");

   Declarator decl = ParseDeclarator(spec.type, in, context);

   if(VariableType::is_bt_function(decl.type->getBasicType()) &&
      in->peekType(SourceTokenC::TT_BRACE_O))
   {
      return ParseFunction(spec, decl, in, context);
   }

   // Parse variable declarations and function prototypes.

   Vector exprs;

   exprs.push_back(ParseVariable(spec, decl, in, context));

   while(in->dropType(SourceTokenC::TT_COMMA))
   {
      decl = ParseDeclarator(spec.type, in, context);

      exprs.push_back(ParseVariable(spec, decl, in, context));
   }

   in->get(SourceTokenC::TT_SEMICOLON);

   return create_value_block(exprs, context, pos);
}

// EOF

