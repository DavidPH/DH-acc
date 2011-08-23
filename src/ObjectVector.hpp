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

#include "bignum.hpp"
#include "CounterPointer.hpp"
#include "ObjectCode.hpp"
#include "SourcePosition.hpp"

#include <string>
#include <vector>

class ObjectExpression;
class ObjectToken;



class ObjectVector
{
public:
	void addLabel(std::string const & label);
	void addLabel(std::vector<std::string> const & labels);

	void addToken(ObjectToken const & token);
	void addToken(ObjectCode code);
	void addToken(ObjectCode code, std::vector<CounterPointer<ObjectExpression> > const & args);
	void addToken(ObjectCode code, ObjectExpression * arg0);
	void addToken(ObjectCode code, ObjectExpression * arg0, ObjectExpression * arg1);
	void addTokenPushZero();

	CounterPointer<ObjectExpression> getValue(bigreal f) const;
	CounterPointer<ObjectExpression> getValue(bigsint i) const;
	CounterPointer<ObjectExpression> getValue(double f) const;
	CounterPointer<ObjectExpression> getValue(int i) const;
	CounterPointer<ObjectExpression> getValue(ObjectExpression * expr) const;
	CounterPointer<ObjectExpression> getValue(std::string const & label) const;
	CounterPointer<ObjectExpression> getValue(unsigned int i) const;

	CounterPointer<ObjectExpression> getValueAdd(ObjectExpression * exprL, ObjectExpression * exprR) const;
	template<typename T1, typename T2> CounterPointer<ObjectExpression> getValueAdd(T1 const & l, T2 const & r) const;

	ObjectToken const & operator [] (bigsint index) const;

	void optimize();

	ObjectVector & setPosition(SourcePosition const & position);

	bigsint size() const;



	friend void read_object(std::istream * in, ObjectVector * out);
	friend void write_object(std::ostream * out, ObjectVector const & in);

private:
	SourcePosition _position;

	std::vector<std::string> _labels;
	std::vector<ObjectToken> _tokens;
};



template<typename T1, typename T2> inline CounterPointer<ObjectExpression> ObjectVector::getValueAdd(T1 const & l, T2 const & r) const
{
	return getValueAdd((ObjectExpression *)getValue(l), (ObjectExpression *)getValue(r));
}



#endif /* HPP_ObjectVector_ */



