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

/* SourceExpressionDS/Binary.cpp
**
** Defines the SourceExpressionDS_Binary methods.
*/

#include "Binary.hpp"



SourceExpressionDS_Binary::SourceExpressionDS_Binary(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position) : SourceExpressionDS_Base(position), _exprL(exprL), _exprR(exprR)
{
	SourceVariable::VariableType const * type(getType());

	if (_exprL.getType() != type)
		_exprL = SourceExpressionDS::make_expression_cast(_exprL, type, getPosition());

	if (_exprR.getType() != type)
		_exprR = SourceExpressionDS::make_expression_cast(_exprR, type, getPosition());
}
SourceExpressionDS_Binary::SourceExpressionDS_Binary(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, bool const castL, SourcePosition const & position) : SourceExpressionDS_Base(position), _exprL(exprL), _exprR(exprR)
{
	if (castL)
	{
		if (_exprL.getType() != _exprR.getType())
			_exprL = SourceExpressionDS::make_expression_cast(_exprL, _exprR.getType(), getPosition());
	}
	else
	{
		if (_exprR.getType() != _exprL.getType())
			_exprR = SourceExpressionDS::make_expression_cast(_exprR, _exprL.getType(), getPosition());
	}
}

SourceVariable::VariableType const * SourceExpressionDS_Binary::getType() const
{
	return SourceExpressionDS::get_promoted_type(_exprL.getType(), _exprR.getType());
}

bool SourceExpressionDS_Binary::isConstant() const
{
	return _exprL.isConstant() && _exprR.isConstant();
}

void SourceExpressionDS_Binary::makeObjectsGet(ObjectVector * objects) const
{
	_exprL.makeObjectsGet(objects);
	_exprR.makeObjectsGet(objects);
}

void SourceExpressionDS_Binary::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_Binary(";
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



