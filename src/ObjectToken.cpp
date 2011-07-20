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

#include "ObjectExpression.hpp"
#include "print_debug.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"



std::map<std::string, ObjectToken::ObjectCode> ObjectToken::_codes;



ObjectToken::ObjectToken(ObjectCode const code, SourcePosition const & position) : _args(), _code(code), _labels(), _position(position)
{

}
ObjectToken::ObjectToken(ObjectCode const code, SourcePosition const & position, ObjectExpression const & arg) : _args(1, arg), _code(code), _labels(), _position(position)
{

}
ObjectToken::ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<ObjectExpression> const & args) : _args(args), _code(code), _labels(), _position(position)
{

}
ObjectToken::ObjectToken(ObjectCode const code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<ObjectExpression> const & args) : _args(args), _code(code), _labels(labels), _position(position)
{

}

void ObjectToken::addLabel(std::string const & label)
{
	_labels.push_back(label);
}

ObjectToken::ObjectCode ObjectToken::get_code(SourceTokenC const & token)
{
	std::map<std::string, ObjectCode>::iterator codeIt(_codes.find(token.getData()));

	if (codeIt == _codes.end())
		throw SourceException("no such OCODE", token.getPosition(), "ObjectToken");

	return codeIt->second;
}

ObjectExpression const & ObjectToken::getArg(uintptr_t const index) const
{
	static ObjectExpression expr;

	if (index < _args.size())
		return _args[index];
	else
		return expr;
}

ObjectToken::ObjectCode ObjectToken::getCode() const
{
	return _code;
}

std::vector<std::string> const & ObjectToken::getLabels() const
{
	return _labels;
}

SourcePosition const & ObjectToken::getPosition() const
{
	return _position;
}

void ObjectToken::init()
{
	#define DO_INIT(NAME,ARGC)\
	_codes[#NAME] = OCODE_##NAME

	// BinaryTokenACS
	DO_INIT(ADD,             0);
	DO_INIT(ASSIGNSCRIPTVAR, 1);
	DO_INIT(BEGINPRINT,      0);
	DO_INIT(DELAY,           0);
	DO_INIT(DELAY_IMM,       1);
	DO_INIT(DIV,             0);
	DO_INIT(DROP,            0);
	DO_INIT(ENDPRINT,        0);
	DO_INIT(GOTO,            1);
	DO_INIT(LSPEC1,          1);
	DO_INIT(LSPEC1_IMM,      2);
	DO_INIT(LSPEC2,          1);
	DO_INIT(LSPEC2_IMM,      3);
	DO_INIT(LSPEC3,          1);
	DO_INIT(LSPEC3_IMM,      4);
	DO_INIT(LSPEC4,          1);
	DO_INIT(LSPEC4_IMM,      5);
	DO_INIT(LSPEC5,          1);
	DO_INIT(LSPEC5_IMM,      6);
	DO_INIT(MUL,             0);
	DO_INIT(MOD,             0);
	DO_INIT(NOP,             0);
	DO_INIT(PRINTCHARACTER,  0);
	DO_INIT(PRINTNUMBER,     0);
	DO_INIT(PRINTSTRING,     0);
	DO_INIT(PUSHNUMBER,      1);
	DO_INIT(PUSHSCRIPTVAR,   1);
	DO_INIT(RANDOM,          0);
	DO_INIT(RANDOM_IMM,      2);
	DO_INIT(RESTART,         0);
	DO_INIT(SHIFTL,          0);
	DO_INIT(SHIFTR,          0);
	DO_INIT(SUB,             0);
	DO_INIT(SUSPEND,         0);
	DO_INIT(TERMINATE,       0);

	// BinaryTokenZDACS
	DO_INIT(ASSIGNGLOBALARRAY, 1);
	DO_INIT(CALLFUNC,          2);
	DO_INIT(DIVFIXED,          0);
	DO_INIT(DUP,               0);
	DO_INIT(ENDLOG,            0);
	DO_INIT(LSPEC5RESULT,      1);
	DO_INIT(MULFIXED,          0);
	DO_INIT(PRINTFIXED,        0);
	DO_INIT(PUSHGLOBALARRAY,   1);
	DO_INIT(SETRESULTVALUE,    0);
	DO_INIT(STRLEN,            0);

	DO_INIT(NONE, 0);

	#undef DO_INIT
}



void print_debug(std::ostream * const out, ObjectToken const & in)
{
	*out << "ObjectToken(";
		*out << "args=(";
		print_debug(out, in._args);
		*out << ")";

		*out << ", ";

		*out << "code=(";
		print_debug(out, in._code);
		*out << ")";

		*out << ", ";

		*out << "labels=(";
		print_debug(out, in._labels);
		*out << ")";

		*out << ", ";

		*out << "position=(";
		print_debug(out, in._position);
		*out << ")";
	*out << ")";
}
void print_debug(std::ostream * const out, ObjectToken::ObjectCode const in)
{
	switch (in)
	{
	case ObjectToken::OCODE_ADD:               *out << "OCODE_ADD";               break;
	case ObjectToken::OCODE_ASSIGNSCRIPTVAR:   *out << "OCODE_ASSIGNSCRIPTVAR";   break;
	case ObjectToken::OCODE_BEGINPRINT:        *out << "OCODE_BEGINPRINT";        break;
	case ObjectToken::OCODE_DELAY:             *out << "OCODE_DELAY";             break;
	case ObjectToken::OCODE_DELAY_IMM:         *out << "OCODE_DELAY_IMM";         break;
	case ObjectToken::OCODE_DIV:               *out << "OCODE_DIV";               break;
	case ObjectToken::OCODE_DROP:              *out << "OCODE_DROP";              break;
	case ObjectToken::OCODE_ENDPRINT:          *out << "OCODE_NOP";               break;
	case ObjectToken::OCODE_GOTO:              *out << "OCODE_GOTO";              break;
	case ObjectToken::OCODE_LSPEC1:            *out << "OCODE_LSPEC1";            break;
	case ObjectToken::OCODE_LSPEC1_IMM:        *out << "OCODE_LSPEC1_IMM";        break;
	case ObjectToken::OCODE_LSPEC2:            *out << "OCODE_LSPEC2";            break;
	case ObjectToken::OCODE_LSPEC2_IMM:        *out << "OCODE_LSPEC2_IMM";        break;
	case ObjectToken::OCODE_LSPEC3:            *out << "OCODE_LSPEC3";            break;
	case ObjectToken::OCODE_LSPEC3_IMM:        *out << "OCODE_LSPEC3_IMM";        break;
	case ObjectToken::OCODE_LSPEC4:            *out << "OCODE_LSPEC4";            break;
	case ObjectToken::OCODE_LSPEC4_IMM:        *out << "OCODE_LSPEC4_IMM";        break;
	case ObjectToken::OCODE_LSPEC5:            *out << "OCODE_LSPEC5";            break;
	case ObjectToken::OCODE_LSPEC5_IMM:        *out << "OCODE_LSPEC5_IMM";        break;
	case ObjectToken::OCODE_MUL:               *out << "OCODE_MUL";               break;
	case ObjectToken::OCODE_MOD:               *out << "OCODE_MOD";               break;
	case ObjectToken::OCODE_NOP:               *out << "OCODE_NOP";               break;
	case ObjectToken::OCODE_PRINTCHARACTER:    *out << "OCODE_PRINTCHARACTER";    break;
	case ObjectToken::OCODE_PRINTNUMBER:       *out << "OCODE_PRINTNUMBER";       break;
	case ObjectToken::OCODE_PRINTSTRING:       *out << "OCODE_PRINTSTRING";       break;
	case ObjectToken::OCODE_PUSHNUMBER:        *out << "OCODE_PUSHNUMBER";        break;
	case ObjectToken::OCODE_PUSHSCRIPTVAR:     *out << "OCODE_PUSHSCRIPTVAR";     break;
	case ObjectToken::OCODE_RANDOM:            *out << "OCODE_RANDOM";            break;
	case ObjectToken::OCODE_RANDOM_IMM:        *out << "OCODE_RANDOM_IMM";        break;
	case ObjectToken::OCODE_RESTART:           *out << "OCODE_RESTART";           break;
	case ObjectToken::OCODE_SHIFTL:            *out << "OCODE_SHIFTL";            break;
	case ObjectToken::OCODE_SHIFTR:            *out << "OCODE_SHIFTR";            break;
	case ObjectToken::OCODE_SUB:               *out << "OCODE_SUB";               break;
	case ObjectToken::OCODE_SUSPEND:           *out << "OCODE_SUSPEND";           break;
	case ObjectToken::OCODE_TERMINATE:         *out << "OCODE_TERMINATE";         break;

	case ObjectToken::OCODE_ASSIGNGLOBALARRAY: *out << "OCODE_ASSIGNGLOBALARRAY"; break;
	case ObjectToken::OCODE_CALLFUNC:          *out << "OCODE_CALLFUNC";          break;
	case ObjectToken::OCODE_DIVFIXED:          *out << "OCODE_DIVFIXED";          break;
	case ObjectToken::OCODE_DUP:               *out << "OCODE_DUP";               break;
	case ObjectToken::OCODE_ENDLOG:            *out << "OCODE_ENDLOG";            break;
	case ObjectToken::OCODE_LSPEC5RESULT:      *out << "OCODE_LSPEC5RESULT";      break;
	case ObjectToken::OCODE_MULFIXED:          *out << "OCODE_MULFIXED";          break;
	case ObjectToken::OCODE_PRINTFIXED:        *out << "OCODE_PRINTFIXED";        break;
	case ObjectToken::OCODE_PUSHGLOBALARRAY:   *out << "OCODE_PUSHGLOBALARRAY";   break;
	case ObjectToken::OCODE_SETRESULTVALUE:    *out << "OCODE_SETRESULTVALUE";    break;
	case ObjectToken::OCODE_STRLEN:            *out << "OCODE_STRLEN";            break;
	case ObjectToken::OCODE_NONE:              *out << "OCODE_NONE";              break;
	}
}



