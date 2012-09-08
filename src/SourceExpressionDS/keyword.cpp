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
// DS handling of single sub-expressions.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionDS.hpp"

#include "../ObjectCode.hpp"
#include "../ObjectData.hpp"
#include "../ObjectExpression.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceFunction.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
#include "../SourceVariable.hpp"
#include "../VariableData.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::is_keyword
//
bool SourceExpressionDS::is_keyword(std::string const &data)
{
   return expr_keyword.find(data) != expr_keyword.end();
}

//
// SourceExpressionDS::make_keyword_address_space
//
// address_space-declaration:
//   <__address_space> address_space-extern(opt) storage-class identifier
//   <__address_space_decl> address_space-extern(opt) storage-class identifier
//
// address_space-extern:
//   <extern>
//   <extern> string-literal
//   <static>
//
SRCEXPDS_KEYWORD_DEFN(address_space)
{
   SourcePosition pos = in->peek()->pos;

   bool externDef = tok->data == "__address_space_decl";

   SourceContext::AddressSpace addr;

   // address_space-extern(opt)
   LinkageSpecifier linkage;
   if(in->peekType(SourceTokenC::TT_NAM, "extern"))
   {
      if(in->peekType(SourceTokenC::TT_STR))
         linkage = make_linkspec(in);
      else
         linkage = LINKAGE_DS;
   }
   else if(in->peekType(SourceTokenC::TT_NAM, "static"))
      linkage = LINKAGE_INTERN;
   else
      linkage = LINKAGE_DS;

   // storage-class
   ObjectExpression::Pointer area;
   addr.store = make_store(in, context, &area);

   // identifier
   std::string nameSrc = in->get(SourceTokenC::TT_NAM)->data;
   addr.array = context->makeNameObj(nameSrc, linkage);

   // Create or declare the array.
   bigsint number = area ? area->resolveINT() : -1;
   switch(addr.store)
   {
   case STORE_MAPARRAY:
      ObjectData_Array::AddMap(addr.array, linkage, externDef, number);
      break;

   case STORE_WORLDARRAY:
      ObjectData_Array::AddWorld(addr.array, linkage, externDef, number);
      break;

   case STORE_GLOBALARRAY:
      ObjectData_Array::AddGlobal(addr.array, linkage, externDef, number);
      break;

   default:
      Error_P("expected array storage-class");
   }

   // Add the address space to the context.
   context->addAddressSpace(nameSrc, addr);

   return create_value_data(context, pos);
}

//
// SourceExpressionDS::make_keyword_break
//
SRCEXPDS_KEYWORD_DEFN(break)
{
   if(in->peekType(SourceTokenC::TT_NAM))
      return create_branch_break(in->get()->data, context, tok->pos);
   else
      return create_branch_break(context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_cast
//
SRCEXPDS_KEYWORD_DEFN(cast)
{
   in->get(SourceTokenC::TT_CMP_LT);
   VariableType::Reference type = make_type(in, context);
   in->get(SourceTokenC::TT_CMP_GT);

   in->get(SourceTokenC::TT_PAREN_O);
   SourceExpression::Pointer expr = make_expression(in, context);
   in->get(SourceTokenC::TT_PAREN_C);

   if (tok->data == "const_cast")
      return create_value_cast_qualifier(expr, type, context, tok->pos);

   if (tok->data == "reinterpret_cast")
      return create_value_cast_raw(expr, type, context, tok->pos);

   if (tok->data == "static_cast")
      return create_value_cast_static(expr, type, context, tok->pos);

   if (tok->data == "__force_cast")
      return create_value_cast_force(expr, type, context, tok->pos);

   if(tok->data == "__store_cast")
      return create_value_cast_storage(expr, type, context, tok->pos);

   Error(tok->pos, "unknown cast: %s", tok->data.c_str());
}

//
// SourceExpressionDS::make_keyword_constexpr
//
SRCEXPDS_KEYWORD_DEFN(constexpr)
{
   VariableType::Reference type = make_type(in, context);
   std::string name = in->get(SourceTokenC::TT_NAM)->data;
   in->get(SourceTokenC::TT_EQUALS);
   SourceExpression::Pointer data = make_assignment(in, context);

   if (type->getBasicType() == VariableType::BT_VOID)
      type = data->getType();
   else
      data = create_value_cast_implicit(data, type, context, tok->pos);

   SourceVariable::Pointer var = SourceVariable::create_constant
      (name, type, data->makeObject(), tok->pos);

   context->addVar(var, LINKAGE_INTERN, false);

   return create_value_variable(var, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_continue
//
SRCEXPDS_KEYWORD_DEFN(continue)
{
   if(in->peekType(SourceTokenC::TT_NAM))
      return create_branch_continue(in->get()->data, context, tok->pos);
   else
      return create_branch_continue(context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_do
//
SRCEXPDS_KEYWORD_DEFN(do)
{
   // Create a context for the entire loop.
   SourceContext::Reference contextLoop =
      SourceContext::create(context, SourceContext::CT_LOOP);

   // Check for named loop.
   if(in->peekType(SourceTokenC::TT_COLON))
   {
      in->get();
      contextLoop->setLabel(in->get(SourceTokenC::TT_NAM)->data);
   }

   // Read body.
   SourceContext::Reference contextBody =
      SourceContext::create(contextLoop, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprBody = make_prefix(in, contextBody);

   in->get(SourceTokenC::TT_NAM, "while");

   // Read condition.
   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(contextBody, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond = make_expression(in, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   return create_branch_do(exprCond, exprBody, contextLoop, tok->pos);
}

//
// SourceExpressionDS::make_keyword_for
//
SRCEXPDS_KEYWORD_DEFN(for)
{
   // Create a context for the entire loop.
   SourceContext::Reference contextLoop =
      SourceContext::create(context, SourceContext::CT_LOOP);

   // Check for named loop.
   if(in->peekType(SourceTokenC::TT_COLON))
   {
      in->get();
      contextLoop->setLabel(in->get(SourceTokenC::TT_NAM)->data);
   }

   in->get(SourceTokenC::TT_PAREN_O);

   // Read initializer.
   SourceContext::Reference contextInit =
      SourceContext::create(contextLoop, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprInit = make_expression(in, contextInit);

   in->get(SourceTokenC::TT_SEMICOLON);

   // Read condition.
   SourceContext::Reference contextCond =
      SourceContext::create(contextInit, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond;

   if(in->peekType(SourceTokenC::TT_SEMICOLON))
      exprCond = create_value_int(1, contextCond, tok->pos);
   else
      exprCond = make_expression(in, contextCond);

   in->get(SourceTokenC::TT_SEMICOLON);

   // Read iterator.
   SourceContext::Reference contextIter =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprIter = make_expression(in, contextIter);

   in->get(SourceTokenC::TT_PAREN_C);

   // Read body.
   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprBody = make_expression(in, contextBody);

   return create_branch_for(exprCond, exprBody, exprIter, exprInit, contextLoop, tok->pos);
}

//
// SourceExpressionDS::make_keyword_goto
//
SRCEXPDS_KEYWORD_DEFN(goto)
{
   SourceTokenC::Reference gotoToken = in->get();

   if(gotoToken->type == SourceTokenC::TT_MUL)
      return create_branch_goto(make_expression(in, context), context, tok->pos);

   if(gotoToken->type != SourceTokenC::TT_NAM)
      Error(gotoToken->pos, "expected TT_MUL or TT_NAM");

   std::string label;

   if(gotoToken->data == "case")
   {
      label = context->getLabelCase(make_expression(in, context)
              ->makeObject()->resolveINT(), tok->pos);
   }
   else if(gotoToken->data == "default")
      label = context->getLabelCaseDefault(tok->pos);
   else
      label = context->getLabelGoto(gotoToken->data, gotoToken->pos);

   return create_branch_goto(label, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_if
//
SRCEXPDS_KEYWORD_DEFN(if)
{
   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(context, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond = make_expression(in, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprBody = make_expression(in, contextBody);

   if (in->peekType(SourceTokenC::TT_NAM, "else"))
   {
      in->get();

      SourceContext::Reference contextElse =
         SourceContext::create(contextCond, SourceContext::CT_BLOCK);
      SourceExpression::Pointer exprElse = make_expression(in, contextElse);

      return create_branch_if(exprCond, exprBody, exprElse, context, tok->pos);
   }
   else
      return create_branch_if(exprCond, exprBody, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_library
//
SRCEXPDS_KEYWORD_DEFN(library)
{
   in->get(SourceTokenC::TT_PAREN_O);
   ObjectExpression::set_library(in->get(SourceTokenC::TT_STR)->data);
   in->get(SourceTokenC::TT_PAREN_C);

   return create_value_data(context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_linespec
//
SRCEXPDS_KEYWORD_DEFN(linespec)
{
   ArgList args; make_arglist(in, context, &args);

   in->get(SourceTokenC::TT_AT); ObjectExpression::Pointer obj =
      make_prefix(in, context)->makeObject();

   VariableType::Reference varType = tok->data == "__linespec"
      ? VariableType::get_bt_fun_lin(args.types, args.retn) : tok->data == "__native"
      ? VariableType::get_bt_fun_nat(args.types, args.retn)
      : VariableType::get_bt_fun_asm(args.types, args.retn);

   SourceVariable::Pointer var = SourceVariable::create_constant
      (args.name, varType, obj, tok->pos);

   SourceFunction::Reference func = SourceFunction::FindFunction(var, args.args);

   if (!args.name.empty()) context->addFunction(func);
   return create_value_function(func, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_literal
//
SRCEXPDS_KEYWORD_DEFN(literal)
{
   (void)in;

   ObjectExpression::Pointer obj;
   SourceVariable::Pointer var;
   VariableType::Pointer type;

   if(tok->data == "true")
   {
      obj = ObjectExpression::create_value_int(1, tok->pos);
      type = VariableType::get_bt_bit_hrd();
   }
   else if(tok->data == "false")
   {
      obj = ObjectExpression::create_value_int(0, tok->pos);
      type = VariableType::get_bt_bit_hrd();
   }
   else if(tok->data == "nullptr")
   {
      obj = ObjectExpression::create_value_uns(0, tok->pos);
      type = VariableType::get_bt_ptr_nul();
   }
   else
      Error(tok->pos, "unknown keyword literal '%s'", tok->data.c_str());

   var = SourceVariable::create_literal(type, obj, tok->pos);
   return create_value_variable(var, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_namespace
//
SRCEXPDS_KEYWORD_DEFN(namespace)
{
   SourceContext::Pointer contextNS;

   if(in->peekType(SourceTokenC::TT_NAM))
      contextNS = SourceContext::create(context, in->get()->data);
   else
      contextNS = SourceContext::create(context, SourceContext::CT_NAMESPACE);

   // Special consideration (definitely not hack) for {} block to use same context.
   if(in->peekType(SourceTokenC::TT_BRACE_O))
   {
      in->get();
      Vector expressions;
      make_statements(in, &expressions, contextNS);
      in->get(SourceTokenC::TT_BRACE_C);
      return create_value_block(expressions, contextNS, tok->pos);
   }
   else
      return make_expression(in, contextNS);
}

//
// SourceExpressionDS::make_keyword_ocode
//
SRCEXPDS_KEYWORD_DEFN(ocode)
{
   ObjectExpression::Pointer ocodeObj;
   SourceVariable::Pointer ocodeVar;
   VariableType::Reference ocodeTyp = VariableType::get_bt_void();
   ObjectCodeSet ocode;

   in->get(SourceTokenC::TT_PAREN_O);

   SourceTokenC::Reference ocodeTok = in->get(SourceTokenC::TT_NAM);
   ocode.ocode = ocode_get_code(ocodeTok->data, ocodeTok->pos);

   if (!in->peekType(SourceTokenC::TT_PAREN_C))
   {
      in->get(SourceTokenC::TT_COMMA);

      ocodeTok = in->get(SourceTokenC::TT_NAM);
      ocode.ocode_imm = ocode_get_code(ocodeTok->data, ocodeTok->pos);
   }

   in->get(SourceTokenC::TT_PAREN_C);

   ocodeObj = ObjectExpression::create_value_ocs(ocode, tok->pos);
   ocodeVar = SourceVariable::create_literal(ocodeTyp, ocodeObj, tok->pos);

   return create_value_variable(ocodeVar, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_output
//
SRCEXPDS_KEYWORD_DEFN(output)
{
   return create_root_output(make_expression(in, context), context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_printf
//
SRCEXPDS_KEYWORD_DEFN(printf)
{
   std::string type;

   if (in->peekType(SourceTokenC::TT_CMP_LT))
   {
      in->get(SourceTokenC::TT_CMP_LT);
      type = in->get(SourceTokenC::TT_NAM)->data;
      in->get(SourceTokenC::TT_CMP_GT);
   }

   Vector exprs;

   in->get(SourceTokenC::TT_PAREN_O);

   std::string format = in->get(SourceTokenC::TT_STR)->data;

   while (in->peekType(SourceTokenC::TT_COMMA))
   {
      in->get(SourceTokenC::TT_COMMA);
      exprs.push_back(make_assignment(in, context));
   }

   in->get(SourceTokenC::TT_PAREN_C);

   return create_root_printf(type, format, exprs, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_return
//
SRCEXPDS_KEYWORD_DEFN(return)
{
   return create_branch_return(make_expression(in, context), context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_sizeof
//
SRCEXPDS_KEYWORD_DEFN(sizeof)
{
   VariableType::Pointer type;

   if (in->peekType(SourceTokenC::TT_PAREN_O))
   {
      in->get(SourceTokenC::TT_PAREN_O);

      if(is_type(in, NULL, context))
         type = make_type(in, context);
      else
         type = make_expression(in, context)->getType();

      in->get(SourceTokenC::TT_PAREN_C);
   }
   else
      type = make_prefix(in, context)->getType();

   return create_value_uint(type->getSize(tok->pos), context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_switch
//
SRCEXPDS_KEYWORD_DEFN(switch)
{
   // Create a context for the entire switch.
   SourceContext::Reference contextSwitch =
      SourceContext::create(context, SourceContext::CT_SWITCH);

   // Check for named switch.
   if(in->peekType(SourceTokenC::TT_COLON))
   {
      in->get();
      contextSwitch->setLabel(in->get(SourceTokenC::TT_NAM)->data);
   }

   // Read condition.
   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(contextSwitch, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond = make_expression(in, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   // Read body.
   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprBody = make_expression(in, contextBody);

   return create_branch_switch(exprCond, exprBody, contextSwitch, tok->pos);
}

//
// SourceExpressionDS::make_keyword_symbol
//
SRCEXPDS_KEYWORD_DEFN(symbol)
{
   in->get(SourceTokenC::TT_PAREN_O);
   VariableType::Reference type = make_type(in, context);
   in->get(SourceTokenC::TT_COMMA);
   std::string name = in->get(SourceTokenC::TT_STR)->data;
   in->get(SourceTokenC::TT_PAREN_C);

   SourceVariable::Pointer var = SourceVariable::
      create_literal(type, name, tok->pos);

   return create_value_variable(var, context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_typestr
//
SRCEXPDS_KEYWORD_DEFN(typestr)
{
   VariableType::Reference type = make_type(in, context);

   return create_value_string(make_string(type), context, tok->pos);
}

//
// SourceExpressionDS::make_keyword_while
//
SRCEXPDS_KEYWORD_DEFN(while)
{
   // Create a context for the entire loop.
   SourceContext::Reference contextLoop =
      SourceContext::create(context, SourceContext::CT_LOOP);

   // Check for named loop.
   if(in->peekType(SourceTokenC::TT_COLON))
   {
      in->get();
      contextLoop->setLabel(in->get(SourceTokenC::TT_NAM)->data);
   }

   // Read condition.
   in->get(SourceTokenC::TT_PAREN_O);
   SourceContext::Reference contextCond =
      SourceContext::create(contextLoop, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprCond = make_expression(in, contextCond);
   in->get(SourceTokenC::TT_PAREN_C);

   // Read body.
   SourceContext::Reference contextBody =
      SourceContext::create(contextCond, SourceContext::CT_BLOCK);
   SourceExpression::Pointer exprBody = make_expression(in, contextBody);

   return create_branch_while(exprCond, exprBody, contextLoop, tok->pos);
}

// EOF

