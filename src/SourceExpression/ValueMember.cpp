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

/* SourceExpression/ValueMember.cpp
**
** Defines the SourceExpression_ValueMember class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceTokenC.hpp"



class SourceExpression_ValueMember : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueMember, SourceExpression);

public:
	SourceExpression_ValueMember(SourceExpression * expr, SourceTokenC const & token);

	virtual SourceVariable::VariableType const * getType() const;

	virtual void makeObjectsGet(ObjectVector * objects) const;
	virtual void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void makeObjectsSet(ObjectVector * objects) const;
	virtual void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const;

	virtual void printDebug(std::ostream * out) const;

private:
	SourceExpression::Pointer _expr;
	std::string _name;
};



SourceExpression::Pointer SourceExpression::create_value_member(SourceExpression * expr, SourceTokenC const & token)
{
	return new SourceExpression_ValueMember(expr, token);
}



SourceExpression_ValueMember::SourceExpression_ValueMember(SourceExpression * expr, SourceTokenC const & token) : Super(token.getPosition()), _expr(expr), _name(token.getData())
{

}

SourceVariable::VariableType const * SourceExpression_ValueMember::getType() const
{
	return _expr->getType()->getType(_name, getPosition());
}

void SourceExpression_ValueMember::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	std::vector<std::string> names(1, _name);
	_expr->makeObjectsGetMember(objects, &names);
}
void SourceExpression_ValueMember::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	objects->addLabel(labels);

	names->push_back(_name);
	_expr->makeObjectsGetMember(objects, names);
}

void SourceExpression_ValueMember::makeObjectsSet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	std::vector<std::string> names(1, _name);
	_expr->makeObjectsSetMember(objects, &names);
}
void SourceExpression_ValueMember::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	objects->addLabel(labels);

	names->push_back(_name);
	_expr->makeObjectsSetMember(objects, names);
}

void SourceExpression_ValueMember::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_ValueMember(";
	Super::printDebug(out);
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



