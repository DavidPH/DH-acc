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

/* SourceExpressionDS.hpp
**
** Defines the SourceExpressionDS class.
*/

#ifndef HPP_SourceExpressionDS_
#define HPP_SourceExpressionDS_

#include "SourceExpression.hpp"

class SourceTokenizerDS;



class SourceExpressionDS : public SourceExpression
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpressionDS, SourceExpression);

public:
	SourceExpressionDS(SourcePosition const & position);

	static void init();

	static SourceExpression::Pointer make_expressions(SourceTokenizerDS * tokenizer);

private:
	typedef SourceExpression::Pointer (*expression_single_handler)(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);



	static SourceExpression::Pointer make_expression(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static void make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceVariable::VariableType const *> * argTypes, SourceVariable::VariableType const * * returnType);
	static void make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceVariable::VariableType const *> * argTypes, std::vector<std::string> * argNames, int * argCount, SourceContext * argContext, SourceVariable::VariableType const * * returnType);
	static void make_expressions(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * expressions, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);

	static SourceExpression::Pointer make_expression_single(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_acsfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_asmfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_const(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_delay(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_if(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_lnspec(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_native(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_out(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_return(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_script(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_struct(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_typedef(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_var(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static SourceExpression::Pointer make_expression_single_while(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);

	static std::map<std::string, expression_single_handler> _expression_single_handlers;
};



#endif /* HPP_SourceExpressionDS_ */



