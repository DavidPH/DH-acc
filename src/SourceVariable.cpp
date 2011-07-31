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

#include "SourceException.hpp"
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



SourceVariable::SourceVariable() : _nameObject(), _nameSource(), _position(), _sc(), _type()
{

}
SourceVariable::SourceVariable(std::string const & nameObject, std::string const & nameSource, StorageClass sc, VariableType const * type, SourcePosition const & position) : _nameObject(nameObject), _nameSource(nameSource), _position(position), _sc(sc), _type(type)
{

}
SourceVariable::SourceVariable(std::string const & name, VariableData_ACSFunc const & vdACSFunc, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdACSFunc.type)
{
	_data.vdACSFunc = vdACSFunc;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_AsmFunc const & vdAsmFunc, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdAsmFunc.type)
{
	_data.vdAsmFunc = vdAsmFunc;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Char const & vdChar, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdChar.type)
{
	_data.vdChar = vdChar;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Int const & vdInt, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdInt.type)
{
	_data.vdInt = vdInt;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_LnSpec const & vdLnSpec, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdLnSpec.type)
{
	_data.vdLnSpec = vdLnSpec;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Native const & vdNative, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdNative.type)
{
	_data.vdNative = vdNative;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Real const & vdReal, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdReal.type)
{
	_data.vdReal = vdReal;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_Script const & vdScript, SourcePosition const & position) : _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdScript.type)
{
	_data.vdScript = vdScript;
}
SourceVariable::SourceVariable(std::string const & name, VariableData_String const & vdString, SourcePosition const & position, std::string const & nameObject) : _nameObject(nameObject), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(vdString.type)
{
	_data.vdString = vdString;
}
SourceVariable::SourceVariable(std::string const & name, VariableType const * type, ObjectExpression * expr, SourcePosition const & position) : _expr(expr), _nameSource(name), _position(position), _sc(SC_CONSTANT), _type(type)
{

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
	if (token.getData() == "auto")
		return SC_AUTO;

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
	return get_VariableType_auto(VT_ACSFUNC, callType, _types[VT_VOID], types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_array(VariableType const * refType, int count)
{
	return get_VariableType_auto(VT_ARRAY, _types[VT_VOID], refType, std::vector<VariableType const *>(count, refType));
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_asmfunc(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_ASMFUNC, callType, _types[VT_VOID], types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_auto(VariableTypeInternal itype, VariableType const * callType, VariableType const * refType, std::vector<VariableType const *> const & types)
{
	for (size_t i(0); i < _types.size(); ++i)
	{
		if (_types[i]->type == itype && _types[i]->callType == callType && _types[i]->refType == refType && _types[i]->types.size() == types.size())
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
	type->refType  = refType;
	type->names    = std::vector<std::string>(types.size(), "");
	type->types    = types;

	_names.push_back("");
	_types.push_back(type);

	return type;
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_block(std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_BLOCK, _types[VT_VOID], _types[VT_VOID], types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_lnspec(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_LNSPEC, callType, _types[VT_VOID], types);
}
SourceVariable::VariableType const * SourceVariable::get_VariableType_native(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_VariableType_auto(VT_NATIVE, callType, _types[VT_VOID], types);
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
	return get_VariableType_auto(VT_SCRIPT, callType, _types[VT_VOID], types);
}

SourceVariable::StorageClass SourceVariable::getClass() const
{
	return _sc;
}

std::string const & SourceVariable::getNameObject() const
{
	return _nameObject;
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



