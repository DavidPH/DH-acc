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

#include <ostream>
#include <vector>

class ObjectExpression;
class ObjectToken;
class SourceContext;
class SourceExpressionDS_Base;
class SourcePosition;
class SourceTokenC;
class SourceTokenizerDS;
class SourceVariable;



class SourceExpressionDS
{
public:
	enum ExpressionType
	{
		ET_FIXED,
		ET_INT,
		ET_STRING,
		ET_VOID
	};

	SourceExpressionDS();
	SourceExpressionDS(SourceExpressionDS const & expr);
	SourceExpressionDS(SourceExpressionDS_Base * const expr);
	~SourceExpressionDS();

	void addLabel(std::string const & label);

	SourcePosition const & getPosition() const;

	ExpressionType getType() const;

	bool isConstant() const;

	ObjectExpression makeObject() const;
	void makeObjectsGet(std::vector<ObjectToken> * const objects) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects) const;

	SourceExpressionDS & operator = (SourceExpressionDS const & expr);



	friend void print_debug(std::ostream * const out, SourceExpressionDS const & in);
	friend void print_debug(std::ostream * const out, SourceExpressionDS::ExpressionType const in);

	static ExpressionType get_promoted_type(ExpressionType const type1, ExpressionType const type2);

	static SourceExpressionDS make_expression_cast(SourceExpressionDS const & expr, ExpressionType const type, SourcePosition const & position);

	static SourceExpressionDS make_expressions(SourceTokenizerDS * const tokenizer);

	static void make_objects(std::vector<SourceExpressionDS> const & expressions, std::vector<ObjectToken> * const objects);

	static SourceExpressionDS const nop;

private:
	SourceExpressionDS_Base * _expr;



	static SourceExpressionDS make_expression(SourceTokenizerDS * const tokenizer, SourceContext * const context);
	static SourceExpressionDS make_expression_single(SourceTokenizerDS * const in, SourceContext * const context);
	static void make_expressions(SourceTokenizerDS * const tokenizer, std::vector<SourceExpressionDS> * const expressions, SourceContext * const context);

	static SourceExpressionDS make_expression_binary_add(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_assign(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_div(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_mod(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_mul(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_sub(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	static SourceExpressionDS make_expression_cast_fixed(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_int(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_string(SourceExpressionDS const & expr, SourcePosition const & position);

	static SourceExpressionDS make_expression_root_block(std::vector<SourceExpressionDS> const & expressions, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_block(std::vector<SourceExpressionDS> const & expressions, std::vector<std::string> const & labels, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_lspec(SourceExpressionDS const & spec, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_out(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_term(SourcePosition const & position);
	static SourceExpressionDS make_expression_root_void(SourceExpressionDS const & expr, SourcePosition const & position);

	static SourceExpressionDS make_expression_value_fixed(SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_int(SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_number(SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_string(ObjectExpression const & symbol, SourcePosition const & position);
	static SourceExpressionDS make_expression_value_variable(SourceVariable const & var, SourcePosition const & position);
};



#endif /* HPP_SourceExpressionDS_ */



