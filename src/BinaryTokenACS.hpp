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

/* BinaryTokenACS.hpp
**
** Defines the BinaryTokenACS class.
*/

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
      #define BINTOKACS_BCODES                 \
      BCODE_NOP                         =   0, \
      BCODE_SCRIPT_TERMINATE            =   1, \
      BCODE_SCRIPT_SUSPEND              =   2, \
      BCODE_GET_LITERAL                 =   3, \
      BCODE_LINE_SPEC_EXEC1             =   4, \
      BCODE_LINE_SPEC_EXEC2             =   5, \
      BCODE_LINE_SPEC_EXEC3             =   6, \
      BCODE_LINE_SPEC_EXEC4             =   7, \
      BCODE_LINE_SPEC_EXEC5             =   8, \
      BCODE_LINE_SPEC_EXEC1_IMM         =   9, \
      BCODE_LINE_SPEC_EXEC2_IMM         =  10, \
      BCODE_LINE_SPEC_EXEC3_IMM         =  11, \
      BCODE_LINE_SPEC_EXEC4_IMM         =  12, \
      BCODE_LINE_SPEC_EXEC5_IMM         =  13, \
      BCODE_ADD                         =  14, \
      BCODE_SUB                         =  15, \
      BCODE_MUL                         =  16, \
      BCODE_DIV                         =  17, \
      BCODE_MOD                         =  18, \
      BCODE_CMP_EQ                      =  19, \
      BCODE_CMP_NE                      =  20, \
      BCODE_CMP_LT                      =  21, \
      BCODE_CMP_GT                      =  22, \
      BCODE_CMP_LE                      =  23, \
      BCODE_CMP_GE                      =  24, \
      BCODE_SET_REGISTER_VAR            =  25, \
      BCODE_SET_MAPREGISTER_VAR         =  26, \
      BCODE_SET_WORLDREGISTER_VAR       =  27, \
      BCODE_GET_REGISTER_VAR            =  28, \
      BCODE_GET_MAPREGISTER_VAR         =  29, \
      BCODE_GET_WORLDREGISTER_VAR       =  30, \
      BCODE_SETOP_ADD_REGISTER_VAR      =  31, \
      BCODE_SETOP_ADD_MAPREGISTER_VAR   =  32, \
      BCODE_SETOP_ADD_WORLDREGISTER_VAR =  33, \
      BCODE_SETOP_SUB_REGISTER_VAR      =  34, \
      BCODE_SETOP_SUB_MAPREGISTER_VAR   =  35, \
      BCODE_SETOP_SUB_WORLDREGISTER_VAR =  36, \
      BCODE_SETOP_MUL_REGISTER_VAR      =  37, \
      BCODE_SETOP_MUL_MAPREGISTER_VAR   =  38, \
      BCODE_SETOP_MUL_WORLDREGISTER_VAR =  39, \
      BCODE_SETOP_DIV_REGISTER_VAR      =  40, \
      BCODE_SETOP_DIV_MAPREGISTER_VAR   =  41, \
      BCODE_SETOP_DIV_WORLDREGISTER_VAR =  42, \
      BCODE_SETOP_MOD_REGISTER_VAR      =  43, \
      BCODE_SETOP_MOD_MAPREGISTER_VAR   =  44, \
      BCODE_SETOP_MOD_WORLDREGISTER_VAR =  45, \
      BCODE_SETOP_INC_REGISTER_VAR      =  46, \
      BCODE_SETOP_INC_MAPREGISTER_VAR   =  47, \
      BCODE_SETOP_INC_WORLDREGISTER_VAR =  48, \
      BCODE_SETOP_DEC_REGISTER_VAR      =  49, \
      BCODE_SETOP_DEC_MAPREGISTER_VAR   =  50, \
      BCODE_SETOP_DEC_WORLDREGISTER_VAR =  51, \
      BCODE_BRANCH_GOTO_IMM             =  52, \
      BCODE_BRANCH_TRUE                 =  53, \
      BCODE_STACK_DROP                  =  54, \
      BCODE_MISC_DELAY                  =  55, \
      BCODE_MISC_DELAY_IMM              =  56, \
      BCODE_MISC_RANDOM                 =  57, \
      BCODE_MISC_RANDOM_IMM             =  58, \
      BCODE_THING_COUNT                 =  59, \
      BCODE_THING_COUNT_IMM             =  60, \
      BCODE_WAIT_SECTOR                 =  61, \
      BCODE_WAIT_SECTOR_IMM             =  62, \
      BCODE_WAIT_POLYOBJECT             =  63, \
      BCODE_WAIT_POLYOBJECT_IMM         =  64, \
      BCODE_TEXTURE_SET_FLOOR           =  65, \
      BCODE_TEXTURE_SET_FLOOR_IMM       =  66, \
      BCODE_TEXTURE_SET_CEILING         =  67, \
      BCODE_TEXTURE_SET_CEILING_IMM     =  68, \
      BCODE_SCRIPT_RESTART              =  69, \
      BCODE_LOGICAL_AND                 =  70, \
      BCODE_LOGICAL_IOR                 =  71, \
      BCODE_BITWISE_AND                 =  72, \
      BCODE_BITWISE_IOR                 =  73, \
      BCODE_BITWISE_XOR                 =  74, \
      BCODE_LOGICAL_NOT                 =  75, \
      BCODE_BITWISE_SHIFTL              =  76, \
      BCODE_BITWISE_SHIFTR              =  77, \
      BCODE_MISC_NEGATE                 =  78, \
      BCODE_BRANCH_ZERO                 =  79, \
      BCODE_LINE_SIDE                   =  80, \
      BCODE_WAIT_SCRIPT                 =  81, \
      BCODE_WAIT_SCRIPT_IMM             =  82, \
      BCODE_LINE_SPEC_CLEAR             =  83, \
      BCODE_BRANCH_CASE                 =  84, \
      BCODE_PRINT_START                 =  85, \
      BCODE_PRINT_END                   =  86, \
      BCODE_PRINT_STRING                =  87, \
      BCODE_PRINT_NUM_DEC               =  88, \
      BCODE_PRINT_CHARACTER             =  89, \
      BCODE_PLAYER_COUNT                =  90, \
      BCODE_GAME_TYPE                   =  91, \
      BCODE_GAME_SKILL                  =  92, \
      BCODE_GAME_TIMER                  =  93, \
      BCODE_SOUND_SECTOR                =  94, \
      BCODE_SOUND_AMBIENT               =  95, \
      BCODE_SOUND_SEQUENCE              =  96, \
      BCODE_TEXTURE_SET_LINE            =  97, \
      BCODE_LINE_BLOCK_SET              =  98, \
      BCODE_LINE_SPEC_SET               =  99, \
      BCODE_SOUND_THING                 = 100, \
      BCODE_PRINT_END_BOLD              = 101

      BINTOKACS_BCODES,

      BCODE_NONE
   };

	typedef std::vector<std::string>::const_iterator label_iterator;



	BinaryTokenACS(BinaryCode code, SourcePosition const & position, std::vector<std::string> const & labels, std::vector<CounterPointer<ObjectExpression> > const & args);

	void addLabel(std::string const & label);

	size_t getArgCount() const;

	label_iterator label_begin() const;
	label_iterator label_end() const;

	size_t size() const;

	void writeACS0(std::ostream * out) const;



	static void init();
	static void init(int * argCounts);

	static void make_tokens(ObjectToken const & object, std::vector<BinaryTokenACS> * instructions);
	static void make_tokens(ObjectVector const & objects, std::vector<BinaryTokenACS> * instructions);

	template<typename T> static void output_ACS0(std::ostream * out, std::vector<T> const & instructions);
	template<typename T> static void output_prep(std::vector<T> const & instructions);

	static void write_ACS0_8(std::ostream * out, bigsint i);
	static void write_ACS0_16(std::ostream * out, bigsint i);
	static void write_ACS0_32(std::ostream * out, ObjectExpression const & expr);
	static void write_ACS0_32(std::ostream * out, bigsint i);
	static void write_ACS0_script(std::ostream * out, ObjectData_Script const & s);
	static void write_ACS0_script_count(std::ostream * out);
	static void write_ACS0_script_counter(std::ostream * out, ObjectData_Script const & s);
	static void write_ACS0_string(std::ostream * out, ObjectData_String const & s);
	static void write_ACS0_string_count(std::ostream * out);
	static void write_ACS0_string_counter(std::ostream * out, ObjectData_String const & s);
	static void write_ACS0_string_offset(std::ostream * out, ObjectData_String const & s);

	static void write_all(std::ostream * out, std::vector<BinaryTokenACS> const & instructions);

private:
	std::vector<CounterPointer<ObjectExpression> > _args;
	BinaryCode _code;
	std::vector<std::string> _labels;
	SourcePosition _position;



	static int _arg_counts[BCODE_NONE];
};

#endif /* HPP_BinaryTokenZDACS_ */

