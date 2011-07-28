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
#include "../SourceException.hpp"



class SourceExpression_ValueBlock : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueBlock, SourceExpression);

public:
	SourceExpression_ValueBlock(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position);

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsCast(ObjectVector * objects, SourceVariable::VariableType const * type) const;

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

private:
	std::vector<SourceExpression::Pointer> _expressions;
	SourceVariable::VariableType const * _type;
};



SourceExpression::Pointer SourceExpression::create_value_block(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position)
{
	return new SourceExpression_ValueBlock(expressions, position);
}



SourceExpression_ValueBlock::SourceExpression_ValueBlock(std::vector<SourceExpression::Pointer> const & expressions, SourcePosition const & position) : Super(position), _expressions(expressions)
{
	std::vector<SourceVariable::VariableType const *> types(_expressions.size());

	for (size_t i(0); i < _expressions.size(); ++i)
		types[i] = _expressions[i]->getType();

	_type = SourceVariable::get_VariableType_block(types);
}

SourceVariable::VariableType const * SourceExpression_ValueBlock::getType() const
{
	return _type;
}

void SourceExpression_ValueBlock::makeObjectsCast(ObjectVector * objects, SourceVariable::VariableType const * type) const
{
	makeObjectsGet(objects);

	if (_expressions.size() != type->types.size())
		throw SourceException("incorrect number of expressions to cast", position, getName());

	for (size_t i(0); i < _expressions.size(); ++i)
		_expressions[i]->makeObjectsCast(objects, type->types[i]);
}

void SourceExpression_ValueBlock::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

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



