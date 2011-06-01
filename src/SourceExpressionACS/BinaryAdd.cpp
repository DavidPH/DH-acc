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

/* SourceExpressionACS/BinaryAdd.cpp
**
** SourceExpressionACSBinaryAdd class and methods.
*/

#include "../SourceExpressionACS.hpp"



class SourceExpressionACSBinaryAdd : public SourceExpressionACSBase
{
public:
	SourceExpressionACSBinaryAdd(SourceExpressionACS const & exprL, SourceExpressionACS const & exprR);

	virtual SourceExpressionACSBinaryAdd * clone() const;

	virtual ExpressionType getType() const;

	virtual bool isConstant() const;

	virtual void makeObjects(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionACS _exprL;
	SourceExpressionACS _exprR;
};



SourceExpressionACS SourceExpressionACS::make_expression_binary_add(SourceExpressionACS const & exprL, SourceExpressionACS const & exprR)
{
	return new SourceExpressionACSBinaryAdd(exprL, exprR);
}



SourceExpressionACSBinaryAdd::SourceExpressionACSBinaryAdd(SourceExpressionACS const & exprL, SourceExpressionACS const & exprR) : SourceExpressionACSBase(_exprL.getPosition()), _exprL(exprL), _exprR(exprR)
{

}

SourceExpressionACSBinaryAdd * SourceExpressionACSBinaryAdd::clone() const
{
	return new SourceExpressionACSBinaryAdd(*this);
}

SourceExpressionACSBase::ExpressionType SourceExpressionACSBinaryAdd::getType() const
{
	return get_promoted_type(_exprL.getType(), _exprR.getType());
}

bool SourceExpressionACSBinaryAdd::isConstant() const
{
	return _exprL.isConstant() && _exprR.isConstant();
}

void SourceExpressionACSBinaryAdd::makeObjects(std::vector<ObjectToken> * const objects) const
{
	// TODO
}

void SourceExpressionACSBinaryAdd::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionACSBinaryAdd(";
		*out << "type=(";
		print_debug(out, getType());
		*out << ")";

		*out << ", ";

		*out << "exprL=(";
		print_debug(out, _exprL);
		*out << ")";

		*out << ", ";

		*out << "exprR=(";
		print_debug(out, _exprR);
		*out << ")";

	*out << ")";
}



