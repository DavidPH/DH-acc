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

/* ObjectCode.cpp
**
** Defines the ObjectCode related function.
*/

#include "ObjectCode.hpp"

#include "object_io.hpp"
#include "SourceException.hpp"
#include "SourceTokenC.hpp"

#include <map>
#include <string>



static std::map<std::string, ObjectCode> _ocode_map;
static char const * _ocode_str[OCODE_NONE+1];

static struct _ocode_init_s {_ocode_init_s();} _ocode_init;

_ocode_init_s::_ocode_init_s()
{
	#define DO_INIT(NAME)\
	_ocode_map[#NAME] = OCODE_##NAME;\
	_ocode_str[OCODE_##NAME] = "OCODE_"#NAME

	DO_INIT(NOP);

	// Arithmetic

	DO_INIT(ADD32F);
	DO_INIT(ADD32I);
	DO_INIT(ADD32U);

	DO_INIT(DIV32F);
	DO_INIT(DIV32I);
	DO_INIT(DIV32U);

	DO_INIT(MOD32F);
	DO_INIT(MOD32I);
	DO_INIT(MOD32U);

	DO_INIT(MUL32F);
	DO_INIT(MUL32I);
	DO_INIT(MUL32U);

	DO_INIT(SUB32F);
	DO_INIT(SUB32I);
	DO_INIT(SUB32U);


	// Bitwise

	DO_INIT(BITWISE_AND32);
	DO_INIT(BITWISE_IOR32);
	DO_INIT(BITWISE_NOT32);
	DO_INIT(BITWISE_SHIFTL32);
	DO_INIT(BITWISE_SHIFTR32);
	DO_INIT(BITWISE_XOR32);


	// Branching

	DO_INIT(BRANCH_CASE);
	DO_INIT(BRANCH_GOTO);
	DO_INIT(BRANCH_GOTO_IMM);
	DO_INIT(BRANCH_TABLE);
	DO_INIT(BRANCH_TRUE);
	DO_INIT(BRANCH_ZERO);


	// Comparison

	DO_INIT(CMP_EQ32F);
	DO_INIT(CMP_EQ32I);

	DO_INIT(CMP_GE32F);
	DO_INIT(CMP_GE32I);
	DO_INIT(CMP_GE32U);

	DO_INIT(CMP_GT32F);
	DO_INIT(CMP_GT32I);
	DO_INIT(CMP_GT32U);

	DO_INIT(CMP_LE32F);
	DO_INIT(CMP_LE32I);
	DO_INIT(CMP_LE32U);

	DO_INIT(CMP_LT32F);
	DO_INIT(CMP_LT32I);
	DO_INIT(CMP_LT32U);

	DO_INIT(CMP_NE32F);
	DO_INIT(CMP_NE32I);


	// Conversion

	DO_INIT(CONVERT_32F_32I);
	DO_INIT(CONVERT_32I_32F);


	// Logical

	DO_INIT(LOGICAL_AND32F);
	DO_INIT(LOGICAL_AND32I);

	DO_INIT(LOGICAL_IOR32F);
	DO_INIT(LOGICAL_IOR32I);

	DO_INIT(LOGICAL_NOT32F);
	DO_INIT(LOGICAL_NOT32I);

	DO_INIT(LOGICAL_XOR32F);
	DO_INIT(LOGICAL_XOR32I);


	// Stack-ops

	DO_INIT(STACK_DROP32);
	DO_INIT(STACK_DUP32);
	DO_INIT(STACK_SWAP32);


	// Trigonometry

	DO_INIT(TRIG_COS32F);
	DO_INIT(TRIG_SIN32F);


	// Variable Address

	DO_INIT(ADDR_STACK_ADD);
	DO_INIT(ADDR_STACK_ADD_IMM);

	DO_INIT(ADDR_STACK_SUB);
	DO_INIT(ADDR_STACK_SUB_IMM);

	DO_INIT(ADDR_STACK_VAR);


	// Variable Get

	DO_INIT(GET_AUTO_VAR32F);
	DO_INIT(GET_AUTO_VAR32I);

	DO_INIT(GET_GLOBALARRAY_VAR32F);
	DO_INIT(GET_GLOBALARRAY_VAR32I);

	DO_INIT(GET_GLOBALARRAYTEMP_VAR32F);
	DO_INIT(GET_GLOBALARRAYTEMP_VAR32I);

	DO_INIT(GET_GLOBALREGISTER_VAR32F);
	DO_INIT(GET_GLOBALREGISTER_VAR32I);

	DO_INIT(GET_LITERAL32F);
	DO_INIT(GET_LITERAL32I);

	DO_INIT(GET_MAPARRAY_VAR32F);
	DO_INIT(GET_MAPARRAY_VAR32I);

	DO_INIT(GET_MAPARRAYTEMP_VAR32F);
	DO_INIT(GET_MAPARRAYTEMP_VAR32I);

	DO_INIT(GET_MAPREGISTER_VAR32F);
	DO_INIT(GET_MAPREGISTER_VAR32I);

	DO_INIT(GET_MOBJ_VAR32F);
	DO_INIT(GET_MOBJ_VAR32I);

	DO_INIT(GET_POINTER_VAR32F);
	DO_INIT(GET_POINTER_VAR32I);

	DO_INIT(GET_POINTERTEMP_VAR32F);
	DO_INIT(GET_POINTERTEMP_VAR32I);

	DO_INIT(GET_REGISTER_VAR32F);
	DO_INIT(GET_REGISTER_VAR32I);

	DO_INIT(GET_STATIC_VAR32F);
	DO_INIT(GET_STATIC_VAR32I);

	DO_INIT(GET_TID_VAR32F);
	DO_INIT(GET_TID_VAR32I);

	DO_INIT(GET_WORLDARRAY_VAR32F);
	DO_INIT(GET_WORLDARRAY_VAR32I);

	DO_INIT(GET_WORLDARRAYTEMP_VAR32F);
	DO_INIT(GET_WORLDARRAYTEMP_VAR32I);

	DO_INIT(GET_WORLDREGISTER_VAR32F);
	DO_INIT(GET_WORLDREGISTER_VAR32I);


	// Variable Set

	DO_INIT(SET_AUTO_VAR32F);
	DO_INIT(SET_AUTO_VAR32I);

	DO_INIT(SET_GLOBALARRAY_VAR32F);
	DO_INIT(SET_GLOBALARRAY_VAR32I);

	DO_INIT(SET_GLOBALARRAYTEMP_VAR32F);
	DO_INIT(SET_GLOBALARRAYTEMP_VAR32I);

	DO_INIT(SET_GLOBALREGISTER_VAR32F);
	DO_INIT(SET_GLOBALREGISTER_VAR32I);

	DO_INIT(SET_MAPARRAY_VAR32F);
	DO_INIT(SET_MAPARRAY_VAR32I);

	DO_INIT(SET_MAPARRAYTEMP_VAR32F);
	DO_INIT(SET_MAPARRAYTEMP_VAR32I);

	DO_INIT(SET_MAPREGISTER_VAR32F);
	DO_INIT(SET_MAPREGISTER_VAR32I);

	DO_INIT(SET_MOBJ_VAR32F);
	DO_INIT(SET_MOBJ_VAR32I);

	DO_INIT(SET_POINTER_VAR32F);
	DO_INIT(SET_POINTER_VAR32I);

	DO_INIT(SET_POINTERTEMP_VAR32F);
	DO_INIT(SET_POINTERTEMP_VAR32I);

	DO_INIT(SET_REGISTER_VAR32F);
	DO_INIT(SET_REGISTER_VAR32I);

	DO_INIT(SET_STATIC_VAR32F);
	DO_INIT(SET_STATIC_VAR32I);

	DO_INIT(SET_TEMP_VAR);

	DO_INIT(SET_TID_VAR32F);
	DO_INIT(SET_TID_VAR32I);

	DO_INIT(SET_WORLDARRAY_VAR32F);
	DO_INIT(SET_WORLDARRAY_VAR32I);

	DO_INIT(SET_WORLDARRAYTEMP_VAR32F);
	DO_INIT(SET_WORLDARRAYTEMP_VAR32I);

	DO_INIT(SET_WORLDREGISTER_VAR32F);
	DO_INIT(SET_WORLDREGISTER_VAR32I);


	// Variable Set Op

	// ADD
	DO_INIT(SETOP_ADD_AUTO_VAR32F);
	DO_INIT(SETOP_ADD_AUTO_VAR32I);
	DO_INIT(SETOP_ADD_AUTO_VAR32U);

	DO_INIT(SETOP_ADD_GLOBALARRAY_VAR32F);
	DO_INIT(SETOP_ADD_GLOBALARRAY_VAR32I);
	DO_INIT(SETOP_ADD_GLOBALARRAY_VAR32U);

	DO_INIT(SETOP_ADD_GLOBALREGISTER_VAR32F);
	DO_INIT(SETOP_ADD_GLOBALREGISTER_VAR32I);
	DO_INIT(SETOP_ADD_GLOBALREGISTER_VAR32U);

	DO_INIT(SETOP_ADD_MAPARRAY_VAR32F);
	DO_INIT(SETOP_ADD_MAPARRAY_VAR32I);
	DO_INIT(SETOP_ADD_MAPARRAY_VAR32U);

	DO_INIT(SETOP_ADD_MAPREGISTER_VAR32F);
	DO_INIT(SETOP_ADD_MAPREGISTER_VAR32I);
	DO_INIT(SETOP_ADD_MAPREGISTER_VAR32U);

	DO_INIT(SETOP_ADD_POINTER_VAR32F);
	DO_INIT(SETOP_ADD_POINTER_VAR32I);
	DO_INIT(SETOP_ADD_POINTER_VAR32U);

	DO_INIT(SETOP_ADD_REGISTER_VAR32F);
	DO_INIT(SETOP_ADD_REGISTER_VAR32I);
	DO_INIT(SETOP_ADD_REGISTER_VAR32U);

	DO_INIT(SETOP_ADD_STATIC_VAR32F);
	DO_INIT(SETOP_ADD_STATIC_VAR32I);
	DO_INIT(SETOP_ADD_STATIC_VAR32U);

	DO_INIT(SETOP_ADD_WORLDARRAY_VAR32F);
	DO_INIT(SETOP_ADD_WORLDARRAY_VAR32I);
	DO_INIT(SETOP_ADD_WORLDARRAY_VAR32U);

	DO_INIT(SETOP_ADD_WORLDREGISTER_VAR32F);
	DO_INIT(SETOP_ADD_WORLDREGISTER_VAR32I);
	DO_INIT(SETOP_ADD_WORLDREGISTER_VAR32U);

	// DEC
	DO_INIT(SETOP_DEC_AUTO_VAR32F);
	DO_INIT(SETOP_DEC_AUTO_VAR32I);
	DO_INIT(SETOP_DEC_AUTO_VAR32U);

	DO_INIT(SETOP_DEC_GLOBALARRAY_VAR32F);
	DO_INIT(SETOP_DEC_GLOBALARRAY_VAR32I);
	DO_INIT(SETOP_DEC_GLOBALARRAY_VAR32U);

	DO_INIT(SETOP_DEC_GLOBALREGISTER_VAR32F);
	DO_INIT(SETOP_DEC_GLOBALREGISTER_VAR32I);
	DO_INIT(SETOP_DEC_GLOBALREGISTER_VAR32U);

	DO_INIT(SETOP_DEC_MAPARRAY_VAR32F);
	DO_INIT(SETOP_DEC_MAPARRAY_VAR32I);
	DO_INIT(SETOP_DEC_MAPARRAY_VAR32U);

	DO_INIT(SETOP_DEC_MAPREGISTER_VAR32F);
	DO_INIT(SETOP_DEC_MAPREGISTER_VAR32I);
	DO_INIT(SETOP_DEC_MAPREGISTER_VAR32U);

	DO_INIT(SETOP_DEC_POINTER_VAR32F);
	DO_INIT(SETOP_DEC_POINTER_VAR32I);
	DO_INIT(SETOP_DEC_POINTER_VAR32U);

	DO_INIT(SETOP_DEC_REGISTER_VAR32F);
	DO_INIT(SETOP_DEC_REGISTER_VAR32I);
	DO_INIT(SETOP_DEC_REGISTER_VAR32U);

	DO_INIT(SETOP_DEC_STATIC_VAR32F);
	DO_INIT(SETOP_DEC_STATIC_VAR32I);
	DO_INIT(SETOP_DEC_STATIC_VAR32U);

	DO_INIT(SETOP_DEC_WORLDARRAY_VAR32F);
	DO_INIT(SETOP_DEC_WORLDARRAY_VAR32I);
	DO_INIT(SETOP_DEC_WORLDARRAY_VAR32U);

	DO_INIT(SETOP_DEC_WORLDREGISTER_VAR32F);
	DO_INIT(SETOP_DEC_WORLDREGISTER_VAR32I);
	DO_INIT(SETOP_DEC_WORLDREGISTER_VAR32U);

	// DIV
	DO_INIT(SETOP_DIV_AUTO_VAR32F);
	DO_INIT(SETOP_DIV_AUTO_VAR32I);
	DO_INIT(SETOP_DIV_AUTO_VAR32U);

	DO_INIT(SETOP_DIV_GLOBALARRAY_VAR32F);
	DO_INIT(SETOP_DIV_GLOBALARRAY_VAR32I);
	DO_INIT(SETOP_DIV_GLOBALARRAY_VAR32U);

	DO_INIT(SETOP_DIV_GLOBALREGISTER_VAR32F);
	DO_INIT(SETOP_DIV_GLOBALREGISTER_VAR32I);
	DO_INIT(SETOP_DIV_GLOBALREGISTER_VAR32U);

	DO_INIT(SETOP_DIV_MAPARRAY_VAR32F);
	DO_INIT(SETOP_DIV_MAPARRAY_VAR32I);
	DO_INIT(SETOP_DIV_MAPARRAY_VAR32U);

	DO_INIT(SETOP_DIV_MAPREGISTER_VAR32F);
	DO_INIT(SETOP_DIV_MAPREGISTER_VAR32I);
	DO_INIT(SETOP_DIV_MAPREGISTER_VAR32U);

	DO_INIT(SETOP_DIV_POINTER_VAR32F);
	DO_INIT(SETOP_DIV_POINTER_VAR32I);
	DO_INIT(SETOP_DIV_POINTER_VAR32U);

	DO_INIT(SETOP_DIV_REGISTER_VAR32F);
	DO_INIT(SETOP_DIV_REGISTER_VAR32I);
	DO_INIT(SETOP_DIV_REGISTER_VAR32U);

	DO_INIT(SETOP_DIV_STATIC_VAR32F);
	DO_INIT(SETOP_DIV_STATIC_VAR32I);
	DO_INIT(SETOP_DIV_STATIC_VAR32U);

	DO_INIT(SETOP_DIV_WORLDARRAY_VAR32F);
	DO_INIT(SETOP_DIV_WORLDARRAY_VAR32I);
	DO_INIT(SETOP_DIV_WORLDARRAY_VAR32U);

	DO_INIT(SETOP_DIV_WORLDREGISTER_VAR32F);
	DO_INIT(SETOP_DIV_WORLDREGISTER_VAR32I);
	DO_INIT(SETOP_DIV_WORLDREGISTER_VAR32U);

	// INC
	DO_INIT(SETOP_INC_AUTO_VAR32F);
	DO_INIT(SETOP_INC_AUTO_VAR32I);
	DO_INIT(SETOP_INC_AUTO_VAR32U);

	DO_INIT(SETOP_INC_GLOBALARRAY_VAR32F);
	DO_INIT(SETOP_INC_GLOBALARRAY_VAR32I);
	DO_INIT(SETOP_INC_GLOBALARRAY_VAR32U);

	DO_INIT(SETOP_INC_GLOBALREGISTER_VAR32F);
	DO_INIT(SETOP_INC_GLOBALREGISTER_VAR32I);
	DO_INIT(SETOP_INC_GLOBALREGISTER_VAR32U);

	DO_INIT(SETOP_INC_MAPARRAY_VAR32F);
	DO_INIT(SETOP_INC_MAPARRAY_VAR32I);
	DO_INIT(SETOP_INC_MAPARRAY_VAR32U);

	DO_INIT(SETOP_INC_MAPREGISTER_VAR32F);
	DO_INIT(SETOP_INC_MAPREGISTER_VAR32I);
	DO_INIT(SETOP_INC_MAPREGISTER_VAR32U);

	DO_INIT(SETOP_INC_POINTER_VAR32F);
	DO_INIT(SETOP_INC_POINTER_VAR32I);
	DO_INIT(SETOP_INC_POINTER_VAR32U);

	DO_INIT(SETOP_INC_REGISTER_VAR32F);
	DO_INIT(SETOP_INC_REGISTER_VAR32I);
	DO_INIT(SETOP_INC_REGISTER_VAR32U);

	DO_INIT(SETOP_INC_STATIC_VAR32F);
	DO_INIT(SETOP_INC_STATIC_VAR32I);
	DO_INIT(SETOP_INC_STATIC_VAR32U);

	DO_INIT(SETOP_INC_WORLDARRAY_VAR32F);
	DO_INIT(SETOP_INC_WORLDARRAY_VAR32I);
	DO_INIT(SETOP_INC_WORLDARRAY_VAR32U);

	DO_INIT(SETOP_INC_WORLDREGISTER_VAR32F);
	DO_INIT(SETOP_INC_WORLDREGISTER_VAR32I);
	DO_INIT(SETOP_INC_WORLDREGISTER_VAR32U);

	// MOD
	DO_INIT(SETOP_MOD_AUTO_VAR32F);
	DO_INIT(SETOP_MOD_AUTO_VAR32I);
	DO_INIT(SETOP_MOD_AUTO_VAR32U);

	DO_INIT(SETOP_MOD_GLOBALARRAY_VAR32F);
	DO_INIT(SETOP_MOD_GLOBALARRAY_VAR32I);
	DO_INIT(SETOP_MOD_GLOBALARRAY_VAR32U);

	DO_INIT(SETOP_MOD_GLOBALREGISTER_VAR32F);
	DO_INIT(SETOP_MOD_GLOBALREGISTER_VAR32I);
	DO_INIT(SETOP_MOD_GLOBALREGISTER_VAR32U);

	DO_INIT(SETOP_MOD_MAPARRAY_VAR32F);
	DO_INIT(SETOP_MOD_MAPARRAY_VAR32I);
	DO_INIT(SETOP_MOD_MAPARRAY_VAR32U);

	DO_INIT(SETOP_MOD_MAPREGISTER_VAR32F);
	DO_INIT(SETOP_MOD_MAPREGISTER_VAR32I);
	DO_INIT(SETOP_MOD_MAPREGISTER_VAR32U);

	DO_INIT(SETOP_MOD_POINTER_VAR32F);
	DO_INIT(SETOP_MOD_POINTER_VAR32I);
	DO_INIT(SETOP_MOD_POINTER_VAR32U);

	DO_INIT(SETOP_MOD_REGISTER_VAR32F);
	DO_INIT(SETOP_MOD_REGISTER_VAR32I);
	DO_INIT(SETOP_MOD_REGISTER_VAR32U);

	DO_INIT(SETOP_MOD_STATIC_VAR32F);
	DO_INIT(SETOP_MOD_STATIC_VAR32I);
	DO_INIT(SETOP_MOD_STATIC_VAR32U);

	DO_INIT(SETOP_MOD_WORLDARRAY_VAR32F);
	DO_INIT(SETOP_MOD_WORLDARRAY_VAR32I);
	DO_INIT(SETOP_MOD_WORLDARRAY_VAR32U);

	DO_INIT(SETOP_MOD_WORLDREGISTER_VAR32F);
	DO_INIT(SETOP_MOD_WORLDREGISTER_VAR32I);
	DO_INIT(SETOP_MOD_WORLDREGISTER_VAR32U);

	// MUL
	DO_INIT(SETOP_MUL_AUTO_VAR32F);
	DO_INIT(SETOP_MUL_AUTO_VAR32I);
	DO_INIT(SETOP_MUL_AUTO_VAR32U);

	DO_INIT(SETOP_MUL_GLOBALARRAY_VAR32F);
	DO_INIT(SETOP_MUL_GLOBALARRAY_VAR32I);
	DO_INIT(SETOP_MUL_GLOBALARRAY_VAR32U);

	DO_INIT(SETOP_MUL_GLOBALREGISTER_VAR32F);
	DO_INIT(SETOP_MUL_GLOBALREGISTER_VAR32I);
	DO_INIT(SETOP_MUL_GLOBALREGISTER_VAR32U);

	DO_INIT(SETOP_MUL_MAPARRAY_VAR32F);
	DO_INIT(SETOP_MUL_MAPARRAY_VAR32I);
	DO_INIT(SETOP_MUL_MAPARRAY_VAR32U);

	DO_INIT(SETOP_MUL_MAPREGISTER_VAR32F);
	DO_INIT(SETOP_MUL_MAPREGISTER_VAR32I);
	DO_INIT(SETOP_MUL_MAPREGISTER_VAR32U);

	DO_INIT(SETOP_MUL_POINTER_VAR32F);
	DO_INIT(SETOP_MUL_POINTER_VAR32I);
	DO_INIT(SETOP_MUL_POINTER_VAR32U);

	DO_INIT(SETOP_MUL_REGISTER_VAR32F);
	DO_INIT(SETOP_MUL_REGISTER_VAR32I);
	DO_INIT(SETOP_MUL_REGISTER_VAR32U);

	DO_INIT(SETOP_MUL_STATIC_VAR32F);
	DO_INIT(SETOP_MUL_STATIC_VAR32I);
	DO_INIT(SETOP_MUL_STATIC_VAR32U);

	DO_INIT(SETOP_MUL_WORLDARRAY_VAR32F);
	DO_INIT(SETOP_MUL_WORLDARRAY_VAR32I);
	DO_INIT(SETOP_MUL_WORLDARRAY_VAR32U);

	DO_INIT(SETOP_MUL_WORLDREGISTER_VAR32F);
	DO_INIT(SETOP_MUL_WORLDREGISTER_VAR32I);
	DO_INIT(SETOP_MUL_WORLDREGISTER_VAR32U);

	// SUB
	DO_INIT(SETOP_SUB_AUTO_VAR32F);
	DO_INIT(SETOP_SUB_AUTO_VAR32I);
	DO_INIT(SETOP_SUB_AUTO_VAR32U);

	DO_INIT(SETOP_SUB_GLOBALARRAY_VAR32F);
	DO_INIT(SETOP_SUB_GLOBALARRAY_VAR32I);
	DO_INIT(SETOP_SUB_GLOBALARRAY_VAR32U);

	DO_INIT(SETOP_SUB_GLOBALREGISTER_VAR32F);
	DO_INIT(SETOP_SUB_GLOBALREGISTER_VAR32I);
	DO_INIT(SETOP_SUB_GLOBALREGISTER_VAR32U);

	DO_INIT(SETOP_SUB_MAPARRAY_VAR32F);
	DO_INIT(SETOP_SUB_MAPARRAY_VAR32I);
	DO_INIT(SETOP_SUB_MAPARRAY_VAR32U);

	DO_INIT(SETOP_SUB_MAPREGISTER_VAR32F);
	DO_INIT(SETOP_SUB_MAPREGISTER_VAR32I);
	DO_INIT(SETOP_SUB_MAPREGISTER_VAR32U);

	DO_INIT(SETOP_SUB_POINTER_VAR32F);
	DO_INIT(SETOP_SUB_POINTER_VAR32I);
	DO_INIT(SETOP_SUB_POINTER_VAR32U);

	DO_INIT(SETOP_SUB_REGISTER_VAR32F);
	DO_INIT(SETOP_SUB_REGISTER_VAR32I);
	DO_INIT(SETOP_SUB_REGISTER_VAR32U);

	DO_INIT(SETOP_SUB_STATIC_VAR32F);
	DO_INIT(SETOP_SUB_STATIC_VAR32I);
	DO_INIT(SETOP_SUB_STATIC_VAR32U);

	DO_INIT(SETOP_SUB_WORLDARRAY_VAR32F);
	DO_INIT(SETOP_SUB_WORLDARRAY_VAR32I);
	DO_INIT(SETOP_SUB_WORLDARRAY_VAR32U);

	DO_INIT(SETOP_SUB_WORLDREGISTER_VAR32F);
	DO_INIT(SETOP_SUB_WORLDREGISTER_VAR32I);
	DO_INIT(SETOP_SUB_WORLDREGISTER_VAR32U);


	// Miscellaneous

	DO_INIT(MISC_DELAY32);
	DO_INIT(MISC_DELAY32_IMM);

	DO_INIT(MISC_NATIVE);

	DO_INIT(MISC_NEGATE32F);
	DO_INIT(MISC_NEGATE32I);

	DO_INIT(MISC_RANDOM32F);
	DO_INIT(MISC_RANDOM32F_IMM);
	DO_INIT(MISC_RANDOM32I);
	DO_INIT(MISC_RANDOM32I_IMM);
	DO_INIT(MISC_RANDOM32U);
	DO_INIT(MISC_RANDOM32U_IMM);

	DO_INIT(MISC_STRLEN);


	// ACS

	DO_INIT(ACS_LINESPEC1);
	DO_INIT(ACS_LINESPEC1_IMM);
	DO_INIT(ACS_LINESPEC2);
	DO_INIT(ACS_LINESPEC2_IMM);
	DO_INIT(ACS_LINESPEC3);
	DO_INIT(ACS_LINESPEC3_IMM);
	DO_INIT(ACS_LINESPEC4);
	DO_INIT(ACS_LINESPEC4_IMM);
	DO_INIT(ACS_LINESPEC5);
	DO_INIT(ACS_LINESPEC5_IMM);
	DO_INIT(ACS_SCRIPT_RESTART);
	DO_INIT(ACS_SCRIPT_SUSPEND);
	DO_INIT(ACS_SCRIPT_TERMINATE);
	DO_INIT(ACS_THINGCOUNT);
	DO_INIT(ACS_THINGCOUNT_IMM);
	DO_INIT(ACS_TIMER);


	// ACS Common Extensions


	// ACS Extensions

	DO_INIT(ACSE_CALLFUNC_IMM);
	DO_INIT(ACSE_CALLFUNCVOID_IMM);

	DO_INIT(ACSE_GET_ACTIVATOR_MID);

	DO_INIT(ACSE_GET_ACTIVATOR_TID);

	DO_INIT(ACSE_GET_AMMO_CAPACITY);

	DO_INIT(ACSE_GET_TID_ANGLE);
	DO_INIT(ACSE_GET_TID_PITCH);
	DO_INIT(ACSE_GET_TID_X);
	DO_INIT(ACSE_GET_TID_Y);
	DO_INIT(ACSE_GET_TID_Z);

	DO_INIT(ACSE_INVENTORY_CHECK);
	DO_INIT(ACSE_INVENTORY_CHECK_IMM);
	DO_INIT(ACSE_INVENTORY_GIVE);
	DO_INIT(ACSE_INVENTORY_GIVE_IMM);
	DO_INIT(ACSE_INVENTORY_TAKE);
	DO_INIT(ACSE_INVENTORY_TAKE_IMM);

	DO_INIT(ACSE_LINESPEC5RESULT);

	DO_INIT(ACSE_PLAYERNUMBER);

	DO_INIT(ACSE_RETNFUNC);
	DO_INIT(ACSE_RETNFUNCVOID);

	DO_INIT(ACSE_SCRIPT_RETURN);
	DO_INIT(ACSE_SCRIPT_SETRETURN);

	DO_INIT(ACSE_SET_ACTIVATOR_MID);

	DO_INIT(ACSE_SET_AMMO_CAPACITY);

	DO_INIT(ACSE_SET_TID_ANGLE);
	DO_INIT(ACSE_SET_TID_PITCH);
	DO_INIT(ACSE_SET_TID_POSITION);

	DO_INIT(ACSE_SPAWN);
	DO_INIT(ACSE_SPAWN_IMM);
	DO_INIT(ACSE_SPAWN_PROJECTILE);


	// ACS Printing

	DO_INIT(ACSP_CHARACTER);

	DO_INIT(ACSP_END);
	DO_INIT(ACSP_END_BOLD);
	DO_INIT(ACSP_END_ERROR);
	DO_INIT(ACSP_END_LOG);

	DO_INIT(ACSP_NUM_DEC32F);
	DO_INIT(ACSP_NUM_DEC32I);
	DO_INIT(ACSP_NUM_DEC32U);

	DO_INIT(ACSP_NUM_HEX32F);
	DO_INIT(ACSP_NUM_HEX32I);
	DO_INIT(ACSP_NUM_HEX32U);

	DO_INIT(ACSP_START);

	DO_INIT(ACSP_STRING);



	DO_INIT(NONE);

	#undef DO_INIT
}



ObjectCodeSet::ObjectCodeSet() : ocode(OCODE_NONE), ocode_imm(OCODE_NONE)
{

}



ObjectCode ocode_get_code(SourceTokenC const & token)
{
	return ocode_get_code(token.getData(), token.getPosition());
}
ObjectCode ocode_get_code(std::string const & data, SourcePosition const & position)
{
	std::map<std::string, ObjectCode>::iterator codeIt(_ocode_map.find(data));

	if (codeIt == _ocode_map.end())
		throw SourceException("no such OCODE '" + data + "'", position, "ObjectToken");

	return codeIt->second;
}

bool ocode_is_push_noarg(ObjectCode ocode)
{
	switch (ocode)
	{
	// ADDRESS.
	case OCODE_ADDR_STACK_VAR:
	// ARRAY.
	case OCODE_GET_GLOBALARRAYTEMP_VAR32F:
	case OCODE_GET_GLOBALARRAYTEMP_VAR32I:
	case OCODE_GET_MAPARRAYTEMP_VAR32F:
	case OCODE_GET_MAPARRAYTEMP_VAR32I:
	case OCODE_GET_WORLDARRAYTEMP_VAR32F:
	case OCODE_GET_WORLDARRAYTEMP_VAR32I:
	// LITERAL.
	case OCODE_GET_LITERAL32F:
	case OCODE_GET_LITERAL32I:
	// POINTER.
	case OCODE_GET_POINTER_VAR32F:
	case OCODE_GET_POINTER_VAR32I:
	// VAR.
	case OCODE_GET_AUTO_VAR32F:
	case OCODE_GET_AUTO_VAR32I:
	case OCODE_GET_GLOBALREGISTER_VAR32F:
	case OCODE_GET_GLOBALREGISTER_VAR32I:
	case OCODE_GET_MAPREGISTER_VAR32F:
	case OCODE_GET_MAPREGISTER_VAR32I:
	case OCODE_GET_REGISTER_VAR32F:
	case OCODE_GET_REGISTER_VAR32I:
	case OCODE_GET_STATIC_VAR32F:
	case OCODE_GET_STATIC_VAR32I:
	case OCODE_GET_WORLDREGISTER_VAR32F:
	case OCODE_GET_WORLDREGISTER_VAR32I:
		return true;

	default:
		return false;
	}
}

char const * make_string(ObjectCode ocode)
{
	if (ocode <= OCODE_NONE)
		return _ocode_str[ocode];
	else
		return "OCODE";
}

void print_debug(std::ostream * out, ObjectCode in)
{
	if (in <= OCODE_NONE)
		*out << _ocode_str[in];
	else
		*out << "OCODE";
}
void print_debug(std::ostream * out, ObjectCodeSet const & in)
{
	*out << "ObjectCodeSet(";
		*out << "ocode=(";
		print_debug(out, in.ocode);
		*out << ")";

		*out << ", ";

		*out << "ocode_imm=(";
		print_debug(out, in.ocode_imm);
		*out << ")";
	*out << ")";
}

void read_object(std::istream * in, ObjectCode * out)
{
	read_object_raw(in, (char *)out, sizeof(*out));

	if (*out > OCODE_NONE)
		*out = OCODE_NONE;
}
void read_object(std::istream * in, ObjectCodeSet * out)
{
	read_object(in, &out->ocode);
	read_object(in, &out->ocode_imm);
}

void write_object(std::ostream * out, ObjectCode const & in)
{
	write_object_raw(out, (char const *)&in, sizeof(in));
}
void write_object(std::ostream * out, ObjectCodeSet const & in)
{
	write_object(out, in.ocode);
	write_object(out, in.ocode_imm);
}



