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
// Static Functions                                                           |
//

//
// CreateIsInit
//
// Creates an extra variable to store isInit.
//
static SourceExpression::Pointer CreateIsInit(std::string const &nameObj,
   std::string const &nameArr, StoreType store, bool externDef, SRCEXP_EXPR_ARGS)
{
   std::string             isInitName = nameObj + "::$isInit";
   VariableType::Reference isInitType = VariableType::get_bt_bit_hrd();
   SourceVariable::Pointer isInitVar  = SourceVariable::create_variable(
      isInitName, isInitType, isInitName, nameArr, store, pos);
   context->addVar(isInitVar, LINKAGE_INTERN, externDef);

   return SourceExpression::create_value_variable(isInitVar, context, pos);
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::CreateObject
//
SourceExpression::Pointer SourceExpressionC::CreateObject(
   std::string const &nameSrc, std::string const &nameObj, VariableType *type,
   LinkageSpecifier linkage, std::string const &nameArr, StoreType store,
   SourceExpression *init, bool externDef, SRCEXP_EXPR_ARGS)
{
   SourceVariable::Pointer var = SourceVariable::create_variable(nameSrc, type,
      nameObj, nameArr, store, pos);

   context->addVar(var, linkage, externDef);

   SourceExpression::Pointer expr = create_value_variable(var, context, pos);

   // isInit (Always needed by array storage.)
   SourceExpression::Pointer isInit;
   if(store == STORE_MAPARRAY || store == STORE_WORLDARRAY || store == STORE_GLOBALARRAY)
      isInit = CreateIsInit(nameObj, nameArr, store, externDef, context, pos);

   if(!init) return expr;

   // Map array storage variables can be initialized via the AINI chunk. In
   // which case, the below init code is unneeded.
   if(store == STORE_MAPARRAY && init->canMakeObject() &&
      ObjectData::ArrayVar::InitMap(nameObj, type, init->makeObject()))
   {
      return expr;
   }

   // If we've reached this point, then we need to initialize the object
   // manually. However, the resulting expression must still be an lvalue. (For
   // things such as C99 anonymous objects.) Basically, this is done using the
   // comma operator to yield a pointer to the object.
   //   int i = 5;      // original code
   //   *((i = 5), &i); // resulting expression

   // Acquire the reference now so that expr can be used as a temporary.
   SourceExpression::Pointer exprRef = create_unary_reference(expr, context, pos);

   expr = create_binary_assign_const(expr, init, context, pos);

   // Herein lies the rub. For static-lifetime objects, we need to only
   // initialize once. So we use an extra isInit variable to track this.
   if(store != STORE_AUTO && store != STORE_REGISTER)
   {
      // isInit
      if(!isInit)
         isInit = CreateIsInit(nameObj, nameArr, store, externDef, context, pos);

      // !isInit
      SourceExpression::Pointer isInitNot = create_branch_not(isInit, context, pos);

      // isInit = true
      SourceExpression::Pointer isInitSet = create_binary_assign(isInit,
         create_value_int(1, context, pos), context, pos);

      // if(!isInit) isInit = true, var = init;
      expr = create_binary_pair(isInitSet, expr, context, pos);
      expr = create_branch_if(isInitNot, expr, context, pos);
   }

   // *(init, &var)
   expr = create_binary_pair(expr, exprRef, context, pos);
   expr = create_unary_dereference(expr, context, pos);

   return expr;
}

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
// SourceExpressionC::ParseAddressSpace
//
// address_space-declaration:
//   <__address_space> storage-class-specifier(opt) address_space-specifier
//     identifier address_space-expression(opt) ;
//
SRCEXPC_PARSE_DEFN_HALF(AddressSpace)
{
   SourceContext::AddressSpace addr;

   // <__address_space>
   SourcePosition pos = in->get(SourceTokenC::TT_NAM, "__address_space")->pos;

   // storage-class-specifier(opt)
   StorageClass storage;
   if(in->dropType(SourceTokenC::TT_NAM, "extern"))
      storage = SC_EXTERN;
   else if(in->dropType(SourceTokenC::TT_NAM, "static"))
      storage = SC_STATIC;
   else
      storage = SC_NONE;

   LinkageSpecifier linkage = storage == SC_STATIC ? LINKAGE_INTERN : LINKAGE_C;

   // address_space-specifier
   auto spaceTok = in->get(SourceTokenC::TT_NAM);
   std::string const &space = spaceTok->data;
   if(space == "map_array" || space == "__map_array" || space == "__map_array__")
      addr.store = STORE_MAPARRAY;
   else if(space == "world_array" || space == "__world_array" || space == "__world_array__")
      addr.store = STORE_WORLDARRAY;
   else if(space == "global_array" || space == "__global_array" || space == "__global_array__")
      addr.store = STORE_GLOBALARRAY;
   else
      Error(spaceTok->pos, "expected address-space-specifier");

   // identifier
   std::string nameSrc = in->get(SourceTokenC::TT_NAM)->data;

   addr.array = context->makeNameObj(nameSrc, linkage);

   // address_space-expression(opt)
   bigsint number;
   if(in->dropType(SourceTokenC::TT_EQUALS))
      number = ParseConditional(in, context)->makeObject()->resolveINT();
   else
      number = -1;

   // ;
   in->get(SourceTokenC::TT_SEMICOLON);

   bool externDef = storage == SC_EXTERN && number == -1;

   switch(addr.store)
   {
   case STORE_MAPARRAY:
      ObjectData::Array::AddMap(addr.array, linkage, externDef, number);
      break;

   case STORE_WORLDARRAY:
      ObjectData::Array::AddWorld(addr.array, linkage, externDef, number);
      break;

   case STORE_GLOBALARRAY:
      ObjectData::Array::AddGlobal(addr.array, linkage, externDef, number);
      break;

   default:
      Error_P("unexpected address-space-specifier");
   }

   context->addAddressSpace(nameSrc, addr);

   return create_value_data(context, pos);
}

//
// SourceExpressionC::ParseInitializer
//
SRCEXPC_PARSE_DEFN_EXT(Initializer, VariableType::Pointer &type, bool root)
{
   SourcePosition pos = in->peek()->pos;

   // char[] = ""
   if(in->peekType(SourceTokenC::TT_STR) &&
      type->getBasicType() == VariableType::BT_ARR &&
      type->getReturn()->getBasicType() == VariableType::BT_CHR)
   {
      std::string data = in->get()->data; data += '\0';

      VariableType::Pointer subType = type->getReturn();
      Vector  exprs;

      for(std::string::iterator itr = data.begin(), end = data.end(); itr != end; ++itr)
      {
         exprs.push_back(create_value_char(*itr, context, pos));
      }

      if(!type->getWidth()) type = type->getReturn()->getArray(exprs.size());

      return create_value_block(exprs, context, pos);
   }

   if(in->dropType(SourceTokenC::TT_BRACE_O))
   {
      // char[] = {""}
      if(in->peekType(SourceTokenC::TT_STR) &&
         type->getBasicType() == VariableType::BT_ARR &&
         type->getReturn()->getBasicType() == VariableType::BT_CHR)
      {
         std::string data = in->get()->data; data += '\0';
         in->get(SourceTokenC::TT_BRACE_C);

         VariableType::Pointer subType = type->getReturn();
         Vector  exprs;

         for(std::string::iterator itr = data.begin(), end = data.end(); itr != end; ++itr)
            exprs.push_back(create_value_char(*itr, context, pos));

         if(!type->getWidth()) type = type->getReturn()->getArray(exprs.size());

         return create_value_block(exprs, context, pos);
      }

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
// SourceExpressionC::ParseInitDeclarator
//
SRCEXPC_PARSE_DEFN_EXT(InitDeclarator, DeclarationSpecifiers const &spec)
{
   Declarator decl = ParseDeclarator(spec.type, in, context);
   return ParseInitDeclarator(spec, decl, in, context);
}

//
// SourceExpressionC::ParseInitDeclarator
//
SRCEXPC_PARSE_DEFN_EXT(InitDeclarator, DeclarationSpecifiers const &spec, Declarator &decl)
{
   SourcePosition pos = in->peek()->pos;

   // Determine linkage.
   LinkageSpecifier linkage;

   if(spec.storage == SC_EXTERN || (spec.storage == SC_NONE && spec.external))
      linkage = LINKAGE_C;
   else
      linkage = LINKAGE_INTERN;

   // Determine storage.
   std::string nameArr = decl.type->getStoreArea();
   StoreType store = decl.type->getStoreType();

   if(store == STORE_NONE)
   {
      if(spec.storage == SC_NONE)
         store = spec.external ? STORE_STATIC : STORE_AUTO;
      else if(spec.storage == SC_AUTO)
         store = STORE_AUTO;
      else if(spec.storage == SC_REGISTER)
         store = STORE_REGISTER;
      else
         store = STORE_STATIC;
   }

   // Function prototype.
   if(VariableType::IsTypeFunction(decl.type->getBasicType()))
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

      ObjectExpression::Pointer obj;

      if(decl.funcAttr.asmfun)
         obj = ObjectExpression::create_value_ocs(decl.funcAttr.asmfunCode, pos);

      else if(decl.funcAttr.lnspec || decl.funcAttr.native)
         obj = ObjectExpression::create_value_int(decl.funcAttr.scriptAddr, pos);

      else if(decl.funcAttr.script)
      {
         if(decl.funcAttr.scriptName.empty()) decl.funcAttr.scriptName = nameObj;

         ObjectData::Script::Add(nameObj, label, paramSize, returnSize, nullptr,
            linkage, decl.funcAttr.scriptAddr, decl.funcAttr.scriptName,
            decl.funcAttr.scriptType, decl.funcAttr.scriptFlag);
      }
      else
         ObjectData::Function::Add(nameObj, label, paramSize, returnSize, nullptr, linkage);

      SourceVariable::Pointer var;
      if(obj) var = SourceVariable::create_constant(decl.name, decl.type,     obj, pos);
      else    var = SourceVariable::create_constant(decl.name, decl.type, nameObj, pos);

      SourceFunction::Reference func = SourceFunction::FindFunction(var);

      context->addFunction(func);

      return create_value_function(func, context, pos);
   }

   std::string nameObj = context->makeNameObj(decl.name, linkage);

   // Variable definition with initializer.
   if(in->dropType(SourceTokenC::TT_EQUALS))
   {
      SourceExpression::Pointer init = ParseInitializer(decl.type, true, in, context);

      return CreateObject(decl.name, nameObj, decl.type, linkage, nameArr, store,
                          init, false, context, pos);
   }
   // Variable declaration or definition.
   else
   {
      return CreateObject(decl.name, nameObj, decl.type, linkage, nameArr, store,
                          NULL, spec.storage == SC_EXTERN, context, pos);
   }
}

//
// SourceExpressionC::ParseLibrary
//
// library-declaration:
//   <__library> ( string-literal ) ;
//
SRCEXPC_PARSE_DEFN_HALF(Library)
{
   SourcePosition pos = in->get(SourceTokenC::TT_NAM, "__library")->pos;

   in->get(SourceTokenC::TT_PAREN_O);
   ObjectExpression::set_library(in->get(SourceTokenC::TT_STR)->data);
   in->get(SourceTokenC::TT_PAREN_C);
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
// SourceExpressionC::ParseDeclaration
//
SRCEXPC_PARSE_DEFN_EXT(Declaration, bool external)
{
   SourcePosition pos = in->peek()->pos;

   // static_assert-declaration
   if(in->peekType(SourceTokenC::TT_NAM, "_Static_assert"))
      return ParseStaticAssert(in, context);

   // address_space-declaration
   if(in->peekType(SourceTokenC::TT_NAM, "__address_space"))
      return ParseAddressSpace(in, context);

   // library-declaration
   if(external && in->peekType(SourceTokenC::TT_NAM, "__library"))
      return ParseLibrary(in, context);

   // declaration-specifiers
   DeclarationSpecifiers spec = ParseDeclarationSpecifiers(in, context);
   spec.external = external;

   // declaration-specifiers ;
   if(spec.storage == SC_NONE && in->dropType(SourceTokenC::TT_SEMICOLON))
   {
      // struct-or-union-specifier identifier ;
      if(!spec.type->getName().empty())
      {
         if(spec.type->getBasicType() == VariableType::BT_STRUCT)
            context->addVariableType_struct(spec.type->getName());
         else if(spec.type->getBasicType() == VariableType::BT_UNION)
            context->addVariableType_union(spec.type->getName());
      }

      return create_value_data(context, pos);
   }

   // typedef
   if(spec.storage == SC_TYPEDEF)
      return ParseTypedef(spec, in, context);

   // Constraint.
   if(spec.external && (spec.storage == SC_AUTO || spec.storage == SC_REGISTER))
      Error_P("auto or register in external-declaration");

   // init-declarator or function-definition
   Vector exprs;
   {
      // declarator
      Declarator decl = ParseDeclarator(spec.type, in, context);

      // function-definition
      if(spec.external && in->peekType(SourceTokenC::TT_BRACE_O) &&
         VariableType::IsTypeFunction(decl.type->getBasicType()))
      {
         return ParseFunction(spec, decl, in, context);
      }

      // init-declarator
      exprs.push_back(ParseInitDeclarator(spec, decl, in, context));
   }

   // , init-declarator
   while(in->dropType(SourceTokenC::TT_COMMA))
      exprs.push_back(ParseInitDeclarator(spec, in, context));

   // ;
   in->get(SourceTokenC::TT_SEMICOLON);

   return create_value_block(exprs, context, pos);
}

//
// SourceExpressionC::ParseDeclaration
//
SRCEXPC_PARSE_DEFN_HALF(Declaration)
{
   return ParseDeclaration(false, in, context);
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
         store, pos), LINKAGE_INTERN, false);

      paramSize += type->getSize(pos);
      paramTypes.push_back(type->setStorage(store));
   }

   // nameObj
   std::string nameObj = context->makeNameObj(decl.name, linkage, paramTypes);
   std::string label = nameObj + "::$label";

   // __func__
   funcContext->addVar(SourceVariable::create_constant("__func__",
      VariableType::get_bt_str(), ObjectData::String::Add(decl.name), pos),
      LINKAGE_INTERN, false);

   if(decl.funcAttr.script)
   {
      if(decl.funcAttr.scriptName.empty()) decl.funcAttr.scriptName = nameObj;

      ObjectData::Script::Add(nameObj, label, paramSize, returnSize, funcContext,
         linkage, decl.funcAttr.scriptAddr, decl.funcAttr.scriptName,
         decl.funcAttr.scriptType, decl.funcAttr.scriptFlag);
   }
   else
      ObjectData::Function::Add(nameObj, label, paramSize, returnSize, funcContext, linkage);

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
   return ParseDeclaration(true, in, context);
}

// EOF

