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

#include "../print_debug.hpp"



SourceExpressionDS_Base::SourceExpressionDS_Base(SourcePosition const & position) : _position(position)
{

}
SourceExpressionDS_Base::~SourceExpressionDS_Base()
{

}

std::vector<std::string> const & SourceExpressionDS_Base::getLabels() const
{
	return _labels;
}

SourcePosition const & SourceExpressionDS_Base::getPosition() const
{
	return _position;
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



