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

/* SourceExpressionDS/BinaryMul.cpp
**
** Defines the SourceExpressionDS_BinaryMul class and methods.
*/

#include "Binary.hpp"

#include "../ObjectToken.hpp"



class SourceExpressionDS_BinaryMul : public SourceExpressionDS_Binary
{
public:
	SourceExpressionDS_BinaryMul(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	virtual SourceExpressionDS_BinaryMul * clone() const;

	virtual void makeObjects(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;
};



SourceExpressionDS SourceExpressionDS::make_expression_binary_mul(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position)
{
	return new SourceExpressionDS_BinaryMul(exprL, exprR, position);
}



SourceExpressionDS_BinaryMul::SourceExpressionDS_BinaryMul(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position) : SourceExpressionDS_Binary(exprL, exprR, position)
{

}

SourceExpressionDS_BinaryMul * SourceExpressionDS_BinaryMul::clone() const
{
	return new SourceExpressionDS_BinaryMul(*this);
}

void SourceExpressionDS_BinaryMul::makeObjects(std::vector<ObjectToken> * const objects) const
{
	SourceExpressionDS_Binary::makeObjects(objects);

	switch (getType())
	{
	case SourceExpressionDS::ET_FIXED:
		objects->push_back(ObjectToken(ObjectToken::OCODE_MULFIXED, getPosition()));
		break;

	case SourceExpressionDS::ET_INT:
		objects->push_back(ObjectToken(ObjectToken::OCODE_MUL, getPosition()));
		break;

	case SourceExpressionDS::ET_VOID:
		break;
	}
}

void SourceExpressionDS_BinaryMul::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_BinaryMul(";
	SourceExpressionDS_Binary::printDebug(out);
	*out << ")";
}



