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

/* SourceExpression.cpp
**
** Defines the SourceExpression methods.
*/

#include "SourceExpression.hpp"

#include "ObjectVector.hpp"
#include "print_debug.hpp"
#include "SourceException.hpp"



SourceExpression::SourceExpression(SourcePosition const & position) : position(position)
{

}

void SourceExpression::addLabel(std::string const & label)
{
	labels.push_back(label);
}

SourceVariable::VariableType const * SourceExpression::get_promoted_type(SourceVariable::VariableType const * type1, SourceVariable::VariableType const * type2, SourcePosition const & position)
{
	if (type1 == type2) return type1;

	if (type1->type == SourceVariable::VT_VOID) return type1;
	if (type2->type == SourceVariable::VT_VOID) return type2;

	if (type1->type == SourceVariable::VT_REAL) return type1;
	if (type2->type == SourceVariable::VT_REAL) return type2;

	if (type1->type == SourceVariable::VT_INT) return type1;
	if (type2->type == SourceVariable::VT_INT) return type2;

	if (type1->type == SourceVariable::VT_CHAR) return type1;
	if (type2->type == SourceVariable::VT_CHAR) return type2;

	if (type1->type == SourceVariable::VT_STRING) return type1;
	if (type2->type == SourceVariable::VT_STRING) return type2;

	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

SourcePosition const & SourceExpression::getPosition() const
{
	return position;
}

SourceVariable::VariableType const * SourceExpression::getType() const
{
	return SourceVariable::get_VariableType(SourceVariable::VT_VOID);
}

void SourceExpression::make_objects(std::vector<SourceExpression::Pointer> const & expressions, ObjectVector * objects)
{
	for (uintptr_t index(0); index < expressions.size(); ++index)
		expressions[index]->makeObjectsGet(objects);
}

void SourceExpression::make_objects_call_acsfunc(ObjectVector * objects, SourceVariable::VariableData_ACSFunc const & data, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call acsfunc", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call acsfunc", args[i]->position, "SourceExpressionDS");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectToken::ObjectCode ocode;
	ObjectExpression::Pointer ofunc(objects->getValue(data.number));
	ObjectExpression::Pointer oretn;

	if (data.type->callType->type == SourceVariable::VT_VOID)
		ocode = ObjectToken::OCODE_CALLZDACSDISCARD;
	else
		ocode = ObjectToken::OCODE_CALLZDACS;

	if (data.type->callType->size() > 1)
		oretn = objects->getValue(data.type->callType->size() - 1);

	objects->addToken(ObjectToken::OCODE_ADDSTACK_IMM, stack);
	if (oretn) objects->addToken(ObjectToken::OCODE_ADDSTACK_IMM, oretn);
	objects->addToken(ocode, ofunc);
	if (oretn)
	{
		for (int i(-data.type->callType->size()); ++i;)
			objects->addToken(ObjectToken::OCODE_PUSHSTACKVAR, objects->getValue(i));

		objects->addToken(ObjectToken::OCODE_SUBSTACK_IMM, oretn);
	}
	objects->addToken(ObjectToken::OCODE_SUBSTACK_IMM, stack);
}

void SourceExpression::make_objects_call_asmfunc(ObjectVector * objects, SourceVariable::VariableData_AsmFunc const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call asmfunc", position, "SourceExpressionDS");

	bool immediate(data.ocode_imm != ObjectToken::OCODE_NONE);

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call asmfunc", args[i]->position, "SourceExpressionDS");

		immediate = immediate && args[i]->isConstant();
	}

	if (immediate)
	{
		std::vector<ObjectExpression::Pointer> oargs;

		for (size_t i(0); i < args.size(); ++i)
			oargs.push_back(args[i]->makeObject());

		objects->setPosition(position).addToken(data.ocode_imm, oargs);
	}
	else
	{
		if (data.ocode == ObjectToken::OCODE_NONE)
			throw SourceException("no ocode", position, "SourceExpressionDS");

		for (size_t i(0); i < args.size(); ++i)
			args[i]->makeObjectsGet(objects);

		objects->setPosition(position).addToken(data.ocode);
	}
}

void SourceExpression::make_objects_call_lnspec(ObjectVector * objects, SourceVariable::VariableData_LnSpec const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call lnspec", position, "SourceExpressionDS");

	if (args.size() > 5)
		throw SourceException("too many args to call lnspec", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call lnpsec", args[i]->position, "SourceExpressionDS");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectToken::ObjectCode ocode;
	ObjectExpression::Pointer ospec(objects->getValue(data.number));

	if (data.type->callType->type == SourceVariable::VT_VOID)
	{
		switch (args.size())
		{
		case 0: ocode = ObjectToken::OCODE_LSPEC1; objects->addTokenPushZero(); break;
		case 1: ocode = ObjectToken::OCODE_LSPEC1; break;
		case 2: ocode = ObjectToken::OCODE_LSPEC2; break;
		case 3: ocode = ObjectToken::OCODE_LSPEC3; break;
		case 4: ocode = ObjectToken::OCODE_LSPEC4; break;
		case 5: ocode = ObjectToken::OCODE_LSPEC5; break;
		default: throw SourceException("unexpected arg count to call lnspec", position, "SourceExpressionDS");
		}
	}
	else
	{
		ocode = ObjectToken::OCODE_LSPEC5RESULT;

		for (size_t i(args.size()); i < 5; ++i)
			objects->addTokenPushZero();
	}

	objects->addToken(ocode, ospec);
}

void SourceExpression::make_objects_call_native(ObjectVector * objects, SourceVariable::VariableData_Native const & data, std::vector<SourceExpression::Pointer> const & args, SourcePosition const & position)
{
	if (args.size() != data.type->types.size())
		throw SourceException("incorrect arg count to call native", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != data.type->types[i])
			throw SourceException("incorrect arg type to call native", args[i]->position, "SourceExpressionDS");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectToken::ObjectCode ocode(ObjectToken::OCODE_CALLZDFUNC);
	ObjectExpression::Pointer oargc(objects->getValue((int)args.size()));
	ObjectExpression::Pointer ofunc(objects->getValue(data.number));

	objects->addToken(ocode, oargc, ofunc);
}

void SourceExpression::make_objects_call_script(ObjectVector * objects, SourceVariable::VariableType const * type, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack, SourcePosition const & position)
{
	if (args.size() != type->types.size())
		throw SourceException("incorrect arg count to call script", position, "SourceExpressionDS");

	if (args.size() > 3)
		throw SourceException("too many args to call script", position, "SourceExpressionDS");

	for (size_t i(0); i < args.size(); ++i)
	{
		if (args[i]->getType() != type->types[i])
			throw SourceException("incorrect arg type to call script", args[i]->position, "SourceExpressionDS");

		args[i]->makeObjectsGet(objects);
	}

	objects->setPosition(position);

	ObjectToken::ObjectCode ocode;
	ObjectExpression::Pointer ospec(objects->getValue(84));
	ObjectExpression::Pointer oretn;

	if (type->callType->type == SourceVariable::VT_VOID)
	{
		switch (args.size())
		{
		case 0: ocode = ObjectToken::OCODE_LSPEC1; break;
		case 1: ocode = ObjectToken::OCODE_LSPEC2; break;
		case 2: ocode = ObjectToken::OCODE_LSPEC3; break;
		case 3: ocode = ObjectToken::OCODE_LSPEC4; break;
		default:
			throw SourceException("unexpected arg count to call script", position, "SourceExpressionDS");
		}
	}
	else
	{
		ocode = ObjectToken::OCODE_LSPEC5RESULT;

		for (size_t i(args.size()); i < 4; ++i)
			objects->addTokenPushZero();
	}

	if (type->callType->size() > 1)
		oretn = objects->getValue(type->callType->size() - 1);

	objects->addToken(ObjectToken::OCODE_ADDSTACK_IMM, stack);
	if (oretn) objects->addToken(ObjectToken::OCODE_ADDSTACK_IMM, oretn);
	objects->addToken(ocode, ospec);
	if (oretn)
	{
		for (int i(-type->callType->size()); ++i;)
			objects->addToken(ObjectToken::OCODE_PUSHSTACKVAR, objects->getValue(i));

		objects->addToken(ObjectToken::OCODE_SUBSTACK_IMM, oretn);
	}
	objects->addToken(ObjectToken::OCODE_SUBSTACK_IMM, stack);
}

void SourceExpression::make_objects_cast(ObjectVector * objects, SourceVariable::VariableType const * typeFrom, SourceVariable::VariableType const * typeTo, SourcePosition const & position)
{
	if (typeFrom == typeTo) return;

	if (typeTo->type == SourceVariable::VT_VOID)
	{
		for (size_t i(typeFrom->size()); i--;)
			objects->addToken(ObjectToken::OCODE_DROP);

		return;
	}

	switch (typeFrom->type)
	{
	case SourceVariable::VT_ACSFUNC:
	case SourceVariable::VT_CHAR:
	case SourceVariable::VT_INT:
	case SourceVariable::VT_LNSPEC:
	case SourceVariable::VT_NATIVE:
	case SourceVariable::VT_SCRIPT:
	case SourceVariable::VT_STRING:
		switch (typeTo->type)
		{
		case SourceVariable::VT_ACSFUNC:
		case SourceVariable::VT_CHAR:
		case SourceVariable::VT_INT:
		case SourceVariable::VT_LNSPEC:
		case SourceVariable::VT_NATIVE:
		case SourceVariable::VT_SCRIPT:
		case SourceVariable::VT_STRING:
			break;

		case SourceVariable::VT_ARRAY:
		case SourceVariable::VT_ASMFUNC:
		case SourceVariable::VT_BLOCK:
		case SourceVariable::VT_STRUCT:
		case SourceVariable::VT_VOID:
			throw SourceException("invalid VT to", position, "SourceExpression");

		case SourceVariable::VT_REAL:
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(16));
			objects->addToken(ObjectToken::OCODE_SHIFTL);
			break;
		}
		break;

	case SourceVariable::VT_ARRAY:
	case SourceVariable::VT_ASMFUNC:
	case SourceVariable::VT_BLOCK:
	case SourceVariable::VT_STRUCT:
	case SourceVariable::VT_VOID:
		throw SourceException("invalid VT from", position, "SourceExpression");

	case SourceVariable::VT_REAL:
		switch (typeTo->type)
		{
		case SourceVariable::VT_ACSFUNC:
		case SourceVariable::VT_CHAR:
		case SourceVariable::VT_INT:
		case SourceVariable::VT_LNSPEC:
		case SourceVariable::VT_NATIVE:
		case SourceVariable::VT_SCRIPT:
		case SourceVariable::VT_STRING:
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(16));
			objects->addToken(ObjectToken::OCODE_SHIFTR);
			break;

		case SourceVariable::VT_ARRAY:
		case SourceVariable::VT_ASMFUNC:
		case SourceVariable::VT_BLOCK:
		case SourceVariable::VT_STRUCT:
		case SourceVariable::VT_VOID:
			throw SourceException("invalid VT to", position, "SourceExpression");

		case SourceVariable::VT_REAL:
			break;
		}
		break;
	}
}

ObjectExpression::Pointer SourceExpression::makeObject() const
{
	throw SourceException("makeObject on invalid expression", position, getName());
}

void SourceExpression::makeObjectsCall(ObjectVector * objects, std::vector<SourceExpression::Pointer> const & args, ObjectExpression * stack) const
{
	throw SourceException("makeObjectsCall on invalid expression", position, getName());
}

void SourceExpression::makeObjectsCast(ObjectVector * objects, SourceVariable::VariableType const * type) const
{
	makeObjectsGet(objects);
	make_objects_cast(objects, getType(), type, position);
}

void SourceExpression::makeObjectsGet(ObjectVector * objects) const
{
	throw SourceException("makeObjectsGet on invalid expression", position, getName());
}
void SourceExpression::makeObjectsGetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	if (dimensions->empty())
	{
		makeObjectsGet(objects);
		return;
	}

	throw SourceException("makeObjectsGetArray on invalid expression", position, getName());
}
void SourceExpression::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	if (names->empty())
	{
		makeObjectsGet(objects);
		return;
	}

	throw SourceException("makeObjectsGetMember on invalid expression", position, getName());
}

void SourceExpression::makeObjectsSet(ObjectVector * objects) const
{
	throw SourceException("makeObjectsSet on invalid expression", position, getName());
}
void SourceExpression::makeObjectsSetArray(ObjectVector * objects, std::vector<SourceExpression::Pointer> * dimensions) const
{
	if (dimensions->empty())
	{
		makeObjectsSet(objects);
		return;
	}

	throw SourceException("makeObjectsSetArray on invalid expression", position, getName());
}
void SourceExpression::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names) const
{
	if (names->empty())
	{
		makeObjectsSet(objects);
		return;
	}

	throw SourceException("makeObjectsSetMember on invalid expression", position, getName());
}

bool SourceExpression::isConstant() const
{
	return false;
}

void SourceExpression::printDebug(std::ostream * out) const
{
	*out << "SourceExpressionDS(";
		*out << "labels=(";
		print_debug(out, labels);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, position);
		*out << ")";
	*out << ")";
}



void print_debug(std::ostream * out, SourceExpression const & in)
{
	in.printDebug(out);
}



