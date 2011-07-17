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

/* SourceExpressionDS/Base.cpp
**
** Defines the SourceExpressionDS_Base methods.
*/

#include "Base.hpp"

#include "../ObjectExpression.hpp"
#include "../print_debug.hpp"
#include "../SourceException.hpp"



SourceExpressionDS_Base::SourceExpressionDS_Base(SourcePosition const & position) : _position(position)
{

}
SourceExpressionDS_Base::SourceExpressionDS_Base(std::vector<std::string> const & labels, SourcePosition const & position) : _labels(labels), _position(position)
{

}
SourceExpressionDS_Base::~SourceExpressionDS_Base()
{

}

void SourceExpressionDS_Base::addLabel(std::string const & label)
{
	_labels.push_back(label);
}

std::vector<std::string> const & SourceExpressionDS_Base::getLabels() const
{
	return _labels;
}

SourcePosition const & SourceExpressionDS_Base::getPosition() const
{
	return _position;
}

std::vector<SourceExpressionDS> SourceExpressionDS_Base::getVector() const
{
	throw SourceException("attempted to getVector on invalid expression", _position, getName());
}

ObjectExpression SourceExpressionDS_Base::makeObject() const
{
	throw SourceException("attempted to makeObject on invalid expression", _position, getName());
}
void SourceExpressionDS_Base::makeObjectsCall(std::vector<ObjectToken> * const objects, std::vector<SourceExpressionDS> const & args) const
{
	throw SourceException("attempted to makeObjectsCall on invalid expression", _position, getName());
}
void SourceExpressionDS_Base::makeObjectsGet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names) const
{
	if (names->empty())
	{
		makeObjectsGet(objects);
		return;
	}

	throw SourceException("attempted to makeObjectsGet-member on invalid expression", _position, getName());
}
void SourceExpressionDS_Base::makeObjectsSet(std::vector<ObjectToken> * const objects) const
{
	throw SourceException("attempted to makeObjectsSet on invalid expression", _position, getName());
}
void SourceExpressionDS_Base::makeObjectsSet(std::vector<ObjectToken> * const objects, std::vector<std::string> * const names) const
{
	if (names->empty())
	{
		makeObjectsSet(objects);
		return;
	}

	throw SourceException("attempted to makeObjectsSet-member on invalid expression", _position, getName());
}

void SourceExpressionDS_Base::printDebug(std::ostream * const out) const
{
	*out << "SourceExpressionDS_Base(";
		*out << "labels=(";
		print_debug(out, _labels);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, _position);
		*out << ")";
	*out << ")";
}



