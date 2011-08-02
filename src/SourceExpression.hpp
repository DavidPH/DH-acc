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



class SourceExpression : public Counter
{
	MAKE_ABSTRACT_COUNTER_CLASS_BASE(SourceExpression, Counter);

public:
	SourceExpression(SourcePosition const & position);

	void addLabel(std::string const & label);

	virtual bool canMakeObject() const;
	virtual bool canMakeObjectAddress() const;

	virtual bool canMakeObjectsAddress() const;

	SourcePosition const & getPosition() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual CounterPointer<ObjectExpression> makeObject() const;
	virtual CounterPointer<ObjectExpression> makeObjectAddress() const;

	virtual void makeObjectsAddress(ObjectVector * objects) const;

	virtual void makeObjectsCall(ObjectVector * objects, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack) const;

	virtual void makeObjectsCast(ObjectVector * objects, SourceVariable::VariableType const * type) const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;
	virtual void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;
	virtual void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void printDebug(std::ostream * out) const;



	friend void print_debug(std::ostream * out, SourceExpression const & in);

	static Pointer create_binary_add(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_array(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_assign(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_div(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_eq(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_ge(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_gt(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_le(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_lt(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_mod(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_mul(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_ne(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);
	static Pointer create_binary_sub(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	static Pointer create_branch_break(SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_call(SourceExpression * expr, std::vector<SourceExpression::Pointer> const & args, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_continue(SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_for(SourceExpression * exprInit, SourceExpression * exprCond, SourceExpression * exprIter, SourceExpression * exprLoop, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_if(SourceExpression * exprCondition, SourceExpression * exprIf, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_if(SourceExpression * exprCondition, SourceExpression * exprIf, SourceExpression * exprElse, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_return(SourceExpression * expr, SourceContext * context, SourcePosition const & position);
	static Pointer create_branch_while(SourceExpression * exprCondition, SourceExpression * exprWhile, SourceContext * context, SourcePosition const & position);

	static Pointer create_unary_dereference(SourceExpression * expr, SourcePosition const & position);
	static Pointer create_unary_reference(SourceExpression * expr, SourcePosition const & position);

	static Pointer create_root_delay(SourceExpression * expr, SourceContext * context, SourcePosition const & position);
	static Pointer create_root_out(SourceExpression * expr, SourcePosition const & position);

	static Pointer create_value_block(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position);
	static Pointer create_value_cast(SourceExpression * expr, SourceVariable::VariableType const * type, SourcePosition const & position);
	static Pointer create_value_char(SourceTokenC const & token);
	static Pointer create_value_data(SourceVariable::VariableType const * type, bool garbage, SourcePosition const & position);
	static Pointer create_value_int(int value, SourcePosition const & position);
	static Pointer create_value_int(SourceTokenC const & token);
	static Pointer create_value_member(SourceExpression * expr, SourceTokenC const & token);
	static Pointer create_value_real(SourceTokenC const & token);
	static Pointer create_value_string(SourceTokenC const & token);
	static Pointer create_value_variable(SourceVariable const & var, SourcePosition const & position);

	static SourceVariable::VariableType const * get_promoted_type(SourceVariable::VariableType const * type1, SourceVariable::VariableType const * type2, SourcePosition const & position);

	static void make_objects(std::vector<SourceExpression::Pointer> const & expressions, ObjectVector * objects);

	static void make_objects_call_acsfunc(ObjectVector * objects, SourceVariable::VariableData_ACSFunc const & data, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position);

	static void make_objects_call_asmfunc(ObjectVector * objects, SourceVariable::VariableData_AsmFunc const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position);

	static void make_objects_call_lnspec(ObjectVector * objects, SourceVariable::VariableData_LnSpec const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position);

	static void make_objects_call_native(ObjectVector * objects, SourceVariable::VariableData_Native const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position);

	static void make_objects_call_script(ObjectVector * objects, SourceVariable::VariableType const * type, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position);

	static void make_objects_cast(ObjectVector * objects, SourceVariable::VariableType const * typeFrom, SourceVariable::VariableType const * typeTo, SourcePosition const & position);

protected:
	std::vector<std::string> labels;
	SourcePosition position;
};



#endif /* HPP_SourceExpression_ */



