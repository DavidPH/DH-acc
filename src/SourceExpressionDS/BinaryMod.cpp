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

/* SourceExpressionDS/BinaryMod.cpp
**
** Defines the SourceExpressionDS_BinaryMod class and methods.
*/

#include "Binary.hpp"

#include "../ObjectToken.hpp"
#include "../SourceException.hpp"



class SourceExpressionDS_BinaryMod : public SourceExpressionDS_Binary
{
public:
	SourceExpressionDS_BinaryMod(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	virtual SourceExpressionDS_BinaryMod * clone() const;

	virtual char const * getName() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;
};



SourceExpressionDS SourceExpressionDS::make_expression_binary_mod(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position)
{
	return new SourceExpressionDS_BinaryMod(exprL, exprR, position);
}



SourceExpressionDS_BinaryMod::SourceExpressionDS_BinaryMod(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position) : SourceExpressionDS_Binary(exprL, exprR, position)
{

}

SourceExpressionDS_BinaryMod * SourceExpressionDS_BinaryMod::clone() const
{
	return new SourceExpressionDS_BinaryMod(*this);
}

char const * SourceExpressionDS_BinaryMod::getName() const
{
	return "SourceExpressionDS_BinaryMod";
}

void SourceExpressionDS_BinaryMod::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	SourceExpressionDS_Binary::makeObjectsGet(objects);

	switch (getType()->type)
	{
	case SourceVariable::VT_FIXED:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_STRING:
		objects->push_back(ObjectToken(ObjectToken::OCODE_MOD, getPosition()));
		break;

	case SourceVariable::VT_VOID:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRUCT:
		throw SourceException("invalid VT", getPosition(), getName());
	}
}

void SourceExpressionDS_BinaryMod::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_BinaryMod(";
	SourceExpressionDS_Binary::printDebug(out);
	*out << ")";
}



