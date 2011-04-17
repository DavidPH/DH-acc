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

#include "ObjectToken.hpp"
#include "SourceException.hpp"



int BinaryTokenZDACS::_arg_counts[BCODE_NONE];



BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position) : _code(code), _position(position)
{

}
BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0) : _code(code), _position(position)
{
	_args[0] = arg0;
}
BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1) : _code(code), _position(position)
{
	_args[0] = arg0;
	_args[1] = arg1;
}
BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2) : _code(code), _position(position)
{
	_args[0] = arg0;
	_args[1] = arg1;
	_args[2] = arg2;
}
BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2, int32_t const arg3) : _code(code), _position(position)
{
	_args[0] = arg0;
	_args[1] = arg1;
	_args[2] = arg2;
	_args[3] = arg3;
}
BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2, int32_t const arg3, int32_t const arg4) : _code(code), _position(position)
{
	_args[0] = arg0;
	_args[1] = arg1;
	_args[2] = arg2;
	_args[3] = arg3;
	_args[4] = arg4;
}
BinaryTokenZDACS::BinaryTokenZDACS(BinaryCode const code, SourcePosition const & position, int32_t const arg0, int32_t const arg1, int32_t const arg2, int32_t const arg3, int32_t const arg4, int32_t const arg5) : _code(code), _position(position)
{
	_args[0] = arg0;
	_args[1] = arg1;
	_args[2] = arg2;
	_args[3] = arg3;
	_args[4] = arg4;
	_args[5] = arg5;
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
	DO_INIT(PRINTNUMBER,     0);
	DO_INIT(PUSHNUMBER,      1);
	DO_INIT(PUSHSCRIPTVAR,   1);
	DO_INIT(RESTART,         0);
	DO_INIT(SUSPEND,         0);
	DO_INIT(TERMINATE,       0);

	// BinaryTokenZDACS
	DO_INIT(ASSIGNGLOBALARRAY, 1);
	DO_INIT(DUP,               0);
	DO_INIT(ENDLOG,            0);
	DO_INIT(PUSHGLOBALARRAY,   1);

	#undef DO_INIT
}

void BinaryTokenZDACS::make_tokens(std::vector<ObjectToken> const & objects, std::vector<BinaryTokenZDACS> * const instructions)
{
	#define CASE_DIRECTMAP0(NAME)\
	case ObjectToken::OCODE_##NAME:\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, object->getPosition()));\
		break

	#define CASE_DIRECTMAP1(NAME)\
	case ObjectToken::OCODE_##NAME:\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, object->getPosition(), object->getArgInt32(0)));\
		break

	#define CASE_DIRECTMAP2(NAME)\
	case ObjectToken::OCODE_##NAME:\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, object->getPosition(), object->getArgInt32(0), object->getArgInt32(1)));\
		break

	#define CASE_DIRECTMAP3(NAME)\
	case ObjectToken::OCODE_##NAME:\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, object->getPosition(), object->getArgInt32(0), object->getArgInt32(1), object->getArgInt32(2)));\
		break

	#define CASE_DIRECTMAP4(NAME)\
	case ObjectToken::OCODE_##NAME:\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, object->getPosition(), object->getArgInt32(0), object->getArgInt32(1), object->getArgInt32(2), object->getArgInt32(3)));\
		break

	#define CASE_DIRECTMAP5(NAME)\
	case ObjectToken::OCODE_##NAME:\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, object->getPosition(), object->getArgInt32(0), object->getArgInt32(1), object->getArgInt32(2), object->getArgInt32(3), object->getArgInt32(4)));\
		break

	#define CASE_DIRECTMAP6(NAME)\
	case ObjectToken::OCODE_##NAME:\
		instructions->push_back(BinaryTokenZDACS(BCODE_##NAME, object->getPosition(), object->getArgInt32(0), object->getArgInt32(1), object->getArgInt32(2), object->getArgInt32(3), object->getArgInt32(4), object->getArgInt32(5)));\
		break

	for (std::vector<ObjectToken>::const_iterator object(objects.begin()); object != objects.end(); ++object)
	{
		switch (object->getCode())
		{
		CASE_DIRECTMAP0(ADD);
		CASE_DIRECTMAP1(ASSIGNSCRIPTVAR);
		CASE_DIRECTMAP0(BEGINPRINT);
		CASE_DIRECTMAP0(DELAY);
		CASE_DIRECTMAP1(DELAYDIRECT);
		CASE_DIRECTMAP0(DROP);
		CASE_DIRECTMAP0(ENDPRINT);
		CASE_DIRECTMAP1(LSPEC1);
		CASE_DIRECTMAP2(LSPEC1DIRECT);
		CASE_DIRECTMAP1(LSPEC2);
		CASE_DIRECTMAP3(LSPEC2DIRECT);
		CASE_DIRECTMAP1(LSPEC3);
		CASE_DIRECTMAP4(LSPEC3DIRECT);
		CASE_DIRECTMAP1(LSPEC4);
		CASE_DIRECTMAP5(LSPEC4DIRECT);
		CASE_DIRECTMAP1(LSPEC5);
		CASE_DIRECTMAP6(LSPEC5DIRECT);
		CASE_DIRECTMAP0(NOP);
		CASE_DIRECTMAP0(PRINTNUMBER);
		CASE_DIRECTMAP1(PUSHNUMBER);
		CASE_DIRECTMAP1(PUSHSCRIPTVAR);
		CASE_DIRECTMAP0(RESTART);
		CASE_DIRECTMAP0(SUSPEND);
		CASE_DIRECTMAP0(TERMINATE);

		CASE_DIRECTMAP1(ASSIGNGLOBALARRAY);
		CASE_DIRECTMAP0(DUP);
		CASE_DIRECTMAP0(ENDLOG);
		CASE_DIRECTMAP1(PUSHGLOBALARRAY);

		case ObjectToken::OCODE_NONE:
			throw SourceException("unknown OCODE", object->getPosition(), "BinaryTokenZDACS");
		}
	}

	#undef CASE_DIRECTMAP6
	#undef CASE_DIRECTMAP5
	#undef CASE_DIRECTMAP4
	#undef CASE_DIRECTMAP3
	#undef CASE_DIRECTMAP2
	#undef CASE_DIRECTMAP1
	#undef CASE_DIRECTMAP0
}

void BinaryTokenZDACS::write(std::ostream * const out) const
{
	write_32(out, _code);

	for (int i(0); i < _arg_counts[_code]; ++i)
		write_32(out, _args[i]);
}

void BinaryTokenZDACS::write_32(std::ostream * const out, uint32_t const i)
{
	out->put((i >>  0) & 0xFF);
	out->put((i >>  8) & 0xFF);
	out->put((i >> 16) & 0xFF);
	out->put((i >> 24) & 0xFF);
}



