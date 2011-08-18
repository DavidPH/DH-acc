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
#include "ObjectCode.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <string>
#include <vector>

class ObjectExpression;
class ObjectVector;
class SourceExpression;
class SourceTokenC;
class VariableType;



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
		SC_REGISTERARRAY_WORLD,
		SC_STATIC
	};

	struct VariableData_AsmFunc
	{
		VariableType const * type;
		ObjectCode ocode;
		ObjectCode ocode_imm;
	};
	struct VariableData_Bool
	{
		VariableType const * type;
		bool value;
	};
	struct VariableData_Char
	{
		VariableType const * type;
		int value;
	};
	struct VariableData_Function
	{
		VariableType const * type;
		int number;
	};
	struct VariableData_Int
	{
		VariableType const * type;
		int value;
	};
	struct VariableData_LineSpec
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
		VariableData_AsmFunc  vdAsmFunc;
		VariableData_Bool     vdBool;
		VariableData_Char     vdChar;
		VariableData_Function vdFunction;
		VariableData_Int      vdInt;
		VariableData_LineSpec vdLineSpec;
		VariableData_Native   vdNative;
		VariableData_Real     vdReal;
		VariableData_Script   vdScript;
		VariableData_String   vdString;
	};



	SourceVariable();
	SourceVariable(std::string const & nameObject, std::string const & nameSource, StorageClass sc, VariableType const * type, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_AsmFunc  const & vdAsmFunc,  SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Bool     const & vdBool,     SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Char     const & vdChar,     SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Function const & vdFunction, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Int      const & vdInt,      SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_LineSpec const & vdLineSpec, SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Native   const & vdNative,   SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Real     const & vdReal,     SourcePosition const & position);
	SourceVariable(std::string const & name, VariableData_Script   const & vdScript,   SourcePosition const & position, std::string const & nameObject);
	SourceVariable(std::string const & name, VariableData_String   const & vdString,   SourcePosition const & position, std::string const & nameObject);
	SourceVariable(std::string const & name, VariableType const * type, ObjectExpression * expr, SourcePosition const & position);

	bool canMakeObject() const;
	bool canMakeObjectAddress() const;

	bool canMakeObjectsAddress() const;

	StorageClass getClass() const;

	std::string const & getNameObject() const;
	std::string const & getNameSource() const;

	VariableType const * getType() const;

	CounterPointer<ObjectExpression> makeObject(SourcePosition const & position) const;
	CounterPointer<ObjectExpression> makeObjectAddress(SourcePosition const & position) const;

	void makeObjectsAddress(ObjectVector * objects, SourcePosition const & position) const;

	void makeObjectsCall(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > const & args, ObjectExpression * stack, SourcePosition const & position) const;

	void makeObjectsGet(ObjectVector * objects, SourcePosition const & position) const;
	void makeObjectsGetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const;
	void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const;

	void makeObjectsSet(ObjectVector * objects, SourcePosition const & position) const;
	void makeObjectsSetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const;
	void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const;



	friend void print_debug(std::ostream * const out, SourceVariable const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::StorageClass const in);

	static StorageClass get_StorageClass(SourceTokenC const & token);

private:
	VariableData _data;
	CounterPointer<ObjectExpression> _expr;
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
};



#endif /* HPP_SourceVariable_ */



