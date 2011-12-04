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

/* BinaryTokenZDACS/output.cpp
**
** Defines the BinaryTokenZDACS::output_* functions.
*/

#include "../BinaryTokenZDACS.hpp"

#include "../ObjectExpression.hpp"

#include <sstream>



template<typename T> void BinaryTokenZDACS::output_ACSE(std::ostream * out, std::vector<T> const & instructions)
{
	std::ostringstream chunkout;

	// Header
	*out << 'A' << 'C' << 'S' << 'E';
	BinaryTokenACS::write_ACS0_32(out, ObjectExpression::get_address_count());

	// Instructions
	for (typename std::vector<T>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
		instr->writeACS0(out);

	// AIMP - Map Array Imports
	_temp_counter = 0;
	ObjectExpression::iter_registerarray_map(write_ACSE_registerarray_AIMP_counter, &chunkout);

	if (_temp_counter)
		BinaryTokenACS::write_ACS0_32(&chunkout, _temp_counter);

	ObjectExpression::iter_registerarray_map(write_ACSE_registerarray_AIMP, &chunkout);
	write_ACSE_chunk(out, &chunkout, "AIMP");

	// ARAY - Map Array Declarations
	ObjectExpression::iter_registerarray_map(write_ACSE_registerarray_ARAY, &chunkout);
	write_ACSE_chunk(out, &chunkout, "ARAY");

	// FNAM - Function Names
	ObjectExpression::iter_function(write_ACSE_function_FNAM, NULL);
	write_ACSE_stringtable(&chunkout, false);
	write_ACSE_chunk(out, &chunkout, "FNAM");

	// FUNC - Functions
	ObjectExpression::iter_function(write_ACSE_function_FUNC, &chunkout);
	write_ACSE_chunk(out, &chunkout, "FUNC");

	// LOAD - Load Libraries
	ObjectExpression::iter_library(write_ACSE_library, &chunkout);
	write_ACSE_chunk(out, &chunkout, "LOAD");

	// MEXP - Map Register Exports
	ObjectExpression::iter_register_map(write_ACSE_register_MEXP, NULL);
	ObjectExpression::iter_registerarray_map(write_ACSE_registerarray_MEXP, NULL);
	write_ACSE_stringtable(&chunkout, false);
	write_ACSE_chunk(out, &chunkout, "MEXP");

	// MIMP - Map Register Imports
	ObjectExpression::iter_register_map(write_ACSE_register_MIMP, &chunkout);
	write_ACSE_chunk(out, &chunkout, "MIMP");

	// SPTR - Script Pointers
	ObjectExpression::iter_script(write_ACSE_script, &chunkout);
	write_ACSE_chunk(out, &chunkout, "SPTR");

	// SFLG - Script Flags
	ObjectExpression::iter_script(write_ACSE_script_flags, &chunkout);
	write_ACSE_chunk(out, &chunkout, "SFLG");

	// STRL - String Literals
	ObjectData_String::iterate(write_ACSE_string_STRL, NULL);
	write_ACSE_stringtable(&chunkout, true);
	write_ACSE_chunk(out, &chunkout, "STRL");

	// SVCT - Script Variable Counts
	ObjectExpression::iter_script(write_ACSE_script_vars, &chunkout);
	write_ACSE_chunk(out, &chunkout, "SVCT");
}
template void BinaryTokenZDACS::output_ACSE<BinaryTokenZDACS>(std::ostream * out, std::vector<BinaryTokenZDACS> const & instructions);


