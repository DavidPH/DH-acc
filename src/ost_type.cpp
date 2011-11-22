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

option_auto<OutputType> option_output_type("output", "input", "Format of source file(s).", &output_type);
option_auto<SourceType> option_source_type("source", "output", "Target game.", &source_type);
option_auto<TargetType> option_target_type("target", "output", "Output type.", &target_type);



template<> OutputType option_auto<OutputType>::parse(std::string const & name, std::string const & arg)
{
	if (arg == "ACS0")
		return OUTPUT_ACS0;
	else if (arg == "ACSE")
		return OUTPUT_ACSE;
	else if (arg == "ACS+")
		return OUTPUT_ACSP;
	else if (arg == "object")
		return OUTPUT_object;
	else
		throw exception(name, arg, "unknown type");
}
template<> SourceType option_auto<SourceType>::parse(std::string const & name, std::string const & arg)
{
	if (arg == "ASMPLX")
		return SOURCE_ASMPLX;
	else if (arg == "DS")
		return SOURCE_DS;
	else if (arg == "object")
		return SOURCE_object;
	else
		throw exception(name, arg, "unknown type");
}
template<> TargetType option_auto<TargetType>::parse(std::string const & name, std::string const & arg)
{
	if (arg == "Hexen")
		return TARGET_Hexen;
	else if (arg == "HexPP")
		return TARGET_HexPP;
	else if (arg == "ZDoom")
		return TARGET_ZDoom;
	else
		throw exception(name, arg, "unknown type");
}

template<> bool option_auto<OutputType>::handler_default(std::string const & name, std::string const & arg, bool barg, OutputType * data)
{
	*data = parse(name, arg);

	return true;
}
template<> bool option_auto<SourceType>::handler_default(std::string const & name, std::string const & arg, bool barg, SourceType * data)
{
	*data = parse(name, arg);

	return true;
}
template<> bool option_auto<TargetType>::handler_default(std::string const & name, std::string const & arg, bool barg, TargetType * data)
{
	*data = parse(name, arg);

	return true;
}


