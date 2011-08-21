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

/* SourceExpression/RootScript.cpp
**
** Defines the SourceExpression_RootScript class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../print_debug.hpp"
#include "../VariableType.hpp"



class SourceExpression_RootScript : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_RootScript, SourceExpression);

public:
	SourceExpression_RootScript(VariableType const * type, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	VariableType const * _type;
};



SourceExpression::Pointer SourceExpression::create_root_script(VariableType const * type, SourcePosition const & position)
{
	return new SourceExpression_RootScript(type, position);
}



SourceExpression_RootScript::SourceExpression_RootScript(VariableType const * type, SourcePosition const & position) : Super(position), _type(type)
{

}

void SourceExpression_RootScript::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	if (_type->sizeCall() > 3) for (int i(_type->sizeCall() - 3); i--;)
	{
		objects->addToken(OCODE_PUSHSTACKVAR, objects->getValue(i));
		objects->addToken(OCODE_ASSIGNSCRIPTVAR, objects->getValue(i+3));
	}
}

void SourceExpression_RootScript::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_RootScript(";
	Super::printDebug(out);
	*out << " ";
		*out << "type=(";
		print_debug(out, _type);
		*out << ")";
	*out << ")";
}


