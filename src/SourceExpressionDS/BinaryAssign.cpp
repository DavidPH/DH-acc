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

/* SourceExpressionDS/BinaryAssign.cpp
**
** Defines the SourceExpressionDS_BinaryAssign class and methods.
*/

#include "Binary.hpp"

#include "../ObjectToken.hpp"



class SourceExpressionDS_BinaryAssign : public SourceExpressionDS_Binary
{
public:
	SourceExpressionDS_BinaryAssign(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position);

	virtual SourceExpressionDS_BinaryAssign * clone() const;

	virtual char const * getName() const;

	virtual void makeObjectsGet(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;
};



SourceExpressionDS SourceExpressionDS::make_expression_binary_assign(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position)
{
	return new SourceExpressionDS_BinaryAssign(exprL, exprR, position);
}



SourceExpressionDS_BinaryAssign::SourceExpressionDS_BinaryAssign(SourceExpressionDS const & exprL, SourceExpressionDS const & exprR, SourcePosition const & position) : SourceExpressionDS_Binary(exprL, exprR, false, position)
{

}

SourceExpressionDS_BinaryAssign * SourceExpressionDS_BinaryAssign::clone() const
{
	return new SourceExpressionDS_BinaryAssign(*this);
}

char const * SourceExpressionDS_BinaryAssign::getName() const
{
	return "SourceExpressionDS_BinaryAssign";
}

void SourceExpressionDS_BinaryAssign::makeObjectsGet(std::vector<ObjectToken> * const objects) const
{
	_exprR.makeObjectsGet(objects);
	_exprL.makeObjectsSet(objects);
}

void SourceExpressionDS_BinaryAssign::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_BinaryAssign(";
	SourceExpressionDS_Binary::printDebug(out);
	*out << ")";
}



