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

/* SourceExpression/UnaryDecInc.cpp
**
** Defines the SourceExpression_UnaryDecInc class and methods.
*/

#include "Unary.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_UnaryDecInc : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_UnaryDecInc, SourceExpression_Unary);

public:
	SourceExpression_UnaryDecInc(SourceExpression * expr, bool inc, bool suf, SourcePosition const & position);

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	void doDecInc(ObjectVector * objects, bool inc);

	virtual void virtual_makeObjects(ObjectVector * objects);

	virtual void virtual_makeObjectsGet(ObjectVector * objects);

	bool _inc;
	bool _suf;
};



SourceExpression::Pointer SourceExpression::create_unary_dec_pre(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryDecInc(expr, false, false, position);
}
SourceExpression::Pointer SourceExpression::create_unary_dec_suf(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryDecInc(expr, false, true, position);
}
SourceExpression::Pointer SourceExpression::create_unary_inc_pre(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryDecInc(expr, true, false, position);
}
SourceExpression::Pointer SourceExpression::create_unary_inc_suf(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_UnaryDecInc(expr, true, true, position);
}



SourceExpression_UnaryDecInc::SourceExpression_UnaryDecInc(SourceExpression * expr_, bool inc, bool suf, SourcePosition const & position_) : Super(expr_, NULL, position_), _inc(inc), _suf(suf)
{

}

void SourceExpression_UnaryDecInc::doDecInc(ObjectVector * objects, bool inc)
{
	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
		objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(1));
		break;

	case VariableType::VT_POINTER:
		objects->addToken(OCODE_GET_LITERAL32I, objects->getValue(getType()->refType->size(position)));
		break;

	case VariableType::VT_REAL:
		objects->addToken(OCODE_GET_LITERAL32F, objects->getValue(1.0));
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}

	// FIXME: Should be based on type.
	objects->addToken(inc ? OCODE_ADD32I : OCODE_SUB32I);
}

void SourceExpression_UnaryDecInc::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_UnaryDecInc(";
	Super::printDebug(out);
		*out << "inc=(";
		print_debug(out, _inc);
		*out << ")";

		*out << ", ";

		*out << "suf=(";
		print_debug(out, _suf);
		*out << ")";
	*out << ")";
}

void SourceExpression_UnaryDecInc::virtual_makeObjects(ObjectVector * objects)
{
	Super::recurse_makeObjects(objects);

	// Only modify for the first evaluation.
	// Note that _suf is irrelevant, since nothing is yielded to stack.
	if (evaluations == 1)
	{
		expr->makeObjectsGet(objects);

		doDecInc(objects, _inc);

		expr->makeObjectsSet(objects);
	}
}

void SourceExpression_UnaryDecInc::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	// Only modify for the first evaluation.
	if (evaluations == 1)
	{
		if (_suf) objects->addToken(OCODE_STACK_DUP32);

		doDecInc(objects, _inc);

		expr->makeObjectsAccess(objects);

		if (_suf) objects->addToken(OCODE_STACK_DROP32);
	}
	// Have undo the inc/dec when re-evaluating.
	else if (_suf)
	{
		doDecInc(objects, !_inc);
	}
}


