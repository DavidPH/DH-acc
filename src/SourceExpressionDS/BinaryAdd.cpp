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

/* SourceExpressionDS/BinaryAdd.cpp
**
** Defines the SourceExpressionDS_BinaryAdd class and methods.
*/

#include "Binary.hpp"

#include "../ObjectToken.hpp"



class SourceExpressionDS_BinaryAdd : public SourceExpressionDS_Binary
{
public:
	SourceExpressionDS_BinaryAdd(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	virtual SourceExpressionDS_BinaryAdd * clone() const;

	virtual char const * getName() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;
};



SourceExpressionDS SourceExpressionDS::make_expression_binary_add(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position)
{
	return new SourceExpressionDS_BinaryAdd(exprL, exprR, position);
}



SourceExpressionDS_BinaryAdd::SourceExpressionDS_BinaryAdd(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position) : SourceExpressionDS_Binary(exprL, exprR, position)
{

}

SourceExpressionDS_BinaryAdd * SourceExpressionDS_BinaryAdd::clone() const
{
	return new SourceExpressionDS_BinaryAdd(*this);
}

char const * SourceExpressionDS_BinaryAdd::getName() const
{
	return "SourceExpressionDS_BinaryAdd";
}

void SourceExpressionDS_BinaryAdd::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	SourceExpressionDS_Binary::makeObjectsGet(objects);

	switch (getType())
	{
	case SourceExpressionDS::ET_FIXED:
	case SourceExpressionDS::ET_INT:
		objects->push_back(ObjectToken(ObjectToken::OCODE_ADD, getPosition()));
		break;

	case SourceExpressionDS::ET_VOID:
		break;
	}
}

void SourceExpressionDS_BinaryAdd::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_BinaryAdd(";
	SourceExpressionDS_Binary::printDebug(out);
	*out << ")";
}



