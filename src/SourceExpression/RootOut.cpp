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

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"



class SourceExpression_RootOut : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_RootOut, SourceExpression);

public:
	SourceExpression_RootOut(SourceExpression * expr, SourcePosition const & position);

	virtual void makeObjectsGet(ObjectVector * objects);

protected:
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
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('{'));
		objects->addToken(OCODE_PRINTCHARACTER);

		for (size_t i(type->types.size()); i--;)
		{
			doOut(objects, type->types[i]);

			if (i)
			{
				objects->addToken(OCODE_PUSHNUMBER, objects->getValue(' '));
				objects->addToken(OCODE_PRINTCHARACTER);
			}
		}

		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('}'));
		objects->addToken(OCODE_PRINTCHARACTER);

		break;

	case SourceVariable::VT_ASMFUNC:
	case SourceVariable::VT_VOID:
		break;

	case SourceVariable::VT_BOOLHARD:
	case SourceVariable::VT_FUNCTION:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_LINESPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_SCRIPT:
		objects->addToken(OCODE_PRINTNUMBER);
		break;

	case SourceVariable::VT_BOOLSOFT:
		objects->addToken(OCODE_LOGICALNOT);
		objects->addToken(OCODE_LOGICALNOT);
		objects->addToken(OCODE_PRINTNUMBER);
		break;

	case SourceVariable::VT_CHAR:
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case SourceVariable::VT_POINTER:
		objects->addToken(OCODE_PRINTHEX);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('p'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case SourceVariable::VT_REAL:
		objects->addToken(OCODE_PRINTFIXED);
		break;

	case SourceVariable::VT_STRING:
		objects->addToken(OCODE_PRINTSTRING);
		break;

	case SourceVariable::VT_UNION:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('U'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('{'));
		objects->addToken(OCODE_PRINTCHARACTER);

		for (int i(type->size()); i--;)
		{
			objects->addToken(OCODE_PRINTHEX);

			if (i)
			{
				objects->addToken(OCODE_PUSHNUMBER, objects->getValue(' '));
				objects->addToken(OCODE_PRINTCHARACTER);
			}
		}

		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('}'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('U'));
		objects->addToken(OCODE_PRINTCHARACTER);
	}

	objects->addToken(OCODE_PUSHNUMBER, objects->getValue(';'));
	objects->addToken(OCODE_PRINTCHARACTER);
}

void SourceExpression_RootOut::makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_expr->makeObjectsGet(objects);

	objects->setPosition(position);

	objects->addToken(OCODE_BEGINPRINT);
	doOut(objects, _expr->getType());
	objects->addToken(OCODE_ENDLOG);
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



