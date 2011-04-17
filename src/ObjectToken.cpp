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

/* ObjectToken.cpp
**
** ObjectToken methods.
*/

#include "ObjectToken.hpp"



ObjectToken::ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<int32_t> const & args) : _args(args), _code(code), _position(position)
{

}

int32_t ObjectToken::getArgInt32(uintptr_t const index) const
{
	if (index < _args.size())
		return _args[index];
	else
		return 0;
}

ObjectToken::ObjectCode ObjectToken::getCode() const
{
	return _code;
}

SourcePosition const & ObjectToken::getPosition() const
{
	return _position;
}



