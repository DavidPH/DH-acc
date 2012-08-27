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
// C expression parsing.
//
//-----------------------------------------------------------------------------

#include "../SourceExpressionC.hpp"

#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../SourceFunction.hpp"
#include "../SourceTokenC.hpp"
#include "../SourceTokenizerC.hpp"
#include "../SourceVariable.hpp"
#include "../VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// SRCEXPC_PARSE_DEFN_MULTI
//
#define SRCEXPC_PARSE_DEFN_MULTI(NAME1,NAME2) \
   SRCEXPC_PARSE_DEFN(NAME1, NAME2) \
   { \
      for(SourceTokenC::Reference tok = in->get();; tok = in->get()) switch(tok->type) \
      { \
      default: \
         in->unget(tok); return expr;

//
// SRCEXPC_PARSE_DEFN_MULTI_END
//
#define SRCEXPC_PARSE_DEFN_MULTI_END() \
      } \
   }

//
// SRCEXPC_PARSE_DEFN_PART
//
#define SRCEXPC_PARSE_DEFN_PART(NAME,TOKN,EXPR) \
   case SourceTokenC::TT_##TOKN: \
      expr = create_##EXPR(expr, Parse##NAME(in, context), context, tok->pos); \
      break

//
// SRCEXPC_PARSE_DEFN_PART_SUF
//
#define SRCEXPC_PARSE_DEFN_PART_SUF(TOKN,EXPR) \
   case SourceTokenC::TT_##TOKN: \
      expr = create_##EXPR(expr, context, tok->pos); \
      break


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionC::ParsePrimary
//
SRCEXPC_PARSE_DEFN_HALF(Primary)
{
   SourceTokenC::Reference tok = in->get(); switch(tok->type)
   {
   default:
      Error(tok->pos, "unexpected token %s", make_string(tok->type).c_str());

   case SourceTokenC::TT_NAM:
      if(context->isVariable(tok->data))
      {
         return create_value_variable(context->getVariable(tok->data, tok->pos),
                                      context, tok->pos);
      }

      if(context->isFunction(tok->data))
      {
         return create_value_function(context->getFunction(tok->data, tok->pos),
                                      context, tok->pos);
      }

      Error(tok->pos, "undeclared identifier '%s'", tok->data.c_str());

   case SourceTokenC::TT_CHR:
      return create_value_char(tok->data, context, tok->pos);

   case SourceTokenC::TT_FLT:
      return create_value_real(tok->data, context, tok->pos);

   case SourceTokenC::TT_INT:
      return ParseInt(tok->data, context, tok->pos);

   case SourceTokenC::TT_STR:
      return create_value_string(tok->data, context, tok->pos);

   case SourceTokenC::TT_PAREN_O:
      {
         SourceExpression::Pointer expr = ParseExpression(in, context);
         in->get(SourceTokenC::TT_PAREN_C);
         return expr;
      }
   }
}

//
// SourceExpressionC::ParseSuffix
//
SRCEXPC_PARSE_DEFN_MULTI(Suffix, Primary)
case SourceTokenC::TT_BRACK_O:
   expr = create_binary_array(expr, ParseExpression(in, context), context, tok->pos);
   in->get(SourceTokenC::TT_BRACK_C);
   break;

case SourceTokenC::TT_PAREN_O:
   {
      Vector args;

      if(!in->peekType(SourceTokenC::TT_PAREN_C)) do
      {
         args.push_back(ParseAssignment(in, context));
      }
      while(in->dropType(SourceTokenC::TT_COMMA));

      in->get(SourceTokenC::TT_PAREN_C);

      expr = create_branch_call(expr, args, context, tok->pos);
   }
   break;

case SourceTokenC::TT_MEM:
   expr = create_unary_dereference(expr, context, tok->pos);
case SourceTokenC::TT_DOT:
   expr = create_value_member(expr, in->get(SourceTokenC::TT_NAM)->data, context, tok->pos);
   break;

SRCEXPC_PARSE_DEFN_PART_SUF(AD2, unary_inc_suf);
SRCEXPC_PARSE_DEFN_PART_SUF(SU2, unary_dec_suf);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParsePrefix
//
SRCEXPC_PARSE_DEFN_HALF(Prefix)
{
   #define CASE(PAR,TOK,EXP) case SourceTokenC::TT_##TOK: return \
      create_##EXP(Parse##PAR(in, context), context, tok->pos)

   SourceTokenC::Reference tok = in->get(); switch(tok->type)
   {
   default: def:
      in->unget(tok);
      return ParseSuffix(in, context);

   CASE(Prefix, AD2,    unary_inc_pre);   CASE(Prefix, SU2,    unary_dec_pre);
   CASE(Cast,   AND,    unary_reference); CASE(Cast,   MUL,    unary_dereference);
   CASE(Cast,   ADD,    unary_add);       CASE(Cast,   SUB,    unary_sub);
   CASE(Cast,   NOTBIT, unary_not);       CASE(Cast,   NOTLOG, branch_not);

   case SourceTokenC::TT_NAM:
      // sizeof prefix-expression
      // sizeof ( type-name )
      if(tok->data == "sizeof")
      {
         VariableType::Pointer type;

         if(in->dropType(SourceTokenC::TT_PAREN_O))
         {
            if(IsType(in, context))
               type = ParseType(in, context);
            else
               type = ParseExpression(in, context)->getType();

            in->get(SourceTokenC::TT_PAREN_C);
         }
         else
            type = ParsePrefix(in, context)->getType();

         return create_value_uint(type->getSize(tok->pos), context, tok->pos);
      }
      else goto def;
   }

   #undef CASE
}

//
// SourceExpressionC::ParseCast
//
SRCEXPC_PARSE_DEFN_HALF(Cast)
{
   if(in->peekType(SourceTokenC::TT_PAREN_O))
   {
      SourceTokenC::Reference tok = in->get();

      if(IsType(in, context))
      {
         VariableType::Reference type = ParseType(in, context);
         in->get(SourceTokenC::TT_PAREN_C);
         return create_value_cast_explicit(ParseCast(in, context), type, context, tok->pos);
      }

      in->unget(tok);
   }

   return ParsePrefix(in, context);
}

//
// SourceExpressionC::ParseMultiplicative
//
SRCEXPC_PARSE_DEFN_MULTI(Multiplicative, Cast)
SRCEXPC_PARSE_DEFN_PART(Cast, MUL, binary_mul);
SRCEXPC_PARSE_DEFN_PART(Cast, DIV, binary_div);
SRCEXPC_PARSE_DEFN_PART(Cast, MOD, binary_mod);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseAdditive
//
SRCEXPC_PARSE_DEFN_MULTI(Additive, Multiplicative)
SRCEXPC_PARSE_DEFN_PART(Multiplicative, ADD, binary_add);
SRCEXPC_PARSE_DEFN_PART(Multiplicative, SUB, binary_sub);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseShift
//
SRCEXPC_PARSE_DEFN_MULTI(Shift, Additive)
SRCEXPC_PARSE_DEFN_PART(Additive, LSH, binary_lsh);
SRCEXPC_PARSE_DEFN_PART(Additive, RSH, binary_rsh);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseRelational
//
SRCEXPC_PARSE_DEFN_MULTI(Relational, Shift)
SRCEXPC_PARSE_DEFN_PART(Shift, CMP_LT, binary_cmp_lt);
SRCEXPC_PARSE_DEFN_PART(Shift, CMP_GT, binary_cmp_gt);
SRCEXPC_PARSE_DEFN_PART(Shift, CMP_LE, binary_cmp_le);
SRCEXPC_PARSE_DEFN_PART(Shift, CMP_GE, binary_cmp_ge);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseEquality
//
SRCEXPC_PARSE_DEFN_MULTI(Equality, Relational)
SRCEXPC_PARSE_DEFN_PART(Relational, CMP_EQ, binary_cmp_eq);
SRCEXPC_PARSE_DEFN_PART(Relational, CMP_NE, binary_cmp_ne);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseBitwiseAnd
//
SRCEXPC_PARSE_DEFN_MULTI(BitwiseAnd, Equality)
SRCEXPC_PARSE_DEFN_PART(Equality, AND, binary_and);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseBitwiseXor
//
SRCEXPC_PARSE_DEFN_MULTI(BitwiseXor, BitwiseAnd)
SRCEXPC_PARSE_DEFN_PART(BitwiseAnd, XOR, binary_xor);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseBitwiseIor
//
SRCEXPC_PARSE_DEFN_MULTI(BitwiseIor, BitwiseXor)
SRCEXPC_PARSE_DEFN_PART(BitwiseXor, IOR, binary_ior);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseLogicalAnd
//
SRCEXPC_PARSE_DEFN_MULTI(LogicalAnd, BitwiseIor)
SRCEXPC_PARSE_DEFN_PART(BitwiseIor, AN2, branch_and);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseLogicalIor
//
SRCEXPC_PARSE_DEFN_MULTI(LogicalIor, LogicalAnd)
SRCEXPC_PARSE_DEFN_PART(LogicalAnd, IO2, branch_ior);
SRCEXPC_PARSE_DEFN_MULTI_END()

//
// SourceExpressionC::ParseConditional
//
SRCEXPC_PARSE_DEFN(Conditional, LogicalIor)
{
   if(!in->peekType(SourceTokenC::TT_QUERY)) return expr;

   SourceTokenC::Reference tok = in->get();

   SourceExpression::Pointer exprBody = ParseExpression(in, context);
   in->get(SourceTokenC::TT_COLON);
   SourceExpression::Pointer exprElse = ParseConditional(in, context);

   return create_branch_if(expr, exprBody, exprElse, context, tok->pos);
}

#define CASE_ALL()    CAS0(     assign); \
   CASE(MUL, mul_eq); CASE(DIV, div_eq); CASE(MOD, mod_eq); \
   CASE(ADD, add_eq); CASE(SUB, sub_eq); \
   CASE(LSH, lsh_eq); CASE(RSH, rsh_eq); \
   CASE(AND, and_eq); CASE(XOR, xor_eq); CASE(IOR, ior_eq); \
   CASB(AN2, and_eq); CASB(XO2, xor_eq); CASB(IO2, ior_eq)

//
// SourceExpressionC::ParseAssignment
//
SRCEXPC_PARSE_DEF1(Assignment)
{
   #define CAS0(EXP) case SourceTokenC::TT_EQUALS:
   #define CASB(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ:
   #define CASE(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ:

   SourceExpression::Pointer expr = ParsePrefix(in, context);

   switch(in->peek()->type)
   {
   default:
      return ParseConditional   (in, context,
             ParseLogicalIor    (in, context,
             ParseLogicalAnd    (in, context,
             ParseBitwiseIor    (in, context,
             ParseBitwiseXor    (in, context,
             ParseBitwiseAnd    (in, context,
             ParseEquality      (in, context,
             ParseRelational    (in, context,
             ParseShift         (in, context,
             ParseAdditive      (in, context,
             ParseMultiplicative(in, context,
                expr)))))))))));

   CASE_ALL();
      return ParseAssignment(in, context, expr);
   }

   #undef CASE
   #undef CASB
   #undef CAS0
}

//
// SourceExpressionC::ParseAssignment
//
SRCEXPC_PARSE_DEF2(Assignment)
{
   #define CAS0(EXP) case SourceTokenC::TT_EQUALS: return \
      create_binary_##EXP(expr, ParseAssignment(in, context), context, tok->pos)
   #define CASB(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ: return \
      create_branch_##EXP(expr, ParseAssignment(in, context), context, tok->pos)
   #define CASE(TOK,EXP) case SourceTokenC::TT_##TOK##_EQ: return \
      create_binary_##EXP(expr, ParseAssignment(in, context), context, tok->pos)

   SourceTokenC::Reference tok = in->get(); switch(tok->type)
   {
   default:
      in->unget(tok);
      return expr;

   CASE_ALL();
   }

   #undef CASE
   #undef CASB
   #undef CAS0
}

#undef CASE_ALL

//
// SourceExpressionC::ParseExpression
//
SRCEXPC_PARSE_DEFN_MULTI(Expression, Assignment)
SRCEXPC_PARSE_DEFN_PART(Assignment, COMMA, binary_pair);
SRCEXPC_PARSE_DEFN_MULTI_END()

// EOF

