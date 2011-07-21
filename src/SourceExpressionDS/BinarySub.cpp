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

/* SourceExpressionDS/BinarySub.cpp
**
** Defines the SourceExpressionDS_BinarySub class and methods.
*/

#include "Binary.hpp"

#include "../ObjectToken.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_BinarySub : public SourceExpressionDS_Binary
{
public:
	SourceExpressionDS_BinarySub(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	virtual SourceExpressionDS_BinarySub * clone() const;

	virtual char const * getName() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;
};



SourceExpressionDS SourceExpressionDS::make_expression_binary_sub(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position)
{
	return new SourceExpressionDS_BinarySub(exprL, exprR, position);
}



SourceExpressionDS_BinarySub::SourceExpressionDS_BinarySub(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position) : SourceExpressionDS_Binary(exprL, exprR, position)
{

}

SourceExpressionDS_BinarySub * SourceExpressionDS_BinarySub::clone() const
{
	return new SourceExpressionDS_BinarySub(*this);
}

char const * SourceExpressionDS_BinarySub::getName() const
{
	return "SourceExpressionDS_BinarySub";
}

void SourceExpressionDS_BinarySub::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	SourceExpressionDS_Binary::makeObjectsGet(objects);

	switch (getType()->type)
	{
	case SourceVariable::VT_ACSFUNC:
	case SourceVariable::VT_ASMFUNC:
	case SourceVariable::VT_LNSPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRING:
	case SourceVariable::VT_STRUCT:
	case SourceVariable::VT_VOID:
		throw SourceException("invalid VT", getPosition(), getName());

	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_REAL:
		objects->push_back(ObjectToken(ObjectToken::OCODE_SUB, getPosition()));
		break;
	}
}

void SourceExpressionDS_BinarySub::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_BinarySub(";
	SourceExpressionDS_Binary::printDebug(out);
	*out << ")";
}



