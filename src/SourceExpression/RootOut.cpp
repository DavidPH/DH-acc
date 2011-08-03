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

/* SourceExpression/RootOut.cpp
**
** Defines the SourceExpression_RootOut class and methods.
*/

#include "../SourceExpression.hpp"

#include "../ObjectVector.hpp"



class SourceExpression_RootOut : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_RootOut, SourceExpression);

public:
	SourceExpression_RootOut(SourceExpression * expr, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects) const;

	virtual void printDebug(std::ostream * out) const;

private:
	SourceExpression::Pointer _expr;

	void doOut(ObjectVector * objects, SourceVariable::VariableType const * type) const;
};



SourceExpression::Pointer SourceExpression::create_root_out(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_RootOut(expr, position);
}



SourceExpression_RootOut::SourceExpression_RootOut(SourceExpression * expr, SourcePosition const & position) : Super(position), _expr(expr)
{

}

void SourceExpression_RootOut::doOut(ObjectVector * objects, SourceVariable::VariableType const * type) const
{
	switch (type->type)
	{
	case SourceVariable::VT_ARRAY:
	case SourceVariable::VT_BLOCK:
	case SourceVariable::VT_STRUCT:
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue('{'));
		objects->addToken(ObjectToken::OCODE_PRINTCHARACTER);

		for (size_t i(type->types.size()); i--;)
		{
			doOut(objects, type->types[i]);

			if (i)
			{
				objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(' '));
				objects->addToken(ObjectToken::OCODE_PRINTCHARACTER);
			}
		}

		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue('}'));
		objects->addToken(ObjectToken::OCODE_PRINTCHARACTER);

		break;

	case SourceVariable::VT_ASMFUNC:
	case SourceVariable::VT_VOID:
		break;

	case SourceVariable::VT_CHAR:
		objects->addToken(ObjectToken::OCODE_PRINTCHARACTER);
		break;

	case SourceVariable::VT_FUNCTION:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_LINESPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_SCRIPT:
		objects->addToken(ObjectToken::OCODE_PRINTNUMBER);
		break;

	case SourceVariable::VT_POINTER:
		objects->addToken(ObjectToken::OCODE_PRINTHEX);
		break;

	case SourceVariable::VT_REAL:
		objects->addToken(ObjectToken::OCODE_PRINTFIXED);
		break;

	case SourceVariable::VT_STRING:
		objects->addToken(ObjectToken::OCODE_PRINTSTRING);
		break;
	}

	objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(';'));
	objects->addToken(ObjectToken::OCODE_PRINTCHARACTER);
}

void SourceExpression_RootOut::makeObjectsGet(ObjectVector * objects) const
{
	objects->addLabel(labels);

	_expr->makeObjectsGet(objects);

	objects->setPosition(position);

	objects->addToken(ObjectToken::OCODE_BEGINPRINT);
	doOut(objects, _expr->getType());
	objects->addToken(ObjectToken::OCODE_ENDLOG);
}

void SourceExpression_RootOut::printDebug(std::ostream * out) const
{
	*out << "SourceExpression_RootOut(";
	Super::printDebug(out);
	*out << " ";
		*out << "expr=(";
		print_debug(out, _expr);
		*out << ")";
	*out << ")";
}



