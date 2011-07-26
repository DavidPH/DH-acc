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

#include "SourceVariable.hpp"

#include <ostream>
#include <stdint.h>
#include <vector>

class ObjectExpression;
class ObjectVector;
class SourceContext;
class SourceExpressionDS_Base;
class SourcePosition;
class SourceTokenC;
class SourceTokenizerDS;



class SourceExpressionDS
{
public:
	SourceExpressionDS();
	SourceExpressionDS(SourceExpressionDS const & expr);
	SourceExpressionDS(SourceExpressionDS_Base * const expr);
	~SourceExpressionDS();

	void addLabel(std::string const & label);

	SourcePosition const & getPosition() const;

	SourceVariable::VariableType const * getType() const;

	std::vector<SourceExpressionDS> getVector() const;

	bool isConstant() const;

	ObjectExpression makeObject() const;

	void makeObjectsCall(ObjectVector * objects, std::vector<SourceExpressionDS> const & args) const;

	void makeObjectsGet(ObjectVector * objects) const;
	void makeObjectsGetArray(ObjectVector * objects, int dimensions) const;
	void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	void makeObjectsSet(ObjectVector * objects) const;
	void makeObjectsSetArray(ObjectVector * objects, int dimensions) const;
	void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	SourceExpressionDS & operator = (SourceExpressionDS const & expr);



	friend void print_debug(std::ostream * const out, SourceExpressionDS const & in);

	static SourceVariable::VariableType const * get_promoted_type(SourceVariable::VariableType const * const type1, SourceVariable::VariableType const * const type2);

	static void init();

	static SourceExpressionDS make_expression_cast(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);

	static SourceExpressionDS make_expressions(SourceTokenizerDS * const tokenizer);

	static void make_objects(std::vector<SourceExpressionDS> const & expressions, ObjectVector * objects);

	static void make_objects_call_acsfunc(ObjectVector * objects, SourceVariable::VariableData_ACSFunc const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);

	static void make_objects_call_asmfunc(ObjectVector * objects, SourceVariable::VariableData_AsmFunc const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);

	static void make_objects_call_lnspec(ObjectVector * objects, SourceVariable::VariableData_LnSpec const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);

	static void make_objects_call_native(ObjectVector * objects, SourceVariable::VariableData_Native const & data, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);

	static void make_objects_call_script(ObjectVector * objects, SourceVariable::VariableType const * type, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);

	static SourceExpressionDS const nop;

private:
	typedef SourceExpressionDS (*expression_single_handler)(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);



	SourceExpressionDS_Base * _expr;



	static SourceExpressionDS make_expression(SourceTokenizerDS * const in, std::vector<SourceExpressionDS> * const blocks, SourceContext * const context);
	static void make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceVariable::VariableType const *> * argTypes, SourceVariable::VariableType const * * returnType);
	static void make_expression_arglist(SourceTokenizerDS * in, std::vector<SourceVariable::VariableType const *> * argTypes, std::vector<std::string> * argNames, int * argCount, SourceContext * argContext, SourceVariable::VariableType const * * returnType);
	static void make_expressions(SourceTokenizerDS * const tokenizer, std::vector<SourceExpressionDS> * const expressions, std::vector<SourceExpressionDS> * const blocks, SourceContext * const context);

	static SourceExpressionDS make_expression_binary_add(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_array(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_assign(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_div(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_mod(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_mul(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);
	static SourceExpressionDS make_expression_binary_sub(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	static SourceExpressionDS make_expression_cast_acsfunc(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_array(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_char(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_int(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_lnspec(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_native(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_real(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_script(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_string(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_struct(SourceExpressionDS const & expr, SourceVariable::VariableType const * const type, SourcePosition const & position);
	static SourceExpressionDS make_expression_cast_void(SourceExpressionDS const & expr, SourcePosition const & position);

	static SourceExpressionDS make_expression_root_block(std::vector<SourceExpressionDS> const & expressions, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_block(std::vector<SourceExpressionDS> const & expressions, std::vector<std::string> const & labels, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_call(SourceExpressionDS const & expr, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_data(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_delay(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_if(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceContext * context, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_if(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprIf, SourceExpressionDS const & exprElse, SourceContext * context, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_lspec(SourceExpressionDS const & spec, std::vector<SourceExpressionDS> const & args, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_out(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_return(SourceExpressionDS const & expr, SourceContext const & context, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_term(SourcePosition const & position);
	static SourceExpressionDS make_expression_root_void(SourceExpressionDS const & expr, SourcePosition const & position);
	static SourceExpressionDS make_expression_root_while(SourceExpressionDS const & exprCondition, SourceExpressionDS const & exprWhile, SourceContext * context, SourcePosition const & position);

	static SourceExpressionDS make_expression_single(SourceTokenizerDS * in, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_acsfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_asmfunc(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_const(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_delay(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_if(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_lnspec(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_native(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_out(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_return(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_script(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_struct(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_term(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_typedef(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_var(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_void(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);
	static SourceExpressionDS make_expression_single_while(SourceTokenizerDS * in, SourceTokenC const & token, std::vector<SourceExpressionDS> * blocks, SourceContext * context);

	static SourceExpressionDS make_expression_value_char(SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_int(SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_member(SourceExpressionDS const & expr, SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_real(SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_string(SourceTokenC const & token);
	static SourceExpressionDS make_expression_value_variable(SourceVariable const & var, SourcePosition const & position);

	static std::map<std::string, expression_single_handler> _expression_single_handlers;
};



#endif /* HPP_SourceExpressionDS_ */



