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
#include "SourceTokenizerDS.hpp"
#include "VariableType.hpp"


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

SourceExpressionDS::expression_single_handler_map SourceExpressionDS::_expression_single_handlers;
SourceExpressionDS::expression_single_handler_map SourceExpressionDS::_expression_single_extern_handlers;


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

   #define CARGS context, operators[iter].getPosition()

   // Terminating case. Only one expression, so just return it.
   if (start == stop) return expressions[start];

   // Some temporaries.
   SourceExpression::Pointer expr, exprl, exprr;

   size_t iter;

   // &= &&= *= ^= ^^= >>= <<= = -= |= ||= += /=
   for (iter = start; iter < stop; ++iter)
   {
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
      {
      case SourceTokenC::TT_OP_CMP_EQ:
         return SourceExpression::create_binary_eq(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_NE:
         return SourceExpression::create_binary_ne(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // >= > <= <
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].getType())
      {
      case SourceTokenC::TT_OP_CMP_GE:
         return SourceExpression::create_binary_ge(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_GT:
         return SourceExpression::create_binary_gt(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_LE:
         return SourceExpression::create_binary_le(EXPRL, EXPRR, CARGS);

      case SourceTokenC::TT_OP_CMP_LT:
         return SourceExpression::create_binary_lt(EXPRL, EXPRR, CARGS);

      default:
         break;
      }
   }

   // >> <<
   for (iter = stop; iter-- > start;)
   {
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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
      switch (operators[iter].getType())
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

   throw SourceException("unexpected operator", operators[start].getPosition(),
                         __func__);
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
	_expression_single_handlers["__asmfunc"]  = make_expression_single_asmfunc;
	_expression_single_handlers[  "break"]    = make_expression_single_break;
	_expression_single_handlers[  "case"]     = make_expression_single_case;
	_expression_single_handlers[  "constexpr"]= make_expression_single_constexpr;
	_expression_single_handlers[  "continue"] = make_expression_single_continue;
	_expression_single_handlers[  "default"]  = make_expression_single_default;
	_expression_single_handlers["__delay"]    = make_expression_single_delay;
	_expression_single_handlers[  "do"]       = make_expression_single_do;
	_expression_single_handlers[  "enum"]     = make_expression_single_type;
	_expression_single_handlers["__extfunc"]  = make_expression_single_function;
	_expression_single_handlers[  "extern"]   = make_expression_single_extern;
	_expression_single_handlers["__extscript"]= make_expression_single_script;
	_expression_single_handlers["__extvar"]   = make_expression_single_variable;
	_expression_single_handlers[  "for"]      = make_expression_single_for;
	_expression_single_handlers["__function"] = make_expression_single_function;
	_expression_single_handlers[  "goto"]     = make_expression_single_goto;
	_expression_single_handlers["__goto_dyn"] = make_expression_single_goto_dyn;
	_expression_single_handlers[  "if"]       = make_expression_single_if;
	_expression_single_handlers["__library"]  = make_expression_single_library;
	_expression_single_handlers["__linespec"] = make_expression_single_linespec;
	_expression_single_handlers["__native"]   = make_expression_single_native;
	_expression_single_handlers["__output"]   = make_expression_single_output;
	_expression_single_handlers["__printf"]   = make_expression_single_printf;
	_expression_single_handlers[  "return"]   = make_expression_single_return;
	_expression_single_handlers["__script"]   = make_expression_single_script;
	_expression_single_handlers[  "sizeof"]   = make_expression_single_sizeof;
	_expression_single_handlers[  "struct"]   = make_expression_single_type;
	_expression_single_handlers[  "switch"]   = make_expression_single_switch;
	_expression_single_handlers["__symbol"]   = make_expression_single_symbol;
	_expression_single_handlers[  "typedef"]  = make_expression_single_typedef;
	_expression_single_handlers[  "union"]    = make_expression_single_type;
	_expression_single_handlers["__variable"] = make_expression_single_variable;
	_expression_single_handlers[  "void"]     = make_expression_single_void;
	_expression_single_handlers[  "while"]    = make_expression_single_while;

	_expression_single_extern_handlers["__function"] = make_expression_single_extern_function;
	_expression_single_extern_handlers["__script"]   = make_expression_single_extern_script;
	_expression_single_extern_handlers["__variable"] = make_expression_single_extern_variable;
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

   while (looping) switch ((token = in->get()).getType())
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
      throw SourceException(std::string("unexpected token type '") +
                            make_string(token.getType()) + "'",
                            token.getPosition(), __func__);
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
	if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
	{
      VariableType::Reference argType = make_expression_type(in, blocks, context);
      if (argCount) *argCount += argType->getSize(SourcePosition::none());
		if (argTypes) argTypes->push_back(argType);

		std::string argName;
		if (in->peek().getType() == SourceTokenC::TT_IDENTIFIER)
			argName = in->get(SourceTokenC::TT_IDENTIFIER).getData();
		if (argNames) argNames->push_back(argName);

		if (argContext)
		{
			std::string argNameObject(argContext->makeNameObject(SourceContext::NT_LOCAL, argClass, argType, argName, SourcePosition::none()));
			SourceVariable::Pointer argVariable(SourceVariable::create_variable(argName, argType, argNameObject, argClass, SourcePosition::none()));
			argContext->addVariable(argVariable);
		}

		if (in->peek().getType() != SourceTokenC::TT_OP_COMMA)
			break;

		in->get(SourceTokenC::TT_OP_COMMA);
	}
	in->get(SourceTokenC::TT_OP_PARENTHESIS_C);

	if (returnType)
	{
		in->get(SourceTokenC::TT_OP_MINUS_GT);

		*returnType = make_expression_type(in, blocks, context);

		if (argContext) argContext->setReturnType(*returnType);
	}
}

//
// SourceExpressionDS::make_expressions
//
SourceExpression::Pointer SourceExpressionDS::make_expressions(SourceTokenizerDS * in)
{
   SourceContext::Pointer context = SourceContext::global_context;

	SourcePosition position(in->peek().getPosition());
	std::vector<SourceExpression::Pointer> expressions;
	std::vector<SourceExpression::Pointer> blocks;

	make_expressions(in, &expressions, &blocks, SourceContext::global_context);

   expressions.push_back
   (create_branch_return
    (create_value_data
     (VariableType::get_bt_void(), context, position), context, position));

	for (size_t i(0); i < blocks.size(); ++i)
		expressions.push_back(blocks[i]);

   return create_value_block(expressions, context, position);
}

//
// SourceExpressionDS::make_expressions
//
void SourceExpressionDS::make_expressions(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * expressions, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context)
{
	bool brackets(in->peek().getType() == SourceTokenC::TT_OP_BRACKET_O);

	in->get(brackets ? SourceTokenC::TT_OP_BRACKET_O : SourceTokenC::TT_OP_BRACE_O);

	while (true)
	{
		if (in->peek().getType() == (brackets ? SourceTokenC::TT_OP_BRACKET_C : SourceTokenC::TT_OP_BRACE_C))
			break;

		expressions->push_back(make_expression(in, blocks, context));
		in->get(SourceTokenC::TT_OP_SEMICOLON);
	}

	in->get(brackets ? SourceTokenC::TT_OP_BRACKET_C : SourceTokenC::TT_OP_BRACE_C);
}

// EOF

