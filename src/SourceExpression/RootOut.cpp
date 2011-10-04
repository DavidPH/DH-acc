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
#include "../VariableType.hpp"



class SourceExpression_RootOut : public SourceExpression
{
	MAKE_COUNTER_CLASS_BASE(SourceExpression_RootOut, SourceExpression);

public:
	SourceExpression_RootOut(SourceExpression * expr, SourcePosition const & position);

protected:
	virtual void printDebug(std::ostream * out) const;

private:
	void doOut(ObjectVector * objects, VariableType const * type) const;

	virtual void virtual_makeObjectsGet(ObjectVector * objects);

	SourceExpression::Pointer _expr;
};



SourceExpression::Pointer SourceExpression::create_root_out(SourceExpression * expr, SourcePosition const & position)
{
	return new SourceExpression_RootOut(expr, position);
}



SourceExpression_RootOut::SourceExpression_RootOut(SourceExpression * expr, SourcePosition const & position_) : Super(position_), _expr(expr)
{

}

void SourceExpression_RootOut::doOut(ObjectVector * objects, VariableType const * type) const
{
	switch (type->vt)
	{
	case VariableType::VT_ARRAY:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('A'));
		objects->addToken(OCODE_PRINTCHARACTER);
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
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('A'));
		objects->addToken(OCODE_PRINTCHARACTER);

		break;

	case VariableType::VT_ASMFUNC:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('P'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('A'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_BLOCK:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('B'));
		objects->addToken(OCODE_PRINTCHARACTER);
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
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('B'));
		objects->addToken(OCODE_PRINTCHARACTER);

		break;

	case VariableType::VT_BOOLHARD:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('B'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_BOOLSOFT:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('B'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('S'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_CHAR:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('\''));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('\''));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_ENUM:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('E'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_FUNCTION:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('P'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('F'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_INT:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('I'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_LINESPEC:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('P'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('L'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_NATIVE:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('P'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('N'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_POINTER:
		objects->addToken(OCODE_PRINTHEX);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('P'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_REAL:
		objects->addToken(OCODE_PRINTFIXED);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('F'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_SCRIPT:
		objects->addToken(OCODE_PRINTNUMBER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('P'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('S'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_STRING:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('"'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PRINTSTRING);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('"'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;

	case VariableType::VT_STRUCT:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('S'));
		objects->addToken(OCODE_PRINTCHARACTER);
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
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('S'));
		objects->addToken(OCODE_PRINTCHARACTER);

		break;

	case VariableType::VT_UNION:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('U'));
		objects->addToken(OCODE_PRINTCHARACTER);
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('{'));
		objects->addToken(OCODE_PRINTCHARACTER);

		for (int i(type->size(position)); i--;)
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

		break;

	case VariableType::VT_VOID:
		objects->addToken(OCODE_PUSHNUMBER, objects->getValue('V'));
		objects->addToken(OCODE_PRINTCHARACTER);
		break;
	}

	objects->addToken(OCODE_PUSHNUMBER, objects->getValue(';'));
	objects->addToken(OCODE_PRINTCHARACTER);
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

void SourceExpression_RootOut::virtual_makeObjectsGet(ObjectVector * objects)
{
	Super::recurse_makeObjectsGet(objects);

	_expr->makeObjectsGet(objects);

	objects->setPosition(position);

	objects->addToken(OCODE_BEGINPRINT);
	doOut(objects, _expr->getType());
	objects->addToken(OCODE_ENDLOG);
}


