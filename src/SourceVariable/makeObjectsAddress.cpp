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

/* SourceVariable/makeObjectsAddress.cpp
**
** Defines the SourceVariable::makeObjectsAddress method.
*/

#include "../SourceVariable.hpp"

#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



void SourceVariable::makeObjectsAddress(ObjectVector * objects, SourcePosition const & position) const
{
	switch (_sc)
	{
	case SC_AUTO:
		objects->addToken(ObjectToken::OCODE_PUSHSTACKADDRESS, objects->getValue(_nameObject));
		break;

	case SC_CONSTANT:
		throw SourceException("makeObjectsAddress on SC_CONSTANT", position, "SourceVariable");

	case SC_REGISTER:
	case SC_REGISTER_GLOBAL:
	case SC_REGISTER_MAP:
	case SC_REGISTER_WORLD:
	case SC_REGISTERARRAY_GLOBAL:
	case SC_REGISTERARRAY_MAP:
	case SC_REGISTERARRAY_WORLD:
		throw SourceException("makeObjectsAddress on register", position, "SourceVariable");
	}
}



