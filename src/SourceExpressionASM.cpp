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
// ASM handling/parsing.
//
//-----------------------------------------------------------------------------

#include "SourceExpressionASM.hpp"

#include "ObjectData.hpp"
#include "ObjectExpression.hpp"
#include "ObjectVector.hpp"
#include "SourceContext.hpp"
#include "SourceStream.hpp"
#include "SourceTokenASM.hpp"
#include "SourceTokenizer.hpp"
#include "SourceVariable.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

//
// SRCEXPASM_EXPR_DEFN_MULTI
//
#define SRCEXPASM_EXPR_DEFN_MULTI(NAME1,NAME2) \
   SRCEXPASM_EXPR_DEFN(NAME1) \
   { \
      SourceExpression::Pointer expr = Make_##NAME2(in, context); \
      for(SourceTokenASM::Reference tok = in->get();; tok = in->get()) switch(tok->type) \
      { \
      default: in->unget(tok); return expr;

//
// SRCEXPASM_EXPR_DEFN_MULTI_END
//
#define SRCEXPASM_EXPR_DEFN_MULTI_END() }}

//
// SRCEXPASM_EXPR_DEFN_PART
//
#define SRCEXPASM_EXPR_DEFN_PART(NAME,TOKN,EXPR) \
case SourceTokenASM::TT_##TOKN: expr = create_##EXPR(expr, \
   Make_##NAME(in, context), context, tok->pos); break

//
// SRCEXPASM_EXPR_DEFN_PART_SUF
//
#define SRCEXPASM_EXPR_DEFN_PART_SUF(TOKN,EXPR) \
case SourceTokenASM::TT_##TOKN: expr = create_##EXPR(expr, \
   context, tok->pos); break


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

// Used to store labels for MakeStatements.
static std::vector<std::string> Labels;


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceExpressionASM::KeywordMap SourceExpressionASM::Keyword;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

static inline int CharToInt(char c)
{
   switch(c)
   {
   case '0': case '1': case '2': case '3': case '4':
   case '5': case '6': case '7': case '8': case '9':
      return c - '0';

   case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      return c - 'A' + 10;

   case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      return c - 'a' + 10;

   default:
      return -1;
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// SourceExpressionASM::CreateValueInteger
//
SourceExpression::Pointer SourceExpressionASM::CreateValueInteger(
   std::string const &value, SRCEXP_EXPR_ARGS)
{
   int         base;
   char const *str = value.c_str();
   biguint     val = 0;

   if(*str++ != '0') Error_P("invalid integer literal '%s'", value.c_str());

   switch(*str++)
   {
   case 'B': case 'b': base =  2; break;
   case 'O': case 'o': base =  8; break;
   case 'D': case 'd': base = 10; break;
   case 'X': case 'x': base = 16; break;
   default: Error_P("invalid integer literal");
   }

   for(char c; (c = *str++);)
   {
      int i = CharToInt(c);

      if(0 > i || i >= base)
         Error_P("character out of range");

      val = val * base + i;
   }

   return create_value_uint(val, context, pos);
}

//
// SourceExpressionASM::Make_Primary
//
SRCEXPASM_EXPR_DEFN(Primary)
{
   SourceTokenASM::Reference tok = in->get(); switch(tok->type)
   {
   default: in->unget(tok); return create_value_data(context, tok->pos);

   case SourceTokenASM::TT_STR:
      return create_value_variable(SourceVariable::create_literal(
         VariableType::get_bt_uns(), tok->data, tok->pos), context, tok->pos);

   case SourceTokenASM::TT_CHR:
      return create_value_char(tok->data, context, tok->pos);

   case SourceTokenASM::TT_INT:
      return CreateValueInteger(tok->data, context, tok->pos);

   case SourceTokenASM::TT_PAREN_O:
   {
      SourceExpression::Pointer expr = Make_Expression(in, context);
      in->get(SourceTokenASM::TT_PAREN_C);
      return expr;
   }
   }
}

//
// SourceExpressionASM::Make_Prefix
//
SRCEXPASM_EXPR_DEFN(Prefix)
{
   SourceTokenASM::Reference tok = in->get(); switch(tok->type)
   {
   default: in->unget(tok); return Make_Primary(in, context);

   case SourceTokenASM::TT_ADD:
      return create_unary_add(Make_Prefix(in, context), context, tok->pos);

   case SourceTokenASM::TT_SUB:
      return create_unary_sub(Make_Prefix(in, context), context, tok->pos);

   case SourceTokenASM::TT_NOTBIT:
      return create_unary_not(Make_Prefix(in, context), context, tok->pos);

   case SourceTokenASM::TT_NOTLOG:
      return create_branch_not(Make_Prefix(in, context), context, tok->pos);
   }
}

//
// SourceExpressionASM::Make_Multiplicative
//
SRCEXPASM_EXPR_DEFN_MULTI(Multiplicative, Prefix)
SRCEXPASM_EXPR_DEFN_PART(Prefix, MUL, binary_mul);
SRCEXPASM_EXPR_DEFN_PART(Prefix, DIV, binary_div);
SRCEXPASM_EXPR_DEFN_PART(Prefix, MOD, binary_mod);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_Additive
//
SRCEXPASM_EXPR_DEFN_MULTI(Additive, Multiplicative)
SRCEXPASM_EXPR_DEFN_PART(Multiplicative, ADD, binary_add);
SRCEXPASM_EXPR_DEFN_PART(Multiplicative, SUB, binary_sub);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_Shift
//
SRCEXPASM_EXPR_DEFN_MULTI(Shift, Additive)
SRCEXPASM_EXPR_DEFN_PART(Additive, LSH, binary_lsh);
SRCEXPASM_EXPR_DEFN_PART(Additive, RSH, binary_rsh);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_Relational
//
SRCEXPASM_EXPR_DEFN_MULTI(Relational, Shift)
SRCEXPASM_EXPR_DEFN_PART(Shift, CMP_LT, binary_cmp_lt);
SRCEXPASM_EXPR_DEFN_PART(Shift, CMP_GT, binary_cmp_gt);
SRCEXPASM_EXPR_DEFN_PART(Shift, CMP_LE, binary_cmp_le);
SRCEXPASM_EXPR_DEFN_PART(Shift, CMP_GE, binary_cmp_ge);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_Equality
//
SRCEXPASM_EXPR_DEFN_MULTI(Equality, Relational)
SRCEXPASM_EXPR_DEFN_PART(Relational, CMP_EQ, binary_cmp_eq);
SRCEXPASM_EXPR_DEFN_PART(Relational, CMP_NE, binary_cmp_ne);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_BitwiseAnd
//
SRCEXPASM_EXPR_DEFN_MULTI(BitwiseAnd, Equality)
SRCEXPASM_EXPR_DEFN_PART(Equality, AND, binary_and);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_BitwiseXor
//
SRCEXPASM_EXPR_DEFN_MULTI(BitwiseXor, BitwiseAnd)
SRCEXPASM_EXPR_DEFN_PART(BitwiseAnd, XOR, binary_xor);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_BitwiseIor
//
SRCEXPASM_EXPR_DEFN_MULTI(BitwiseIor, BitwiseXor)
SRCEXPASM_EXPR_DEFN_PART(BitwiseXor, IOR, binary_ior);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_LogicalAnd
//
SRCEXPASM_EXPR_DEFN_MULTI(LogicalAnd, BitwiseIor)
SRCEXPASM_EXPR_DEFN_PART(BitwiseIor, AN2, branch_and);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_LogicalXor
//
SRCEXPASM_EXPR_DEFN_MULTI(LogicalXor, LogicalAnd)
SRCEXPASM_EXPR_DEFN_PART(LogicalAnd, XO2, branch_xor);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_LogicalIor
//
SRCEXPASM_EXPR_DEFN_MULTI(LogicalIor, LogicalXor)
SRCEXPASM_EXPR_DEFN_PART(LogicalXor, IO2, branch_ior);
SRCEXPASM_EXPR_DEFN_MULTI_END()

//
// SourceExpressionASM::Make_Expression
//
SRCEXPASM_EXPR_DEFN(Expression)
{
   SourceExpression::Pointer expr = Make_LogicalIor(in, context);

   if(!in->peekType(SourceTokenASM::TT_QUERY)) return expr;

   SourceTokenASM::Reference tok = in->get(SourceTokenASM::TT_QUERY);

   SourceExpression::Pointer exprBody = Make_Expression(in, context);
   in->get(SourceTokenASM::TT_COLON);
   SourceExpression::Pointer exprElse = Make_Expression(in, context);

   return create_branch_if(expr, exprBody, exprElse, context, tok->pos);
}

//
// SourceExpressionASM::MakeStatement
//
SourceExpression::Pointer SourceExpressionASM::MakeStatement(
   SourceTokenizerASM *in, SourceContext *context)
{
   SourceTokenASM::Reference tok = in->get();

   switch(tok->type)
   {
   case SourceTokenASM::TT_STR:
   {
      if(tok->data == "ACSA") break;

      ObjectCode code = ocode_get_code(tok->data, tok->pos);
      Vector args;

      if(!in->peekType(SourceTokenASM::TT_ENDL)) do
      {
         args.push_back(Make_Expression(in, context));
      }
      while(in->dropType(SourceTokenASM::TT_COMMA));

      return new SourceExpressionASM(code, args, context, tok->pos);
   }

   case SourceTokenASM::TT_COLON:
      Labels.push_back(in->get(SourceTokenASM::TT_STR)->data);
      break;

   case SourceTokenASM::TT_NOTLOG:
      tok = in->get(SourceTokenASM::TT_STR);

      if(tok->data == "FUNCTION")
      {
         std::string name = in->get(SourceTokenASM::TT_STR)->data;
         in->get(SourceTokenASM::TT_COMMA);
         std::string label = in->get(SourceTokenASM::TT_STR)->data;
         in->get(SourceTokenASM::TT_COMMA);
         bigsint retCount = Make_Expression(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenASM::TT_COMMA);
         bigsint argCount = Make_Expression(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenASM::TT_COMMA);
         bigsint varCount = Make_Expression(in, context)->makeObject()->resolveINT();

         ObjectData_Function::Add(name, label, argCount, retCount, varCount);
      }
      else if(tok->data == "SCRIPT")
      {
         std::string name = in->get(SourceTokenASM::TT_STR)->data;
         in->get(SourceTokenASM::TT_COMMA);
         std::string label = in->get(SourceTokenASM::TT_STR)->data;
         in->get(SourceTokenASM::TT_COMMA);
         bigsint retCount = Make_Expression(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenASM::TT_COMMA);
         bigsint argCount = Make_Expression(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenASM::TT_COMMA);
         bigsint varCount = Make_Expression(in, context)->makeObject()->resolveINT();

         in->get(SourceTokenASM::TT_COMMA);
         bigsint stype = Make_Expression(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenASM::TT_COMMA);
         bigsint flags = Make_Expression(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenASM::TT_COMMA);
         bigsint number = Make_Expression(in, context)->makeObject()->resolveINT();
         in->get(SourceTokenASM::TT_COMMA);
         std::string string = in->get(SourceTokenASM::TT_STR)->data;

         ObjectData_Script::Add(name, label, (ObjectData_Script::ScriptType)stype,
                                flags, argCount, varCount, true, number, string);
      }
      else if(tok->data == "STRING")
      {
         std::string name = in->get(SourceTokenASM::TT_STR)->data;
         in->get(SourceTokenASM::TT_COMMA);
         std::string string = in->get(SourceTokenASM::TT_STR)->data;

         ObjectData_String::Add(name, string);
      }
      else
        Error(tok->pos, "unrecognized command '%s'", tok->data.c_str());

      break;

   default:
      break;
   }

   return NULL;
}

//
// SourceExpressionASM::MakeStatements
//
SourceExpression::Pointer SourceExpressionASM::MakeStatements(SourceTokenizerASM *in)
{
   Labels.clear();

   SourceContext::Reference context(SourceContext::global_context);

   // If at EOF, return now.
   try {in->peek();} catch (SourceStream::EndOfStream const &)
   {return create_value_data(context, SourcePosition::builtin());}

   SourceTokenASM::Reference tok = in->peek();

   Vector exprs;

   MakeStatements(in, &exprs, context);

   return create_value_block(exprs, context, tok->pos);
}

//
// SourceExpressionASM::MakeStatements
//
void SourceExpressionASM::MakeStatements(SourceTokenizerASM *in, Vector *exprs,
   SourceContext *context)
{
   for(;;)
   {
      try
      {
         while(in->peekType(SourceTokenASM::TT_ENDL))
            in->get();
      }
      catch (SourceStream::EndOfStream const &)
      {
         return;
      }

      SourceExpression::Pointer expr = MakeStatement(in, context);

      if(expr)
      {
         expr->addLabel(Labels);
         exprs->push_back(expr);
         Labels.clear();
      }

      in->get(SourceTokenASM::TT_ENDL);
   }
}

//
// SourceExpressionASM::virtual_makeObjects
//
void SourceExpressionASM::virtual_makeObjects(ObjectVector *objects, VariableData *)
{
   ObjectExpression::Vector objs;

   for(Vector::iterator itr = args.begin(), end = args.end(); itr != end; ++itr)
      objs.push_back((*itr)->makeObject());

   objects->addToken(code, objs);
}

// EOF

