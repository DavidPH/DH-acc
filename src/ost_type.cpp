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
// Output/Source/Target Type.
//
//-----------------------------------------------------------------------------

#include "ost_type.hpp"

#include "option.hpp"

#include <cstring>


//----------------------------------------------------------------------------|
// Static Prototypes                                                          |
//

static int output_handler
(char const *opt, int optf, int argc, char const *const *argv);

static int output_object
(char const *opt, int optf, int argc, char const *const *argv);

static int source_handler
(char const *opt, int optf, int argc, char const *const *argv);

static int target_handler
(char const *opt, int optf, int argc, char const *const *argv);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_call option_output
('\0', "output-type", "output", "Output type.", NULL, output_handler);

static option::option_call option_output_object
('c', NULL, "output", "Equal to --output-type=object.", NULL, output_object);

static option::option_call option_source
('\0', "source-type", "input", "Source file type.", NULL, source_handler);

static option::option_call option_target
('\0', "target-type", "output", "Target engine.", NULL, target_handler);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

OutputType output_type = OUTPUT_UNKNOWN;
SourceType source_type = SOURCE_UNKNOWN;
TargetType target_type = TARGET_UNKNOWN;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// output_handler
//
static int output_handler
(char const *opt, int optf, int argc, char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   if (!strcmp(argv[0], "ACS0"))
      output_type = OUTPUT_ACS0;
   else if (!strcmp(argv[0], "ACSE"))
      output_type = OUTPUT_ACSE;
   else if (!strcmp(argv[0], "ACS+"))
      output_type = OUTPUT_ACSP;
   else if (!strcmp(argv[0], "object"))
      output_type = OUTPUT_object;
   else
      option::exception::error(opt, optf, "unrecognized type");

   return 1;
}

static int output_object(char const *, int, int, char const *const *)
{
   output_type = OUTPUT_object;

   return 0;
}

static int source_handler
(char const *opt, int optf, int argc, char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   if (!strcmp(argv[0], "ASMPLX"))
      source_type = SOURCE_ASMPLX;
   else if (!strcmp(argv[0], "DS"))
      source_type = SOURCE_DS;
   else if (!strcmp(argv[0], "object"))
      source_type = SOURCE_object;
   else
      option::exception::error(opt, optf, "unrecognized type");

   return 1;
}

static int target_handler
(char const *opt, int optf, int argc, char const *const *argv)
{
   if (!argc) option::exception::error(opt, optf, "requires argument");

   if (!strcmp(argv[0], "Eternity"))
      target_type = TARGET_Eternity;
   else if (!strcmp(argv[0], "Hexen"))
      target_type = TARGET_Hexen;
   else if (!strcmp(argv[0], "Hex++"))
      target_type = TARGET_HexPP;
   else if (!strcmp(argv[0], "ZDoom"))
      target_type = TARGET_ZDoom;
   else
      option::exception::error(opt, optf, "unrecognized type");

   return 1;
}

// EOF

