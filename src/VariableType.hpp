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

/* VariableType.hpp
**
** Defines the VariableType struct.
*/

#ifndef HPP_VariableType_
#define HPP_VariableType_

#include <ostream>
#include <string>
#include <vector>

#include "bignum.hpp"

class SourcePosition;



struct VariableType
{
	enum Type
	{
		VT_BOOLHARD,
		VT_BOOLSOFT,
		VT_CHAR,
		VT_INT,
		VT_LABEL,
		VT_REAL,
		VT_STRING,

		VT_VOID,

		VT_ARRAY,
		VT_ASMFUNC,
		VT_BLOCK,
		VT_ENUM,
		VT_FUNCTION,
		VT_LINESPEC,
		VT_NATIVE,
		VT_POINTER,
		VT_SCRIPT,
		VT_STRUCT,
		VT_UNION
	};

	VariableType const * doConst();

	int getOffset(std::string const & name, SourcePosition const & position) const;

	VariableType const * getType(std::string const & name, SourcePosition const & position) const;

	bool isVoid(SourcePosition const & position) const;

	int size(SourcePosition const & position) const;
	int sizeCall(SourcePosition const & position) const;

	Type vt;

	// Type fully defined?
	bool complete;

	// Type returned when called.
	VariableType const * callType;

	// Type's constant version.
	VariableType const * constType;

	// Type returned when dereferenced.
	VariableType const * refType;

	// VT_STRUCT members or VT_FUNCTION args.
	std::vector<std::string>          names;
	std::vector<VariableType const *> types;



	friend char const * make_string(Type vt);

	friend Type & operator ++ (Type & vt);

	friend void print_debug(std::ostream * out, VariableType const & in);
	friend void print_debug(std::ostream * out, VariableType::Type in);

	static VariableType const * get_array(VariableType const * refType, bigsint count);
	static VariableType const * get_asmfunc(VariableType const * callType, std::vector<VariableType const *> const & types);
	static VariableType const * get_block(std::vector<VariableType const *> const & types);
	static VariableType const * get_function(VariableType const * callType, std::vector<VariableType const *> const & types);
	static VariableType const * get_linespec(VariableType const * callType, std::vector<VariableType const *> const & types);
	static VariableType const * get_native(VariableType const * callType, std::vector<VariableType const *> const & types);
	static VariableType const * get_pointer(VariableType const * refType);
	static VariableType const * get_script(VariableType const * callType, std::vector<VariableType const *> const & types);

	static VariableType const * get_vt(Type vt);
	static VariableType const * get_vt_boolhard();
	static VariableType const * get_vt_boolsoft();
	static VariableType const * get_vt_char();
	static VariableType const * get_vt_int();
	static VariableType const * get_vt_label();
	static VariableType const * get_vt_real();
	static VariableType const * get_vt_string();
	static VariableType const * get_vt_void();

private:
	static VariableType * create(Type vt, VariableType const * callType, VariableType const * refType);
	static VariableType * create(Type vt, VariableType const * callType, VariableType const * refType, std::vector<VariableType const *> const & types);

	static VariableType const * get_function_like(Type vt, std::vector<VariableType *> * type_vector, VariableType const * callType, std::vector<VariableType const *> const & types);

	static std::vector<VariableType *> type_array;
	static std::vector<VariableType *> type_asmfunc;
	static std::vector<VariableType *> type_block;
	static std::vector<VariableType *> type_function;
	static std::vector<VariableType *> type_linespec;
	static std::vector<VariableType *> type_native;
	static std::vector<VariableType *> type_pointer;
	static std::vector<VariableType *> type_script;

	static VariableType vt_boolhard;
	static VariableType vt_boolsoft;
	static VariableType vt_char;
	static VariableType vt_int;
	static VariableType vt_label;
	static VariableType vt_real;
	static VariableType vt_string;
	static VariableType vt_void;

	static VariableType vtc_boolhard;
	static VariableType vtc_boolsoft;
	static VariableType vtc_char;
	static VariableType vtc_int;
	static VariableType vtc_label;
	static VariableType vtc_real;
	static VariableType vtc_string;
	static VariableType vtc_void;
};



#endif /* HPP_VariableType_ */


