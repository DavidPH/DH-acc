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

/* SourceExpression/ValueBlock.cpp
**
** Defines the SourceExpression_ValueBlock class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceContext.hpp"
#include "../SourceException.hpp"



class SourceExpression_ValueBlock : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueBlock, SourceExpression);

public:
	SourceExpression_ValueBlock(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position);

	virtual VariableType const * getType() const;

	virtual void makeObjects(ObjectVector * objects);

	virtual void makeObjectsCast(ObjectVector * objects, VariableType const * type);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	std::vector<SourceExpression::Pointer> _expressions;
	VariableType const * _type;
};



SourceExpression::Pointer SourceExpression::create_value_block(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position)
{
	return new SourceExpression_ValueBlock(expressions, position);
}



SourceExpression_ValueBlock::SourceExpression_ValueBlock(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position) : Super(position), _expressions(expressions)
{
	std::vector<VariableType const *> types(_expressions.size());

	for (size_t i(0); i < _expressions.size(); ++i)
		types[i] = _expressions[i]->getType();

	_type = SourceContext::global_context.getVariableType_block(types);
}

VariableType const * SourceExpression_ValueBlock::getType() const
{
	return _type;
}

void SourceExpression_ValueBlock::makeObjects(ObjectVector * objects)
{
	Super::recurse_makeObjects(objects);

	for (size_t i(0); i < _expressions.size(); ++i)
		_expressions[i]->makeObjects(objects);
}

void SourceExpression_ValueBlock::makeObjectsCast(ObjectVector * objects, VariableType const * type)
{
	if (type->vt == VariableType::VT_VOID)
	{
		makeObjects(objects);
	}
	else
	{
		Super::recurse_makeObjectsCast(objects, type);

		if (_expressions.size() != type->types.size())
			throw SourceException("incorrect number of expressions to cast", position, getName());

		for (size_t i(0); i < _expressions.size(); ++i)
			_expressions[i]->makeObjectsCast(objects, type->types[i]);
	}
}

void SourceExpression_ValueBlock::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	for (size_t i(0); i < _expressions.size(); ++i)
		_expressions[i]->makeObjectsGet(objects);
}

void SourceExpression_ValueBlock::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_ValueBlock(";
	Super::printDebug(out);
	*out << " ";
		*out << "expressions=(";
		print_debug(out, _expressions);
		*out << ")";

		*out << ", ";

		*out << "type=(";
		print_debug(out, _type);
		*out << ")";
	*out << ")";
}



