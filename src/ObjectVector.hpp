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

/* ObjectVector.hpp
**
** Defines the ObjectVector class.
*/

#ifndef HPP_ObjectVector_
#define HPP_ObjectVector_

#include "ObjectCode.hpp"
#include "ObjectExpression.hpp"
#include "SourcePosition.hpp"

#include <string>
#include <vector>

class ObjectToken;



class ObjectVector
{
public:
	void addLabel(std::string const & label);
	void addLabel(std::vector<std::string> const & labels);

	void addToken(ObjectToken const & token);
	void addToken(ObjectCode code);
	void addToken(ObjectCode code, std::vector<ObjectExpression::Pointer> const & args);
	void addToken(ObjectCode code, ObjectExpression * arg0);
	void addToken(ObjectCode code, ObjectExpression * arg0, ObjectExpression * arg1);
	void addTokenPushZero();

	ObjectExpression::Pointer getValue(ObjectExpression::float_t f) const;
	ObjectExpression::Pointer getValue(ObjectExpression::int_t i) const;
	ObjectExpression::Pointer getValue(int i) const;
	ObjectExpression::Pointer getValue(ObjectExpression * expr) const;
	ObjectExpression::Pointer getValue(std::string const & label) const;
	ObjectExpression::Pointer getValue(unsigned int i) const;

	template<typename T1, typename T2> ObjectExpression::Pointer getValueAdd(T1 const & l, T2 const & r) const;

	ObjectToken const & operator [] (ObjectExpression::int_t index) const;

	void optimize();

	ObjectVector & setPosition(SourcePosition const & position);

	ObjectExpression::int_t size() const;



	friend void read_object(std::istream * in, ObjectVector * out);
	friend void write_object(std::ostream * out, ObjectVector const & in);

private:
	SourcePosition _position;

	std::vector<std::string> _labels;
	std::vector<ObjectToken> _tokens;
};



template<typename T1, typename T2> inline ObjectExpression::Pointer ObjectVector::getValueAdd(T1 const & l, T2 const & r) const
{
	return ObjectExpression::create_binary_add(getValue(l), getValue(r), _position);
}



#endif /* HPP_ObjectVector_ */



