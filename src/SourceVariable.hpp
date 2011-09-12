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
#include "SourcePosition.hpp"

#include <ostream>
#include <string>
#include <vector>

class ObjectExpression;
class ObjectVector;
class SourceExpression;
class SourceTokenC;
class VariableType;



class SourceVariable : Counter
{
	MAKE_COUNTER_CLASS_BASE(SourceVariable, Counter);

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



	bool canMakeObject() const;
	bool canMakeObjectAddress() const;

	bool canMakeObjectsAddress() const;

	StorageClass getClass() const;

	std::string const & getNameObject() const;
	std::string const & getNameSource() const;

	VariableType const * getType() const;

	CounterPointer<ObjectExpression> makeObject(SourcePosition const & position) const;
	CounterPointer<ObjectExpression> makeObjectAddress(SourcePosition const & position) const;

	void makeObjectsAccess(ObjectVector * objects, SourcePosition const & position) const;
	void makeObjectsAccessArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const;
	void makeObjectsAccessMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const;

	void makeObjectsAddress(ObjectVector * objects, SourcePosition const & position) const;

	void makeObjectsGet(ObjectVector * objects, SourcePosition const & position) const;
	void makeObjectsGetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const;
	void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const;

	void makeObjectsSet(ObjectVector * objects, SourcePosition const & position) const;
	void makeObjectsSetArray(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, SourcePosition const & position) const;
	void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position) const;



	friend void print_debug(std::ostream * const out, SourceVariable const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::StorageClass const in);

	static Pointer create_constant(std::string const & nameSource, VariableType const * type, ObjectExpression * expr, SourcePosition const & position);
	static Pointer create_constant(std::string const & nameSource, VariableType const * type, std::string const & nameObject, SourcePosition const & position);
	static Pointer create_literal(VariableType const * type, ObjectExpression * expr, SourcePosition const & position);
	static Pointer create_literal(VariableType const * type, std::string const & nameObject, SourcePosition const & position);
	static Pointer create_variable(std::string const & nameSource, VariableType const * type, std::string const & nameObject, StorageClass sc, SourcePosition const & position);

	static StorageClass get_StorageClass(SourceTokenC const & token);

private:
	SourceVariable(SourceVariable const & var);
	SourceVariable(std::string const & nameSource, VariableType const * type, ObjectExpression * expr, SourcePosition const & position);
	SourceVariable(std::string const & nameSource, VariableType const * type, std::string const & nameObject, SourcePosition const & position);
	SourceVariable(std::string const & nameSource, VariableType const * type, std::string const & nameObject, StorageClass sc, SourcePosition const & position);
	~SourceVariable();

	void makeObjectsAccessPrep(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, CounterPointer<ObjectExpression> * addressBase, int * address, SourcePosition const & position) const;

	void makeObjectsGet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address, bool dimensioned) const;
	void makeObjectsGetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsGetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsGetPrep(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, CounterPointer<ObjectExpression> * addressBase, int * address, SourcePosition const & position) const;
	void makeObjectsGetSkip(VariableType const * type, int * address) const;

	void makeObjectsSet(ObjectVector * objects, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address, bool dimensioned) const;
	void makeObjectsSetArray(ObjectVector * objects, int dimensions, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsSetMember(ObjectVector * objects, std::vector<std::string> * names, SourcePosition const & position, VariableType const * type, ObjectExpression * addressBase, int * address) const;
	void makeObjectsSetPrep(ObjectVector * objects, std::vector<CounterPointer<SourceExpression> > * dimensions, CounterPointer<ObjectExpression> * addressBase, int * address, SourcePosition const & position) const;
	void makeObjectsSetSkip(VariableType const * type, int * address) const;

	CounterPointer<ObjectExpression> _expr;
	std::string _nameObject;
	std::string _nameSource;
	SourcePosition _position;
	StorageClass _sc;
	VariableType const * _type;
};



#endif /* HPP_SourceVariable_ */


