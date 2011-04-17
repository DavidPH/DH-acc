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

/* main.cpp
**
** Program start point.
*/

#include "BinaryTokenZDACS.hpp"
#include "ObjectToken.hpp"
#include "SourceException.hpp"
#include "SourceStream.hpp"
#include "SourceTokenASMPLX.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>



static inline int _main(int const argc, char const * const * const argv)
{
	if (argc != 3) return 1;

	std::ifstream ifs(argv[1]);
	std::ofstream ofs(argv[2]);

	SourceStream in(&ifs, argv[1], SourceStream::ST_ASMPLX);

	std::vector<SourceTokenASMPLX> tokens;

	SourceTokenASMPLX::read_tokens(&in, &tokens);

	std::vector<ObjectToken> objects;

	for (std::vector<SourceTokenASMPLX>::iterator it(tokens.begin()); it != tokens.end(); ++it)
		it->initObject();
	for (std::vector<SourceTokenASMPLX>::iterator it(tokens.begin()); it != tokens.end(); ++it)
		it->makeObject(&objects);

	std::vector<BinaryTokenZDACS> instructions;

	BinaryTokenZDACS::make_tokens(objects, &instructions);

	#if 0
	// 0
	ofs << 'A' << 'C' << 'S' << 'E';
	ofs << '\x08' << '\x00' << '\x00' << '\x00';

	// 8
	ofs << 'S' << 'P' << 'T' << 'R';
	ofs << '\x0C' << '\x00' << '\x00' << '\x00';

	// 16
	ofs << '\x08' << '\x00';
	ofs << '\x04' << '\x00';
	ofs << '\x1C' << '\x00' << '\x00' << '\x00';
	ofs << '\x00' << '\x00' << '\x00' << '\x00';

	// 28
	#else
	// 0
	ofs << 'A' << 'C' << 'S' << '\0';
	ofs << '\x08' << '\x00' << '\x00' << '\x00';

	// 8
	ofs << '\x01' << '\x00' << '\x00' << '\x00';

	// 12
	ofs << '\xA8' << '\x0F' << '\x00' << '\x00';
	ofs << '\x1C' << '\x00' << '\x00' << '\x00';
	ofs << '\x00' << '\x00' << '\x00' << '\x00';

	// 24
	ofs << '\x00' << '\x00' << '\x00' << '\x00';

	// 28
	#endif

	for (std::vector<BinaryTokenZDACS>::iterator it(instructions.begin()); it != instructions.end(); ++it)
		it->write(&ofs);

	return 0;
}

int main(int argc, char * * argv)
{
	try
	{
		BinaryTokenZDACS::init();
		SourceTokenASMPLX::init();

		return _main(argc, argv);
	}
	catch (SourceException & e)
	{
		std::cerr << e.where() << " (" << e.who() << "): " << e.what() << std::endl;
	}
	catch (std::exception & e)
	{
		std::cerr << "(std::exception): " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception.\n";
	}

	return 1;
}



