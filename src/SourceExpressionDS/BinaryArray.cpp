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

/* SourceExpressionDS/BinaryArray.cpp
**
** Defines the SourceExpressionDS_BinaryArray class and methods.
*/

#include "Binary.hpp"



class SourceExpressionDS_BinaryArray : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_BinaryArray(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	virtual SourceExpressionDS_BinaryArray * clone() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual char const * getName() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGetArray(ObjectVector * objects, int dimensions) const;

	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSetArray(ObjectVector * objects, int dimensions) const;

	virtual void printDebug(std::ostream * const out) const;

protected:
	SourceExpressionDS _exprL;
	SourceExpressionDS _exprR;
};



SourceExpressionDS SourceExpressionDS::make_expression_binary_array(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position)
{
	return new SourceExpressionDS_BinaryArray(exprL, exprR, position);
}



SourceExpressionDS_BinaryArray::SourceExpressionDS_BinaryArray(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position) : SourceExpressionDS_Base(position), _exprL(exprL), _exprR(exprR)
{
	SourceVariable::VariableType const * type(SourceVariable::get_VariableType(SourceVariable::VT_INT));

	if (_exprR.getType() != type)
		_exprR = SourceExpressionDS::make_expression_cast(_exprR, type, getPosition());
}

SourceExpressionDS_BinaryArray * SourceExpressionDS_BinaryArray::clone() const
{
	return new SourceExpressionDS_BinaryArray(*this);
}

SourceVariable::VariableType const * SourceExpressionDS_BinaryArray::getType() const
{
	return _exprL.getType()->refType;
}

bool SourceExpressionDS_BinaryArray::isConstant() const
{
	return false;
}

char const * SourceExpressionDS_BinaryArray::getName() const
{
	return "SourceExpressionDS_BinaryArray";
}

void SourceExpressionDS_BinaryArray::makeObjectsGet(ObjectVector * objects) const
{
	_exprR.makeObjectsGet(objects);
	_exprL.makeObjectsGetArray(objects, 1);
}
void SourceExpressionDS_BinaryArray::makeObjectsGetArray(ObjectVector * objects, int dimensions) const
{
	_exprR.makeObjectsGet(objects);
	_exprL.makeObjectsGetArray(objects, dimensions+1);
}

void SourceExpressionDS_BinaryArray::makeObjectsSet(ObjectVector * objects) const
{
	_exprR.makeObjectsGet(objects);
	_exprL.makeObjectsSetArray(objects, 1);
}
void SourceExpressionDS_BinaryArray::makeObjectsSetArray(ObjectVector * objects, int dimensions) const
{
	_exprR.makeObjectsGet(objects);
	_exprL.makeObjectsSetArray(objects, dimensions+1);
}

void SourceExpressionDS_BinaryArray::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_BinaryArray(";
	SourceExpressionDS_Base::printDebug(out);
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



