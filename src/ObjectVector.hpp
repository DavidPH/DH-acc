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

#include "ObjectExpression.hpp"
#include "ObjectToken.hpp"
#include "SourcePosition.hpp"

#include <string>
#include <vector>



class ObjectVector
{
public:
	void addLabel(std::string const & label);
	void addLabel(std::vector<std::string> const & labels);

	void addToken(ObjectToken const & token);
	void addToken(ObjectToken::ObjectCode code);
	void addToken(ObjectToken::ObjectCode code, std::vector<ObjectExpression> const & args);
	void addToken(ObjectToken::ObjectCode code, ObjectExpression const & arg0);
	void addToken(ObjectToken::ObjectCode code, ObjectExpression const & arg0, ObjectExpression const & arg1);
	void addTokenPushZero();

	ObjectExpression getValue(ObjectExpression::float_t f);
	ObjectExpression getValue(ObjectExpression::int_t i);
	ObjectExpression getValue(int i);

	ObjectToken const & operator [] (ObjectExpression::int_t index) const;

	ObjectVector & setPosition(SourcePosition const & position);

	ObjectExpression::int_t size() const;

private:
	SourcePosition _position;

	std::vector<std::string> _labels;
	std::vector<ObjectToken> _tokens;
};



#endif /* HPP_ObjectVector_ */



