/* Copyright (C) 2011 David Hill
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* SourceExpressionDS.cpp
**
** Defines the SourceExpressionDS methods.
*/

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
   #define EXPR_S make_expression_single(in, blocks, context)
   #define EXPR_L make_expression(in, blocks, context)
   #define PASS_A context, position

   SourceExpression::Pointer expr(make_expression_single(in, blocks, context));

   while (true)
   {
      SourceTokenC token(in->get());

      SourcePosition const &position = token.getPosition();

      switch (token.getType())
      {
      case SourceTokenC::TT_OP_AND:
         expr = create_binary_and(expr, EXPR_S, context, position);
         break;

      case SourceTokenC::TT_OP_AND_EQUALS:
         expr = create_binary_assign(expr, create_binary_and(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_AND2:
         expr = create_branch_and(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_AND2_EQUALS:
         expr = create_binary_assign(expr, create_branch_and(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_ASTERISK:
         expr = create_binary_mul(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_ASTERISK_EQUALS:
         expr = create_binary_assign(expr, create_binary_mul(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_BRACKET_C:
         in->unget(token);
         return expr;

      case SourceTokenC::TT_OP_CARET:
         expr = create_binary_xor(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CARET_EQUALS:
         expr = create_binary_assign(expr, create_binary_xor(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_CARET2:
         expr = create_branch_xor(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CARET2_EQUALS:
         expr = create_binary_assign(expr, create_branch_xor(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_EQ:
         expr = create_binary_eq(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_GE:
         expr = create_binary_ge(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_GT:
         expr = create_binary_gt(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_GT2:
         expr = create_binary_shiftr(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_GT2_EQUALS:
         expr = create_binary_assign(expr, create_binary_shiftr(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_LE:
         expr = create_binary_le(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_LT:
         expr = create_binary_lt(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_LT2:
         expr = create_binary_shiftl(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_LT2_EQUALS:
         expr = create_binary_assign(expr, create_binary_shiftl(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_CMP_NE:
         expr = create_binary_ne(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_COLON:
         in->unget(token);
         return expr;

      case SourceTokenC::TT_OP_COMMA:
         in->unget(token);
         return expr;

      case SourceTokenC::TT_OP_EQUALS:
         return create_binary_assign(expr, EXPR_L, PASS_A);

      case SourceTokenC::TT_OP_MINUS:
         expr = create_binary_sub(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_MINUS_EQUALS:
         expr = create_binary_assign(expr, create_binary_sub(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_PARENTHESIS_C:
         in->unget(token);
         return expr;

      case SourceTokenC::TT_OP_PERCENT:
         expr = create_binary_mod(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_PERCENT_EQUALS:
         expr = create_binary_assign(expr, create_binary_mod(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_PIPE:
         expr = create_binary_ior(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_PIPE_EQUALS:
         expr = create_binary_assign(expr, create_binary_ior(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_PIPE2:
         expr = create_branch_ior(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_PIPE2_EQUALS:
         expr = create_binary_assign(expr, create_branch_ior(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_PLUS:
         expr = create_binary_add(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_PLUS_EQUALS:
         expr = create_binary_assign(expr, create_binary_add(expr, EXPR_L, PASS_A), PASS_A);
         break;

      case SourceTokenC::TT_OP_QUERY:
      {
         SourceContext::Reference contextBody =
            SourceContext::create(context, SourceContext::CT_BLOCK);

         contextBody->setAllowLabel(false);
         SourceExpression::Pointer exprBody =
            make_expression_single(in, blocks, contextBody);
         contextBody->setAllowLabel(true);

         in->get(SourceTokenC::TT_OP_COLON);

         SourceContext::Reference contextElse =
            SourceContext::create(context, SourceContext::CT_BLOCK);
         SourceExpression::Pointer exprElse =
            make_expression_single(in, blocks, contextElse);

         expr = create_branch_if(expr, exprBody, exprElse, PASS_A);
      }
         break;

      case SourceTokenC::TT_OP_SEMICOLON:
         in->unget(token);
         return expr;

      case SourceTokenC::TT_OP_SLASH:
         expr = create_binary_div(expr, EXPR_S, PASS_A);
         break;

      case SourceTokenC::TT_OP_SLASH_EQUALS:
         expr = create_binary_assign(expr, create_binary_div(expr, EXPR_L, PASS_A), PASS_A);
         break;

      default:
         in->unget(token);
         throw SourceException("unexpected token type", position,
                               "SourceExpressionDS::make_expression");
      }
   }

   #undef PASS_A
   #undef EXPR_L
   #undef EXPR_S
}

//
// SourceExpressionDS::make_expression_arglist
//
void SourceExpressionDS::make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context, std::vector<VariableType const *> * argTypes, VariableType const * * returnType, SourceVariable::StorageClass argClass)
{
	make_expression_arglist(in, blocks, context, argTypes, NULL, NULL, NULL, returnType, argClass);
}

//
// SourceExpressionDS::make_expression_arglist
//
void SourceExpressionDS::make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context, std::vector<VariableType const *> * argTypes, std::vector<std::string> * argNames, int * argCount, SourceContext * argContext, VariableType const * * returnType, SourceVariable::StorageClass argClass)
{
	if (argCount) *argCount = 0;

	in->get(SourceTokenC::TT_OP_PARENTHESIS_O);
	if (in->peek().getType() != SourceTokenC::TT_OP_PARENTHESIS_C) while (true)
	{
		VariableType const * argType(make_expression_type(in, blocks, context));
		if (argCount) *argCount += argType->size(SourcePosition::none());
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
                 (VariableType::get_vt_void(), context, position),
                 context, position));

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

