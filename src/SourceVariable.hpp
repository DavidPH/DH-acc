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

#include "CounterPointer.hpp"
#include "ObjectToken.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <string>
#include <vector>

class ObjectVector;
class SourceExpression;
class SourceTokenC;



class SourceVariable
{
public:
	enum StorageClass
	{
		SC_AUTO,
		SC_CONSTANT,
		SC_REGISTER,
		SC_REGISTER_GLOBAL,
		SC_REGISTER_MAP,
		SC_REGISTER_WORLD,
		SC_REGISTERARRAY_GLOBAL,
		SC_REGISTERARRAY_MAP,
		SC_REGISTERARRAY_WORLD
	};

	enum VariableTypeInternal
	{
		VT_CHAR,
		VT_INT,
		VT_REAL,
		VT_STRING,

		VT_VOID,

		VT_ACSFUNC,
		VT_ARRAY,
		VT_ASMFUNC,
		VT_BLOCK,
		VT_LNSPEC,
		VT_NATIVE,
		VT_SCRIPT,
		VT_STRUCT
	};

	struct VariableType
	{
		VariableType const * getType(std::string const & name, SourcePosition const & position) const;

		bool isVoid() const;

		int size() const;

		VariableTypeInternal type;

		// Type returned when called.
		VariableType const * callType;

		// Type returned when dereferenced.
		VariableType const * refType;

		// VT_STRUCT members or VT_FUNCTION/VT_SCRIPT args.
		std::vector<std::string>          names;
		std::vector<VariableType const *> types;
	};

	struct VariableData_ACSFunc
	{
		VariableType const * type;
		int number;
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
		VariableData_ACSFunc vdACSFunc;
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
	SourceVariable(std::string const & nameObject, std::string const & nameSource, StorageClass sc, VariableType const * type, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_ACSFunc const & vdACSFunc, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_AsmFunc const & vdAsmFunc, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Char    const & vdChar,    SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Int     const & vdInt,     SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_LnSpec  const & vdLnSpec,  SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Native  const & vdNative,  SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Real    const & vdReal,    SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Script  const & vdScript,  SourcePosition const & position, std::string const & nameObject);
	SourceVariable(std::string const & name, VariableData_String  const & vdString,  SourcePosition const & position, std::string const & nameObject);
	SourceVariable(std::string const & name, VariableType const * type, ObjectExpression * expr, SourcePosition const & position);

	StorageClass getClass() const;

	std::string const & getNameObject() const;
	std::string const & getNameSource() const;

	VariableType const * getType() const;

	bool isConstant() const;

	ObjectExpression::Pointer makeObject(SourcePosition const & position) const;

	void makeObjectsCall(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > const & args, ObjectExpression * stack, SourcePosition const & position) const;

	void makeObjectsGet(ObjectVector * objects, SourcePosition const & position) const;
	void makeObjectsGetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const;
	void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const;

	void makeObjectsSet(ObjectVector * objects, SourcePosition const & position) const;
	void makeObjectsSetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const;
	void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const;



	friend void print_debug(std::ostream * const out, SourceVariable const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::StorageClass const in);
	friend void print_debug(std::ostream * const out, SourceVariable::VariableType const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::VariableTypeInternal const in);

	static void add_struct(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types);

	static void add_typedef(std::string const & name, VariableType const * type);

	static StorageClass get_StorageClass(SourceTokenC const & token);

	static VariableType const * get_VariableType(SourceTokenC const & token);
	static VariableType const * get_VariableType(VariableTypeInternal const type);
	// ACSFunc types work like script types (see below).
	static VariableType const * get_VariableType_acsfunc(VariableType const * callType, std::vector<VariableType const *> const & types);
	static VariableType const * get_VariableType_array(VariableType const * refType, int count);
	// AsmFunc types work like script types (see below).
	static VariableType const * get_VariableType_asmfunc(VariableType const * callType, std::vector<VariableType const *> const & types);
	static VariableType const * get_VariableType_block(std::vector<VariableType const *> const & types);
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
	VariableData _data;
	ObjectExpression::Pointer _expr;
	std::string _nameObject;
	std::string _nameSource;
	SourcePosition _position;
	StorageClass _sc;
	VariableType const * _type;

	void makeObjectsGet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address, bool dimensioned) const;
	void makeObjectsGetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsGetPrep(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, CounterPointer<ObjectExpression> * addressBase, int * address) const;
	void makeObjectsGetSkip(VariableType const * type, int * address) const;

	void makeObjectsSet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address, bool dimensioned) const;
	void makeObjectsSetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsSetPrep(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, CounterPointer<ObjectExpression> * addressBase, int * address) const;
	void makeObjectsSetSkip(VariableType const * type, int * address) const;



	static VariableType const * get_VariableType_auto(VariableTypeInternal itype, VariableType const * callType, VariableType const * refType, std::vector<VariableType const *> const & types);

	static std::vector<std::string>    _names;
	static std::vector<VariableType *> _types;
};



#endif /* HPP_SourceVariable_ */



