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

/* BinaryTokenACS/make_tokens.cpp
**
** Defines the BinaryTokenACS::make_tokens functions.
*/

#include "../BinaryTokenACS.hpp"
#include "make_tokens.hpp"

#include "../ObjectExpression.hpp"
#include "../ObjectToken.hpp"
#include "../ObjectVector.hpp"
#include "../SourceException.hpp"



#define TOKEN_CLASS BinaryTokenACS



void BinaryTokenACS::make_tokens(ObjectToken const & object, std::vector<BinaryTokenACS> * instructions)
{
	static ObjectExpression::Pointer const fracbits(ObjectExpression::create_value_int(16, SourcePosition::builtin()));

	static std::vector<std::string> const nolabels;

	std::vector<ObjectExpression::Pointer> args;

	SourcePosition const & position(object.getPosition());

	std::vector<std::string> const * labels(&object.getLabels());

	switch (object.getCode())
	{
	// Direct Mappings
	BINTOKACS_TOKENS_MAP_ALL_ALL();

	// Translations
	BINTOKACS_TOKENS_TRAN_ALL_ALL();

	case OCODE_NONE:
	default:
		throw SourceException("unknown OCODE: " + (std::string)make_string(object.getCode()), position, "BinaryTokenACS");
	}
}
void BinaryTokenACS::make_tokens(ObjectVector const & objects, std::vector<BinaryTokenACS> * instructions)
{
	for (bigsint index(0); index < objects.size(); ++index)
		make_tokens(objects[index], instructions);
}


