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

/* ObjectVector.cpp
**
** Defines the ObjectVector methods.
*/

#include "ObjectVector.hpp"

#include "ObjectExpression.hpp"
#include "object_io.hpp"
#include "ObjectToken.hpp"
#include "option.hpp"



option::option_b option_opt_nop(false);
option::option_b option_opt_pushdrop(true);

static struct _ObjectVector_init_s {_ObjectVector_init_s();} _ObjectVector_init;

_ObjectVector_init_s::_ObjectVector_init_s()
{
	option::option_add("opt-nop", "optimization", "Strips NOP instructions.", &option_opt_nop, option::option_handler_default_b);
	option::option_add("opt-pushdrop", "optimization", "Strips PUSH/DROP pairs. On by default.", &option_opt_pushdrop, option::option_handler_default_b);
}



void ObjectVector::addLabel(std::string const & label)
{
	_labels.push_back(label);
}
void ObjectVector::addLabel(std::vector<std::string> const & labels)
{
	for (size_t i(0); i < labels.size(); ++i)
		addLabel(labels[i]);
}

void ObjectVector::addToken(ObjectToken const & token)
{
	_tokens.push_back(token);
}
void ObjectVector::addToken(ObjectCode code)
{
	addToken(code, std::vector<ObjectExpression::Pointer>());
}
void ObjectVector::addToken(ObjectCode code, std::vector<ObjectExpression::Pointer> const & args)
{
	_tokens.push_back(ObjectToken(code, _position, _labels, args));
	_labels.clear();
}
void ObjectVector::addToken(ObjectCode code, ObjectExpression * arg0)
{
	std::vector<ObjectExpression::Pointer> args;

	args.push_back(arg0);

	addToken(code, args);
}
void ObjectVector::addToken(ObjectCode code, ObjectExpression * arg0, ObjectExpression * arg1)
{
	std::vector<ObjectExpression::Pointer> args;

	args.push_back(arg0);
	args.push_back(arg1);

	addToken(code, args);
}
void ObjectVector::addTokenPushZero()
{
	addToken(OCODE_PUSHNUMBER, getValue(0));
}

ObjectExpression::Pointer ObjectVector::getValue(bigreal f) const
{
	return ObjectExpression::create_value_float(f, _position);
}
ObjectExpression::Pointer ObjectVector::getValue(bigsint i) const
{
	return ObjectExpression::create_value_int(i, _position);
}
ObjectExpression::Pointer ObjectVector::getValue(double f) const
{
	return getValue((bigreal)f);
}
ObjectExpression::Pointer ObjectVector::getValue(int i) const
{
	return getValue((bigsint)i);
}
ObjectExpression::Pointer ObjectVector::getValue(ObjectExpression * expr) const
{
	return expr;
}
ObjectExpression::Pointer ObjectVector::getValue(std::string const & label) const
{
	return ObjectExpression::create_value_symbol(label, _position);
}
ObjectExpression::Pointer ObjectVector::getValue(unsigned int i) const
{
	return getValue((bigsint)i);
}

CounterPointer<ObjectExpression> ObjectVector::getValueAdd(ObjectExpression * exprL, ObjectExpression * exprR) const
{
	return ObjectExpression::create_binary_add(exprL, exprR, _position);
}

ObjectToken const & ObjectVector::operator [] (bigsint index) const
{
	return _tokens[(size_t)index];
}

void ObjectVector::optimize()
{
	// NOP removal.
	// Off by default because NOPs do not normally get generated.
	if (option_opt_pushdrop) for(size_t i(0); i+1 < _tokens.size();)
	{
		if (_tokens[i].getCode() == OCODE_NOP)
		{
			std::vector<std::string> labels(_tokens[i].getLabels());

			_tokens[i+1].addLabel(labels);

			for (size_t j(i+1); j < _tokens.size(); ++j)
				_tokens[j-1] = _tokens[j];

			_tokens.resize(_tokens.size() - 1);
		}
		else
		{
			++i;
		}
	}

	// PUSH/DROP removal.
	if (option_opt_pushdrop) for(size_t i(0); i+2 < _tokens.size();)
	{
		if (ocode_is_push(_tokens[i].getCode()) && _tokens[i+1].getCode() == OCODE_DROP)
		{
			std::vector<std::string> labels0(_tokens[i+0].getLabels());
			std::vector<std::string> labels1(_tokens[i+1].getLabels());
			for (size_t j(0); j < labels1.size(); ++j)
				labels0.push_back(labels1[j]);

			_tokens[i+2].addLabel(labels0);

			for (size_t j(i+2); j < _tokens.size(); ++j)
				_tokens[j-2] = _tokens[j];

			_tokens.resize(_tokens.size() - 2);
		}
		else
		{
			++i;
		}
	}
}

bigsint ObjectVector::size() const
{
	return (bigsint)_tokens.size();
}

ObjectVector & ObjectVector::setPosition(SourcePosition const & position)
{
	_position = position;

	return *this;
}



void read_object(std::istream * in, ObjectVector * out)
{
	read_object(in, &out->_position);
	read_object(in, &out->_labels);
	read_object(in, &out->_tokens);
}

void write_object(std::ostream * out, ObjectVector const & in)
{
	write_object(out, in._position);
	write_object(out, in._labels);
	write_object(out, in._tokens);
}


