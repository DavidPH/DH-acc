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
// DS handling/parsing.
//
//-----------------------------------------------------------------------------

#include "SourceExpressionDS.hpp"

#include "ObjectExpression.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceExpressionC.hpp"
#include "SourceFunction.hpp"
#include "SourceStream.hpp"
#include "SourceTokenizerC.hpp"
#include "SourceVariable.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// SRCEXPDS_EXPR_DEFN_MULTI
//
#define SRCEXPDS_EXPR_DEFN_MULTI(NAME1,NAME2)                   \
SRCEXPDS_EXPR_DEFN(NAME1, NAME2) {                              \
for (SourceTokenC::Reference tok = in->get();; tok = in->get()) \
switch (tok->type) { default: in->unget(tok); return expr;

//
// SRCEXPDS_EXPR_DEFN_MULTI_END
//
#define SRCEXPDS_EXPR_DEFN_MULTI_END() }}

//
// SRCEXPDS_EXPR_DEFN_PART
//
#define SRCEXPDS_EXPR_DEFN_PART(NAME,TOKN,EXPR)          \
case SourceTokenC::TT_##TOKN: expr = create_##EXPR(expr, \
   make_##NAME(in, context), context, tok->pos); break

//
// SRCEXPDS_EXPR_DEFN_PART_SUF
//
#define SRCEXPDS_EXPR_DEFN_PART_SUF(TOKN,EXPR)           \
case SourceTokenC::TT_##TOKN: expr = create_##EXPR(expr, \
   context, tok->pos); break


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceExpressionDS::ExternMap SourceExpressionDS::expr_extern;
SourceExpressionDS::KeywordMap SourceExpressionDS::expr_keyword;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionDS::SourceExpressionDS
//
SourceExpressionDS::SourceExpressionDS(SRCEXP_EXPR_PARM)
                                       : Super(SRCEXP_EXPR_PASS)
{
}

//
// SourceExpressionDS::init
//
void SourceExpressionDS::init()
{
   expr_extern["__function"] = make_extern_function;
   expr_extern["__script"]   = make_extern_script;
   expr_extern["__variable"] = make_extern_variable;

   expr_keyword["__address_space"]      = make_keyword_address_space;
   expr_keyword["__address_space_decl"] = make_keyword_address_space;
   expr_keyword["__asmfunc"]  = make_keyword_linespec;
   expr_keyword[  "break"]    = make_keyword_break;
   expr_keyword[  "const"]    = make_keyword_constexpr;
   expr_keyword[  "constexpr"]= make_keyword_constexpr;
   expr_keyword[  "continue"] = make_keyword_continue;
   expr_keyword[  "do"]       = make_keyword_do;
   expr_keyword[  "extern"]   = make_keyword_extern;
   expr_keyword["__extfunc"]  = make_keyword_function;
   expr_keyword["__extscript"]= make_keyword_script;
   expr_keyword["__extvar"]   = make_keyword_variable;
   expr_keyword[  "false"]    = make_keyword_literal;
   expr_keyword[  "for"]      = make_keyword_for;
   expr_keyword["__function"] = make_keyword_function;
   expr_keyword[  "goto"]     = make_keyword_goto;
   expr_keyword[  "if"]       = make_keyword_if;
   expr_keyword["__intern"]   = make_keyword_extern;
   expr_keyword["__intfunc"]  = make_keyword_function;
   expr_keyword["__intscript"]= make_keyword_script;
   expr_keyword["__intvar"]   = make_keyword_variable;
   expr_keyword["__library"]  = make_keyword_library;
   expr_keyword["__linespec"] = make_keyword_linespec;
   expr_keyword[  "namespace"]= make_keyword_namespace;
   expr_keyword["__native"]   = make_keyword_linespec;
   expr_keyword[  "nullptr"]  = make_keyword_literal;
   expr_keyword["__ocode"]    = make_keyword_ocode;
   expr_keyword["__output"]   = make_keyword_output;
   expr_keyword["__printf"]   = make_keyword_printf;
   expr_keyword[  "return"]   = make_keyword_return;
   expr_keyword["__script"]   = make_keyword_script;
   expr_keyword[  "sizeof"]   = make_keyword_sizeof;
   expr_keyword[  "switch"]   = make_keyword_switch;
   expr_keyword["__symbol"]   = make_keyword_symbol;
   expr_keyword[  "true"]     = make_keyword_literal;
   expr_keyword["__typestr"]  = make_keyword_typestr;
   expr_keyword["__variable"] = make_keyword_variable;
   expr_keyword[  "while"]    = make_keyword_while;

   // Template casts.
   expr_keyword[  "const_cast"]       = make_keyword_cast;
   expr_keyword[  "dynamic_cast"]     = make_keyword_cast;
   expr_keyword[  "reinterpret_cast"] = make_keyword_cast;
   expr_keyword[  "static_cast"]      = make_keyword_cast;
   expr_keyword["__force_cast"]       = make_keyword_cast;
   expr_keyword["__store_cast"]       = make_keyword_cast;
}

//
// SourceExpression::make_primary
//
SRCEXPDS_EXPR_DEF1(primary)
{
   SourceContext::Pointer contextNS;

   SourceTokenC::Reference tok = in->get(); switch (tok->type)
   {
   default: in->unget(tok); return create_value_data(context, tok->pos);

   case SourceTokenC::TT_NAM:
      if (is_keyword(tok->data))
         return expr_keyword[tok->data](in, tok, context);

      if(context->isAddressSpace(tok->data) || is_store(tok->data))
         return make_keyword_variable_store(in, tok, context);

   case SourceTokenC::TT_COLON2:
      if(is_type(in, tok, context))
         return make_keyword_variable_type(in, tok, context);

      if(tok->type == SourceTokenC::TT_COLON2)
      {
         tok = in->get(SourceTokenC::TT_NAM);
         contextNS = SourceContext::global_context;
      }
      else
         contextNS = context;

      while(in->peekType(SourceTokenC::TT_COLON2))
      {
         contextNS = contextNS->getContext(tok->data, tok->pos);

         in->get();
         tok = in->get(SourceTokenC::TT_NAM);
      }

   {  // Check for function designator.
      int count = contextNS->isFunction(tok->data);
      if (count == 1)
         return create_value_function(contextNS->getFunction(tok->data, tok->pos),
                                      context, tok->pos);
      else if (count)
         return create_value_function(tok->data, contextNS, tok->pos);
   }

      return create_value_variable(contextNS->getVariable(tok->data, tok->pos),
                                   contextNS, tok->pos);

   case SourceTokenC::TT_CHR:
      return create_value_char(tok->data, context, tok->pos);

   case SourceTokenC::TT_FLT:
      return SourceExpressionC::ParseFloat(tok->data, context, tok->pos);

   case SourceTokenC::TT_INT:
      return SourceExpressionC::ParseInt(tok->data, context, tok->pos);

   case SourceTokenC::TT_STR:
      return create_value_string(tok->data, context, tok->pos);

   case SourceTokenC::TT_AN2:
   {
      SourceTokenC::Reference gotoToken = in->get(SourceTokenC::TT_NAM);

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

      label = ObjectData::Label::Add(label);

      return create_value_variable(SourceVariable::create_literal(
         VariableType::get_bt_label(), label, gotoToken->pos), context, tok->pos);
   }

   case SourceTokenC::TT_PAREN_O:
   {
      SourceExpression::Pointer expr = make_expression(in, context);
      in->get(SourceTokenC::TT_PAREN_C);
      return expr;
   }

   case SourceTokenC::TT_BRACE_O:
   {
      SourceContext::Reference blockContext =
         SourceContext::create(context, SourceContext::CT_BLOCK);
      Vector expressions;
      make_statements(in, &expressions, blockContext);
      in->get(SourceTokenC::TT_BRACE_C);
      return create_value_block(expressions, blockContext, tok->pos);
   }

   case SourceTokenC::TT_BRACK_O:
   {
      Vector expressions;
      make_statements(in, &expressions, context);
      in->get(SourceTokenC::TT_BRACK_C);
      return create_value_block(expressions, context, tok->pos);
   }
   }
}

//
// SourceExpression::make_primary
//
SRCEXPDS_EXPR_DEF2(primary)
{
   (void)in; (void)context; (void)expr;
   Error_p("stub");
}

//
// SourceExpression::make_suffix
//
SRCEXPDS_EXPR_DEFN_MULTI(suffix, primary)
case SourceTokenC::TT_BRACK_O: expr = create_binary_array(expr,
   make_expression(in, context), context, tok->pos);
   in->get(SourceTokenC::TT_BRACK_C); break;

case SourceTokenC::TT_PAREN_O:
   if(is_type(in, NULL, context))
   {
      VariableType::Vector types;

      if (!in->peekType(SourceTokenC::TT_PAREN_C)) while (true)
      {
         types.push_back(make_type(in, context));

         if (!in->peekType(SourceTokenC::TT_COMMA)) break; in->get();
      }
      in->get(SourceTokenC::TT_PAREN_C);

      if (types.size() == 1 && types[0]->getBasicType() == VariableType::BT_VOID)
         types.clear();

      expr = expr->makeExpressionFunction(types);
   }
   else
   {
      SourceContext::Reference contextCall =
         SourceContext::create(context, SourceContext::CT_BLOCK);

      Vector args;
      VariableType::Vector types;
      ObjectExpression::Vector objs;

      if (!in->peekType(SourceTokenC::TT_PAREN_C)) while (true)
      {
         args.push_back(make_assignment(in, contextCall));
         types.push_back(args.back()->getType()->getUnqualified());
         objs.push_back(args.back()->makeObjectPartial());

         if (!in->peekType(SourceTokenC::TT_COMMA)) break; in->get();
      }
      in->get(SourceTokenC::TT_PAREN_C);

      expr = expr->makeExpressionFunction(types, objs);
      expr = create_branch_call(expr, args, contextCall, tok->pos);
   }
   break;

case SourceTokenC::TT_MEM: expr = create_unary_dereference(expr,
   context, tok->pos);
case SourceTokenC::TT_DOT: expr = create_value_member(expr,
   in->get(SourceTokenC::TT_NAM)->data, context, tok->pos); break;

SRCEXPDS_EXPR_DEFN_PART_SUF(AD2, unary_inc_suf);
SRCEXPDS_EXPR_DEFN_PART_SUF(SU2, unary_dec_suf);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpression::make_prefix
//
SRCEXPDS_EXPR_DEF1(prefix)
{
   #define CASE(TOK,EXP) case SourceTokenC::TT_##TOK: return \
      create_##EXP(make_prefix(in, context), context, tok->pos)

   SourceTokenC::Reference tok = in->get(); switch (tok->type)
   {
   default: def: in->unget(tok); return make_suffix(in, context);

   CASE(AD2,    unary_inc_pre);   CASE(SU2,    unary_dec_pre);
   CASE(AND,    unary_reference); CASE(MUL,    unary_dereference);
   CASE(ADD,    unary_add);       CASE(SUB,    unary_sub);
   CASE(NOTBIT, unary_not);       CASE(NOTLOG, branch_not);

   case SourceTokenC::TT_AT:
   {
      VariableData::Pointer data = make_prefix(in, context)->getData();

      ObjectExpression::Pointer addr;

      switch (data->type)
      {
      case VariableData::MT_AUTO:
      case VariableData::MT_POINTER:
      case VariableData::MT_STATIC:
         addr = ObjectExpression::CreateValueINT(option_addr_array, tok->pos);
         break;

      case VariableData::MT_FARPTR:
      case VariableData::MT_LITERAL:
      case VariableData::MT_NONE:
      case VariableData::MT_REGISTER:
      case VariableData::MT_STACK:
      case VariableData::MT_STRING:
      case VariableData::MT_VOID:
         Error(tok->pos, "invalid MT for @");

      case VariableData::MT_ARRAY:
         addr = data->address;
         break;
      }

      VariableType::Reference type = VariableType::get_bt_uns();
      SourceVariable::Pointer var  =
         SourceVariable::create_literal(type, addr, tok->pos);

      return create_value_variable(var, context, tok->pos);
   }

   case SourceTokenC::TT_PAREN_O:
      if(is_type(in, NULL, context))
      {
         VariableType::Reference type = make_type(in, context);
         in->get(SourceTokenC::TT_PAREN_C);
         return create_value_cast_explicit(make_prefix(in, context), type, context, tok->pos);
      }
      else goto def;
   }

   #undef CASE
}

//
// SourceExpression::make_prefix
//
SRCEXPDS_EXPR_DEF2(prefix)
{
   (void)in; (void)context; (void)expr;
   Error_p("stub");
}

//
// SourceExpressionDS::make_multiplicative
//
SRCEXPDS_EXPR_DEFN_MULTI(multiplicative, prefix)
SRCEXPDS_EXPR_DEFN_PART(prefix, MUL, binary_mul);
SRCEXPDS_EXPR_DEFN_PART(prefix, DIV, binary_div);
SRCEXPDS_EXPR_DEFN_PART(prefix, MOD, binary_mod);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_additive
//
SRCEXPDS_EXPR_DEFN_MULTI(additive, multiplicative)
SRCEXPDS_EXPR_DEFN_PART(multiplicative, ADD, binary_add);
SRCEXPDS_EXPR_DEFN_PART(multiplicative, SUB, binary_sub);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_shift
//
SRCEXPDS_EXPR_DEFN_MULTI(shift, additive)
SRCEXPDS_EXPR_DEFN_PART(additive, LSH, binary_lsh);
SRCEXPDS_EXPR_DEFN_PART(additive, RSH, binary_rsh);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_relational
//
SRCEXPDS_EXPR_DEFN_MULTI(relational, shift)
SRCEXPDS_EXPR_DEFN_PART(shift, CMP_LT, binary_cmp_lt);
SRCEXPDS_EXPR_DEFN_PART(shift, CMP_GT, binary_cmp_gt);
SRCEXPDS_EXPR_DEFN_PART(shift, CMP_LE, binary_cmp_le);
SRCEXPDS_EXPR_DEFN_PART(shift, CMP_GE, binary_cmp_ge);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_equality
//
SRCEXPDS_EXPR_DEFN_MULTI(equality, relational)
SRCEXPDS_EXPR_DEFN_PART(relational, CMP_EQ, binary_cmp_eq);
SRCEXPDS_EXPR_DEFN_PART(relational, CMP_NE, binary_cmp_ne);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_bitwise_and
//
SRCEXPDS_EXPR_DEFN_MULTI(bitwise_and, equality)
SRCEXPDS_EXPR_DEFN_PART(equality, AND, binary_and);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_bitwise_xor
//
SRCEXPDS_EXPR_DEFN_MULTI(bitwise_xor, bitwise_and)
SRCEXPDS_EXPR_DEFN_PART(bitwise_and, XOR, binary_xor);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_bitwise_ior
//
SRCEXPDS_EXPR_DEFN_MULTI(bitwise_ior, bitwise_xor)
SRCEXPDS_EXPR_DEFN_PART(bitwise_xor, IOR, binary_ior);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_logical_and
//
SRCEXPDS_EXPR_DEFN_MULTI(logical_and, bitwise_ior)
SRCEXPDS_EXPR_DEFN_PART(bitwise_ior, AN2, branch_and);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_logical_xor
//
SRCEXPDS_EXPR_DEFN_MULTI(logical_xor, logical_and)
SRCEXPDS_EXPR_DEFN_PART(logical_and, XO2, branch_xor);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_logical_ior
//
SRCEXPDS_EXPR_DEFN_MULTI(logical_ior, logical_xor)
SRCEXPDS_EXPR_DEFN_PART(logical_xor, IO2, branch_ior);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_conditional
//
SRCEXPDS_EXPR_DEFN(conditional, logical_ior)
{
   if (!in->peekType(SourceTokenC::TT_QUERY)) return expr;

   SourceTokenC::Reference tok = in->get(SourceTokenC::TT_QUERY);

   SourceExpression::Pointer exprBody = make_expression(in, context);
   in->get(SourceTokenC::TT_COLON);
   SourceExpression::Pointer exprElse = make_conditional(in, context);

   return create_branch_if(expr, exprBody, exprElse, context, tok->pos);
}

#define CASE_ALL() CAS0(     assign);                    \
CASE(MUL, mul_eq); CASE(DIV, div_eq); CASE(MOD, mod_eq); \
CASE(ADD, add_eq); CASE(SUB, sub_eq);                    \
CASE(LSH, lsh_eq); CASE(RSH, rsh_eq);                    \
CASE(AND, and_eq); CASE(XOR, xor_eq); CASE(IOR, ior_eq); \
CASB(AN2, and_eq); CASB(XO2, xor_eq); CASB(IO2, ior_eq)

//
// SourceExpressionDS::make_assignment
//
SRCEXPDS_EXPR_DEF1(assignment)
{
   #define CAS0(EXP) case SourceTokenC::TT_EQUALS:
   #define CASB(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ:
   #define CASE(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ:

   SourceExpression::Pointer expr = make_prefix(in, context);

   switch (in->peek()->type)
   {
   default: return
      make_conditional(in, context,
      make_logical_ior(in, context,
      make_logical_xor(in, context,
      make_logical_and(in, context,
      make_bitwise_ior(in, context,
      make_bitwise_xor(in, context,
      make_bitwise_and(in, context,
      make_equality   (in, context,
      make_relational (in, context,
      make_shift      (in, context,
      make_additive   (in, context,
      make_multiplicative(in, context,
         expr))))))))))));

   CASE_ALL();
      return make_assignment(in, context, expr);
   }

   #undef CASE
   #undef CASB
   #undef CAS0
}

//
// SourceExpressionDS::make_assignment
//
SRCEXPDS_EXPR_DEF2(assignment)
{
   #define CAS0(EXP) case SourceTokenC::TT_EQUALS: return \
      create_binary_##EXP(expr, make_assignment(in, context), context, tok->pos)
   #define CASB(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ: return \
      create_branch_##EXP(expr, make_assignment(in, context), context, tok->pos)
   #define CASE(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ: return \
      create_binary_##EXP(expr, make_assignment(in, context), context, tok->pos)

   SourceTokenC::Reference tok = in->get(); switch (tok->type)
   {default: in->unget(tok); return expr; CASE_ALL();}

   #undef CASE
   #undef CASB
   #undef CAS0
}

#undef CASE_ALL

//
// SourceExpressionDS::make_expression
//
SRCEXPDS_EXPR_DEFN_MULTI(expression, assignment)
SRCEXPDS_EXPR_DEFN_PART(assignment, COMMA, binary_pair);
SRCEXPDS_EXPR_DEFN_MULTI_END()

//
// SourceExpressionDS::make_linkspec
//
LinkageSpecifier SourceExpressionDS::make_linkspec(SourceTokenizerC *in)
{
   SourceTokenC::Reference linkSpecTok = in->get(SourceTokenC::TT_STR);

   if (linkSpecTok->data == "internal")
      return LINKAGE_INTERN;
   else if (linkSpecTok->data == "ACS")
      return LINKAGE_ACS;
   else if (linkSpecTok->data == "C")
      return LINKAGE_C;
   else if (linkSpecTok->data == "C++")
      return LINKAGE_CPP;
   else if (linkSpecTok->data == "DS")
      return LINKAGE_DS;
   else
      Error(linkSpecTok->pos, "unknown linkage specifier '%s'",
            linkSpecTok->data.c_str());
}

//
// SourceExpressionDS::make_statement
//
SourceExpression::Pointer SourceExpressionDS::make_statement(
   SourceTokenizerC *in, SourceContext *context)
{
   if (in->peekType(SourceTokenC::TT_NAM))
   {
      SourceExpression::Pointer expr;
      SourceTokenC::Reference tok = in->get();

      if (tok->data == "default")
      {
         in->get(SourceTokenC::TT_COLON);
         expr = make_statement(in, context);
         expr->addLabel(context->addLabelCaseDefault(tok->pos));
         return expr;
      }
      else if (tok->data == "case")
      {
         expr = make_expression(in, context);
         bigsint value = expr->makeObject()->resolveINT();

         in->get(SourceTokenC::TT_COLON);
         expr = make_statement(in, context);
         expr->addLabel(context->addLabelCase(value, tok->pos));
         return expr;
      }
      else if(in->peekType(SourceTokenC::TT_COLON) && !is_keyword(tok->data))
      {
         in->get(SourceTokenC::TT_COLON);
         expr = make_statement(in, context);
         expr->addLabel(context->addLabelGoto(tok->data, tok->pos));
         return expr;
      }
      else
         in->unget(tok);
   }

   return make_expression(in, context);
}

//
// SourceExpressionDS::make_statements
//
SourceExpression::Pointer SourceExpressionDS::make_statements(SourceTokenizerC *in)
{
   SourceContext::Reference context(SourceContext::global_context);
   SourcePosition pos;
   Vector exprs;

   try {pos = in->peek()->pos;} catch(SourceStream::EndOfStream const &) {}

   make_statements(in, &exprs, context);

   return create_value_block(exprs, context, pos);
}

//
// SourceExpressionDS::make_statements
//
void SourceExpressionDS::make_statements(SourceTokenizerC *in, Vector *exprs,
   SourceContext *context)
{
   while (true)
   {
      try
      {
         if (in->peekType(SourceTokenC::TT_BRACE_C)) return;
         if (in->peekType(SourceTokenC::TT_BRACK_C)) return;
      }
      catch (SourceStream::EndOfStream const &)
      {
         return;
      }

      exprs->push_back(make_statement(in, context));
      in->get(SourceTokenC::TT_SEMICOLON);
   }
}

// EOF

