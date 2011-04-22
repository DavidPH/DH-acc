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

/* BinaryTokenZDACS.cpp
**
** BinaryTokenZDACS methods.
*/

#include "BinaryTokenZDACS.hpp"

#include "ObjectExpression.hpp"
#include "SourceException.hpp"



uintptr_t BinaryTokenZDACS::_arg_counts[BCODE_NONE];



BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression> const & args) : _args(args), _code(code), _labels(labels), _position(position)
{

}

void BinaryTokenZDACS::init()
{
	#define DO_INIT(NAME,ARGC)\
	_arg_counts[BCODE_##NAME] = ARGC

	// BinaryTokenACS
	DO_INIT(ADD,             0);
	DO_INIT(ASSIGNSCRIPTVAR, 1);
	DO_INIT(BEGINPRINT,      0);
	DO_INIT(DELAY,           0);
	DO_INIT(DELAYDIRECT,     1);
	DO_INIT(DROP,            0);
	DO_INIT(ENDPRINT,        0);
	DO_INIT(GOTO,            1);
	DO_INIT(LSPEC1,          1);
	DO_INIT(LSPEC1DIRECT,    2);
	DO_INIT(LSPEC2,          1);
	DO_INIT(LSPEC2DIRECT,    3);
	DO_INIT(LSPEC3,          1);
	DO_INIT(LSPEC3DIRECT,    4);
	DO_INIT(LSPEC4,          1);
	DO_INIT(LSPEC4DIRECT,    5);
	DO_INIT(LSPEC5,          1);
	DO_INIT(LSPEC5DIRECT,    6);
	DO_INIT(NOP,             0);
	DO_INIT(PRINTCHARACTER,  0);
	DO_INIT(PRINTNUMBER,     0);
	DO_INIT(PRINTSTRING,     0);
	DO_INIT(PUSHNUMBER,      1);
	DO_INIT(PUSHSCRIPTVAR,   1);
	DO_INIT(RESTART,         0);
	DO_INIT(SUSPEND,         0);
	DO_INIT(TERMINATE,       0);

	// BinaryTokenZDACS
	DO_INIT(ASSIGNGLOBALARRAY, 1);
	DO_INIT(DUP,               0);
	DO_INIT(ENDLOG,            0);
	DO_INIT(PRINTFIXED,        0);
	DO_INIT(PUSHGLOBALARRAY,   1);

	#undef DO_INIT
}

void BinaryTokenZDACS::make_tokens(std::vector<ObjectToken> const & objects, std::vector<BinaryTokenZDACS> * const instructions)
{
	#define CASE_DIRECTMAP(NAME)\
	case ObjectToken::OCODE_##NAME:\
	{\
		std::vector<ObjectExpression> args;\
		for (uintptr_t i(0); i < _arg_counts[BCODE_##NAME]; ++i)\
			args.push_back(objects[index].getArg(i));\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, objects[index].getPosition(), objects[index].getLabels(), args));\
	}\
	break

	for (uintptr_t index(0); index < objects.size(); ++index) switch (objects[index].getCode())
	{
	CASE_DIRECTMAP(ADD);
	CASE_DIRECTMAP(ASSIGNSCRIPTVAR);
	CASE_DIRECTMAP(BEGINPRINT);
	CASE_DIRECTMAP(DELAY);
	CASE_DIRECTMAP(DELAYDIRECT);
	CASE_DIRECTMAP(DROP);
	CASE_DIRECTMAP(ENDPRINT);
	CASE_DIRECTMAP(GOTO);
	CASE_DIRECTMAP(LSPEC1);
	CASE_DIRECTMAP(LSPEC1DIRECT);
	CASE_DIRECTMAP(LSPEC2);
	CASE_DIRECTMAP(LSPEC2DIRECT);
	CASE_DIRECTMAP(LSPEC3);
	CASE_DIRECTMAP(LSPEC3DIRECT);
	CASE_DIRECTMAP(LSPEC4);
	CASE_DIRECTMAP(LSPEC4DIRECT);
	CASE_DIRECTMAP(LSPEC5);
	CASE_DIRECTMAP(LSPEC5DIRECT);
	CASE_DIRECTMAP(NOP);
	CASE_DIRECTMAP(PRINTCHARACTER);
	CASE_DIRECTMAP(PRINTNUMBER);
	CASE_DIRECTMAP(PRINTSTRING);
	CASE_DIRECTMAP(PUSHNUMBER);
	CASE_DIRECTMAP(PUSHSCRIPTVAR);
	CASE_DIRECTMAP(RESTART);
	CASE_DIRECTMAP(SUSPEND);
	CASE_DIRECTMAP(TERMINATE);

	CASE_DIRECTMAP(ASSIGNGLOBALARRAY);
	CASE_DIRECTMAP(DUP);
	CASE_DIRECTMAP(ENDLOG);
	CASE_DIRECTMAP(PRINTFIXED);
	CASE_DIRECTMAP(PUSHGLOBALARRAY);

	case ObjectToken::OCODE_NONE:
		throw SourceException("unknown OCODE", objects[index].getPosition(), "BinaryTokenZDACS");
	}

	#undef CASE_DIRECTMAP
}

void BinaryTokenZDACS::write(std::ostream * const out) const
{
	write_32(out, _code);

	for (uintptr_t i(0); i < _arg_counts[_code]; ++i)
		write_32(out, i < _args.size() ? _args[i].resolveInt32() : 0);
}

void BinaryTokenZDACS::write_32(std::ostream * const out, uint32_t const i)
{
	out->put((i >>  0) & 0xFF);
	out->put((i >>  8) & 0xFF);
	out->put((i >> 16) & 0xFF);
	out->put((i >> 24) & 0xFF);
}

void BinaryTokenZDACS::write_all(std::ostream * const out, std::vector<BinaryTokenZDACS> const & instructions)
{
	for (uintptr_t index(0); index < instructions.size(); ++index)
	{
		for (uintptr_t i(0); i < instructions[index]._labels.size(); ++i)
			ObjectExpression::add_label(instructions[index]._labels[i]);

		ObjectExpression::add_address_count(_arg_counts[instructions[index]._code]*4 + 4);
	}

	for (uintptr_t index(0); index < instructions.size(); ++index)
	{
		instructions[index].write(out);
	}
}

void BinaryTokenZDACS::write_string(std::ostream * const out, std::string const & s)
{
	*out << s;
}



