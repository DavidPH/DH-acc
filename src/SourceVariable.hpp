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
class SourceTokenC;



class SourceVariable
{
public:
	enum StorageClass
	{
		SC_REGISTER
	};

	enum VariableType
	{
		VT_FIXED,
		VT_INT
	};



	SourceVariable();
	SourceVariable(std::string const & nameObject, std::string const & nameSource, int const address, StorageClass const sc, VariableType const type, SourcePosition const & position);

	StorageClass getClass() const;

	std::string const & getNameObject() const;
	std::string const & getNameSource() const;

	VariableType getType() const;

	void makeObjectsGet(std::vector<ObjectToken> * const objects) const;
	void makeObjectsSet(std::vector<ObjectToken> * const objects) const;



	friend void print_debug(std::ostream * const out, SourceVariable const & in);
	friend void print_debug(std::ostream * const out, SourceVariable::StorageClass const in);
	friend void print_debug(std::ostream * const out, SourceVariable::VariableType const in);

	static StorageClass get_StorageClass(SourceTokenC const & token);

	static VariableType get_VariableType(SourceTokenC const & token);

private:
	int _address;
	std::string _nameObject;
	std::string _nameSource;
	SourcePosition _position;
	StorageClass _sc;
	VariableType _type;
};



#endif /* HPP_SourceVariable_ */



