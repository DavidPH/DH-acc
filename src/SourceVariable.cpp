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

/* SourceVariable.hpp
**
** Defines the SourceVariable methods.
*/

#include "SourceVariable.hpp"

#include "ObjectExpression.hpp"
#include "ObjectVector.hpp"
#include "print_debug.hpp"
#include "SourceException.hpp"
#include "SourceExpressionDS.hpp"
#include "SourceTokenC.hpp"



std::vector<std::string>                    SourceVariable::_names;
std::vector<SourceVariable::VariableType *> SourceVariable::_types;



static struct _SourceVariable_init_c
{
	_SourceVariable_init_c()
	{
		SourceVariable::init();
	}
} _SourceVarible_init;



SourceVariable::VariableType const * SourceVariable::VariableType::getType(std::string const & name, SourcePosition const & position) const
{
	for (size_t i(0); i < names.size(); ++i)
		if (name == names[i])
			return types[i];

	throw SourceException("invalid member-variable-type", position, "SourceVariable::VariableType");
}

int SourceVariable::VariableType::size() const
{
	switch (type)
	{
	case VT_ACSFUNC:
	case VT_CHAR:
	case VT_INT:
	case VT_LNSPEC:
	case VT_NATIVE:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		return 1;

	case VT_ARRAY:
	case VT_STRUCT:
	{
		int s(0);

		for (size_t i(0); i < types.size(); ++i)
			s += types[i]->size();

		return s;
	}

	case VT_ASMFUNC:
	case VT_VOID:
		return 0;
	}

	return 0;
}



SourceVariable::SourceVariable() : _address(-1), _nameObject(), _nameSource(), _position(), _sc(), _type()
{

}
SourceVariable::SourceVariable(std::string const & nameObject, std::string const & nameSource, int const address, StorageClass const sc, VariableType const * const type, SourcePosition const & position) : _address(address), _nameObject(nameObject), _nameSource(nameSource), _position(position), _sc(sc), _type(type)
{

}
SourceVariable::SourceVariable(std::string const & name, VariableData_ACSFunc const & vdACSFunc, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdACSFunc.type)
{
	_data.vdACSFunc = vdACSFunc;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_AsmFunc const & vdAsmFunc, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdAsmFunc.type)
{
	_data.vdAsmFunc = vdAsmFunc;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Char const & vdChar, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdChar.type)
{
	_data.vdChar = vdChar;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Int const & vdInt, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdInt.type)
{
	_data.vdInt = vdInt;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_LnSpec const & vdLnSpec, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdLnSpec.type)
{
	_data.vdLnSpec = vdLnSpec;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Native const & vdNative, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdNative.type)
{
	_data.vdNative = vdNative;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Real const & vdReal, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdReal.type)
{
	_data.vdReal = vdReal;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Script const & vdScript, SourcePosition const & position) : _address(-1), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdScript.type)
{
	_data.vdScript = vdScript;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_String const & vdString, SourcePosition const & position, std::string const & nameObject) : _address(-1), _nameObject(nameObject), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdString.type)
{
	_data.vdString = vdString;
}

void SourceVariable::add_struct(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types)
{
	VariableType * type(new VariableType);

	type->type     = VT_STRUCT;
	type->callType = _types[VT_VOID];
	type->refType  = _types[VT_VOID];
	type->names    = names;
	type->types    = types;

	_names.push_back(name);
	_types.push_back(type);
}

void SourceVariable::add_typedef(std::string const & name, VariableType const * type)
{
	_names.push_back(name);
	_types.push_back((VariableType*)type);
}

SourceVariable::StorageClass SourceVariable::get_StorageClass(SourceTokenC const & token)
{
	if (token.getData() == "register")
		return SC_REGISTER;

	if (token.getData() == "register_global")
		return SC_REGISTER_GLOBAL;

	if (token.getData() == "register_map")
		return SC_REGISTER_MAP;

	if (token.getData() == "register_world")
		return SC_REGISTER_WORLD;

	if (token.getData() == "registerarray_global")
		return SC_REGISTERARRAY_GLOBAL;

	if (token.getData() == "registerarray_map")
		return SC_REGISTERARRAY_MAP;

	if (token.getData() == "registerarray_world")
		return SC_REGISTERARRAY_WORLD;

	throw SourceException("invalid storage-class", token.getPosition(), "SourceVariable");
}

SourceVariable::VariableType const * SourceVariable::get_VariableType(SourceTokenC const & token)
{
	VariableType const * type(get_VariableType_null(token.getData()));

	if (!type) throw SourceException("invalid variable-type", token.getPosition(), "SourceVariable");

	return type;
}
SourceVariable::VariableType const * SourceVariable::get_VariableType(VariableTypeInternal const type)
{
	return _types[(type <= VT_VOID) ? type : VT_VOID];
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_acsfunc(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_ACSFUNC, callType, types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_array(VariableType const * refType, int count)
{
	for (size_t i(0); i < _types.size(); ++i)
	{
		if (_types[i]->type == VT_ARRAY && _types[i]->refType == refType && _types[i]->types.size() == (size_t)count)
		{
			return _types[i];
		}
	}

	VariableType * type(new VariableType);

	type->type     = VT_ARRAY;
	type->callType = _types[VT_VOID];
	type->refType  = refType;
	type->names    = std::vector<std::string>(count, "");
	type->types    = std::vector<VariableType const *>(count, refType);

	_names.push_back("");
	_types.push_back(type);

	return type;
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_asmfunc(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_ASMFUNC, callType, types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_auto(VariableTypeInternal itype, VariableType const * callType, std::vector<VariableType const *> const & types)
{
	for (size_t i(0); i < _types.size(); ++i)
	{
		if (_types[i]->type == itype && _types[i]->callType == callType && _types[i]->types.size() == types.size())
		{
			bool matched(true);

			for (size_t j(0); j < _types[i]->types.size(); ++j)
			{
				if (_types[i]->types[j] != types[j])
				{
					matched = false;
					break;
				}
			}

			if (matched) return _types[i];
		}
	}

	VariableType * type(new VariableType);

	type->type     = itype;
	type->callType = callType;
	type->refType  = _types[VT_VOID];
	type->names    = std::vector<std::string>(types.size(), "");
	type->types    = types;

	_names.push_back("");
	_types.push_back(type);

	return type;
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_lnspec(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_LNSPEC, callType, types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_native(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_NATIVE, callType, types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_null(std::string const & name)
{
	for (size_t i(0); i < _names.size(); ++i)
		if (name == _names[i])
			return _types[i];

	return NULL;
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_script(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_SCRIPT, callType, types);
}

SourceVariable::StorageClass SourceVariable::getClass() const
{
	return _sc;
}

std::string const & SourceVariable::getNameSource() const
{
	return _nameSource;
}

SourceVariable::VariableType const * SourceVariable::getType() const
{
	return _type;
}

void SourceVariable::init()
{
	_names.resize(VT_VOID+1);
	_types.resize(VT_VOID+1);
	VariableType type;

	type.type = VT_VOID;
	_names[VT_VOID] = "void";
	_types[VT_VOID] = new VariableType(type);
	_types[VT_VOID]->callType = type.callType = _types[VT_VOID];
	_types[VT_VOID]->refType  = type.refType  = _types[VT_VOID];

	type.type = VT_CHAR;
	_names[VT_CHAR] = "char";
	_types[VT_CHAR] = new VariableType(type);

	type.type = VT_INT;
	_names[VT_INT] = "int";
	_types[VT_INT] = new VariableType(type);

	type.type = VT_REAL;
	_names[VT_REAL] = "real";
	_types[VT_REAL] = new VariableType(type);

	type.type = VT_STRING;
	_names[VT_STRING] = "string";
	_types[VT_STRING] = new VariableType(type);
}

bool SourceVariable::isConstant() const
{
	return _sc == SC_CONSTANT;
}

ObjectExpression SourceVariable::makeObject(SourcePosition const & position) const
{
	switch (_sc)
	{
	case SC_CONSTANT:
		switch (_type->type)
		{
		case VT_ACSFUNC:
			return ObjectExpression::create_value_int(_data.vdACSFunc.number, position);

		case VT_ARRAY:
		case VT_STRUCT:
			throw SourceException("makeObject on compound VT", position, "SourceVariable");

		case VT_ASMFUNC:
		case VT_VOID:
			throw SourceException("makeObject on void VT", position, "SourceVariable");

		case VT_CHAR:
			return ObjectExpression::create_value_int(_data.vdChar.value, position);

		case VT_INT:
			return ObjectExpression::create_value_int(_data.vdInt.value, position);

		case VT_LNSPEC:
			return ObjectExpression::create_value_int(_data.vdLnSpec.number, position);

		case VT_NATIVE:
			return ObjectExpression::create_value_int(_data.vdNative.number, position);

		case VT_REAL:
			return ObjectExpression::create_value_float(_data.vdReal.value, position);

		case VT_STRING:
			return ObjectExpression::create_value_symbol(_nameObject, position);

		case VT_SCRIPT:
			return ObjectExpression::create_value_int(_data.vdScript.number, position);
		}
		break;

	case SC_REGISTER:
		throw SourceException("makeObject on SC_REGISTER", position, "SourceVariable");

	case SC_REGISTER_GLOBAL:
		throw SourceException("makeObject on SC_REGISTER_GLOBAL", position, "SourceVariable");

	case SC_REGISTER_MAP:
		throw SourceException("makeObject on SC_REGISTER_MAP", position, "SourceVariable");

	case SC_REGISTER_WORLD:
		throw SourceException("makeObject on SC_REGISTER_WORLD", position, "SourceVariable");

	case SC_REGISTERARRAY_GLOBAL:
		throw SourceException("makeObject on SC_REGISTERARRAY_GLOBAL", position, "SourceVariable");

	case SC_REGISTERARRAY_MAP:
		throw SourceException("makeObject on SC_REGISTERARRAY_MAP", position, "SourceVariable");

	case SC_REGISTERARRAY_WORLD:
		throw SourceException("makeObject on SC_REGISTERARRAY_WORLD", position, "SourceVariable");
	}

	throw SourceException("makeObject", _position, "SourceVariable");
}

void SourceVariable::makeObjectsCall(ObjectVector * objects, std::vector<SourceExpressionDS> const & args, SourcePosition const & position) const
{
	objects->setPosition(position);

	switch (_type->type)
	{
	case VT_ACSFUNC:
		if (_sc == SC_CONSTANT)
			SourceExpressionDS::make_objects_call_acsfunc(objects, _data.vdACSFunc, args, position);
		else
			throw SourceException("non-constant acsfuncs not yet supported", position, "SourceVariable");
		break;

	case VT_ARRAY:
	case VT_CHAR:
	case VT_INT:
	case VT_REAL:
	case VT_STRING:
	case VT_STRUCT:
	case VT_VOID:
		throw SourceException("attempt to call uncallable", position, "SourceVariable");

	case VT_ASMFUNC:
		if (_sc == SC_CONSTANT)
			SourceExpressionDS::make_objects_call_asmfunc(objects, _data.vdAsmFunc, args, position);
		else
			throw SourceException("non-constant asmfuncs not supported", position, "SourceVariable");
		break;

	case VT_LNSPEC:
		if (_sc == SC_CONSTANT)
			SourceExpressionDS::make_objects_call_lnspec(objects, _data.vdLnSpec, args, position);
		else
			throw SourceException("non-constant lnspecs not yet supported", position, "SourceVariable");
		break;

	case VT_NATIVE:
		if (_sc == SC_CONSTANT)
			SourceExpressionDS::make_objects_call_native(objects, _data.vdNative, args, position);
		else
			throw SourceException("non-constant natives not yet supported", position, "SourceVariable");
		break;

	case VT_SCRIPT:
		makeObjectsGet(objects, position);
		SourceExpressionDS::make_objects_call_script(objects, _type, args, position);
		break;
	}
}

void SourceVariable::makeObjectsGet(ObjectVector * objects, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsGetPrep(objects, &address, 0);
	makeObjectsGet(objects, position, _type, &address);
}
void SourceVariable::makeObjectsGet(ObjectVector * objects, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	ObjectToken::ObjectCode ocode;

	switch (_sc)
	{
	case SC_CONSTANT:
		ocode = ObjectToken::OCODE_PUSHNUMBER;

		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ARRAY:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_STRUCT:
		case VT_STRING:
		case VT_SCRIPT:
			objects->addToken(ocode, makeObject(position));
			++*address;
			break;

		case VT_ASMFUNC:
		case VT_VOID:
			break;
		}
		break;

	case SC_REGISTER:
		ocode = ObjectToken::OCODE_PUSHSCRIPTVAR;
	sc_register_case:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
			objects->addToken(ocode, objects->getValue((*address)++));
			break;

		case VT_ARRAY:
			throw SourceException("makeObjectsGet on register VT_ARRAY", position, "SourceVariable");

		case VT_ASMFUNC:
		case VT_VOID:
			break;

		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
				makeObjectsGet(objects, position, type->types[i], address);
			break;
		}
		break;

	case SC_REGISTER_GLOBAL:
		ocode = ObjectToken::OCODE_PUSHGLOBALVAR;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = ObjectToken::OCODE_PUSHMAPVAR;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = ObjectToken::OCODE_PUSHWORLDVAR;
		goto sc_register_case;

	sc_registerarray_case:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue((*address)++));
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
			objects->addToken(ocode, objects->getValue(_address));
			objects->addToken(ObjectToken::OCODE_ADD);
			objects->addToken(ocode, objects->getValue(_address));
			break;

		case VT_ARRAY:
		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
				makeObjectsGet(objects, position, type->types[i], address);
			break;

		case VT_ASMFUNC:
		case VT_VOID:
			break;
		}
		break;

	case SC_REGISTERARRAY_GLOBAL:
		ocode = ObjectToken::OCODE_PUSHGLOBALARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_MAP:
		ocode = ObjectToken::OCODE_PUSHMAPARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_WORLD:
		ocode = ObjectToken::OCODE_PUSHWORLDARRAY;
		goto sc_registerarray_case;
	}
}

void SourceVariable::makeObjectsGetArray(ObjectVector * objects, int dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsGetPrep(objects, &address, dimensions);
	makeObjectsGetArray(objects, dimensions, position, _type, &address);
}
void SourceVariable::makeObjectsGetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	if (dimensions == 0)
	{
		makeObjectsGet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectsGetArray on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		throw SourceException("makeObjectsGetArray on register", position, "SourceVariable");

	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ASMFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_STRUCT:
		case VT_VOID:
			throw SourceException("makeObjectsGetArray on non-VT_ARRAY", position, "SourceVariable");

		case VT_ARRAY:
			makeObjectsGetArray(objects, dimensions-1, position, type->refType, address);
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsGetPrep(objects, &address, 0);
	makeObjectsGetMember(objects, names, position, _type, &address);
}
void SourceVariable::makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	if (names->empty())
	{
		makeObjectsGet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectGetMember on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ARRAY:
		case VT_ASMFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_VOID:
			throw SourceException("makeObjectGetMember on non-VT_STRUCT", position, "SourceVariable");

		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
			{
				if (type->names[i] == names->back())
				{
					names->pop_back();
					makeObjectsGetMember(objects, names, position, type->types[i], address);
					return;
				}
				else
				{
					makeObjectsGetSkip(type->types[i], address);
				}
			}
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsGetPrep(ObjectVector * objects, int * address, int dimensions) const
{
	switch (_sc)
	{
	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		*address = _address;
		break;

	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		makeObjectsSetPrep(objects, address, dimensions);
		*address = 0;
		break;
	}
}

void SourceVariable::makeObjectsGetSkip(VariableType const * const type, int * const address) const
{
	switch (type->type)
	{
	case VT_ACSFUNC:
	case VT_CHAR:
	case VT_INT:
	case VT_LNSPEC:
	case VT_NATIVE:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		++*address;
		break;

	case VT_ARRAY:
	case VT_STRUCT:
		for (size_t i(0); i < type->types.size(); ++i)
			makeObjectsGetSkip(type->types[i], address);
		break;

	case VT_ASMFUNC:
	case VT_VOID:
		break;
	}
}

void SourceVariable::makeObjectsSet(ObjectVector * objects, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsSetPrep(objects, &address, 0);
	makeObjectsSet(objects, position, _type, &address);
	makeObjectsSetPost(objects, 0);
	makeObjectsGet(objects, position);
}
void SourceVariable::makeObjectsSet(ObjectVector * objects, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	ObjectToken::ObjectCode ocode;
	ObjectToken::ObjectCode opush;

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectsSet on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
		ocode = ObjectToken::OCODE_ASSIGNSCRIPTVAR;
	sc_register_case:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
			objects->addToken(ocode, objects->getValue((*address)--));
			break;

		case VT_ARRAY:
			throw SourceException("makeObjectsSet on register VT_ARRAY", position, "SourceVariable");

		case VT_ASMFUNC:
		case VT_VOID:
			break;

		case VT_STRUCT:
			for (size_t i(type->types.size()); i--;)
				makeObjectsSet(objects, position, type->types[i], address);
			break;
		}
		break;

	case SC_REGISTER_GLOBAL:
		ocode = ObjectToken::OCODE_ASSIGNGLOBALVAR;
		goto sc_register_case;

	case SC_REGISTER_MAP:
		ocode = ObjectToken::OCODE_ASSIGNMAPVAR;
		goto sc_register_case;

	case SC_REGISTER_WORLD:
		ocode = ObjectToken::OCODE_ASSIGNWORLDVAR;
		goto sc_register_case;

	sc_registerarray_case:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue((*address)--));
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
			objects->addToken(opush, objects->getValue(_address));
			objects->addToken(ObjectToken::OCODE_ADD);
			objects->addToken(ObjectToken::OCODE_SWAP);
			objects->addToken(ocode, objects->getValue(_address));
			break;

		case VT_ARRAY:
		case VT_STRUCT:
			for (size_t i(type->types.size()); i--;)
				makeObjectsSet(objects, position, type->types[i], address);
			break;

		case VT_ASMFUNC:
		case VT_VOID:
			break;
		}
		break;

	case SC_REGISTERARRAY_GLOBAL:
		ocode = ObjectToken::OCODE_ASSIGNGLOBALARRAY;
		opush = ObjectToken::OCODE_PUSHGLOBALARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_MAP:
		ocode = ObjectToken::OCODE_ASSIGNMAPARRAY;
		opush = ObjectToken::OCODE_PUSHMAPARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_WORLD:
		ocode = ObjectToken::OCODE_ASSIGNWORLDARRAY;
		opush = ObjectToken::OCODE_PUSHWORLDARRAY;
		goto sc_registerarray_case;
	}
}

void SourceVariable::makeObjectsSetArray(ObjectVector * objects, int dimensions, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	makeObjectsSetPrep(objects, &address, dimensions);
	makeObjectsSetArray(objects, dimensions, position, _type, &address);
	makeObjectsSetPost(objects, dimensions);
	makeObjectsGetArray(objects, dimensions, position);
}
void SourceVariable::makeObjectsSetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	if (dimensions == 0)
	{
		makeObjectsSet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("makeObjectsSetArray on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		throw SourceException("makeObjectsSetArray on register", position, "SourceVariable");

	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ASMFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_STRUCT:
		case VT_VOID:
			throw SourceException("makeObjectsSetArray on non-VT_ARRAY", position, "SourceVariable");

		case VT_ARRAY:
			makeObjectsSetArray(objects, dimensions-1, position, type->refType, address);
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const
{
	objects->setPosition(position);

	int address;
	std::vector<std::string> namesOriginal(*names);
	makeObjectsSetPrep(objects, &address, 0);
	makeObjectsSetMember(objects, names, position, _type, &address);
	makeObjectsSetPost(objects, 0);
	makeObjectsGetMember(objects, &namesOriginal, position);
}
void SourceVariable::makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * const type, int * const address) const
{
	if (names->empty())
	{
		makeObjectsSet(objects, position, type, address);
		return;
	}

	switch (_sc)
	{
	case SC_CONSTANT:
		throw SourceException("attempt to set SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		switch (type->type)
		{
		case VT_ACSFUNC:
		case VT_ARRAY:
		case VT_ASMFUNC:
		case VT_CHAR:
		case VT_INT:
		case VT_LNSPEC:
		case VT_NATIVE:
		case VT_REAL:
		case VT_SCRIPT:
		case VT_STRING:
		case VT_VOID:
			throw SourceException("attempt to set member from non-struct", position, "SourceVariable");

		case VT_STRUCT:
			for (size_t i(0); i < type->types.size(); ++i)
			{
				if (type->names[i] == names->back())
				{
					names->pop_back();
					makeObjectsSetMember(objects, names, position, type->types[i], address);
					return;
				}
				else
				{
					makeObjectsSetSkip(type->types[i], address);
				}
			}
			break;
		}
		break;
	}
}

void SourceVariable::makeObjectsSetPost(ObjectVector * objects, int dimensions) const
{
	ObjectToken::ObjectCode ocode;

	switch (_sc)
	{
	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		break;

	sc_registerarray_case:
	{
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
		objects->addToken(ocode, objects->getValue(_address));
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(1));
		objects->addToken(ObjectToken::OCODE_SUB);

		VariableType const * type = _type;

		for (int i(dimensions); i--;) type = type->refType;
		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(type->size()));
		objects->addToken(ObjectToken::OCODE_DIV);

		if (dimensions) for (int i(dimensions); --i;) objects->addTokenPushZero();
	}
		break;

	case SC_REGISTERARRAY_GLOBAL:
		ocode = ObjectToken::OCODE_PUSHGLOBALARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_MAP:
		ocode = ObjectToken::OCODE_PUSHMAPARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_WORLD:
		ocode = ObjectToken::OCODE_PUSHWORLDARRAY;
		goto sc_registerarray_case;
	}
}

void SourceVariable::makeObjectsSetPrep(ObjectVector * objects, int * address, int dimensions) const
{
	ObjectToken::ObjectCode ocode;

	switch (_sc)
	{
	case SC_CONSTANT:
	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
		*address = _address + _type->size() - 1;
		break;

	sc_registerarray_case:
	{
		VariableType const * type = _type;
		if (!dimensions) objects->addTokenPushZero();
		else
		{
			objects->addTokenPushZero();
			objects->addToken(ObjectToken::OCODE_SWAP);
		}

		for (int i(dimensions); i--;)
		{
			type = type->refType;
			objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(type->size()));
			objects->addToken(ObjectToken::OCODE_MUL);
			objects->addToken(ObjectToken::OCODE_ADD);
			if (i) objects->addToken(ObjectToken::OCODE_SWAP);
		}

		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(1));
		objects->addToken(ObjectToken::OCODE_ADD);

		objects->addToken(ObjectToken::OCODE_PUSHNUMBER, objects->getValue(0));
		objects->addToken(ObjectToken::OCODE_SWAP);

		objects->addToken(ocode, objects->getValue(_address));

		*address = type->size() - 1;
	}
		break;

	case SC_REGISTERARRAY_GLOBAL:
		ocode = ObjectToken::OCODE_ASSIGNGLOBALARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_MAP:
		ocode = ObjectToken::OCODE_ASSIGNMAPARRAY;
		goto sc_registerarray_case;

	case SC_REGISTERARRAY_WORLD:
		ocode = ObjectToken::OCODE_ASSIGNWORLDARRAY;
		goto sc_registerarray_case;
	}
}

void SourceVariable::makeObjectsSetSkip(VariableType const * const type, int * const address) const
{
	switch (type->type)
	{
	case VT_ACSFUNC:
	case VT_CHAR:
	case VT_INT:
	case VT_LNSPEC:
	case VT_NATIVE:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		--*address;
		break;

	case VT_ARRAY:
	case VT_STRUCT:
		for (size_t i(type->types.size()); i--;)
			makeObjectsSetSkip(type->types[i], address);
		break;

	case VT_ASMFUNC:
	case VT_VOID:
		break;
	}
}



void print_debug(std::ostream * const out, SourceVariable const & in)
{
	*out << "SourceVariable(";
		*out << "address=(";
		*out << in._address;
		*out << ")";

		*out << ", ";

		*out << "nameObject=(";
		print_debug(out, in._nameObject);
		*out << ")";

		*out << ", ";

		*out << "nameSource=(";
		print_debug(out, in._nameSource);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, in._position);
		*out << ")";

		*out << ", ";

		*out << "sc=(";
		print_debug(out, in._sc);
		*out << ")";

		*out << ", ";

		*out << "type=(";
		print_debug(out, in._type);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * const out, SourceVariable::StorageClass const in)
{
	switch (in)
	{
	case SourceVariable::SC_CONSTANT:             *out << "SC_CONSTANT";             break;
	case SourceVariable::SC_REGISTER:             *out << "SC_REGISTER";             break;
	case SourceVariable::SC_REGISTER_GLOBAL:      *out << "SC_REGISTER_GLOBAL";      break;
	case SourceVariable::SC_REGISTER_MAP:         *out << "SC_REGISTER_MAP";         break;
	case SourceVariable::SC_REGISTER_WORLD:       *out << "SC_REGISTER_WORLD";       break;
	case SourceVariable::SC_REGISTERARRAY_GLOBAL: *out << "SC_REGISTERARRAY_GLOBAL"; break;
	case SourceVariable::SC_REGISTERARRAY_MAP:    *out << "SC_REGISTERARRAY_MAP";    break;
	case SourceVariable::SC_REGISTERARRAY_WORLD:  *out << "SC_REGISTERARRAY_WORLD";  break;
	}
}
void print_debug(std::ostream * const out, SourceVariable::VariableType const & in)
{
	*out << "SourceVariable::VariableType(";
		*out << "type=(";
		print_debug(out, in.type);
		*out << ")";

		*out << ", ";

		*out << "names=(";
		print_debug(out, in.names);
		*out << ")";

		*out << ", ";

		*out << "types=(";
		print_debug(out, in.types);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * const out, SourceVariable::VariableTypeInternal const in)
{
	switch (in)
	{
	case SourceVariable::VT_ACSFUNC: *out << "VT_ACSFUNC"; break;
	case SourceVariable::VT_ARRAY:   *out << "VT_ARRAY";   break;
	case SourceVariable::VT_ASMFUNC: *out << "VT_ASMFUNC"; break;
	case SourceVariable::VT_CHAR:    *out << "VT_CHAR";    break;
	case SourceVariable::VT_INT:     *out << "VT_INT";     break;
	case SourceVariable::VT_LNSPEC:  *out << "VT_LNSPEC";  break;
	case SourceVariable::VT_NATIVE:  *out << "VT_NATIVE";  break;
	case SourceVariable::VT_REAL:    *out << "VT_REAL" ;   break;
	case SourceVariable::VT_SCRIPT:  *out << "VT_SCRIPT";  break;
	case SourceVariable::VT_STRING:  *out << "VT_STRING";  break;
	case SourceVariable::VT_STRUCT:  *out << "VT_STRUCT";  break;
	case SourceVariable::VT_VOID:    *out << "VT_VOID";    break;
	}
}



