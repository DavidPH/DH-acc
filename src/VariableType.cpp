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

/* VariableType.cpp
**
** Defines the VariableType methods.
*/

#include "VariableType.hpp"

#include "print_debug.hpp"
#include "SourceException.hpp"



std::vector<VariableType *> VariableType::type_array;
std::vector<VariableType *> VariableType::type_asmfunc;
std::vector<VariableType *> VariableType::type_block;
std::vector<VariableType *> VariableType::type_function;
std::vector<VariableType *> VariableType::type_linespec;
std::vector<VariableType *> VariableType::type_native;
std::vector<VariableType *> VariableType::type_pointer;
std::vector<VariableType *> VariableType::type_script;

VariableType VariableType::vt_boolhard = {VT_BOOLHARD, true, &vtc_void, &vtc_boolhard, &vtc_void};
VariableType VariableType::vt_boolsoft = {VT_BOOLSOFT, true, &vtc_void, &vtc_boolsoft, &vtc_void};
VariableType VariableType::vt_char     = {VT_CHAR,     true, &vtc_void, &vtc_char,     &vtc_void};
VariableType VariableType::vt_int      = {VT_INT,      true, &vtc_void, &vtc_int,      &vtc_void};
VariableType VariableType::vt_real     = {VT_REAL,     true, &vtc_void, &vtc_real,     &vtc_void};
VariableType VariableType::vt_string   = {VT_STRING,   true, &vtc_void, &vtc_string,   &vtc_char};
VariableType VariableType::vt_void     = {VT_VOID,     true, &vtc_void, &vtc_void,     &vtc_void};

VariableType VariableType::vtc_boolhard = {VT_BOOLHARD, true, &vtc_void, NULL, &vtc_void};
VariableType VariableType::vtc_boolsoft = {VT_BOOLSOFT, true, &vtc_void, NULL, &vtc_void};
VariableType VariableType::vtc_char     = {VT_CHAR,     true, &vtc_void, NULL, &vtc_void};
VariableType VariableType::vtc_int      = {VT_INT,      true, &vtc_void, NULL, &vtc_void};
VariableType VariableType::vtc_real     = {VT_REAL,     true, &vtc_void, NULL, &vtc_void};
VariableType VariableType::vtc_string   = {VT_STRING,   true, &vtc_void, NULL, &vtc_char};
VariableType VariableType::vtc_void     = {VT_VOID,     true, &vtc_void, NULL, &vtc_void};



template<typename T> static bool operator == (std::vector<T> const & l, std::vector<T> const & r)
{
	if (l.size() != r.size()) return false;

	for (size_t i(l.size()); i--;)
		if (l[i] != r[i]) return false;

	return true;
}



VariableType * VariableType::create(Type vt, VariableType const * callType, VariableType const * refType)
{
	static std::vector<VariableType const *> types;

	return create(vt, callType, refType, types);
}
VariableType * VariableType::create(Type vt, VariableType const * callType, VariableType const * refType, std::vector<VariableType const *> const & types)
{
	VariableType * type(new VariableType);

	type->vt        = vt;
	type->complete  = true;
	type->callType  = callType;
	type->constType = NULL;
	type->refType   = refType;
	type->names     = std::vector<std::string>(types.size(), "");
	type->types     = types;

	type->constType = (new VariableType(*type))->doConst();

	return type;
}

VariableType const * VariableType::doConst()
{
	switch (vt)
	{
	case VT_ARRAY:
	case VT_BLOCK:
	case VT_STRUCT:
	case VT_UNION:
		for (size_t i(0); i < types.size(); ++i)
			if (types[i]->constType)
				types[i] = types[i]->constType;

		break;

	case VT_ASMFUNC:
	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_ENUM:
	case VT_FUNCTION:
	case VT_INT:
	case VT_LINESPEC:
	case VT_NATIVE:
	case VT_POINTER:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
	case VT_VOID:
		break;
	}

	return this;
}

VariableType const * VariableType::get_array(VariableType const * refType, bigsint count)
{
	for (std::vector<VariableType *>::iterator it(type_array.begin()); it != type_array.end(); ++it)
		if ((*it)->refType == refType && (bigsint)(*it)->types.size() == count)
			return *it;

	VariableType * type(create(VT_ARRAY, &vt_void, refType, std::vector<VariableType const *>(count, refType)));

	type_array.push_back(type);

	return type;
}

VariableType const * VariableType::get_asmfunc(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_function_like(VT_ASMFUNC, &type_asmfunc, callType, types);
}

VariableType const * VariableType::get_block(std::vector<VariableType const *> const & types)
{
	for (std::vector<VariableType *>::iterator it(type_block.begin()); it != type_block.end(); ++it)
		if ((*it)->types == types)
			return *it;

	VariableType * type(create(VT_BLOCK, &vt_void, &vt_void, types));

	type_block.push_back(type);

	return type;
}

VariableType const * VariableType::get_function(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_function_like(VT_FUNCTION, &type_function, callType, types);
}

VariableType const * VariableType::get_function_like(Type vt, std::vector<VariableType *> * type_vector, VariableType const * callType, std::vector<VariableType const *> const & types)
{
	for (std::vector<VariableType *>::iterator it(type_vector->begin()); it != type_vector->end(); ++it)
		if ((*it)->callType == callType && (*it)->types == types)
			return *it;

	VariableType * type(create(vt, callType, &vt_void, types));

	type_vector->push_back(type);

	return type;
}

VariableType const * VariableType::get_linespec(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_function_like(VT_LINESPEC, &type_linespec, callType, types);
}

VariableType const * VariableType::get_native(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_function_like(VT_NATIVE, &type_native, callType, types);
}

VariableType const * VariableType::get_pointer(VariableType const * refType)
{
	for (std::vector<VariableType *>::iterator it(type_pointer.begin()); it != type_pointer.end(); ++it)
		if ((*it)->refType == refType)
			return *it;

	VariableType * type(create(VT_POINTER, &vt_void, refType));

	type_pointer.push_back(type);

	return type;
}

VariableType const * VariableType::get_script(VariableType const * callType, std::vector<VariableType const *> const & types)
{
	return get_function_like(VT_SCRIPT, &type_script, callType, types);
}

VariableType const * VariableType::get_vt(Type vt)
{
	switch (vt)
	{
	case VT_BOOLHARD: return &vt_boolhard;
	case VT_BOOLSOFT: return &vt_boolsoft;
	case VT_CHAR:     return &vt_char;
	case VT_INT:      return &vt_int;
	case VT_REAL:     return &vt_real;
	case VT_STRING:   return &vt_string;
	case VT_VOID:     return &vt_void;

	default: throw "VariableType::get_vt";
	}
}
VariableType const * VariableType::get_vt_boolhard()
{
	return &vt_boolhard;
}
VariableType const * VariableType::get_vt_boolsoft()
{
	return &vt_boolsoft;
}
VariableType const * VariableType::get_vt_char()
{
	return &vt_char;
}
VariableType const * VariableType::get_vt_int()
{
	return &vt_int;
}
VariableType const * VariableType::get_vt_real()
{
	return &vt_real;
}
VariableType const * VariableType::get_vt_string()
{
	return &vt_string;
}
VariableType const * VariableType::get_vt_void()
{
	return &vt_void;
}

int VariableType::getOffset(std::string const & name, SourcePosition const & position) const
{
	int offset(0);

	if (vt == VT_UNION)
		return offset;

	for (size_t i(0); i < names.size(); ++i)
		if (name == names[i])
			return offset;
		else
			offset += types[i]->size();

	throw SourceException("invalid member-variable-type '" + name + "'", position, "VariableType::getOffset");
}

VariableType const * VariableType::getType(std::string const & name, SourcePosition const & position) const
{
	for (size_t i(0); i < names.size(); ++i)
		if (name == names[i])
			return types[i];

	throw SourceException("invalid member-variable-type '" + name + "'", position, "VariableType::getType");
}

bool VariableType::isVoid() const
{
	switch (vt)
	{
	case VT_ARRAY:
	case VT_BLOCK:
	case VT_STRUCT:
	case VT_UNION:
	{
		for (size_t i(0); i < types.size(); ++i)
			if (!types[i]->isVoid()) return false;

		return true;
	}

	case VT_VOID:
		return true;

	case VT_ASMFUNC:
	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_ENUM:
	case VT_FUNCTION:
	case VT_INT:
	case VT_LINESPEC:
	case VT_NATIVE:
	case VT_POINTER:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		return false;
	}

	return 0;
}

int VariableType::size() const
{
	switch (vt)
	{
	case VT_ARRAY:
	case VT_BLOCK:
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

	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_ENUM:
	case VT_FUNCTION:
	case VT_INT:
	case VT_LINESPEC:
	case VT_NATIVE:
	case VT_POINTER:
	case VT_REAL:
	case VT_SCRIPT:
	case VT_STRING:
		return 1;

	case VT_UNION:
	{
		int s(0);

		for (size_t i(0); i < types.size(); ++i)
		{
			if (types[i]->size() > s)
				s = types[i]->size();
		}

		return s;
	}
	}

	return 0;
}

int VariableType::sizeCall() const
{
	switch (vt)
	{
	case VT_ARRAY:
	case VT_BLOCK:
	case VT_BOOLHARD:
	case VT_BOOLSOFT:
	case VT_CHAR:
	case VT_ENUM:
	case VT_INT:
	case VT_POINTER:
	case VT_REAL:
	case VT_STRING:
	case VT_STRUCT:
	case VT_UNION:
	case VT_VOID:
		return 0;

	case VT_ASMFUNC:
	case VT_FUNCTION:
	case VT_LINESPEC:
	case VT_NATIVE:
	case VT_SCRIPT:
	{
		int s(0);

		for (std::vector<VariableType const *>::const_iterator it(types.begin()); it != types.end(); ++it)
			s += (*it)->size();

		return s;
	}
	}

	return 0;
}



char const * make_string(VariableType::Type vt)
{
	switch (vt)
	{
	case VariableType::VT_ARRAY:    return "VT_ARRAY";
	case VariableType::VT_ASMFUNC:  return "VT_ASMFUNC";
	case VariableType::VT_BLOCK:    return "VT_BLOCK";
	case VariableType::VT_BOOLHARD: return "VT_BOOLHARD";
	case VariableType::VT_BOOLSOFT: return "VT_BOOLSOFT";
	case VariableType::VT_CHAR:     return "VT_CHAR";
	case VariableType::VT_ENUM:     return "VT_ENUM";
	case VariableType::VT_FUNCTION: return "VT_FUNCTION";
	case VariableType::VT_INT:      return "VT_INT";
	case VariableType::VT_LINESPEC: return "VT_LINESPEC";
	case VariableType::VT_NATIVE:   return "VT_NATIVE";
	case VariableType::VT_POINTER:  return "VT_POINTER";
	case VariableType::VT_REAL:     return "VT_REAL";
	case VariableType::VT_SCRIPT:   return "VT_SCRIPT";
	case VariableType::VT_STRING:   return "VT_STRING";
	case VariableType::VT_STRUCT:   return "VT_STRUCT";
	case VariableType::VT_UNION:    return "VT_UNION";
	case VariableType::VT_VOID:     return "VT_VOID";
	}

	return "VT_";
}

VariableType::Type & operator ++ (VariableType::Type & vt)
{
	if (vt < VariableType::VT_UNION)
		vt = (VariableType::Type)((int)vt + 1);
	else
		vt = VariableType::VT_BOOLHARD;

	return vt;
}

void print_debug(std::ostream * out, VariableType const & in)
{
	*out << "VariableType(";
		*out << "vt=(";
		print_debug(out, in.vt);
		*out << ")";

		*out << ", ";

		*out << "callType->vt=(";
		print_debug(out, in.callType->vt);
		*out << ")";

		*out << ", ";

		*out << "refType->vt=(";
		print_debug(out, in.refType->vt);
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
void print_debug(std::ostream * out, VariableType::Type in)
{
	*out << make_string(in);
}


