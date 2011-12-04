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

/* BinaryTokenACS/output.cpp
**
** Defines the BinaryTokenACS::output_* functions.
*/

#include "../BinaryTokenACS.hpp"

#include "../BinaryTokenPPACS.hpp"
#include "../BinaryTokenZDACS.hpp"
#include "../ObjectExpression.hpp"
#include "../ost_type.hpp"



template<typename T> void BinaryTokenACS::output_ACS0(std::ostream * out, std::vector<T> const & instructions)
{
	// 0
	*out << 'A' << 'C' << 'S' << '\0';
	write_ACS0_32(out, ObjectExpression::get_address_count());

	// 8
	for (typename std::vector<T>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
		instr->writeACS0(out);

	// directoryOffset
	ObjectExpression::iter_script(write_ACS0_script_counter, NULL);
	write_ACS0_script_count(out);

	// directoryOffset+4
	ObjectExpression::iter_script(write_ACS0_script, out);

	// directoryOffset+4+(scriptCount*12)
	ObjectData_String::iterate(write_ACS0_string_counter, NULL);
	write_ACS0_string_count(out);

	// directoryOffset+4+(scriptCount*12)+4
	ObjectData_String::iterate(write_ACS0_string_offset, out);

	// directoryOffset+4+(scriptCount*12)+4+(stringCount*4)
	ObjectData_String::iterate(write_ACS0_string, out);
}
template void BinaryTokenACS::output_ACS0<BinaryTokenACS  >(std::ostream * out, std::vector<BinaryTokenACS  > const & instructions);
template void BinaryTokenACS::output_ACS0<BinaryTokenPPACS>(std::ostream * out, std::vector<BinaryTokenPPACS> const & instructions);
template void BinaryTokenACS::output_ACS0<BinaryTokenZDACS>(std::ostream * out, std::vector<BinaryTokenZDACS> const & instructions);

template<typename T> void BinaryTokenACS::output_prep(std::vector<T> const & instructions)
{
	if (output_type == OUTPUT_ACSP) return;

	ObjectExpression::set_address_count(8);

	for (typename std::vector<T>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
	{
		for (typename T::label_iterator label(instr->label_begin()); label != instr->label_end(); ++label)
			ObjectExpression::add_label(*label);

		ObjectExpression::add_address_count(instr->size());
	}
}
template void BinaryTokenACS::output_prep<BinaryTokenACS  >(std::vector<BinaryTokenACS  > const & instructions);
template void BinaryTokenACS::output_prep<BinaryTokenPPACS>(std::vector<BinaryTokenPPACS> const & instructions);
template void BinaryTokenACS::output_prep<BinaryTokenZDACS>(std::vector<BinaryTokenZDACS> const & instructions);


