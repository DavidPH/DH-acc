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
// DS handling/parsing.
//
//-----------------------------------------------------------------------------

#include "SourceExpressionDS.hpp"

#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"
#include "SourceTokenizerDS.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceExpressionDS::expr_single_handler_map SourceExpressionDS::expr_single;
SourceExpressionDS::expr_single_handler_map SourceExpressionDS::expr_single_extern;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// make_expression
//
// Recursively splits a vector of expressions.
//
// start and stop are indexes into operators.
//
static SourceExpression::Pointer
make_expression(SourceExpression::Vector const &expressions,
                std::vector<SourceTokenC> const &operators, size_t start,
                size_t stop, SourceContext *context)
{
   #define EXPRL make_expression(expressions, operators, start, iter, context)
   #define EXPRR make_expression(expressions, operators, iter+1, stop, context)

   #define CARGS context, operators[iter].pos

   // Terminating case. Only one expression, so just return it.
   if (start == stop) return expressions[start];

   // Some temporaries.
   SourceExpression::Pointer expr, exprl, exprr;

   size_t iter;

   // &= &&= *= ^= ^^= >>= <<= = -= |= ||= += /=
   for (iter = start; iter < stop; ++iter)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_AND_EQUALS:
         return SourceExpression::create_binary_and_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_AND2_EQUALS:
         return SourceExpression::create_branch_and_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_ASTERISK_EQUALS:
         return SourceExpression::create_binary_mul_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CARET_EQUALS:
         return SourceExpression::create_binary_xor_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CARET2_EQUALS:
         return SourceExpression::create_branch_xor_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_GT2_EQUALS:
         return SourceExpression::create_binary_rsh_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_LT2_EQUALS:
         return SourceExpression::create_binary_lsh_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_EQUALS:
         return SourceExpression::create_binary_assign(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_MINUS_EQUALS:
         return SourceExpression::create_binary_sub_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PERCENT_EQUALS:
         return SourceExpression::create_binary_mod_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PIPE_EQUALS:
         return SourceExpression::create_binary_ior_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PIPE2_EQUALS:
         return SourceExpression::create_branch_ior_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PLUS_EQUALS:
         return SourceExpression::create_binary_add_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_SLASH_EQUALS:
         return SourceExpression::create_binary_div_eq(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // ?:
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_QUERY:
      {
         exprl = EXPRL; expr = expressions[++iter]; exprr = EXPRR;
         return SourceExpression::create_branch_if(exprl, expr, exprr, CARGS);
      }

      default:
         break;
      }
   }

   // ||
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_PIPE2:
         return SourceExpression::create_branch_ior(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // ^^
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_CARET2:
         return SourceExpression::create_branch_xor(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // &&
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_AND2:
         return SourceExpression::create_branch_and(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // |
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_PIPE:
         return SourceExpression::create_binary_ior(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // ^
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_CARET:
         return SourceExpression::create_binary_xor(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // &
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_AND:
         return SourceExpression::create_binary_and(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // == !=
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_CMP_EQ:
         return SourceExpression::create_binary_cmp_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_NE:
         return SourceExpression::create_binary_cmp_ne(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // >= > <= <
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_CMP_GE:
         return SourceExpression::create_binary_cmp_ge(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_GT:
         return SourceExpression::create_binary_cmp_gt(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_LE:
         return SourceExpression::create_binary_cmp_le(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_LT:
         return SourceExpression::create_binary_cmp_lt(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // >> <<
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_CMP_GT2:
         return SourceExpression::create_binary_rsh(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_LT2:
         return SourceExpression::create_binary_lsh(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // - +
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_MINUS:
         return SourceExpression::create_binary_sub(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PLUS:
         return SourceExpression::create_binary_add(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // * % /
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].type)
      {
      case SourceTokenC::TT_OP_ASTERISK:
         return SourceExpression::create_binary_mul(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_PERCENT:
         return SourceExpression::create_binary_mod(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_SLASH:
         return SourceExpression::create_binary_div(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   ERROR(operators[start].pos, "unexpected operator");
}


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
   expr_single["__asmfunc"]  = make_expression_single_asmfunc;
   expr_single[  "break"]    = make_expression_single_break;
   expr_single[  "case"]     = make_expression_single_case;
   expr_single[  "const"]    = make_expression_single_constexpr;
   expr_single[  "constexpr"]= make_expression_single_constexpr;
   expr_single[  "continue"] = make_expression_single_continue;
   expr_single[  "default"]  = make_expression_single_default;
   expr_single["__delay"]    = make_expression_single_delay;
   expr_single[  "do"]       = make_expression_single_do;
   expr_single[  "enum"]     = make_expression_single_type;
   expr_single["__extfunc"]  = make_expression_single_function;
   expr_single[  "extern"]   = make_expression_single_extern;
   expr_single["__extscript"]= make_expression_single_script;
   expr_single["__extvar"]   = make_expression_single_variable;
   expr_single[  "for"]      = make_expression_single_for;
   expr_single["__function"] = make_expression_single_function;
   expr_single[  "goto"]     = make_expression_single_goto;
   expr_single["__goto_dyn"] = make_expression_single_goto_dyn;
   expr_single[  "if"]       = make_expression_single_if;
   expr_single["__library"]  = make_expression_single_library;
   expr_single["__linespec"] = make_expression_single_linespec;
   expr_single["__native"]   = make_expression_single_native;
   expr_single["__output"]   = make_expression_single_output;
   expr_single["__printf"]   = make_expression_single_printf;
   expr_single[  "return"]   = make_expression_single_return;
   expr_single["__script"]   = make_expression_single_script;
   expr_single[  "sizeof"]   = make_expression_single_sizeof;
   expr_single[  "struct"]   = make_expression_single_type;
   expr_single[  "switch"]   = make_expression_single_switch;
   expr_single["__symbol"]   = make_expression_single_symbol;
   expr_single[  "typedef"]  = make_expression_single_typedef;
   expr_single["__typestr"]  = make_expression_single_typestr;
   expr_single[  "union"]    = make_expression_single_type;
   expr_single["__variable"] = make_expression_single_variable;
   expr_single[  "void"]     = make_expression_single_void;
   expr_single[  "while"]    = make_expression_single_while;

   // Template casts.
   expr_single[  "const_cast"]       = make_expression_single_cast;
   expr_single[  "dynamic_cast"]     = make_expression_single_cast;
   expr_single[  "reinterpret_cast"] = make_expression_single_cast;
   expr_single[  "static_cast"]      = make_expression_single_cast;
   expr_single["__force_cast"]       = make_expression_single_cast;

   expr_single_extern["__function"] = make_expression_single_extern_function;
   expr_single_extern["__script"]   = make_expression_single_extern_script;
   expr_single_extern["__variable"] = make_expression_single_extern_variable;
}

//
// SourceExpressionDS::make_expression
//
SourceExpression::Pointer SourceExpressionDS::
make_expression(SourceTokenizerDS *in, SourceExpression::Vector *blocks,
                SourceContext *context)
{
   SourceExpression::Vector expressions;
   std::vector<SourceTokenC> operators;

   SourceTokenC token;

   bool looping = true;

   expressions.push_back(make_expression_single(in, blocks, context));

   while (looping) switch ((token = in->get()).type)
   {
   case SourceTokenC::TT_OP_AND:
   case SourceTokenC::TT_OP_AND_EQUALS:
   case SourceTokenC::TT_OP_AND2:
   case SourceTokenC::TT_OP_AND2_EQUALS:
   case SourceTokenC::TT_OP_ASTERISK:
   case SourceTokenC::TT_OP_ASTERISK_EQUALS:
   case SourceTokenC::TT_OP_CARET:
   case SourceTokenC::TT_OP_CARET_EQUALS:
   case SourceTokenC::TT_OP_CARET2:
   case SourceTokenC::TT_OP_CARET2_EQUALS:
   case SourceTokenC::TT_OP_CMP_EQ:
   case SourceTokenC::TT_OP_CMP_GE:
   case SourceTokenC::TT_OP_CMP_GT:
   case SourceTokenC::TT_OP_CMP_GT2:
   case SourceTokenC::TT_OP_CMP_GT2_EQUALS:
   case SourceTokenC::TT_OP_CMP_LE:
   case SourceTokenC::TT_OP_CMP_LT:
   case SourceTokenC::TT_OP_CMP_LT2:
   case SourceTokenC::TT_OP_CMP_LT2_EQUALS:
   case SourceTokenC::TT_OP_CMP_NE:
   case SourceTokenC::TT_OP_EQUALS:
   case SourceTokenC::TT_OP_MINUS:
   case SourceTokenC::TT_OP_MINUS_EQUALS:
   case SourceTokenC::TT_OP_PERCENT:
   case SourceTokenC::TT_OP_PERCENT_EQUALS:
   case SourceTokenC::TT_OP_PIPE:
   case SourceTokenC::TT_OP_PIPE_EQUALS:
   case SourceTokenC::TT_OP_PIPE2:
   case SourceTokenC::TT_OP_PIPE2_EQUALS:
   case SourceTokenC::TT_OP_PLUS:
   case SourceTokenC::TT_OP_PLUS_EQUALS:
   case SourceTokenC::TT_OP_SLASH:
   case SourceTokenC::TT_OP_SLASH_EQUALS:
   case_expr:
      operators.push_back(token);
      expressions.push_back(make_expression_single(in, blocks, context));
      break;

   case SourceTokenC::TT_OP_BRACE_C:
   case SourceTokenC::TT_OP_BRACKET_C:
   case SourceTokenC::TT_OP_COLON:
   case SourceTokenC::TT_OP_COMMA:
   case SourceTokenC::TT_OP_PARENTHESIS_C:
   case SourceTokenC::TT_OP_SEMICOLON:
      in->unget(token);
      looping = false;
      break;

   case SourceTokenC::TT_OP_QUERY:
      operators.push_back(token);
      context->setAllowLabel(false);
      expressions.push_back(make_expression(in, blocks, context));
      context->setAllowLabel(true);
      token = in->get(SourceTokenC::TT_OP_COLON);
      goto case_expr;

   default:
      in->unget(token);
      ERROR(token.pos, "unexpected token type: %s", make_string(token.type).c_str());
   }

   return ::make_expression(expressions, operators, 0, operators.size(), context);
}

//
// SourceExpressionDS::make_expression_arglist
//
void SourceExpressionDS::make_expression_arglist
(SourceTokenizerDS *in, Vector *blocks, SourceContext *context,
 VariableType::Vector *argTypes, VariableType::Pointer *returnType,
 SourceVariable::StorageClass argClass)
{
   make_expression_arglist
   (in, blocks, context, argTypes, NULL, NULL, NULL, returnType, argClass);
}

//
// SourceExpressionDS::make_expression_arglist
//
void SourceExpressionDS::make_expression_arglist
(SourceTokenizerDS *in, Vector *blocks, SourceContext *context,
 VariableType::Vector *argTypes, VariableType::VecStr *argNames, int *argCount,
 SourceContext *argContext, VariableType::Pointer *returnType,
 SourceVariable::StorageClass argClass)
{
	if (argCount) *argCount = 0;

	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
   if (!in->peekType(SourceTokenC::TT_OP_PARENTHESIS_C)) while (true)
	{
      VariableType::Reference argType = make_expression_type(in, blocks, context);
      if (argCount) *argCount += argType->getSize(SourcePosition::none());
		if (argTypes) argTypes->push_back(argType);

		std::string argName;
      if (in->peekType(SourceTokenC::TT_IDENTIFIER))
         argName = in->get(SourceTokenC::TT_IDENTIFIER).data;
		if (argNames) argNames->push_back(argName);

		if (argContext)
		{
			std::string argNameObject(argContext->makeNameObject(SourceContext::NT_LOCAL, argClass, argType, argName, SourcePosition::none()));
			SourceVariable::Pointer argVariable(SourceVariable::create_variable(argName, argType, argNameObject, argClass, SourcePosition::none()));
			argContext->addVariable(argVariable);
		}

		if (!in->peekType(SourceTokenC::TT_OP_COMMA))
			break;

		in->get(SourceTokenC::TT_OP_COMMA);
	}
	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

	if (returnType)
	{
      if (in->peekType(SourceTokenC::TT_OP_MINUS_GT))
      {
         in->get(SourceTokenC::TT_OP_MINUS_GT);

         *returnType = make_expression_type(in, blocks, context);
      }
      else
         *returnType = VariableType::get_bt_void();

		if (argContext) argContext->setReturnType(*returnType);
	}
}

//
// SourceExpressionDS::make_expressions
//
SourceExpression::Pointer SourceExpressionDS::make_expressions
(SourceTokenizerDS *in)
{
   Vector::iterator block;
   SourceTokenC const *token;

   SourceContext::Reference context(SourceContext::global_context);
   VariableType::Reference retnType = VariableType::get_bt_void();

   try
   {
      token = &in->peek();
   }
   catch (SourceStream::EndOfStream const &)
   {
      return create_value_data(retnType, context, SourcePosition::builtin());
   }

   SourcePosition pos = token->pos;

   Vector exprs;
   Vector blocks;

   make_expressions(in, &exprs, &blocks, context);

   exprs.push_back(create_branch_return
      (create_value_data(retnType, context, pos), context, pos));

   for (block = blocks.begin(); block != blocks.end(); ++block)
      exprs.push_back(*block);

   return create_value_block(exprs, context, pos);
}

//
// SourceExpressionDS::make_expressions
//
void SourceExpressionDS::make_expressions
(SourceTokenizerDS *in, Vector *exprs, Vector *blocks, SourceContext *context)
{
   while (true)
   {
      try
      {
         if (in->peekType(SourceTokenC::TT_OP_BRACE_C)) return;
         if (in->peekType(SourceTokenC::TT_OP_BRACKET_C)) return;
      }
      catch (SourceStream::EndOfStream const &)
      {
         return;
      }

      exprs->push_back(make_expression(in, blocks, context));
      in->get(SourceTokenC::TT_OP_SEMICOLON);
   }
}

// EOF

