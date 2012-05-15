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
// C preprocessing expressions.
//
//-----------------------------------------------------------------------------

#include "../SourceTokenizerC.hpp"

#include "../ObjectExpression.hpp"
#include "../SourceException.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// EXPR_DEFN
//
#define EXPR_DEFN(NAME) \
ObjectExpression::Reference SourceTokenizerC::getExpr##NAME()

//
// EXPR_DEFN_MULTI
//
#define EXPR_DEFN_MULTI(NAME1,NAME2) EXPR_DEFN(NAME1) {             \
ObjectExpression::Reference expr = getExpr##NAME2();                \
for (SourceTokenC::Reference tok = getExpand();; tok = getExpand()) \
switch (tok->type) { default: unget(tok); return expr;

//
// EXPR_DEFN_MULTI_END
//
#define EXPR_DEFN_MULTI_END() }}

//
// EXPR_DEFN_PART
//
#define EXPR_DEFN_PART(NAME,TOKN,EXPR)   \
case SourceTokenC::TT_##TOKN: expr =     \
   ObjectExpression::create_##EXPR(expr, getExpr##NAME(), tok->pos); break

//
// EXPR_DEFN_PART_SUF
//
#define EXPR_DEFN_PART_SUF(TOKN,EXPR) \
case SourceTokenC::TT_##TOKN: expr =  \
   ObjectExpression::create_##EXPR(expr, tok->pos); break

//
// EXPR_DEFN_SINGLE
//
#define EXPR_DEFN_SINGLE(NAME1,NAME2,TOKN,EXPR) \
EXPR_DEFN_MULTI(NAME1, NAME2)                   \
EXPR_DEFN_PART(NAME2, TOKN, EXPR);              \
EXPR_DEFN_MULTI_END()


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceTokenizerC::getExprPrimary
//
EXPR_DEFN(Primary)
{
   SourceTokenC::Reference tok = getExpand(); switch (tok->type)
   {
   default: unget(tok);
      ERROR(tok->pos, "unexpected token: %s", make_string(tok->type).c_str());

   case SourceTokenC::TT_NAM:
      if (tok->data == "defined")
      {
         bool hasParen = (tok = getRaw())->type == SourceTokenC::TT_PAREN_O;

         doAssert((hasParen ? (tok = getRaw()) : tok), SourceTokenC::TT_NAM);

         bool isdef = hasDefine(tok->data) || hasMacro(tok->data);

         ObjectExpression::Reference expr =
            ObjectExpression::create_value_int(isdef, tok->pos);

         if (hasParen) doAssert(getRaw(), SourceTokenC::TT_PAREN_C);

         return expr;
      }
      else
         return ObjectExpression::create_value_int(0, tok->pos);

   case SourceTokenC::TT_INT:
      return ObjectExpression::create_value_int(
         get_bigsint(tok->data, tok->pos), tok->pos);

   case SourceTokenC::TT_PAREN_O:
   {
      ObjectExpression::Reference expr = getExprPrimary();
      doAssert(getExpand(), SourceTokenC::TT_PAREN_C);
      return expr;
   }
   }
}

//
// SourceTokenizerC::getExprPrefix
//
EXPR_DEFN(Prefix)
{
   #define CASE(TOK,EXP) case SourceTokenC::TT_##TOK: \
      return ObjectExpression::create_##EXP(getExprPrefix(), tok->pos)

   SourceTokenC::Reference tok = getExpand(); switch (tok->type)
   {
   default: unget(tok); return getExprPrimary();

   CASE(ADD,    unary_add); CASE(SUB,    unary_sub);
   CASE(NOTBIT, unary_not); CASE(NOTLOG, branch_not);
   }

   #undef CASE
}

//
// SourceTokenizerC::getExprMultiplicative
//
EXPR_DEFN_MULTI(Multiplicative, Prefix)
EXPR_DEFN_PART(Prefix, MUL, binary_mul);
EXPR_DEFN_PART(Prefix, DIV, binary_div);
EXPR_DEFN_PART(Prefix, MOD, binary_mod);
EXPR_DEFN_MULTI_END()

//
// SourceTokenizerC::getExprAdditive
//
EXPR_DEFN_MULTI(Additive, Multiplicative)
EXPR_DEFN_PART(Multiplicative, ADD, binary_add);
EXPR_DEFN_PART(Multiplicative, SUB, binary_sub);
EXPR_DEFN_MULTI_END()

//
// SourceTokenizerC::getExprShift
//
EXPR_DEFN_MULTI(Shift, Additive)
EXPR_DEFN_PART(Additive, LSH, binary_lsh);
EXPR_DEFN_PART(Additive, RSH, binary_rsh);
EXPR_DEFN_MULTI_END()

//
// SourceTokenizerC::getExprRelational
//
EXPR_DEFN_MULTI(Relational, Shift)
EXPR_DEFN_PART(Shift, CMP_LT, binary_cmp_lt);
EXPR_DEFN_PART(Shift, CMP_GT, binary_cmp_gt);
EXPR_DEFN_PART(Shift, CMP_LE, binary_cmp_le);
EXPR_DEFN_PART(Shift, CMP_GE, binary_cmp_ge);
EXPR_DEFN_MULTI_END()

//
// SourceTokenizerC::getExprEquality
//
EXPR_DEFN_MULTI(Equality, Relational)
EXPR_DEFN_PART(Relational, CMP_EQ, binary_cmp_eq);
EXPR_DEFN_PART(Relational, CMP_NE, binary_cmp_ne);
EXPR_DEFN_MULTI_END()

//
// SourceTokenizerC::getExprBitwiseAnd
//
EXPR_DEFN_SINGLE(BitwiseAnd, Equality, AND, branch_and)

//
// SourceTokenizerC::getExprBitwiseXOr
//
EXPR_DEFN_SINGLE(BitwiseXOr, BitwiseAnd, XOR, branch_xor)

//
// SourceTokenizerC::getExprBitwiseIOr
//
EXPR_DEFN_SINGLE(BitwiseIOr, BitwiseXOr, IOR, branch_ior)

//
// SourceTokenizerC::getExprLogicalAnd
//
EXPR_DEFN_SINGLE(LogicalAnd, BitwiseIOr, AN2, branch_and)

//
// SourceTokenizerC::getExprLogicalXOr
//
EXPR_DEFN_SINGLE(LogicalXOr, LogicalAnd, XO2, branch_xor)

//
// SourceTokenizerC::getExprLogicalIOr
//
EXPR_DEFN_SINGLE(LogicalIOr, LogicalXOr, IO2, branch_ior)

//
// SourceTokenizerC::getExpr
//
EXPR_DEFN()
{
   ObjectExpression::Reference expr = getExprLogicalIOr();

   SourceTokenC::Reference tok = getExpand();

   if (tok->type != SourceTokenC::TT_QUERY) return unget(tok), expr;

   ObjectExpression::Reference exprL = getExpr();
   doAssert(getExpand(), SourceTokenC::TT_COLON);
   ObjectExpression::Reference exprR = getExpr();

   return ObjectExpression::create_branch_if(expr, exprL, exprR, tok->pos);
}

// EOF

