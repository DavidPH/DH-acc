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

/* SourceExpression/BinaryArray.cpp
**
** Defines the SourceExpression_BinaryArray class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"



class SourceExpression_BinaryArray : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_BinaryArray, SourceExpression);

public:
	SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position);

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;

	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const;

	virtual void printDebug(std::ostream * out) const;

protected:
	SourceExpression::Pointer _exprL;
	SourceExpression::Pointer _exprR;
};



SourceExpression::Pointer SourceExpression::create_binary_array(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position)
{
	return new SourceExpression_BinaryArray(exprL, exprR, position);
}



SourceExpression_BinaryArray::SourceExpression_BinaryArray(SourceExpression * exprL, SourceExpression * exprR, SourcePosition const & position) : Super(position), _exprL(exprL), _exprR(exprR)
{
	SourceVariable::VariableType const * type(SourceVariable::get_VariableType(SourceVariable::VT_INT));

	if (_exprR->getType() != type)
		_exprR = create_value_cast(_exprR, type, position);
}

SourceVariable::VariableType const * SourceExpression_BinaryArray::getType() const
{
	return _exprL->getType()->refType;
}

bool SourceExpression_BinaryArray::isConstant() const
{
	return false;
}

void SourceExpression_BinaryArray::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	std::vector<SourceExpression::Pointer> dimensions(1, _exprR);
	_exprL->makeObjectsGetArray(objects, &dimensions);
}
void SourceExpression_BinaryArray::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	objects->addLabel(labels);

	dimensions->push_back(_exprR);
	_exprL->makeObjectsGetArray(objects, dimensions);
}

void SourceExpression_BinaryArray::makeObjectsSet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	std::vector<SourceExpression::Pointer> dimensions(1, _exprR);
	_exprL->makeObjectsSetArray(objects, &dimensions);
}
void SourceExpression_BinaryArray::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	objects->addLabel(labels);

	dimensions->push_back(_exprR);
	_exprL->makeObjectsSetArray(objects, dimensions);
}

void SourceExpression_BinaryArray::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_BinaryArray(";
	Super::printDebug(out);
	*out << " ";
		*out << "exprL=(";
		print_debug(out, _exprL);
		*out << ")";

		*out << ", ";

		*out << "exprR=(";
		print_debug(out, _exprR);
		*out << ")";
	*out << ")";
}


