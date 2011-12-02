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

/* BinaryTokenPPACS/write_ACSP.cpp
**
** Defines the BinaryTokenPPACS::write_ACSP_* functions.
*/

#include "../BinaryTokenPPACS.hpp"

#include "../ACSP.hpp"
#include "../ObjectData.hpp"

#include <sstream>



void BinaryTokenPPACS::write_ACSP_auto(std::ostream * out, ObjectData_Auto const & a)
{
	std::ostringstream token;

	write_ACSP_string(&token, a.name);
	BinaryTokenACS::write_ACS0_32(&token, ACSP_EXPR_LITERAL);
	BinaryTokenACS::write_ACS0_32(&token, a.number);

	BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_SYMBOL);
	BinaryTokenACS::write_ACS0_32(out, token.str().size());
	*out << token.str();
}

void BinaryTokenPPACS::write_ACSP_label(std::ostream * out, std::string const & label)
{
	std::ostringstream token;

	write_ACSP_string(&token, label);

	BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_LABEL);
	BinaryTokenACS::write_ACS0_32(out, token.str().size());
	*out << token.str();
}

void BinaryTokenPPACS::write_ACSP_script(std::ostream * out, ObjectData_Script const & s)
{
	std::ostringstream token;

	write_ACSP_string(&token, s.name);
	BinaryTokenACS::write_ACS0_32(&token, ACSP_EXPR_SYMBOL);
	write_ACSP_string(&token, s.label);
	BinaryTokenACS::write_ACS0_32(&token, s.number);
	BinaryTokenACS::write_ACS0_32(&token, s.stype);
	BinaryTokenACS::write_ACS0_32(&token, s.flags);
	BinaryTokenACS::write_ACS0_32(&token, s.argCount);
	BinaryTokenACS::write_ACS0_32(&token, s.varCount);

	BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_SCRIPT);
	BinaryTokenACS::write_ACS0_32(out, token.str().size());
	*out << token.str();
}

void BinaryTokenPPACS::write_ACSP_static(std::ostream * out, ObjectData_Static const & s)
{
	std::ostringstream token;

	write_ACSP_string(&token, s.name);

	if (s.number == -1)
	{
		BinaryTokenACS::write_ACS0_32(&token, s.size);
		BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_ALLOCATE);
	}
	else
	{
		BinaryTokenACS::write_ACS0_32(&token, ACSP_EXPR_LITERAL);
		BinaryTokenACS::write_ACS0_32(&token, s.number);
		BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_SYMBOL);
	}

	BinaryTokenACS::write_ACS0_32(out, token.str().size());

	*out << token.str();
}

void BinaryTokenPPACS::write_ACSP_string(std::ostream * out, ObjectData_String const & s)
{
	std::ostringstream token;

	write_ACSP_string(&token, s.name);
	write_ACSP_string(&token, s.string);

	BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_STRING);
	BinaryTokenACS::write_ACS0_32(out, token.str().size());
	*out << token.str();
}
void BinaryTokenPPACS::write_ACSP_string(std::ostream * out, std::string const & s)
{
	size_t length = (s.size() + 4) & ~3;
	BinaryTokenACS::write_ACS0_32(out, length);
	*out << s;
	while (length-- > s.size()) *out << '\0';
}


