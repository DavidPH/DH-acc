//-----------------------------------------------------------------------------
//
// Copyright(C) 2011, 2012 David Hill
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------------
//
// Program start point.
//
//-----------------------------------------------------------------------------

#include "BinaryTokenACS.hpp"
#include "BinaryTokenPPACS.hpp"
#include "BinaryTokenZDACS.hpp"
#include "ObjectExpression.hpp"
#include "ObjectToken.hpp"
#include "ObjectVector.hpp"
#include "option.hpp"
#include "ost_type.hpp"
#include "SourceBlockC.hpp"
#include "SourceContext.hpp"
#include "SourceException.hpp"
#include "SourceExpressionACS.hpp"
#include "SourceExpressionDS.hpp"
#include "SourceStream.hpp"
#include "SourceTokenASMPLX.hpp"
#include "SourceTokenizerC.hpp"
#include "SourceTokenizerDS.hpp"
#include "VariableData.hpp"
#include "VariableType.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option_data<std::string> option_out("out", "output", "Output File.");


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// divine_source_type
//
// Attempts to determine the source type by examining a given file's contents
// and name.
//
static SourceType divine_source_type(std::string const &name)
{
   char buf[7];
   std::ifstream in(name.c_str());
   char const *suf;
   std::string::size_type sufIndex;


   // Read the first bytes. If there aren't enough, that's OK.
   for (size_t i = 0; i < sizeof(buf); ++i)
      buf[i] = in.get();

   // DS source?
   if (buf[0] == '/' && (buf[1] == '*' || buf[1] == '/') &&
       buf[2] == 'D' && buf[3] == 'S')
      return SOURCE_DS;

   // ASMPLX source?
   if (buf[0] == 'A' && buf[1] == 'S' && buf[2] == 'M' &&
       buf[3] == 'P' && buf[4] == 'L' && buf[5] == 'X' && buf[6] == '\n')
      return SOURCE_ASMPLX;

   // object source?
   if (buf[0] == 'o' && buf[1] == 'b' && buf[2] == 'j' &&
       buf[3] == 'e' && buf[4] == 'c' && buf[5] == 't')
      return SOURCE_object;


   // Look for a file extension.
   sufIndex = name.find_last_of('.');

   if (sufIndex == std::string::npos)
      return SOURCE_UNKNOWN;

   suf = name.c_str() + sufIndex + 1;

   // DS source?
   if (suf[0] == 'd' && suf[1] == 's' && suf[2] == '\0')
      return SOURCE_DS;

   // ASMPLX source?
   if (suf[0] == 'a' && suf[1] == 's' && suf[2] == 'm' && suf[3] == '\0')
      return SOURCE_ASMPLX;

   // object source?
   if (suf[0] == 'o' && suf[1] == 'b' && suf[2] == 'j' && suf[3] == '\0')
      return SOURCE_object;


   return SOURCE_UNKNOWN;
}

//
// read_source
//
static void read_source(std::string const &name, SourceType type,
                        ObjectVector *objects)
{
   ObjectExpression::set_filename(name);

   if (type == SOURCE_UNKNOWN)
      type = divine_source_type(name);

   switch (type)
   {
   case SOURCE_ASMPLX:
   {
      SourceStream in(name, SourceStream::ST_ASMPLX);

      std::vector<SourceTokenASMPLX> tokens;
      SourceTokenASMPLX::read_tokens(&in, &tokens);

      // Implicit main. Should be replaced by a proper script defining syntax.
      ObjectData_Script::ScriptType stype = ObjectData_Script::ST_OPEN;
      ObjectExpression::add_script("main", 0, stype, 0, 0, 0);

      SourceTokenASMPLX::make_objects(tokens, objects);
   }
      break;

   case SOURCE_DS:
   {
      SourceStream in(name, SourceStream::ST_C);

      SourceTokenizerDS tokenizer(&in);

      bool mainGen = tokenizer.peek().getType() == SourceTokenC::TT_OP_BRACE_O;

      SourceExpression::Pointer expressions =
         SourceExpressionDS::make_expressions(&tokenizer);

      if (mainGen)
      {
         std::string mainName = name + "_main";
         std::string mainID = mainName + "_id";

         ObjectData_Script::ScriptType stype = ObjectData_Script::ST_OPEN;

         bigsint varCount =
            SourceContext::global_context->getLimit(SourceVariable::SC_REGISTER);

         expressions->addLabel(mainName);
         ObjectExpression::add_script(mainID, mainName, stype, 0, 0, varCount);
      }

      expressions->makeObjects(objects, VariableData::create_void(0));
   }
      break;

   case SOURCE_object:
   {
      std::ifstream in(name.c_str());
      ObjectExpression::read_objects(&in, objects);
   }
      break;

   default:
      throw "Unknown source type.";
   }
}

//
// _init
//
static inline void _init(int argc, char const *const *argv)
{
   BinaryTokenACS::init();
   BinaryTokenPPACS::init();
   BinaryTokenZDACS::init();
   SourceContext::init();
   SourceExpressionDS::init();

   option::program = argv[0];

   if (argc == 1)
   {
      option::print_help(&std::cerr);
      throw 0;
   }

   option::process(argc-1, argv+1);

   if (option_out.data.empty() && !option::args_vector.empty())
   {
      option_out.data = option::args_vector.back();
      option::args_vector.pop_back();
   }

   if (option::args_vector.empty())
   {
      option::print_help(&std::cerr);
      throw 1;
   }
}

//
// _main
//
static inline int _main()
{
   ObjectVector objects;

   // Default target.
   if (target_type == TARGET_UNKNOWN)
      target_type = TARGET_Hexen;

   // Read source file(s).
   for (std::vector<std::string>::iterator arg(option::args_vector.begin());
        arg != option::args_vector.end();
        ++arg)
      read_source(*arg, source_type, &objects);

   // If doing object output, don't process object data.
   if (output_type == OUTPUT_object)
   {
      std::ofstream ofs(option_out.data.c_str());

      ObjectExpression::write_objects(&ofs, objects);

      return 0;
   }

   // Process object data.
   ObjectExpression::do_deferred_allocation();
   objects.optimize();

   // Default output.
   if (output_type == OUTPUT_UNKNOWN) switch (target_type)
   {
   case TARGET_Eternity: output_type = OUTPUT_ACS0; break;
   case TARGET_Hexen:    output_type = OUTPUT_ACS0; break;
   case TARGET_HexPP:    output_type = OUTPUT_ACSP; break;
   case TARGET_ZDoom:    output_type = OUTPUT_ACSE; break;
   case TARGET_UNKNOWN: break;
   }

   // Write output file.
   std::ofstream ofs(option_out.data.c_str());

   switch (target_type)
   {
   case TARGET_Hexen:
   {
      std::vector<BinaryTokenACS> instructions;
      BinaryTokenACS::make_tokens(objects, &instructions);
      BinaryTokenACS::write_all(&ofs, instructions);
   }
      break;

   case TARGET_Eternity:
   case TARGET_HexPP:
   {
      std::vector<BinaryTokenPPACS> instructions;
      BinaryTokenPPACS::make_tokens(objects, &instructions);
      BinaryTokenPPACS::write_all(&ofs, instructions);
   }
      break;

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


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// main
//
int main(int argc, char *argv[])
{
   try
   {
      _init(argc, argv);

      return _main();
   }
   catch (SourceException const &e)
   {
      std::cerr << e.where() << " (" << e.who() << "): " << e.what() << std::endl;
   }
   catch (option::exception const &e)
   {
      std::cerr << "(option::exception): " << e.what() << std::endl;
      option::print_help(&std::cerr);
   }
   catch (std::exception const &e)
   {
      std::cerr << "(std::exception): " << e.what() << std::endl;
   }
   catch (char const *e)
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
      std::cerr << "Unknown exception." << std::endl;
   }

   return 1;
}

// EOF

