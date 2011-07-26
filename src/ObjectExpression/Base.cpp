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

/* ObjectExpression/Base.cpp
**
** Defines the ObjectExpression_Base methods.
*/

#include "Base.hpp"



ObjectExpression_Base::ObjectExpression_Base(SourcePosition const & position) : _position(position)
{

}
ObjectExpression_Base::~ObjectExpression_Base()
{

}

SourcePosition const & ObjectExpression_Base::getPosition() const
{
	return _position;
}

void ObjectExpression_Base::printDebug(std::ostream * out) const
{
	*out << "ObjectExpression_Base(";
		*out << "position=(";
		print_debug(out, _position);
		*out << ")";
	*out << ")";
}


