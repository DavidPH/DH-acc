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

/* SourceExpressionDS/RootTerm.cpp
**
** Defines the SourceExpressionDS_RootTerm class and methods.
*/

#include "Base.hpp"

#include "../ObjectToken.hpp"



class SourceExpressionDS_RootTerm : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_RootTerm(SourcePosition const & position);

	virtual SourceExpressionDS_RootTerm * clone() const;

	virtual SourceExpressionDS::ExpressionType getType() const;

	virtual bool isConstant() const;

	virtual void makeObjects(std::vector<ObjectToken> * const objects) const;

	virtual void printDebug(std::ostream * const out) const;

private:
};



SourceExpressionDS SourceExpressionDS::make_expression_root_term(SourcePosition const & position)
{
	return new SourceExpressionDS_RootTerm(position);
}



SourceExpressionDS_RootTerm::SourceExpressionDS_RootTerm(SourcePosition const & position) : SourceExpressionDS_Base(position)
{

}

SourceExpressionDS_RootTerm * SourceExpressionDS_RootTerm::clone() const
{
	return new SourceExpressionDS_RootTerm(*this);
}

SourceExpressionDS::ExpressionType SourceExpressionDS_RootTerm::getType() const
{
	return SourceExpressionDS::ET_VOID;
}

bool SourceExpressionDS_RootTerm::isConstant() const
{
	return false;
}

void SourceExpressionDS_RootTerm::makeObjects(std::vector<ObjectToken> * const objects) const
{
	objects->push_back(ObjectToken(ObjectToken::OCODE_TERMINATE, getPosition()));
}

void SourceExpressionDS_RootTerm::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_RootTerm(";
	SourceExpressionDS_Base::printDebug(out);
	*out << ")";
}



