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

/* SourceExpression.hpp
**
** Defines the SourceExpression class.
*/

#ifndef HPP_SourceExpression_
#define HPP_SourceExpression_

#include "CounterPointer.hpp"
#include "SourcePosition.hpp"
#include "SourceVariable.hpp"

#include <ostream>
#include <string>
#include <vector>

class ObjectExpression;
class ObjectVector;
class SourceContext;
class SourceTokenC;
class VariableType;



class SourceExpression : public Counter
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression, Counter);

public:
	void addLabel(std::string const & label);

	virtual bool canMakeObject() const;
	virtual bool canMakeObjectAddress() const;

	virtual bool canMakeObjectsAddress() const;

	virtual VariableType const * getType() const;

	virtual CounterPointer<ObjectExpression> makeObject() const;
	virtual CounterPointer<ObjectExpression> makeObjectAddress() const;

	void makeObjects(ObjectVector * objects);

	virtual void makeObjectsAddress(ObjectVector * objects);

	virtual void makeObjectsCall(ObjectVector * objects, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack);

	virtual void makeObjectsCast(ObjectVector * objects, VariableType const * type);

	virtual void makeObjectsGet(ObjectVector * objects);
	virtual void makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	virtual void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names);

	virtual void makeObjectsSet(ObjectVector * objects);
	virtual void makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	virtual void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names);



	friend class SourceVariable;

	friend void print_debug(std::ostream * out, SourceExpression const & in);

	static Pointer create_binary_add(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_and(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_array(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_assign(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_div(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_eq(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_ge(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_gt(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_ior(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_le(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_lt(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_mod(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_mul(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_ne(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_shiftl(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_shiftr(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_sub(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_xor(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	static Pointer create_branch_and(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_break(SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_call(SourceExpression * expr, std::vector<SourceExpression::Pointer> const & args, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_continue(SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_for(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_goto(std::string const & label, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_if(SourceExpression * exprCondition, SourceExpression * exprIf, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_if(SourceExpression * exprCondition, SourceExpression * exprIf, SourceExpression * exprElse, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_ior(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_not(SourceExpression * expr, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_return(SourceExpression * expr, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_switch(SourceExpression * expr, SourceExpression * exprCases, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_while(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_xor(SourceExpression * exprL, SourceExpression * exprR, SourceContext * context, SourcePosition const & position);

	static Pointer create_unary_dec_pre(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_dec_suf(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_dereference(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_inc_pre(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_inc_suf(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_not(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_reference(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_sub(SourceExpression * expr, SourcePosition const & position);

	static Pointer create_root_delay(SourceExpression * expr, SourceContext * context, SourcePosition const & position);
	static Pointer create_root_out(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_root_script(VariableType const * type, SourcePosition const & position);

	static Pointer create_value_block(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position);
	static Pointer create_value_cast(SourceExpression * expr, VariableType const * type, SourcePosition const & position);
	static Pointer create_value_char(SourceTokenC const & token);
	static Pointer create_value_data(VariableType const * type, bool garbage, SourcePosition const & position);
	static Pointer create_value_int(int value, SourcePosition const & position);
	static Pointer create_value_int(SourceTokenC const & token);
	static Pointer create_value_member(SourceExpression * expr, SourceTokenC const & token);
	static Pointer create_value_real(SourceTokenC const & token);
	static Pointer create_value_string(SourceTokenC const & token);
	static Pointer create_value_variable(SourceVariable const & var, SourcePosition const & position);

	static VariableType const * get_promoted_type(VariableType const * type1, VariableType const * type2, SourcePosition const & position);

	static void make_objects_call_asmfunc(ObjectVector * objects, SourceVariable::VariableData_AsmFunc const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position);

	static void make_objects_call_function(ObjectVector * objects, SourceVariable::VariableData_Function const & data, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position);

	static void make_objects_call_linespec(ObjectVector * objects, SourceVariable::VariableData_LineSpec const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position);

	static void make_objects_call_native(ObjectVector * objects, SourceVariable::VariableData_Native const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position);

	static void make_objects_call_script(ObjectVector * objects, VariableType const * type, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position);

	static void make_objects_cast(ObjectVector * objects, VariableType const * typeFrom, VariableType const * typeTo, SourcePosition const & position);

protected:
	SourceExpression(SourcePosition const & position);

	virtual void printDebug(std::ostream * out) const;

	void recurse_makeObjectsAddress(ObjectVector * objects);

	void recurse_makeObjectsCall(ObjectVector * objects, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack);

	void recurse_makeObjectsCast(ObjectVector * objects, VariableType const * type);

	void recurse_makeObjectsGet(ObjectVector * objects);
	void recurse_makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	void recurse_makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names);

	void recurse_makeObjectsSet(ObjectVector * objects);
	void recurse_makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions);
	void recurse_makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names);

	int evaluations;
	SourcePosition position;

private:
	SourceExpression();

	std::vector<std::string> _labels;
};



#endif /* HPP_SourceExpression_ */



