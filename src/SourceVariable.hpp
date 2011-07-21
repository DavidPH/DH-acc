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
** Defines the SourceVariable class.
*/

#ifndef HPP_SourceVariable_
#define HPP_SourceVariable_

#include "ObjectToken.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <string>
#include <vector>

class SourceExpressionDS;
class SourceTokenC;



class SourceVariable
{
public:
	enum StorageClass
	{
		SC_CONSTANT,
		SC_REGISTER
	};

	enum VariableTypeInternal
	{
		VT_CHAR,
		VT_INT,
		VT_REAL,
		VT_STRING,

		VT_VOID,

		VT_ASMFUNC,
		VT_LNSPEC,
		VT_NATIVE,
		VT_SCRIPT,
		VT_STRUCT
	};

	struct VariableType
	{
		VariableType const * getType(std::string const & name, SourcePosition const & position) const;

		int size() const;

		VariableTypeInternal type;

		// Type returned when called.
		VariableType const * callType;

		// VT_STRUCT members or VT_FUNCTION/VT_SCRIPT args.
		std::vector<std::string>          names;
		std::vector<VariableType const *> types;
	};

	struct VariableData_AsmFunc
	{
		VariableType const * type;
		ObjectToken::ObjectCode ocode;
		ObjectToken::ObjectCode ocode_imm;
	};
	struct VariableData_Char
	{
		VariableType const * type;
		int value;
	};
	struct VariableData_Int
	{
		VariableType const * type;
		int value;
	};
	struct VariableData_LnSpec
	{
		VariableType const * type;
		int number;
	};
	struct VariableData_Native
	{
		VariableType const * type;
		int number;
	};
	struct VariableData_Real
	{
		VariableType const * type;
		double value;
	};
	struct VariableData_Script
	{
		VariableType const * type;
		int number;
	};
	struct VariableData_String
	{
		VariableType const * type;
		int number;
	};

	union VariableData
	{
		VariableData_AsmFunc vdAsmFunc;
		VariableData_Char    vdChar;
		VariableData_Int     vdInt;
		VariableData_LnSpec  vdLnSpec;
		VariableData_Native  vdNative;
		VariableData_Real    vdReal;
		VariableData_Script  vdScript;
		VariableData_String  vdString;
	};



	SourceVariable();
	SourceVariable(std::string const & nameObject, std::string const & nameSource, int const address, StorageClass const sc, VariableType const * const type, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_AsmFunc const & vdAsmFunc, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Char    const & vdChar,    SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Int     const & vdInt,     SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_LnSpec  const & vdLnSpec,  SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Native  const & vdNative,  SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Real    const & vdReal,    SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Script  const & vdScript,  SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_String  const & vdString,  SourcePosition const & position, std::string const & nameObject);

	StorageClass getClass() const;

	std::string const & getNameObject() const;
	std::string const & getNameSource() const;

	VariableType const * getType() const;

	bool isConstant() const;

	ObjectExpression makeObject() const;
	void makeObjectsCall(std::vector<ObjectToken> * const objects, std::vector<SourceExpressionDS> const & args, SourcePosition const & position) const;
	void makeObjectsGet(std::vector<ObjectToken> * const objects, SourcePosition const & position) const;
	void makeObjectsGet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names, SourcePosition const & position) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects, SourcePosition const & position) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names, SourcePosition const & position) const;



	friend void print_debug(std::ostream * const out, SourceVariable const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::StorageClass const in);
	friend void print_debug(std::ostream * const out, SourceVariable::VariableType const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::VariableTypeInternal const in);

	static void add_struct(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types);

	static void add_typedef(std::string const & name, VariableType const * type);

	static StorageClass get_StorageClass(SourceTokenC const & token);

	static VariableType const * get_VariableType(SourceTokenC const & token);
	static VariableType const * get_VariableType(VariableTypeInternal const type);
	// AsmFunc types work like script types (see below).
	static VariableType const * get_VariableType_asmfunc(VariableType const * callType, std::vector<VariableType const *> const & types);
	// LnSpec types work like script types (see below).
	static VariableType const * get_VariableType_lnspec(VariableType const * callType, std::vector<VariableType const *> const & types);
	// Native types work like script types (see below).
	static VariableType const * get_VariableType_native(VariableType const * callType, std::vector<VariableType const *> const & types);
	// Returns NULL if type not found.
	static VariableType const * get_VariableType_null(std::string const & name);
	// Script types are identified by their associated types.
	// If there is no such script type defined, it will be added.
	static VariableType const * get_VariableType_script(VariableType const * callType, std::vector<VariableType const *> const & types);

	static void init();

private:
	int _address;
	VariableData _data;
	std::string _nameObject;
	std::string _nameSource;
	SourcePosition _position;
	StorageClass _sc;
	VariableType const * _type;

	void makeObjectsGet(std::vector<ObjectToken> * const objects, SourcePosition const & position, VariableType const * const type, int * const address) const;
	void makeObjectsGet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names, SourcePosition const & position, VariableType const * const type, int * const address) const;
	void makeObjectsGet(VariableType const * const type, int * const address) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects, SourcePosition const & position, VariableType const * const type, int * const address) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names, SourcePosition const & position, VariableType const * const type, int * const address) const;
	void makeObjectsSet(VariableType const * const type, int * const address) const;



	static VariableType const * get_VariableType_auto(VariableTypeInternal itype, VariableType const * callType, std::vector<VariableType const *> const & types);

	static std::vector<std::string>    _names;
	static std::vector<VariableType *> _types;
};



#endif /* HPP_SourceVariable_ */



