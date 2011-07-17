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

#include "SourcePosition.hpp"

#include <ostream>
#include <string>
#include <vector>

class ObjectToken;
class SourceExpressionDS;
class SourceTokenC;



class SourceVariable
{
public:
	enum StorageClass
	{
		SC_REGISTER
	};

	enum VariableTypeInternal
	{
		VT_FIXED,
		VT_INT,
		VT_STRING,

		VT_VOID,

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



	SourceVariable();
	SourceVariable(std::string const & nameObject, std::string const & nameSource, int const address, StorageClass const sc, VariableType const * const type, SourcePosition const & position);

	StorageClass getClass() const;

	std::string const & getNameObject() const;
	std::string const & getNameSource() const;

	VariableType const * getType() const;

	void makeObjectsCall(std::vector<ObjectToken> * const objects, std::vector<SourceExpressionDS> const & args) const;
	void makeObjectsGet(std::vector<ObjectToken> * const objects) const;
	void makeObjectsGet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names) const;



	friend void print_debug(std::ostream * const out, SourceVariable const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::StorageClass const in);
	friend void print_debug(std::ostream * const out, SourceVariable::VariableType const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::VariableTypeInternal const in);

	static void add_struct(std::string const & name, std::vector<std::string> const & names, std::vector<VariableType const *> const & types);

	static void add_typedef(std::string const & name, VariableType const * type);

	static StorageClass get_StorageClass(SourceTokenC const & token);

	static VariableType const * get_VariableType(SourceTokenC const & token);
	static VariableType const * get_VariableType(VariableTypeInternal const type);
	static VariableType const * get_VariableType_null(std::string const & name);
	// Script types are identified by their associated types.
	// If there is no such script type defined, it will be added.
	static VariableType const * get_VariableType_script(VariableType const * callType, std::vector<VariableType const *> const & types);

	static void init();

private:
	int _address;
	std::string _nameObject;
	std::string _nameSource;
	SourcePosition _position;
	StorageClass _sc;
	VariableType const * _type;

	void makeObjectsGet(std::vector<ObjectToken> * const objects, VariableType const * const type, int * const address) const;
	void makeObjectsGet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names, VariableType const * const type, int * const address) const;
	void makeObjectsGet(VariableType const * const type, int * const address) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects, VariableType const * const type, int * const address) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names, VariableType const * const type, int * const address) const;
	void makeObjectsSet(VariableType const * const type, int * const address) const;



	static std::vector<std::string>    _names;
	static std::vector<VariableType *> _types;
};



#endif /* HPP_SourceVariable_ */



