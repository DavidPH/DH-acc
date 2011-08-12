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

/* ObjectExpression/objects_io.cpp
**
** Defines the ObjectExpression::read_objects and write_objects functions.
*/

#include "../ObjectExpression.hpp"
#include "../ObjectVector.hpp"
#include "../object_io.hpp"

#include <iostream>



void ObjectExpression::write_objects(std::ostream * out, ObjectVector const & objects)
{
	*out << "object";

	write_object(out, objects);

	write_object(out, _function_table);

	write_object(out, _register_global_table);
	write_object(out, _register_map_table);
	write_object(out, _register_world_table);

	write_object(out, _registerarray_global_table);
	write_object(out, _registerarray_map_table);
	write_object(out, _registerarray_world_table);

	write_object(out, _script_table);

	write_object(out, _static_table);

	write_object(out, _string_table);

	write_object(out, _symbol_table);
	write_object(out, _symbol_type_table);
}

void ObjectExpression::read_objects(std::istream * in, ObjectVector * objects)
{
	if (in->get() != 'o' || in->get() != 'b' || in->get() != 'j' || in->get() != 'e' || in->get() != 'c' || in->get() != 't')
		throw "Not object file.";

	read_object(in, objects);

	read_object(in, &_function_table);

	read_object(in, &_register_global_table);
	read_object(in, &_register_map_table);
	read_object(in, &_register_world_table);

	read_object(in, &_registerarray_global_table);
	read_object(in, &_registerarray_map_table);
	read_object(in, &_registerarray_world_table);

	read_object(in, &_script_table);

	read_object(in, &_static_table);

	read_object(in, &_string_table);

	read_object(in, &_symbol_table);
	read_object(in, &_symbol_type_table);
}



bool override_object(ObjectExpression::Function * out, ObjectExpression::Function const & in)
{
	if (out->label != in.label)
		return false;

	return true;
}
bool override_object(ObjectExpression::Register * out, ObjectExpression::Register const & in)
{
	if (out->name != in.name)
		return false;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectExpression::RegisterArray * out, ObjectExpression::RegisterArray const & in)
{
	if (out->name != in.name)
		return false;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectExpression::Script * out, ObjectExpression::Script const & in)
{
	if (out->name != in.name)
		return false;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectExpression::Static * out, ObjectExpression::Static const & in)
{
	if (out->name != in.name)
		return false;

	if (out->number == -1)
		*out = in;

	return true;
}
bool override_object(ObjectExpression::String * out, ObjectExpression::String const & in)
{
	return false;
}

void read_object(std::istream * in, ObjectExpression::ExpressionType * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));

	if (*out > ObjectExpression::ET_INT)
		*out = ObjectExpression::ET_INT;
}
void read_object(std::istream * in, ObjectExpression::Function * out)
{
	read_object(in, &out->argCount);
	read_object(in, &out->label);
	read_object(in, &out->retCount);
	read_object(in, &out->varCount);
}
void read_object(std::istream * in, ObjectExpression::ObjectType * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));

	if (*out > ObjectExpression::OT_NONE)
		*out = ObjectExpression::OT_NONE;
}
void read_object(std::istream * in, ObjectExpression::Pointer * out)
{
	ObjectExpression::ObjectType type;
	read_object(in, &type);

	switch (type)
	{
	case ObjectExpression::OT_BINARY_ADD:
		*out = ObjectExpression::create_binary_add(in);
		break;

	case ObjectExpression::OT_BINARY_AND:
		*out = ObjectExpression::create_binary_and(in);
		break;

	case ObjectExpression::OT_BINARY_DIV:
		*out = ObjectExpression::create_binary_div(in);
		break;

	case ObjectExpression::OT_BINARY_IOR:
		*out = ObjectExpression::create_binary_ior(in);
		break;

	case ObjectExpression::OT_BINARY_MOD:
		*out = ObjectExpression::create_binary_mod(in);
		break;

	case ObjectExpression::OT_BINARY_MUL:
		*out = ObjectExpression::create_binary_mul(in);
		break;

	case ObjectExpression::OT_BINARY_SUB:
		*out = ObjectExpression::create_binary_sub(in);
		break;

	case ObjectExpression::OT_BINARY_XOR:
		*out = ObjectExpression::create_binary_xor(in);
		break;

	case ObjectExpression::OT_UNARY_ADD:
		*out = ObjectExpression::create_unary_add(in);
		break;

	case ObjectExpression::OT_UNARY_SUB:
		*out = ObjectExpression::create_unary_sub(in);
		break;

	case ObjectExpression::OT_VALUE_FLOAT:
		*out = ObjectExpression::create_value_float(in);
		break;

	case ObjectExpression::OT_VALUE_INT:
		*out = ObjectExpression::create_value_int(in);
		break;

	case ObjectExpression::OT_VALUE_SYMBOL:
		*out = ObjectExpression::create_value_symbol(in);
		break;

	case ObjectExpression::OT_NONE:
		*out = NULL;
		break;
	}
}
void read_object(std::istream * in, ObjectExpression::Register * out)
{
	read_object(in, &out->name);
	read_object(in, &out->number);
	read_object(in, &out->size);
}
void read_object(std::istream * in, ObjectExpression::RegisterArray * out)
{
	read_object(in, &out->name);
	read_object(in, &out->number);
	read_object(in, &out->size);
}
void read_object(std::istream * in, ObjectExpression::Script * out)
{
	read_object(in, &out->label);
	read_object(in, &out->name);
	read_object(in, &out->stype);
	read_object(in, &out->argCount);
	read_object(in, &out->flags);
	read_object(in, &out->number);
	read_object(in, &out->varCount);
}
void read_object(std::istream * in, ObjectExpression::ScriptType * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));

	if (*out > ObjectExpression::ST_NONE)
		*out = ObjectExpression::ST_NONE;
}
void read_object(std::istream * in, ObjectExpression::Static * out)
{
	read_object(in, &out->name);
	read_object(in, &out->number);
	read_object(in, &out->size);
}
void read_object(std::istream * in, ObjectExpression::String * out)
{
	read_object(in, &out->name);
	read_object(in, &out->string);
	read_object(in, &out->offset);
}

void write_object(std::ostream * out, ObjectExpression::ExpressionType const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, ObjectExpression::Function const & in)
{
	write_object(out, in.argCount);
	write_object(out, in.label);
	write_object(out, in.retCount);
	write_object(out, in.varCount);
}
void write_object(std::ostream * out, ObjectExpression::ObjectType const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, ObjectExpression::Pointer const & in)
{
	in->writeObject(out);
}
void write_object(std::ostream * out, ObjectExpression::Register const & in)
{
	write_object(out, in.name);
	write_object(out, in.number);
	write_object(out, in.size);
}
void write_object(std::ostream * out, ObjectExpression::RegisterArray const & in)
{
	write_object(out, in.name);
	write_object(out, in.number);
	write_object(out, in.size);
}
void write_object(std::ostream * out, ObjectExpression::Script const & in)
{
	write_object(out, in.label);
	write_object(out, in.name);
	write_object(out, in.stype);
	write_object(out, in.argCount);
	write_object(out, in.flags);
	write_object(out, in.number);
	write_object(out, in.varCount);
}
void write_object(std::ostream * out, ObjectExpression::ScriptType const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, ObjectExpression::Static const & in)
{
	write_object(out, in.name);
	write_object(out, in.number);
	write_object(out, in.size);
}
void write_object(std::ostream * out, ObjectExpression::String const & in)
{
	write_object(out, in.name);
	write_object(out, in.string);
	write_object(out, in.offset);
}



