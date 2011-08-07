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
#include "ObjectVector.hpp"
#include "option.hpp"
#include "ost_type.hpp"
#include "print_debug.hpp"
#include "SourceBlockC.hpp"
#include "SourceContext.hpp"
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



SourceType divine_source_type(std::string const & name)
{
	std::ifstream in(name.c_str());


	// DS source?
	in.seekg(0);

	if (in.get() == '/' && in.get() == '*' && in.get() == 'D' && in.get() == 'S')
		return SOURCE_DS;


	// ASMPLX source?
	in.seekg(0);

	// Can have empty lines before the header.
	while (in.get() == '\n');
	in.unget();

	if (in.get() == 'A' && in.get() == 'S' && in.get() == 'M' && in.get() == 'P' && in.get() == 'L' && in.get() == 'X' && in.get() == '\n')
		return SOURCE_ASMPLX;


	return SOURCE_UNKNOWN;
}

static inline void _init(int argc, char const * const * argv)
{
	ost_init();

	BinaryTokenZDACS::init();
	SourceExpressionDS::init();
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

	if (source_type == SOURCE_UNKNOWN)
		source_type = divine_source_type(option::option_args[0]);

	std::ifstream ifs(option::option_args[0].c_str());

	ObjectVector objects;

	switch (source_type)
	{
	#if 0
	case SOURCE_ACS:
	{
		SourceStream in(&ifs, option::option_args[0], SourceStream::ST_C);

		SourceTokenizerC tokenizer(&in);

		SourceBlockC block(&tokenizer);
		print_debug(&ofs, block); return 0;

		std::vector<SourceExpressionACS> expressions;
		SourceExpressionACS::make_expressions(block, &expressions);
		print_debug(&ofs, expressions); return 0;

		SourceExpressionACS::make_objects(expressions, &objects);
	}
		break;
	#endif

	case SOURCE_ASMPLX:
	{
		SourceStream in(&ifs, option::option_args[0], SourceStream::ST_ASMPLX);

		std::vector<SourceTokenASMPLX> tokens;
		SourceTokenASMPLX::read_tokens(&in, &tokens);

		ObjectExpression::add_script("main", 0, ObjectExpression::ST_OPEN, 0, 0, 0);

		SourceTokenASMPLX::make_objects(tokens, &objects);
	}
		break;

	case SOURCE_DS:
	{
		SourceStream in(&ifs, option::option_args[0], SourceStream::ST_C);

		SourceTokenizerDS tokenizer(&in);

		ObjectExpression::reserve_script_number(0);

		SourceExpression::Pointer expressions(SourceExpressionDS::make_expressions(&tokenizer));
		expressions->addLabel("main");

		ObjectExpression::add_script("main_id", "main", ObjectExpression::ST_OPEN, 0, 0, SourceContext::global_context.getLimit(SourceVariable::SC_REGISTER), 0);

		expressions->makeObjects(&objects);
	}
		break;

	default:
		std::cerr << "Unknown source type.\n";
		return 1;
	}

	ObjectExpression::do_deferred_allocation();
	objects.optimize();

	if (target_type == TARGET_UNKNOWN)
		target_type = TARGET_ZDOOM;

	if (output_type == OUTPUT_UNKNOWN) switch (target_type)
	{
	case TARGET_HEXEN: output_type = OUTPUT_ACS0; break;
	case TARGET_ZDOOM: output_type = OUTPUT_ACSE; break;
	case TARGET_UNKNOWN: break;
	}

	std::ofstream ofs(option::option_args[1].c_str());

	switch (target_type)
	{
	case TARGET_ZDOOM:
	{
		std::vector<BinaryTokenZDACS> instructions;
		BinaryTokenZDACS::make_tokens(objects, &instructions);
		BinaryTokenZDACS::write_all(&ofs, instructions);
	}
		break;

	default:
		std::cerr << "Unknown target type.\n";
		return 1;
	}

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



