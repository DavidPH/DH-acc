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

#include <map>

class SourceTokenizerDS;



#define SRCEXPDS_EXPRSINGLE_ARGS                            \
	SourceTokenizerDS * in, SourceTokenC const & token, \
	std::vector<SourceExpression::Pointer> * blocks,    \
	SourceContext * context

#define SRCEXPDS_EXPRSINGLE_DECL(NAME)   \
	static SourceExpression::Pointer \
	make_expression_single_##NAME(SRCEXPDS_EXPRSINGLE_ARGS)

#define SRCEXPDS_EXPRSINGLE_DEFN(NAME)                 \
	SourceExpression::Pointer SourceExpressionDS:: \
	make_expression_single_##NAME(SRCEXPDS_EXPRSINGLE_ARGS)



class SourceExpressionDS : public SourceExpression
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpressionDS, SourceExpression);

public:
	SourceExpressionDS(SourcePosition const & position);

	static void init();

	static SourceExpression::Pointer make_expressions(SourceTokenizerDS * tokenizer);

private:
	typedef SourceExpression::Pointer (*expression_single_handler)(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);



	static bool is_expression_type(std::string const & data, SourceContext * context);

	static SourceExpression::Pointer make_expression(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);
	static void make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context, std::vector<VariableType const *> * argTypes, VariableType const * * returnType, SourceVariable::StorageClass argClass = SourceVariable::SC_REGISTER);
	static void make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context, std::vector<VariableType const *> * argTypes, std::vector<std::string> * argNames, int * argCount, SourceContext * argContext, VariableType const * * returnType, SourceVariable::StorageClass argClass = SourceVariable::SC_REGISTER);
	static void make_expressions(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * expressions, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);

	static SourceExpression::Pointer make_expression_single(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);

	SRCEXPDS_EXPRSINGLE_DECL(asmfunc);
	SRCEXPDS_EXPRSINGLE_DECL(break);
	SRCEXPDS_EXPRSINGLE_DECL(case);
	SRCEXPDS_EXPRSINGLE_DECL(const);
	SRCEXPDS_EXPRSINGLE_DECL(continue);
	SRCEXPDS_EXPRSINGLE_DECL(default);
	SRCEXPDS_EXPRSINGLE_DECL(delay);
	SRCEXPDS_EXPRSINGLE_DECL(do);
	SRCEXPDS_EXPRSINGLE_DECL(extern);
	SRCEXPDS_EXPRSINGLE_DECL(for);
	SRCEXPDS_EXPRSINGLE_DECL(function);
	SRCEXPDS_EXPRSINGLE_DECL(goto);
	SRCEXPDS_EXPRSINGLE_DECL(if);
	SRCEXPDS_EXPRSINGLE_DECL(linespec);
	SRCEXPDS_EXPRSINGLE_DECL(native);
	SRCEXPDS_EXPRSINGLE_DECL(output);
	SRCEXPDS_EXPRSINGLE_DECL(return);
	SRCEXPDS_EXPRSINGLE_DECL(script);
	SRCEXPDS_EXPRSINGLE_DECL(sizeof);
	SRCEXPDS_EXPRSINGLE_DECL(switch);
	SRCEXPDS_EXPRSINGLE_DECL(symbol);
	SRCEXPDS_EXPRSINGLE_DECL(type);
	SRCEXPDS_EXPRSINGLE_DECL(typedef);
	SRCEXPDS_EXPRSINGLE_DECL(variable);
	SRCEXPDS_EXPRSINGLE_DECL(void);
	SRCEXPDS_EXPRSINGLE_DECL(while);

	static VariableType const * make_expression_type(SourceTokenizerDS * in, std::vector<SourceExpression::Pointer> * blocks, SourceContext * context);

	static std::map<std::string, expression_single_handler> _expression_single_handlers;
};



#endif /* HPP_SourceExpressionDS_ */


