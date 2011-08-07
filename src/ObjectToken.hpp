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

/* ObjectToken.hpp
**
** Defines the ObjectToken class.
*/

#ifndef HPP_ObjectToken_
#define HPP_ObjectToken_

#include "ObjectCode.hpp"
#include "ObjectExpression.hpp"
#include "SourcePosition.hpp"

#include <map>
#include <ostream>
#include <stdint.h>
#include <string>
#include <vector>

class ObjectExpression;
class SourceTokenC;



class ObjectToken
{
public:
	ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression::Pointer> const & args);

	void addLabel(std::string const & label);
	void addLabel(std::vector<std::string> const & labels);

	ObjectExpression::Pointer getArg(size_t index) const;

	ObjectCode getCode() const;

	std::vector<std::string> const & getLabels() const;
	SourcePosition const & getPosition() const;



	friend void print_debug(std::ostream * const out, ObjectToken const & in);

private:
	std::vector<ObjectExpression::Pointer> _args;
	ObjectCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;
};



#endif /* HPP_ObjectToken_ */



