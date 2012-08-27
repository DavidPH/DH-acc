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

static TargetType GetTarget(char const *target);

static int OutputHandler(char const *opt, int optf, int argc, char const *const *argv);
static int Output_object(char const *opt, int optf, int argc, char const *const *argv);

static int SourceHandler(char const *opt, int optf, int argc, char const *const *argv);

static int TargetHandler(char const *opt, int optf, int argc, char const *const *argv);
static int Target_ZDoom(char const *opt, int optf, int argc, char const *const *argv);

static int TuneHandler(char const *opt, int optf, int argc, char const *const *argv);


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_call option_Output
('\0', "output-type", "output", "Output type.", NULL, OutputHandler);

static option::option_call option_Output_object
('c', NULL, "output", "Equal to --output-type=object.", NULL, Output_object);

static option::option_call option_Source
('\0', "source-type", "input", "Source file type.", NULL, SourceHandler);

static option::option_call option_Target
('\0', "target-type", "output", "Target engine.", NULL, TargetHandler);

static option::option_call option_Target_ZDoom
('Z', NULL, "output", "Equal to --target-type=ZDoom.", NULL, Target_ZDoom);

static option::option_call option_Tune
('\0', "tune-type", "output", "Tune codegen for given target.", NULL, TuneHandler);


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

OutputType Output = OUTPUT_UNKNOWN;
SourceType Source = SOURCE_UNKNOWN;
TargetType Target = TARGET_UNKNOWN, Tune = TARGET_UNKNOWN;


//----------------------------------------------------------------------------|
// Static Functions                                                           |
//

//
// GetTarget
//
static TargetType GetTarget(char const *target)
{
   if(!strcmp(target, "Eternity"))
      return TARGET_Eternity;
   else if(!strcmp(target, "Hexen"))
      return TARGET_Hexen;
   else if(!strcmp(target, "ZDoom"))
      return TARGET_ZDoom;
   else
      return TARGET_UNKNOWN;
}

//
// OutputHandler
//
static int OutputHandler(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   if(!strcmp(argv[0], "ACS0"))
      Output = OUTPUT_ACS0;
   else if(!strcmp(argv[0], "ACSE"))
      Output = OUTPUT_ACSE;
   else if(!strcmp(argv[0], "ACS+"))
      Output = OUTPUT_ACSP;
   else if(!strcmp(argv[0], "object"))
      Output = OUTPUT_object;
   else
      option::exception::error(opt, optf, "unrecognized type");

   return 1;
}

//
// Output_object
//
static int Output_object(char const *, int, int, char const *const *)
{
   Output = OUTPUT_object;

   return 0;
}

//
// SourceHandler
//
static int SourceHandler(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   if(!strcmp(argv[0], "ASM"))
      Source = SOURCE_ASM;
   else if(!strcmp(argv[0], "C"))
      Source = SOURCE_C;
   else if(!strcmp(argv[0], "DS"))
      Source = SOURCE_DS;
   else if(!strcmp(argv[0], "object"))
      Source = SOURCE_object;
   else
      option::exception::error(opt, optf, "unrecognized type");

   return 1;
}

//
// TargetHandler
//
static int TargetHandler(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   if((Target = GetTarget(argv[0])) == TARGET_UNKNOWN)
      option::exception::error(opt, optf, "unrecognized type");

   return 1;
}

//
// Target_ZDoom
//
static int Target_ZDoom(char const *, int, int, char const *const *)
{
   // Make -ZZ be --tune=ZDoom.
   if(Target == TARGET_ZDoom)
      Tune = TARGET_ZDoom;
   else
      Target = TARGET_ZDoom;

   return 0;
}

//
// TuneHandler
//
static int TuneHandler(char const *opt, int optf, int argc, char const *const *argv)
{
   if(!argc) option::exception::error(opt, optf, "requires argument");

   // Make empty argument reset to no tune.
   if(!*argv[0])
      Tune = TARGET_UNKNOWN;
   else if((Tune = GetTarget(argv[0])) == TARGET_UNKNOWN)
      option::exception::error(opt, optf, "unrecognized type");

   return 1;
}

// EOF

