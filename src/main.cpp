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



static std::string option_out;



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


	// object source?
	in.seekg(0);

	if (in.get() == 'o' && in.get() == 'b' && in.get() == 'j' && in.get() == 'e' && in.get() == 'c' && in.get() == 't')
		return SOURCE_object;


	return SOURCE_UNKNOWN;
}

void read_source(std::string const & name, SourceType type, ObjectVector * objects)
{
	ObjectExpression::set_filename(name);

	if (type == SOURCE_UNKNOWN)
		type = divine_source_type(name);

	std::ifstream ifs(name.c_str());

	switch (type)
	{
	case SOURCE_ASMPLX:
	{
		SourceStream in(&ifs, name, SourceStream::ST_ASMPLX);

		std::vector<SourceTokenASMPLX> tokens;
		SourceTokenASMPLX::read_tokens(&in, &tokens);

		ObjectExpression::add_script("main", 0, ObjectExpression::ST_OPEN, 0, 0, 0);

		SourceTokenASMPLX::make_objects(tokens, objects);
	}
		break;

	case SOURCE_DS:
	{
		SourceStream in(&ifs, name, SourceStream::ST_C);

		SourceTokenizerDS tokenizer(&in);

		SourceExpression::Pointer expressions(SourceExpressionDS::make_expressions(&tokenizer));
		expressions->addLabel("main");

		ObjectExpression::add_script("main_id", "main", ObjectExpression::ST_OPEN, 0, 0, SourceContext::global_context.getLimit(SourceVariable::SC_REGISTER), 0);

		expressions->makeObjects(objects);
	}
		break;

	case SOURCE_object:
		ObjectExpression::read_objects(&ifs, objects);
		break;

	default:
		throw "Unknown source type.";
	}
}

static inline void _init(int argc, char const * const * argv)
{
	ost_init();

	BinaryTokenZDACS::init();
	SourceExpressionDS::init();
	SourceTokenASMPLX::init();

	option::option_add("out", "output", "Output file.", &option_out, option::option_handler_default_s);

	option::option_set_name(argv[0]);

	if (argc == 1)
	{
		option::option_print_help(&std::cout);
		throw 0;
	}

	option::option_process(argc-1, argv+1);

	if (option_out.empty() && !option::option_args.empty())
	{
		option_out = option::option_args.back();
		option::option_args.pop_back();
	}

	if (option::option_args.empty())
	{
		option::option_print_help(&std::cout);
		throw 0;
	}
}

static inline int _main()
{
	ObjectVector objects;

	// Read source file(s).
	for (option::option_sv::iterator arg(option::option_args.begin()); arg != option::option_args.end(); ++arg)
		read_source(*arg, source_type, &objects);

	// If doing object output, don't process object data.
	if (output_type == OUTPUT_object)
	{
		std::ofstream ofs(option_out.c_str());

		ObjectExpression::write_objects(&ofs, objects);

		return 0;
	}

	// Process object data.
	ObjectExpression::do_deferred_allocation();
	objects.optimize();

	// Default target.
	// TODO: Default to Hexen.
	if (target_type == TARGET_UNKNOWN)
		target_type = TARGET_ZDoom;

	// Default output.
	if (output_type == OUTPUT_UNKNOWN) switch (target_type)
	{
	case TARGET_Hexen: output_type = OUTPUT_ACS0; break;
	case TARGET_ZDoom: output_type = OUTPUT_ACSE; break;
	case TARGET_UNKNOWN: break;
	}

	// Write output file.
	std::ofstream ofs(option_out.c_str());

	switch (target_type)
	{
	case TARGET_ZDoom:
	{
		std::vector<BinaryTokenZDACS> instructions;
		BinaryTokenZDACS::make_tokens(objects, &instructions);
		BinaryTokenZDACS::write_all(&ofs, instructions);
	}
		break;

	default:
		throw "Unknown target type.";
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
	catch (char const * e)
	{
		std::cerr << e << std::endl;
		return 1;
	}
	catch (int e)
	{
		return e;
	}
	catch (...)
	{
		std::cerr << "Unknown exception.\n";
	}

	return 1;
}



