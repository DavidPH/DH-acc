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

#include "CounterPointer.hpp"
#include "ObjectCode.hpp"
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
	ObjectToken();
	ObjectToken(ObjectCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<CounterPointer<ObjectExpression> > const & args);

	void addLabel(std::string const & label);
	void addLabel(std::vector<std::string> const & labels);

	std::vector<CounterPointer<ObjectExpression> > const & getArgs() const;
	CounterPointer<ObjectExpression> getArg(size_t index) const;

	ObjectCode getCode() const;

	std::vector<std::string> const & getLabels() const;
	SourcePosition const & getPosition() const;

	// Swaps args and code.
	void swapData(ObjectToken & token);



	friend bool override_object(ObjectToken * out, ObjectToken const & in);
	friend void read_object(std::istream * in, ObjectToken * out);
	friend void write_object(std::ostream * out, ObjectToken const & in);

private:
	std::vector<CounterPointer<ObjectExpression> > _args;
	ObjectCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;
};

#endif /* HPP_ObjectToken_ */

