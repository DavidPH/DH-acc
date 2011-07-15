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
#include "ObjectExpression.hpp"
#include "ObjectToken.hpp"
#include "option.hpp"
#include "print_debug.hpp"
#include "SourceBlockC.hpp"
#include "SourceException.hpp"
#include "SourceExpressionACS.hpp"
#include "SourceExpressionDS.hpp"
#include "SourceStream.hpp"
#include "SourceTokenASMPLX.hpp"
#include "SourceTokenizerC.hpp"
#include "SourceTokenizerDS.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>



static inline void _init(int argc, char const * const * argv)
{
	BinaryTokenZDACS::init();
	SourceTokenASMPLX::init();

	option::option_set_name(argv[0]);

	if (argc == 1)
	{
		option::option_print_help(&std::cout);
		throw 0;
	}

	option::option_process(argc-1, argv+1);
}

static inline int _main()
{
	if (option::option_args.size() != 2) return 1;

	std::ifstream ifs(option::option_args[0].c_str());
	std::ofstream ofs(option::option_args[1].c_str());

	ObjectExpression::add_script("main", 0, ObjectExpression::ST_OPEN);
	std::vector<ObjectToken> objects;

	#if 0
	SourceStream in(&ifs, option::option_args[0], SourceStream::ST_C);

	SourceTokenizerC tokenizer(&in);

	SourceBlockC block(&tokenizer);
	print_debug(&ofs, block); return 0;

	std::vector<SourceExpressionACS> expressions;
	SourceExpressionACS::make_expressions(block, &expressions);
	print_debug(&ofs, expressions); return 0;

	SourceExpressionACS::make_objects(expressions, &objects);
	#elif 1
	SourceStream in(&ifs, option::option_args[0], SourceStream::ST_C);

	SourceTokenizerDS tokenizer(&in);

	SourceExpressionDS expressions(SourceExpressionDS::make_expressions(&tokenizer));
	expressions.addLabel("main");

	expressions.makeObjectsGet(&objects);
	#else
	SourceStream in(&ifs, option::option_args[0], SourceStream::ST_ASMPLX);

	std::vector<SourceTokenASMPLX> tokens;
	SourceTokenASMPLX::read_tokens(&in, &tokens);

	SourceTokenASMPLX::make_objects(tokens, &objects);
	#endif

	std::vector<BinaryTokenZDACS> instructions;
	BinaryTokenZDACS::make_tokens(objects, &instructions);

	int32_t scriptCount(ObjectExpression::get_script_count());
	int32_t stringCount(ObjectExpression::get_string_count());

	BinaryTokenZDACS::prepare_all(instructions);

	// 0
	ofs << 'A' << 'C' << 'S' << '\0';
	ofs << '\x08' << '\x00' << '\x00' << '\x00';

	// 8
	BinaryTokenZDACS::write_32(&ofs, scriptCount);

	// 12
	for (int32_t index(0); index < ObjectExpression::get_script_count(); ++index)
		BinaryTokenZDACS::write_script(&ofs, ObjectExpression::get_script(index));

	// 12+(scriptCount*12)
	BinaryTokenZDACS::write_32(&ofs, stringCount);

	// 12+(scriptCount*12)+4
	for (int32_t index(0); index < ObjectExpression::get_string_count(); ++index)
		BinaryTokenZDACS::write_32(&ofs, 12 + (scriptCount*12) + 4 + (stringCount*4) + ObjectExpression::get_string(index).offset);

	// 12+(scriptCount*12)+4+(stringCount*4)
	for (int32_t index(0); index < ObjectExpression::get_string_count(); ++index)
		BinaryTokenZDACS::write_string(&ofs, ObjectExpression::get_string(index).string);

	// 12+(scriptCount*12)+4+(stringCount*4)+stringLength
	BinaryTokenZDACS::write_all(&ofs, instructions);

	return 0;
}

int main(int argc, char * * argv)
{
	try
	{
		_init(argc, argv);

		return _main();
	}
	catch (SourceException & e)
	{
		std::cerr << e.where() << " (" << e.who() << "): " << e.what() << std::endl;
	}
	catch (option::option_exception & e)
	{
		std::cerr << "(option_exception): " << e.what() << std::endl;
		option::option_print_help(&std::cout);
	}
	catch (std::exception & e)
	{
		std::cerr << "(std::exception): " << e.what() << std::endl;
	}
	catch (int & e)
	{
		return e;
	}
	catch (...)
	{
		std::cerr << "Unknown exception.\n";
	}

	return 1;
}



