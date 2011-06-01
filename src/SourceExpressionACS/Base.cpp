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

/* SourceExpressionACS/Base.cpp
**
** SourceExpressionACSBase methods.
*/

#include "../SourceExpressionACS.hpp"



SourceExpressionACSBase::SourceExpressionACSBase(SourcePosition const & position) : _position(position)
{

}
SourceExpressionACSBase::~SourceExpressionACSBase()
{

}

SourceExpressionACSBase::ExpressionType SourceExpressionACSBase::get_promoted_type(ExpressionType const type1, ExpressionType const type2)
{
	if (type1 == type2) return type1;

	if (type1 == ET_FIXED || type2 == ET_FIXED) return ET_FIXED;

	return ET_INT;
}

SourcePosition const & SourceExpressionACSBase::getPosition() const
{
	return _position;
}



void print_debug(std::ostream * const out, SourceExpressionACSBase::ExpressionType const type)
{
	switch (type)
	{
	case SourceExpressionACSBase::ET_FIXED: *out << "ET_FIXED"; break;
	case SourceExpressionACSBase::ET_INT:   *out << "ET_INT"; break;
	default: *out << "ET_"; break;
	}
}



