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

#include <string>
#include <vector>

class SourcePosition;



struct VariableType
{
	enum Type
	{
		VT_BOOLHARD,
		VT_BOOLSOFT,
		VT_CHAR,
		VT_INT,
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

	bool isVoid() const;

	int size() const;
	int sizeCall() const;

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



	friend Type & operator ++ (Type & vt);

	friend void print_debug(std::ostream * out, VariableType const & in);
	friend void print_debug(std::ostream * out, VariableType::Type in);
};



#endif /* HPP_VariableType_ */



