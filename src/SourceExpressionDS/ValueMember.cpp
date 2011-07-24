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

/* SourceExpressionDS/ValueMember.cpp
**
** Defines the SourceExpressionDS_ValueMember class and methods.
*/

#include "Base.hpp"

#include "../print_debug.hpp"
#include "../SourceTokenC.hpp"



class SourceExpressionDS_ValueMember : public SourceExpressionDS_Base
{
public:
	SourceExpressionDS_ValueMember(SourceExpressionDS const & expr, SourceTokenC const & token);

	virtual SourceExpressionDS_ValueMember * clone() const;

	virtual char const * getName() const;

	virtual SourceVariable::VariableType const * getType() const;

	virtual bool isConstant() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void printDebug(std::ostream * const out) const;

private:
	SourceExpressionDS _expr;
	std::string _name;
};



SourceExpressionDS SourceExpressionDS::make_expression_value_member(SourceExpressionDS const & expr, SourceTokenC const & token)
{
	return new SourceExpressionDS_ValueMember(expr, token);
}



SourceExpressionDS_ValueMember::SourceExpressionDS_ValueMember(SourceExpressionDS const & expr, SourceTokenC const & token) : SourceExpressionDS_Base(token.getPosition()), _expr(expr), _name(token.getData())
{

}

SourceExpressionDS_ValueMember * SourceExpressionDS_ValueMember::clone() const
{
	return new SourceExpressionDS_ValueMember(*this);
}

char const * SourceExpressionDS_ValueMember::getName() const
{
	return "SourceExpressionDS_ValueMember";
}

SourceVariable::VariableType const * SourceExpressionDS_ValueMember::getType() const
{
	return _expr.getType()->getType(_name, getPosition());
}

bool SourceExpressionDS_ValueMember::isConstant() const
{
	return false;
}

void SourceExpressionDS_ValueMember::makeObjectsGet(ObjectVector * objects) const
{
	std::vector<std::string> names(1, _name);
	_expr.makeObjectsGetMember(objects, &names);
}
void SourceExpressionDS_ValueMember::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	names->push_back(_name);
	_expr.makeObjectsGetMember(objects, names);
}

void SourceExpressionDS_ValueMember::makeObjectsSet(ObjectVector * objects) const
{
	std::vector<std::string> names(1, _name);
	_expr.makeObjectsSetMember(objects, &names);
}
void SourceExpressionDS_ValueMember::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	names->push_back(_name);
	_expr.makeObjectsSetMember(objects, names);
}

void SourceExpressionDS_ValueMember::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_ValueMember(";
	SourceExpressionDS_Base::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";

		*out << ", ";

		*out << "name=(";
		print_debug(out, _name);
		*out << ")";
	*out << ")";
}



