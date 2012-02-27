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
// ACS (Hexen ACS) handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_BinaryTokenACS_
#define HPP_BinaryTokenACS_

#include "bignum.hpp"
#include "Counter.hpp"
#include "SourcePosition.hpp"

#include <ostream>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

struct ObjectData_Script;
struct ObjectData_String;
class ObjectExpression;
class ObjectToken;
class ObjectVector;

//
// BinaryTokenACS
//
class BinaryTokenACS
{
public:
   enum BinaryCode
   {
      #define BINTOKACS_BCODES                  \
      BCODE_NOP                          =   0, \
      BCODE_SCRIPT_TERMINATE             =   1, \
      BCODE_SCRIPT_SUSPEND               =   2, \
      BCODE_GET_LITERAL                  =   3, \
      BCODE_SPECIAL_EXEC1                =   4, \
      BCODE_SPECIAL_EXEC2                =   5, \
      BCODE_SPECIAL_EXEC3                =   6, \
      BCODE_SPECIAL_EXEC4                =   7, \
      BCODE_SPECIAL_EXEC5                =   8, \
      BCODE_SPECIAL_EXEC1_IMM            =   9, \
      BCODE_SPECIAL_EXEC2_IMM            =  10, \
      BCODE_SPECIAL_EXEC3_IMM            =  11, \
      BCODE_SPECIAL_EXEC4_IMM            =  12, \
      BCODE_SPECIAL_EXEC5_IMM            =  13, \
      BCODE_ADD                          =  14, \
      BCODE_SUB                          =  15, \
      BCODE_MUL                          =  16, \
      BCODE_DIV                          =  17, \
      BCODE_MOD                          =  18, \
      BCODE_CMP_EQ                       =  19, \
      BCODE_CMP_NE                       =  20, \
      BCODE_CMP_LT                       =  21, \
      BCODE_CMP_GT                       =  22, \
      BCODE_CMP_LE                       =  23, \
      BCODE_CMP_GE                       =  24, \
      BCODE_SET_REGISTER                 =  25, \
      BCODE_SET_MAPREGISTER              =  26, \
      BCODE_SET_WORLDREGISTER            =  27, \
      BCODE_GET_REGISTER                 =  28, \
      BCODE_GET_MAPREGISTER              =  29, \
      BCODE_GET_WORLDREGISTER            =  30, \
      BCODE_SETOP_ADD_REGISTER           =  31, \
      BCODE_SETOP_ADD_MAPREGISTER        =  32, \
      BCODE_SETOP_ADD_WORLDREGISTER      =  33, \
      BCODE_SETOP_SUB_REGISTER           =  34, \
      BCODE_SETOP_SUB_MAPREGISTER        =  35, \
      BCODE_SETOP_SUB_WORLDREGISTER      =  36, \
      BCODE_SETOP_MUL_REGISTER           =  37, \
      BCODE_SETOP_MUL_MAPREGISTER        =  38, \
      BCODE_SETOP_MUL_WORLDREGISTER      =  39, \
      BCODE_SETOP_DIV_REGISTER           =  40, \
      BCODE_SETOP_DIV_MAPREGISTER        =  41, \
      BCODE_SETOP_DIV_WORLDREGISTER      =  42, \
      BCODE_SETOP_MOD_REGISTER           =  43, \
      BCODE_SETOP_MOD_MAPREGISTER        =  44, \
      BCODE_SETOP_MOD_WORLDREGISTER      =  45, \
      BCODE_SETOP_INC_REGISTER           =  46, \
      BCODE_SETOP_INC_MAPREGISTER        =  47, \
      BCODE_SETOP_INC_WORLDREGISTER      =  48, \
      BCODE_SETOP_DEC_REGISTER           =  49, \
      BCODE_SETOP_DEC_MAPREGISTER        =  50, \
      BCODE_SETOP_DEC_WORLDREGISTER      =  51, \
      BCODE_BRANCH_GOTO_IMM              =  52, \
      BCODE_BRANCH_TRUE                  =  53, \
      BCODE_STACK_DROP                   =  54, \
      BCODE_WAIT_TICS                    =  55, \
      BCODE_WAIT_TICS_IMM                =  56, \
      BCODE_MISC_RANDOM                  =  57, \
      BCODE_MISC_RANDOM_IMM              =  58, \
      BCODE_GAME_GET_THINGCOUNT_SID      =  59, \
      BCODE_GAME_GET_THINGCOUNT_SID_IMM  =  60, \
      BCODE_WAIT_STAG                    =  61, \
      BCODE_WAIT_STAG_IMM                =  62, \
      BCODE_WAIT_POLYOBJECT              =  63, \
      BCODE_WAIT_POLYOBJECT_IMM          =  64, \
      BCODE_STAG_SET_TEXTURE_FLOOR       =  65, \
      BCODE_STAG_SET_TEXTURE_FLOOR_IMM   =  66, \
      BCODE_STAG_SET_TEXTURE_CEILING     =  67, \
      BCODE_STAG_SET_TEXTURE_CEILING_IMM =  68, \
      BCODE_SCRIPT_RESTART               =  69, \
      BCODE_LOGICAL_AND                  =  70, \
      BCODE_LOGICAL_IOR                  =  71, \
      BCODE_BITWISE_AND                  =  72, \
      BCODE_BITWISE_IOR                  =  73, \
      BCODE_BITWISE_XOR                  =  74, \
      BCODE_LOGICAL_NOT                  =  75, \
      BCODE_BITWISE_LSH                  =  76, \
      BCODE_BITWISE_RSH                  =  77, \
      BCODE_MISC_NEGATE                  =  78, \
      BCODE_BRANCH_ZERO                  =  79, \
      BCODE_LINE_GET_SIDE                =  80, \
      BCODE_WAIT_SCRIPT                  =  81, \
      BCODE_WAIT_SCRIPT_IMM              =  82, \
      BCODE_LINE_CLR_SPECIAL             =  83, \
      BCODE_BRANCH_CASE                  =  84, \
      BCODE_PRINT_START                  =  85, \
      BCODE_PRINT_END                    =  86, \
      BCODE_PRINT_STRING                 =  87, \
      BCODE_PRINT_NUM_DEC                =  88, \
      BCODE_PRINT_CHARACTER              =  89, \
      BCODE_GAME_GET_PLAYERCOUNT         =  90, \
      BCODE_GAME_GET_TYPE                =  91, \
      BCODE_GAME_GET_SKILL               =  92, \
      BCODE_GAME_GET_TIME                =  93, \
      BCODE_SOUND_SECTOR                 =  94, \
      BCODE_SOUND_AMBIENT                =  95, \
      BCODE_SOUND_SEQUENCE               =  96, \
      BCODE_LTAG_SET_TEXTURE             =  97, \
      BCODE_LTAG_SET_BLOCK               =  98, \
      BCODE_LTAG_SET_SPECIAL             =  99, \
      BCODE_SOUND_MTAG                   = 100, \
      BCODE_PRINT_END_BOLD               = 101

      BINTOKACS_BCODES,

      BCODE_NONE
   };

   typedef std::vector<std::string>::const_iterator label_iterator;



   BinaryTokenACS(BinaryCode code, SourcePosition const &position,
                  std::vector<std::string> const &labels,
                  std::vector<CounterPointer<ObjectExpression> > const &args);

   void addLabel(std::string const &label);

   size_t getArgCount() const;

   label_iterator label_begin() const;
   label_iterator label_end() const;

   size_t size() const;

   void writeACS0(std::ostream *out) const;



   static void init();
   static void init(int *argCounts);

   static void make_tokens(ObjectToken const &object, std::vector<BinaryTokenACS> *instructions);
   static void make_tokens(ObjectVector const &objects, std::vector<BinaryTokenACS> *instructions);

   template<typename T>
   static void output_ACS0(std::ostream *out, std::vector<T> const &instructions);
   template<typename T>
   static void output_prep(std::vector<T> const &instructions);

   static void write_ACS0_8(std::ostream *out, bigsint i);
   static void write_ACS0_16(std::ostream *out, bigsint i);
   static void write_ACS0_32(std::ostream *out, ObjectExpression const &expr);
   static void write_ACS0_32(std::ostream *out, bigsint i);
   static void write_ACS0_script(std::ostream *out, ObjectData_Script const &s);
   static void write_ACS0_script_count(std::ostream *out);
   static void write_ACS0_script_counter(std::ostream *out, ObjectData_Script const &s);
   static void write_ACS0_string(std::ostream *out, ObjectData_String const &s);
   static void write_ACS0_string_count(std::ostream *out);
   static void write_ACS0_string_counter(std::ostream *out, ObjectData_String const &s);
   static void write_ACS0_string_offset(std::ostream *out, ObjectData_String const &s);

   static void write_all(std::ostream *out, std::vector<BinaryTokenACS> const &instructions);

private:
   std::vector<CounterPointer<ObjectExpression> > args;
   BinaryCode code;
   std::vector<std::string> labels;
   SourcePosition position;



   static int arg_counts[BCODE_NONE];
};

#endif//HPP_BinaryTokenZDACS_

