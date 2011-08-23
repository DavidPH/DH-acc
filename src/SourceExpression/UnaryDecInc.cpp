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
#include "../SourceContext.hpp"
#include "../SourceException.hpp"
#include "../VariableType.hpp"



class SourceExpression_UnaryDecInc : public SourceExpression_Unary
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_UnaryDecInc, SourceExpression_Unary);

public:
	SourceExpression_UnaryDecInc(SourceExpression * expr, bool inc, bool suf, SourcePosition const & position);

	virtual void makeObjects(ObjectVector * objects);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * const out) const;

private:
	void doDecInc(ObjectVector * objects, bool inc);

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



SourceExpression_UnaryDecInc::SourceExpression_UnaryDecInc(SourceExpression * expr, bool inc, bool suf, SourcePosition const & position) : Super(expr, NULL, position), _inc(inc), _suf(suf)
{

}

void SourceExpression_UnaryDecInc::doDecInc(ObjectVector * objects, bool inc)
{
	switch (getType()->vt)
	{
	case VariableType::VT_CHAR:
	case VariableType::VT_INT:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue(1));
		break;

	case VariableType::VT_POINTER:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue(getType()->refType->size()));
		break;

	case VariableType::VT_REAL:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue(1.0));
		break;

	default:
		throw SourceException("invalid VT", position, getName());
	}

	objects->addToken(inc ? OCODE_ADD : OCODE_SUB);
}

void SourceExpression_UnaryDecInc::makeObjects(ObjectVector * objects)
{
	// Only modify for the first evaluation.
	// Note that _suf is irrelevant, since nothing is yielded to stack.
	if (evaluations == 0)
	{
		Super::recurse_makeObjectsGet(objects);

		doDecInc(objects, _inc);

		expr->makeObjectsSet(objects);

		make_objects_cast(objects, getType(), SourceContext::global_context.getVariableType(VariableType::VT_VOID), position);
	}
	else
	{
		Super::recurse_makeObjects(objects);
	}
}

void SourceExpression_UnaryDecInc::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	// Only modify for the first evaluation.
	if (evaluations == 1)
	{
		if (_suf) objects->addToken(OCODE_DUP);

		doDecInc(objects, _inc);

		expr->makeObjectsSet(objects);

		if (_suf) objects->addToken(OCODE_DROP);
	}
	// Have undo the inc/dec when re-evaluating.
	else if (_suf)
	{
		doDecInc(objects, !_inc);
	}
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



