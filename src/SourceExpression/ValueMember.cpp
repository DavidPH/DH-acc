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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../SourceTokenC.hpp"
#include "../VariableType.hpp"



class SourceExpression_ValueMember : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_ValueMember, SourceExpression);

public:
	SourceExpression_ValueMember(SourceExpression * expr, SourceTokenC const & token);

	virtual bool canMakeObjectsAddress() const;

	virtual VariableType const * getType() const;

	virtual void makeObjectsAddress(ObjectVector * objects);

	virtual void makeObjectsGet(ObjectVector * objects);
	virtual void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names);

	virtual void makeObjectsSet(ObjectVector * objects);
	virtual void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names);

protected:
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

bool SourceExpression_ValueMember::canMakeObjectsAddress() const
{
	return _expr->canMakeObjectsAddress();
}

VariableType const * SourceExpression_ValueMember::getType() const
{
	return _expr->getType()->getType(_name, position);
}

void SourceExpression_ValueMember::makeObjectsAddress(ObjectVector * objects)
{
	Super::recurse_makeObjectsAddress(objects);

	_expr->makeObjectsAddress(objects);
	objects->setPosition(position);
	objects->addToken(OCODE_PUSHNUMBER, objects->getValue(_expr->getType()->getOffset(_name, position)));
	objects->addToken(OCODE_ADD);
}

void SourceExpression_ValueMember::makeObjectsGet(ObjectVector * objects)
{
	if (canMakeObjectsAddress())
	{
		makeObjectsAddress(objects);
		objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

		for (int i(0); i < getType()->size(); ++i)
			objects->addToken(OCODE_PUSHPOINTER, objects->getValue(i));
	}
	else
	{
		Super::recurse_makeObjectsGet(objects);

		std::vector<std::string> names(1, _name);
		_expr->makeObjectsGetMember(objects, &names);
	}
}
void SourceExpression_ValueMember::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	Super::recurse_makeObjectsGetMember(objects, names);

	names->push_back(_name);
	_expr->makeObjectsGetMember(objects, names);
}

void SourceExpression_ValueMember::makeObjectsSet(ObjectVector * objects)
{
	if (canMakeObjectsAddress())
	{
		makeObjectsAddress(objects);
		objects->addToken(OCODE_ASSIGNWORLDVAR, objects->getValue(1));

		for (int i(getType()->size()); i--;)
			objects->addToken(OCODE_ASSIGNPOINTER, objects->getValue(i));

		for (int i(0); i < getType()->size(); ++i)
			objects->addToken(OCODE_PUSHPOINTER, objects->getValue(i));
	}
	else
	{
		Super::recurse_makeObjectsSet(objects);

		std::vector<std::string> names(1, _name);
		_expr->makeObjectsSetMember(objects, &names);
	}
}
void SourceExpression_ValueMember::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names)
{
	Super::recurse_makeObjectsSetMember(objects, names);

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



