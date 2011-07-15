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

/* ost_type.cpp
**
** Defines functions, variables, and enums for dealing with output, source, and
** target types.
*/

#include "ost_type.hpp"

#include "option.hpp"



OutputType output_type(OUTPUT_UNKNOWN);
SourceType source_type(SOURCE_UNKNOWN);
TargetType target_type(TARGET_UNKNOWN);

bool handle_output(char const * name, char const * arg, bool barg, option::option_s * data)
{
	option::option_assert_arg(name, arg);

	std::string args(arg);

	if (args == "ACS0")
		output_type = OUTPUT_ACS0;
	else if (args == "ACSE")
		output_type = OUTPUT_ACSE;
	else
		throw option::option_exception(name, arg, "unknown type");

	return true;
}
bool handle_source(char const * name, char const * arg, bool barg, option::option_s * data)
{
	option::option_assert_arg(name, arg);

	std::string args(arg);

	if (args == "ASMPLX")
		source_type = SOURCE_ASMPLX;
	else if (args == "DS")
		source_type = SOURCE_DS;
	else
		throw option::option_exception(name, arg, "unknown type");

	return true;
}
bool handle_target(char const * name, char const * arg, bool barg, option::option_s * data)
{
	option::option_assert_arg(name, arg);

	std::string args(arg);

	if (args == "hexen")
		target_type = TARGET_HEXEN;
	else if (args == "zdoom")
		target_type = TARGET_ZDOOM;
	else
		throw option::option_exception(name, arg, "unknown type");

	return true;
}

void ost_init()
{
	option::option_add("source", "input", "Format of source file(s).", NULL, handle_source);

	option::option_add("target", "output", "Target game.", NULL, handle_target);
	option::option_add("output", "output", "Output type.", NULL, handle_output);
}



