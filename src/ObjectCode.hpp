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

/* ObjectCode.hpp
**
** Defines the ObjectCode enum and related functions.
*/

#ifndef HPP_ObjectCode_
#define HPP_ObjectCode_

#include <ostream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class SourcePosition;
class SourceTokenC;

enum ObjectCode
{
   OCODE_NOP,

   // =========================================================================
   // Arithmetic                                                              |

   OCODE_ADD32F,
   OCODE_ADD32I,
   OCODE_ADD32U,

   OCODE_DIV32F,
   OCODE_DIV32I,
   OCODE_DIV32U,

   OCODE_MOD32F,
   OCODE_MOD32I,
   OCODE_MOD32U,

   OCODE_MUL32F,
   OCODE_MUL32I,
   OCODE_MUL32U,

   OCODE_SUB32F,
   OCODE_SUB32I,
   OCODE_SUB32U,


   // =========================================================================
   // Bitwise                                                                 |

   OCODE_BITWISE_AND32,
   OCODE_BITWISE_IOR32,
   OCODE_BITWISE_NOT32,
   OCODE_BITWISE_SHIFTL32,
   OCODE_BITWISE_SHIFTR32,
   OCODE_BITWISE_XOR32,


   // =========================================================================
   // Branching                                                               |

   OCODE_BRANCH_CASE,
   OCODE_BRANCH_GOTO,
   OCODE_BRANCH_GOTO_IMM,
   OCODE_BRANCH_TABLE,
   OCODE_BRANCH_TRUE,
   OCODE_BRANCH_ZERO,


   // =========================================================================
   // Comparison                                                              |

   OCODE_CMP_EQ32F,
   OCODE_CMP_EQ32I,

   OCODE_CMP_GE32F,
   OCODE_CMP_GE32I,
   OCODE_CMP_GE32U,

   OCODE_CMP_GT32F,
   OCODE_CMP_GT32I,
   OCODE_CMP_GT32U,

   OCODE_CMP_LE32F,
   OCODE_CMP_LE32I,
   OCODE_CMP_LE32U,

   OCODE_CMP_LT32F,
   OCODE_CMP_LT32I,
   OCODE_CMP_LT32U,

   OCODE_CMP_NE32F,
   OCODE_CMP_NE32I,


   // =========================================================================
   // Conversion                                                              |

   OCODE_CONVERT_32F_32I,
   OCODE_CONVERT_32I_32F,


   // =========================================================================
   // Logical                                                                 |

   OCODE_LOGICAL_AND32F,
   OCODE_LOGICAL_AND32I,

   OCODE_LOGICAL_IOR32F,
   OCODE_LOGICAL_IOR32I,

   OCODE_LOGICAL_NOT32F,
   OCODE_LOGICAL_NOT32I,

   OCODE_LOGICAL_XOR32F,
   OCODE_LOGICAL_XOR32I,


   // =========================================================================
   // Stack-ops                                                               |

   OCODE_STACK_DROP32,
   OCODE_STACK_DUP32,
   OCODE_STACK_SWAP32,


   // =========================================================================
   // Trigonometry                                                            |

   OCODE_TRIG_COS32F,
   OCODE_TRIG_SIN32F,


   // =========================================================================
   // Variable Address                                                        |

   OCODE_ADDR_STACK_ADD,
   OCODE_ADDR_STACK_ADD_IMM,

   OCODE_ADDR_STACK_SUB,
   OCODE_ADDR_STACK_SUB_IMM,

   /* a:offset s:->addr */
   OCODE_ADDR_STACK_VAR,


   // =========================================================================
   // Variable Get                                                            |

   OCODE_GET_AUTO_VAR32F,
   OCODE_GET_AUTO_VAR32I,

   /* a:array s:addr->value */
   OCODE_GET_GLOBALARRAY_VAR32F,
   OCODE_GET_GLOBALARRAY_VAR32I,

   OCODE_GET_GLOBALREGISTER_VAR32F,
   OCODE_GET_GLOBALREGISTER_VAR32I,

   OCODE_GET_LITERAL32F,
   OCODE_GET_LITERAL32I,

   /* a:array s:addr->value */
   OCODE_GET_MAPARRAY_VAR32F,
   OCODE_GET_MAPARRAY_VAR32I,

   OCODE_GET_MAPREGISTER_VAR32F,
   OCODE_GET_MAPREGISTER_VAR32I,

   /* s:mid,var->val */
   OCODE_GET_MOBJ_VAR32F,
   OCODE_GET_MOBJ_VAR32I,

   /* a:offset s:addr->value */
   OCODE_GET_POINTER_VAR32F,
   OCODE_GET_POINTER_VAR32I,

   OCODE_GET_REGISTER_VAR32F,
   OCODE_GET_REGISTER_VAR32I,

   OCODE_GET_STATIC_VAR32F,
   OCODE_GET_STATIC_VAR32I,

   OCODE_GET_TEMP_VAR,

   /* s:tid,var->val */
   OCODE_GET_TID_VAR32F,
   OCODE_GET_TID_VAR32I,

   /* a:array s:addr->value */
   OCODE_GET_WORLDARRAY_VAR32F,
   OCODE_GET_WORLDARRAY_VAR32I,

   OCODE_GET_WORLDREGISTER_VAR32F,
   OCODE_GET_WORLDREGISTER_VAR32I,


   // =========================================================================
   // Variable Set                                                            |

   OCODE_SET_AUTO_VAR32F,
   OCODE_SET_AUTO_VAR32I,

   /* a:array s:addr,value */
   OCODE_SET_GLOBALARRAY_VAR32F,
   OCODE_SET_GLOBALARRAY_VAR32I,

   OCODE_SET_GLOBALREGISTER_VAR32F,
   OCODE_SET_GLOBALREGISTER_VAR32I,

   /* a:array s:addr,value */
   OCODE_SET_MAPARRAY_VAR32F,
   OCODE_SET_MAPARRAY_VAR32I,

   OCODE_SET_MAPREGISTER_VAR32F,
   OCODE_SET_MAPREGISTER_VAR32I,

   /* s:mid,var,val */
   OCODE_SET_MOBJ_VAR32F,
   OCODE_SET_MOBJ_VAR32I,

   /* a:offset s:value,addr */
   OCODE_SET_POINTER_VAR32F,
   OCODE_SET_POINTER_VAR32I,

   OCODE_SET_REGISTER_VAR32F,
   OCODE_SET_REGISTER_VAR32I,

   OCODE_SET_STATIC_VAR32F,
   OCODE_SET_STATIC_VAR32I,

   OCODE_SET_TEMP_VAR,

   /* s:tid,var,val */
   OCODE_SET_TID_VAR32F,
   OCODE_SET_TID_VAR32I,

   /* a:array s:addr,value */
   OCODE_SET_WORLDARRAY_VAR32F,
   OCODE_SET_WORLDARRAY_VAR32I,

   OCODE_SET_WORLDREGISTER_VAR32F,
   OCODE_SET_WORLDREGISTER_VAR32I,


   // =========================================================================
   // Variable Set Op                                                         |

   // ------------------------------------------------------------------------|
   // ADD                                                                     |

   OCODE_SETOP_ADD_AUTO_VAR32F,
   OCODE_SETOP_ADD_AUTO_VAR32I,
   OCODE_SETOP_ADD_AUTO_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_ADD_GLOBALARRAY_VAR32F,
   OCODE_SETOP_ADD_GLOBALARRAY_VAR32I,
   OCODE_SETOP_ADD_GLOBALARRAY_VAR32U,

   OCODE_SETOP_ADD_GLOBALREGISTER_VAR32F,
   OCODE_SETOP_ADD_GLOBALREGISTER_VAR32I,
   OCODE_SETOP_ADD_GLOBALREGISTER_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_ADD_MAPARRAY_VAR32F,
   OCODE_SETOP_ADD_MAPARRAY_VAR32I,
   OCODE_SETOP_ADD_MAPARRAY_VAR32U,

   OCODE_SETOP_ADD_MAPREGISTER_VAR32F,
   OCODE_SETOP_ADD_MAPREGISTER_VAR32I,
   OCODE_SETOP_ADD_MAPREGISTER_VAR32U,

   /* a:offset s:addr,value */
   OCODE_SETOP_ADD_POINTER_VAR32F,
   OCODE_SETOP_ADD_POINTER_VAR32I,
   OCODE_SETOP_ADD_POINTER_VAR32U,

   OCODE_SETOP_ADD_REGISTER_VAR32F,
   OCODE_SETOP_ADD_REGISTER_VAR32I,
   OCODE_SETOP_ADD_REGISTER_VAR32U,

   OCODE_SETOP_ADD_STATIC_VAR32F,
   OCODE_SETOP_ADD_STATIC_VAR32I,
   OCODE_SETOP_ADD_STATIC_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_ADD_WORLDARRAY_VAR32F,
   OCODE_SETOP_ADD_WORLDARRAY_VAR32I,
   OCODE_SETOP_ADD_WORLDARRAY_VAR32U,

   OCODE_SETOP_ADD_WORLDREGISTER_VAR32F,
   OCODE_SETOP_ADD_WORLDREGISTER_VAR32I,
   OCODE_SETOP_ADD_WORLDREGISTER_VAR32U,

   // ------------------------------------------------------------------------|
   // DEC                                                                     |

   OCODE_SETOP_DEC_AUTO_VAR32F,
   OCODE_SETOP_DEC_AUTO_VAR32I,
   OCODE_SETOP_DEC_AUTO_VAR32U,

   /* a:array s:addr */
   OCODE_SETOP_DEC_GLOBALARRAY_VAR32F,
   OCODE_SETOP_DEC_GLOBALARRAY_VAR32I,
   OCODE_SETOP_DEC_GLOBALARRAY_VAR32U,

   OCODE_SETOP_DEC_GLOBALREGISTER_VAR32F,
   OCODE_SETOP_DEC_GLOBALREGISTER_VAR32I,
   OCODE_SETOP_DEC_GLOBALREGISTER_VAR32U,

   /* a:array s:addr */
   OCODE_SETOP_DEC_MAPARRAY_VAR32F,
   OCODE_SETOP_DEC_MAPARRAY_VAR32I,
   OCODE_SETOP_DEC_MAPARRAY_VAR32U,

   OCODE_SETOP_DEC_MAPREGISTER_VAR32F,
   OCODE_SETOP_DEC_MAPREGISTER_VAR32I,
   OCODE_SETOP_DEC_MAPREGISTER_VAR32U,

   /* a:offset s:addr */
   OCODE_SETOP_DEC_POINTER_VAR32F,
   OCODE_SETOP_DEC_POINTER_VAR32I,
   OCODE_SETOP_DEC_POINTER_VAR32U,

   OCODE_SETOP_DEC_REGISTER_VAR32F,
   OCODE_SETOP_DEC_REGISTER_VAR32I,
   OCODE_SETOP_DEC_REGISTER_VAR32U,

   OCODE_SETOP_DEC_STATIC_VAR32F,
   OCODE_SETOP_DEC_STATIC_VAR32I,
   OCODE_SETOP_DEC_STATIC_VAR32U,

   /* a:array s:addr */
   OCODE_SETOP_DEC_WORLDARRAY_VAR32F,
   OCODE_SETOP_DEC_WORLDARRAY_VAR32I,
   OCODE_SETOP_DEC_WORLDARRAY_VAR32U,

   OCODE_SETOP_DEC_WORLDREGISTER_VAR32F,
   OCODE_SETOP_DEC_WORLDREGISTER_VAR32I,
   OCODE_SETOP_DEC_WORLDREGISTER_VAR32U,

   // ------------------------------------------------------------------------|
   // DIV                                                                     |

   OCODE_SETOP_DIV_AUTO_VAR32F,
   OCODE_SETOP_DIV_AUTO_VAR32I,
   OCODE_SETOP_DIV_AUTO_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_DIV_GLOBALARRAY_VAR32F,
   OCODE_SETOP_DIV_GLOBALARRAY_VAR32I,
   OCODE_SETOP_DIV_GLOBALARRAY_VAR32U,

   OCODE_SETOP_DIV_GLOBALREGISTER_VAR32F,
   OCODE_SETOP_DIV_GLOBALREGISTER_VAR32I,
   OCODE_SETOP_DIV_GLOBALREGISTER_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_DIV_MAPARRAY_VAR32F,
   OCODE_SETOP_DIV_MAPARRAY_VAR32I,
   OCODE_SETOP_DIV_MAPARRAY_VAR32U,

   OCODE_SETOP_DIV_MAPREGISTER_VAR32F,
   OCODE_SETOP_DIV_MAPREGISTER_VAR32I,
   OCODE_SETOP_DIV_MAPREGISTER_VAR32U,

   /* a:offset s:addr,value */
   OCODE_SETOP_DIV_POINTER_VAR32F,
   OCODE_SETOP_DIV_POINTER_VAR32I,
   OCODE_SETOP_DIV_POINTER_VAR32U,

   OCODE_SETOP_DIV_REGISTER_VAR32F,
   OCODE_SETOP_DIV_REGISTER_VAR32I,
   OCODE_SETOP_DIV_REGISTER_VAR32U,

   OCODE_SETOP_DIV_STATIC_VAR32F,
   OCODE_SETOP_DIV_STATIC_VAR32I,
   OCODE_SETOP_DIV_STATIC_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_DIV_WORLDARRAY_VAR32F,
   OCODE_SETOP_DIV_WORLDARRAY_VAR32I,
   OCODE_SETOP_DIV_WORLDARRAY_VAR32U,

   OCODE_SETOP_DIV_WORLDREGISTER_VAR32F,
   OCODE_SETOP_DIV_WORLDREGISTER_VAR32I,
   OCODE_SETOP_DIV_WORLDREGISTER_VAR32U,

   // ------------------------------------------------------------------------|
   // INC                                                                     |

   OCODE_SETOP_INC_AUTO_VAR32F,
   OCODE_SETOP_INC_AUTO_VAR32I,
   OCODE_SETOP_INC_AUTO_VAR32U,

   /* a:array s:addr */
   OCODE_SETOP_INC_GLOBALARRAY_VAR32F,
   OCODE_SETOP_INC_GLOBALARRAY_VAR32I,
   OCODE_SETOP_INC_GLOBALARRAY_VAR32U,

   OCODE_SETOP_INC_GLOBALREGISTER_VAR32F,
   OCODE_SETOP_INC_GLOBALREGISTER_VAR32I,
   OCODE_SETOP_INC_GLOBALREGISTER_VAR32U,

   /* a:array s:addr */
   OCODE_SETOP_INC_MAPARRAY_VAR32F,
   OCODE_SETOP_INC_MAPARRAY_VAR32I,
   OCODE_SETOP_INC_MAPARRAY_VAR32U,

   OCODE_SETOP_INC_MAPREGISTER_VAR32F,
   OCODE_SETOP_INC_MAPREGISTER_VAR32I,
   OCODE_SETOP_INC_MAPREGISTER_VAR32U,

   /* a:offset s:addr */
   OCODE_SETOP_INC_POINTER_VAR32F,
   OCODE_SETOP_INC_POINTER_VAR32I,
   OCODE_SETOP_INC_POINTER_VAR32U,

   OCODE_SETOP_INC_REGISTER_VAR32F,
   OCODE_SETOP_INC_REGISTER_VAR32I,
   OCODE_SETOP_INC_REGISTER_VAR32U,

   OCODE_SETOP_INC_STATIC_VAR32F,
   OCODE_SETOP_INC_STATIC_VAR32I,
   OCODE_SETOP_INC_STATIC_VAR32U,

   /* a:array s:addr */
   OCODE_SETOP_INC_WORLDARRAY_VAR32F,
   OCODE_SETOP_INC_WORLDARRAY_VAR32I,
   OCODE_SETOP_INC_WORLDARRAY_VAR32U,

   OCODE_SETOP_INC_WORLDREGISTER_VAR32F,
   OCODE_SETOP_INC_WORLDREGISTER_VAR32I,
   OCODE_SETOP_INC_WORLDREGISTER_VAR32U,

   // ------------------------------------------------------------------------|
   // MOD                                                                     |

   OCODE_SETOP_MOD_AUTO_VAR32F,
   OCODE_SETOP_MOD_AUTO_VAR32I,
   OCODE_SETOP_MOD_AUTO_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_MOD_GLOBALARRAY_VAR32F,
   OCODE_SETOP_MOD_GLOBALARRAY_VAR32I,
   OCODE_SETOP_MOD_GLOBALARRAY_VAR32U,

   OCODE_SETOP_MOD_GLOBALREGISTER_VAR32F,
   OCODE_SETOP_MOD_GLOBALREGISTER_VAR32I,
   OCODE_SETOP_MOD_GLOBALREGISTER_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_MOD_MAPARRAY_VAR32F,
   OCODE_SETOP_MOD_MAPARRAY_VAR32I,
   OCODE_SETOP_MOD_MAPARRAY_VAR32U,

   OCODE_SETOP_MOD_MAPREGISTER_VAR32F,
   OCODE_SETOP_MOD_MAPREGISTER_VAR32I,
   OCODE_SETOP_MOD_MAPREGISTER_VAR32U,

   /* a:offset s:addr,value */
   OCODE_SETOP_MOD_POINTER_VAR32F,
   OCODE_SETOP_MOD_POINTER_VAR32I,
   OCODE_SETOP_MOD_POINTER_VAR32U,

   OCODE_SETOP_MOD_REGISTER_VAR32F,
   OCODE_SETOP_MOD_REGISTER_VAR32I,
   OCODE_SETOP_MOD_REGISTER_VAR32U,

   OCODE_SETOP_MOD_STATIC_VAR32F,
   OCODE_SETOP_MOD_STATIC_VAR32I,
   OCODE_SETOP_MOD_STATIC_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_MOD_WORLDARRAY_VAR32F,
   OCODE_SETOP_MOD_WORLDARRAY_VAR32I,
   OCODE_SETOP_MOD_WORLDARRAY_VAR32U,

   OCODE_SETOP_MOD_WORLDREGISTER_VAR32F,
   OCODE_SETOP_MOD_WORLDREGISTER_VAR32I,
   OCODE_SETOP_MOD_WORLDREGISTER_VAR32U,

   // ------------------------------------------------------------------------|
   // MUL                                                                     |

   OCODE_SETOP_MUL_AUTO_VAR32F,
   OCODE_SETOP_MUL_AUTO_VAR32I,
   OCODE_SETOP_MUL_AUTO_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_MUL_GLOBALARRAY_VAR32F,
   OCODE_SETOP_MUL_GLOBALARRAY_VAR32I,
   OCODE_SETOP_MUL_GLOBALARRAY_VAR32U,

   OCODE_SETOP_MUL_GLOBALREGISTER_VAR32F,
   OCODE_SETOP_MUL_GLOBALREGISTER_VAR32I,
   OCODE_SETOP_MUL_GLOBALREGISTER_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_MUL_MAPARRAY_VAR32F,
   OCODE_SETOP_MUL_MAPARRAY_VAR32I,
   OCODE_SETOP_MUL_MAPARRAY_VAR32U,

   OCODE_SETOP_MUL_MAPREGISTER_VAR32F,
   OCODE_SETOP_MUL_MAPREGISTER_VAR32I,
   OCODE_SETOP_MUL_MAPREGISTER_VAR32U,

   /* a:offset s:addr,value */
   OCODE_SETOP_MUL_POINTER_VAR32F,
   OCODE_SETOP_MUL_POINTER_VAR32I,
   OCODE_SETOP_MUL_POINTER_VAR32U,

   OCODE_SETOP_MUL_REGISTER_VAR32F,
   OCODE_SETOP_MUL_REGISTER_VAR32I,
   OCODE_SETOP_MUL_REGISTER_VAR32U,

   OCODE_SETOP_MUL_STATIC_VAR32F,
   OCODE_SETOP_MUL_STATIC_VAR32I,
   OCODE_SETOP_MUL_STATIC_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_MUL_WORLDARRAY_VAR32F,
   OCODE_SETOP_MUL_WORLDARRAY_VAR32I,
   OCODE_SETOP_MUL_WORLDARRAY_VAR32U,

   OCODE_SETOP_MUL_WORLDREGISTER_VAR32F,
   OCODE_SETOP_MUL_WORLDREGISTER_VAR32I,
   OCODE_SETOP_MUL_WORLDREGISTER_VAR32U,

   // ------------------------------------------------------------------------|
   // SUB                                                                     |

   OCODE_SETOP_SUB_AUTO_VAR32F,
   OCODE_SETOP_SUB_AUTO_VAR32I,
   OCODE_SETOP_SUB_AUTO_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_SUB_GLOBALARRAY_VAR32F,
   OCODE_SETOP_SUB_GLOBALARRAY_VAR32I,
   OCODE_SETOP_SUB_GLOBALARRAY_VAR32U,

   OCODE_SETOP_SUB_GLOBALREGISTER_VAR32F,
   OCODE_SETOP_SUB_GLOBALREGISTER_VAR32I,
   OCODE_SETOP_SUB_GLOBALREGISTER_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_SUB_MAPARRAY_VAR32F,
   OCODE_SETOP_SUB_MAPARRAY_VAR32I,
   OCODE_SETOP_SUB_MAPARRAY_VAR32U,

   OCODE_SETOP_SUB_MAPREGISTER_VAR32F,
   OCODE_SETOP_SUB_MAPREGISTER_VAR32I,
   OCODE_SETOP_SUB_MAPREGISTER_VAR32U,

   /* a:offset s:addr,value */
   OCODE_SETOP_SUB_POINTER_VAR32F,
   OCODE_SETOP_SUB_POINTER_VAR32I,
   OCODE_SETOP_SUB_POINTER_VAR32U,

   OCODE_SETOP_SUB_REGISTER_VAR32F,
   OCODE_SETOP_SUB_REGISTER_VAR32I,
   OCODE_SETOP_SUB_REGISTER_VAR32U,

   OCODE_SETOP_SUB_STATIC_VAR32F,
   OCODE_SETOP_SUB_STATIC_VAR32I,
   OCODE_SETOP_SUB_STATIC_VAR32U,

   /* a:array s:addr,value */
   OCODE_SETOP_SUB_WORLDARRAY_VAR32F,
   OCODE_SETOP_SUB_WORLDARRAY_VAR32I,
   OCODE_SETOP_SUB_WORLDARRAY_VAR32U,

   OCODE_SETOP_SUB_WORLDREGISTER_VAR32F,
   OCODE_SETOP_SUB_WORLDREGISTER_VAR32I,
   OCODE_SETOP_SUB_WORLDREGISTER_VAR32U,


   // =========================================================================
   // Miscellaneous                                                           |

   OCODE_MISC_DELAY32,
   OCODE_MISC_DELAY32_IMM,

   /* target-dependent */
   OCODE_MISC_NATIVE,

   OCODE_MISC_NEGATE32F,
   OCODE_MISC_NEGATE32I,

   OCODE_MISC_RANDOM32F,
   OCODE_MISC_RANDOM32F_IMM,
   OCODE_MISC_RANDOM32I,
   OCODE_MISC_RANDOM32I_IMM,
   OCODE_MISC_RANDOM32U,
   OCODE_MISC_RANDOM32U_IMM,

   OCODE_MISC_STRLEN,


   // =========================================================================
   // ACS                                                                     |

   OCODE_ACS_GAME_SKILL,
   OCODE_ACS_GAME_TIMER,
   OCODE_ACS_GAME_TYPE,
   OCODE_ACS_LINE_BLOCK_SET,
   OCODE_ACS_LINE_SIDE,
   OCODE_ACS_LINE_SPEC_CLEAR,
   OCODE_ACS_LINE_SPEC_EXEC1,
   OCODE_ACS_LINE_SPEC_EXEC1_IMM,
   OCODE_ACS_LINE_SPEC_EXEC2,
   OCODE_ACS_LINE_SPEC_EXEC2_IMM,
   OCODE_ACS_LINE_SPEC_EXEC3,
   OCODE_ACS_LINE_SPEC_EXEC3_IMM,
   OCODE_ACS_LINE_SPEC_EXEC4,
   OCODE_ACS_LINE_SPEC_EXEC4_IMM,
   OCODE_ACS_LINE_SPEC_EXEC5,
   OCODE_ACS_LINE_SPEC_EXEC5_IMM,
   OCODE_ACS_LINE_SPEC_SET,
   OCODE_ACS_PLAYER_COUNT,
   OCODE_ACS_SCRIPT_RESTART,
   OCODE_ACS_SCRIPT_SUSPEND,
   OCODE_ACS_SCRIPT_TERMINATE,
   OCODE_ACS_SOUND_AMBIENT,
   OCODE_ACS_SOUND_SECTOR,
   OCODE_ACS_SOUND_SEQUENCE,
   OCODE_ACS_SOUND_THING,
   OCODE_ACS_TEXTURE_SET_CEILING,
   OCODE_ACS_TEXTURE_SET_CEILING_IMM,
   OCODE_ACS_TEXTURE_SET_FLOOR,
   OCODE_ACS_TEXTURE_SET_FLOOR_IMM,
   OCODE_ACS_TEXTURE_SET_LINE,
   OCODE_ACS_THING_COUNT,
   OCODE_ACS_THING_COUNT_IMM,
   OCODE_ACS_WAIT_POLYOBJECT,
   OCODE_ACS_WAIT_POLYOBJECT_IMM,
   OCODE_ACS_WAIT_SCRIPT,
   OCODE_ACS_WAIT_SCRIPT_IMM,
   OCODE_ACS_WAIT_SECTOR,
   OCODE_ACS_WAIT_SECTOR_IMM,


   // =========================================================================
   // ACS Common Extensions                                                   |


   // =========================================================================
   // ACS Extensions                                                          |

   OCODE_ACSE_CALLFUNC_IMM,
   OCODE_ACSE_CALLFUNCVOID_IMM,

   /* s:->mid */
   OCODE_ACSE_GET_ACTIVATOR_MID,

   /* s:->tid */
   OCODE_ACSE_GET_ACTIVATOR_TID,

   OCODE_ACSE_GET_AMMO_CAPACITY,

   OCODE_ACSE_GET_TID_ANGLE,
   OCODE_ACSE_GET_TID_PITCH,
   OCODE_ACSE_GET_TID_X,
   OCODE_ACSE_GET_TID_Y,
   OCODE_ACSE_GET_TID_Z,

   OCODE_ACSE_INVENTORY_CHECK,
   OCODE_ACSE_INVENTORY_CHECK_IMM,
   OCODE_ACSE_INVENTORY_GIVE,
   OCODE_ACSE_INVENTORY_GIVE_IMM,
   OCODE_ACSE_INVENTORY_TAKE,
   OCODE_ACSE_INVENTORY_TAKE_IMM,

   OCODE_ACSE_LINESPEC5RESULT,

   OCODE_ACSE_PLAYERNUMBER,

   OCODE_ACSE_RETNFUNC,
   OCODE_ACSE_RETNFUNCVOID,

   OCODE_ACSE_SCRIPT_RETURN,
   OCODE_ACSE_SCRIPT_SETRETURN,

   /* s:mid */
   OCODE_ACSE_SET_ACTIVATOR_MID,

   OCODE_ACSE_SET_AMMO_CAPACITY,

   OCODE_ACSE_SET_TID_ANGLE,
   OCODE_ACSE_SET_TID_PITCH,
   OCODE_ACSE_SET_TID_POSITION,

   OCODE_ACSE_SPAWN,
   OCODE_ACSE_SPAWN_IMM,
   OCODE_ACSE_SPAWN_PROJECTILE,


   // =========================================================================
   // ACS Printing                                                            |

   OCODE_ACSP_CHARACTER,

   OCODE_ACSP_END,
   OCODE_ACSP_END_BOLD,
   /* noreturn */
   OCODE_ACSP_END_ERROR,
   OCODE_ACSP_END_LOG,

   OCODE_ACSP_NUM_DEC32F,
   OCODE_ACSP_NUM_DEC32I,
   OCODE_ACSP_NUM_DEC32U,

   OCODE_ACSP_NUM_HEX32F,
   OCODE_ACSP_NUM_HEX32I,
   OCODE_ACSP_NUM_HEX32U,

   OCODE_ACSP_START,

   OCODE_ACSP_STRING,



   OCODE_NONE
};

struct ObjectCodeSet
{
   ObjectCodeSet();

   ObjectCode ocode;
   ObjectCode ocode_imm;
};


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

ObjectCode ocode_get_code(SourceTokenC const & token);
ObjectCode ocode_get_code(std::string const & data, SourcePosition const & position);

// Returns true if the passed ocode pushes something to the stack without taking
// anything.
bool ocode_is_push_noarg(ObjectCode ocode);

char const * make_string(ObjectCode ocode);

void read_object(std::istream * in, ObjectCode * out);
void read_object(std::istream * in, ObjectCodeSet * out);

void write_object(std::ostream * out, ObjectCode const & in);
void write_object(std::ostream * out, ObjectCodeSet const & in);

#endif /* HPP_ObjectCode_ */

