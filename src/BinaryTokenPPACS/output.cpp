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

/* BinaryTokenPPACS/output.cpp
**
** Defines the BinaryTokenPPACS::output_* functions.
*/

#include "../BinaryTokenPPACS.hpp"

#include "../ACSP.hpp"
#include "../ObjectExpression.hpp"



template<typename T> void BinaryTokenPPACS::output_ACSP(std::ostream * out, std::vector<T> const & instructions)
{
	*out << 'A' << 'C' << 'S' << '+';

	for (typename std::vector<T>::const_iterator instr(instructions.begin()); instr != instructions.end(); ++instr)
		instr->writeACSP(out);

	ObjectExpression ::iter_auto  (write_ACSP_auto,   out);
	ObjectExpression ::iter_script(write_ACSP_script, out);
	ObjectExpression ::iter_static(write_ACSP_static, out);
	ObjectData_String::iterate    (write_ACSP_string, out);

	BinaryTokenACS::write_ACS0_32(out, ACSP_TOKEN_EOF);
	BinaryTokenACS::write_ACS0_32(out, 0);
}
template void BinaryTokenPPACS::output_ACSP<BinaryTokenPPACS>(std::ostream * out, std::vector<BinaryTokenPPACS> const & instructions);


