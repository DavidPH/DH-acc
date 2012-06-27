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
// Re-usable header that invokes DO_INIT for every OCODE except OCODE_NONE.
//
//-----------------------------------------------------------------------------

#define DO_INIT_OP_SIZE(OP) \
   DO_INIT(OP)

#define DO_INIT_OP_TYPE(OP) \
   DO_INIT_OP_SIZE(OP##_I)  \
   DO_INIT_OP_SIZE(OP##_U)  \
   DO_INIT_OP_SIZE(OP##_X)

#define DO_INIT_OP_AREA(OP)     \
   DO_INIT_OP_TYPE(OP##_STK)    \
   DO_INIT_OP_TYPE(OP##_STATIC) \
   DO_INIT_OP_TYPE(OP##_AUTO)   \
   DO_INIT_OP_TYPE(OP##_PTR)    \
   DO_INIT_OP_TYPE(OP##_REG)    \
   DO_INIT_OP_TYPE(OP##_MAPREG) \
   DO_INIT_OP_TYPE(OP##_WLDREG) \
   DO_INIT_OP_TYPE(OP##_GBLREG) \
   DO_INIT_OP_TYPE(OP##_MAPARR) \
   DO_INIT_OP_TYPE(OP##_WLDARR) \
   DO_INIT_OP_TYPE(OP##_GBLARR) \
   DO_INIT_OP_TYPE(OP##_TEMP)


   //-------------------------------------------------------------------------|
   // NOP                                                                     |
   //

   DO_INIT(NOP)


   //-------------------------------------------------------------------------|
   // Operators                                                               |
   //

   // Binary
   DO_INIT_OP_AREA(ADD)
   DO_INIT_OP_AREA(AND)
   DO_INIT_OP_TYPE(CMP_EQ)
   DO_INIT_OP_TYPE(CMP_GE)
   DO_INIT_OP_TYPE(CMP_GT)
   DO_INIT_OP_TYPE(CMP_LE)
   DO_INIT_OP_TYPE(CMP_LT)
   DO_INIT_OP_TYPE(CMP_NE)
   DO_INIT_OP_AREA(DIV)
   DO_INIT_OP_AREA(IOR)
   DO_INIT_OP_AREA(LSH)
   DO_INIT_OP_AREA(MOD)
   DO_INIT_OP_AREA(MUL)
   DO_INIT_OP_AREA(RSH)
   DO_INIT_OP_AREA(SUB)
   DO_INIT_OP_AREA(XOR)

   // Unary
   DO_INIT_OP_AREA(DEC)
   DO_INIT_OP_AREA(INC)
   DO_INIT_OP_TYPE(INV_STK)
   DO_INIT_OP_TYPE(NEG_STK)
   DO_INIT_OP_TYPE(NOT_STK)

   // Logical
   DO_INIT_OP_TYPE(LOGAND_STK)
   DO_INIT_OP_TYPE(LOGIOR_STK)
   DO_INIT_OP_TYPE(LOGXOR_STK)


   //-------------------------------------------------------------------------|
   // Jumps                                                                   |
   //

   DO_INIT(JMP)
   DO_INIT(JMP_IMM)
   DO_INIT(JMP_NIL)
   DO_INIT(JMP_TRU)
   DO_INIT(JMP_VAL)
   DO_INIT(JMP_TAB)


   //-------------------------------------------------------------------------|
   // Type Casts                                                              |
   //

   DO_INIT(CAST_STK_X2I)
   DO_INIT(CAST_STK_I2X)


   //-------------------------------------------------------------------------|
   // Stack-ops                                                               |
   //

   DO_INIT(STK_COPY)
   DO_INIT(STK_DROP)
   DO_INIT(STK_SWAP)


   //-------------------------------------------------------------------------|
   // Trigonometry                                                            |
   //

   DO_INIT(TRIG_COS_X)
   DO_INIT(TRIG_SIN_X)


   //-------------------------------------------------------------------------|
   // Variable Address                                                        |
   //

   DO_INIT(GET_AUTPTR)
   DO_INIT(GET_AUTPTR_IMM)

   DO_INIT(SET_AUTPTR)
   DO_INIT(SET_AUTPTR_IMM)

   DO_INIT(ADD_AUTPTR)
   DO_INIT(ADD_AUTPTR_IMM)

   DO_INIT(SUB_AUTPTR)
   DO_INIT(SUB_AUTPTR_IMM)


   //-------------------------------------------------------------------------|
   // Variable Get                                                            |
   //

   DO_INIT(GET_IMM)
   DO_INIT(GET_STATIC)
   DO_INIT(GET_AUTO)
   DO_INIT(GET_PTR)
   DO_INIT(GET_REG)
   DO_INIT(GET_MAPREG)
   DO_INIT(GET_WLDREG)
   DO_INIT(GET_GBLREG)
   DO_INIT(GET_MAPARR)
   DO_INIT(GET_WLDARR)
   DO_INIT(GET_GBLARR)
   DO_INIT(GET_TEMP)


   //-------------------------------------------------------------------------|
   // Variable Set                                                            |
   //

   DO_INIT(SET_STATIC)
   DO_INIT(SET_AUTO)
   DO_INIT(SET_PTR)
   DO_INIT(SET_REG)
   DO_INIT(SET_MAPREG)
   DO_INIT(SET_WLDREG)
   DO_INIT(SET_GBLREG)
   DO_INIT(SET_MAPARR)
   DO_INIT(SET_WLDARR)
   DO_INIT(SET_GBLARR)
   DO_INIT(SET_TEMP)


   //-------------------------------------------------------------------------|
   // Miscellaneous                                                           |
   //

   DO_INIT(NATIVE)


   //-------------------------------------------------------------------------|
   // ACS                                                                     |
   //

   DO_INIT(ACS_GAME_GET_PLAYERCOUNT)
   DO_INIT(ACS_GAME_GET_SKILL)
   DO_INIT(ACS_GAME_GET_THINGCOUNT_SID)
   DO_INIT(ACS_GAME_GET_THINGCOUNT_SID_IMM)
   DO_INIT(ACS_GAME_GET_TIME)
   DO_INIT(ACS_GAME_GET_TYPE)

   //
   // Activator Line
   //
   DO_INIT(ACS_LINE_CLR_SPECIAL)
   DO_INIT(ACS_LINE_GET_SIDE)

   //
   // Line Tag
   //
   DO_INIT(ACS_LTAG_SET_BLOCK)
   DO_INIT(ACS_LTAG_SET_SPECIAL)
   DO_INIT(ACS_LTAG_SET_TEXTURE)

   DO_INIT(ACS_MISC_RANDOM)
   DO_INIT(ACS_MISC_RANDOM_IMM)

   DO_INIT(ACS_SCRIPT_RESTART)
   DO_INIT(ACS_SCRIPT_SUSPEND)
   DO_INIT(ACS_SCRIPT_TERMINATE)

   DO_INIT(ACS_SOUND_AMBIENT)
   DO_INIT(ACS_SOUND_MTAG)
   DO_INIT(ACS_SOUND_SECTOR)
   DO_INIT(ACS_SOUND_SEQUENCE)

   DO_INIT(ACS_SPECIAL_EXEC1)
   DO_INIT(ACS_SPECIAL_EXEC1_IMM)
   DO_INIT(ACS_SPECIAL_EXEC2)
   DO_INIT(ACS_SPECIAL_EXEC2_IMM)
   DO_INIT(ACS_SPECIAL_EXEC3)
   DO_INIT(ACS_SPECIAL_EXEC3_IMM)
   DO_INIT(ACS_SPECIAL_EXEC4)
   DO_INIT(ACS_SPECIAL_EXEC4_IMM)
   DO_INIT(ACS_SPECIAL_EXEC5)
   DO_INIT(ACS_SPECIAL_EXEC5_IMM)

   //
   // Sector Tag
   //
   DO_INIT(ACS_STAG_SET_TEXTURE_CEILING)
   DO_INIT(ACS_STAG_SET_TEXTURE_CEILING_IMM)
   DO_INIT(ACS_STAG_SET_TEXTURE_FLOOR)
   DO_INIT(ACS_STAG_SET_TEXTURE_FLOOR_IMM)

   DO_INIT(ACS_WAIT_POLYOBJECT)
   DO_INIT(ACS_WAIT_POLYOBJECT_IMM)
   DO_INIT(ACS_WAIT_SCRIPT)
   DO_INIT(ACS_WAIT_SCRIPT_IMM)
   DO_INIT(ACS_WAIT_STAG)
   DO_INIT(ACS_WAIT_STAG_IMM)
   DO_INIT(ACS_WAIT_TICS)
   DO_INIT(ACS_WAIT_TICS_IMM)


   //-------------------------------------------------------------------------|
   // ACS Common Extensions                                                   |
   //


   //-------------------------------------------------------------------------|
   // ACS Extensions                                                          |
   //

   DO_INIT(ACSE_FUNC_CALL_IMM)
   DO_INIT(ACSE_FUNC_CALLVOID_IMM)
   DO_INIT(ACSE_FUNC_RETN)
   DO_INIT(ACSE_FUNC_RETNVOID)

   DO_INIT(ACSE_GAME_EXEC)
   DO_INIT(ACSE_GAME_EXEC_IMM)
   DO_INIT(ACSE_GAME_GET_CVAR)
   DO_INIT(ACSE_GAME_GET_INVASIONSTATE)
   DO_INIT(ACSE_GAME_GET_INVASIONWAVE)
   DO_INIT(ACSE_GAME_GET_LEVELINFO)
   DO_INIT(ACSE_GAME_GET_TEAMINFO_PLAYERCOUNT_BLUE)
   DO_INIT(ACSE_GAME_GET_TEAMINFO_PLAYERCOUNT_RED)
   DO_INIT(ACSE_GAME_GET_TEAMINFO_SCORE_BLUE)
   DO_INIT(ACSE_GAME_GET_TEAMINFO_SCORE_RED)
   DO_INIT(ACSE_GAME_GET_THINGCOUNT_STR)
   DO_INIT(ACSE_GAME_GET_TYPE_ONEFLAGCTF)
   DO_INIT(ACSE_GAME_GET_TYPE_SINGLEPLAYER)
   DO_INIT(ACSE_GAME_REPLACETEXTURES)
   DO_INIT(ACSE_GAME_SET_AIRCONTROL)
   DO_INIT(ACSE_GAME_SET_AIRCONTROL_IMM)
   DO_INIT(ACSE_GAME_SET_GRAVITY)
   DO_INIT(ACSE_GAME_SET_GRAVITY_IMM)
   DO_INIT(ACSE_GAME_SET_LEVEL)
   DO_INIT(ACSE_GAME_SET_MUSIC)
   DO_INIT(ACSE_GAME_SET_MUSIC_IMM)
   DO_INIT(ACSE_GAME_SET_MUSICLOCAL)
   DO_INIT(ACSE_GAME_SET_MUSICLOCAL_IMM)
   DO_INIT(ACSE_GAME_SET_MUSICST)
   DO_INIT(ACSE_GAME_SET_SKY)

   //
   // Activator Line
   //
   DO_INIT(ACSE_LINE_GET_OFFSETY)

   //
   // Line Tag
   //
   DO_INIT(ACSE_LTAG_SET_BLOCKMONSTER)

   //
   // Mobj ID
   //
   DO_INIT(ACSE_MID_GET)
   DO_INIT(ACSE_MID_SET)

   DO_INIT(ACSE_MISC_PLAYMOVIE)

   //
   // Mobj Tag
   //
   DO_INIT(ACSE_MTAG_ADD_INVENTORY)
   DO_INIT(ACSE_MTAG_CHK_TEXTURE_CEILING)
   DO_INIT(ACSE_MTAG_CHK_TEXTURE_FLOOR)
   DO_INIT(ACSE_MTAG_CLR_INVENTORY)
   DO_INIT(ACSE_MTAG_DAMAGE)
   DO_INIT(ACSE_MTAG_GET)
   DO_INIT(ACSE_MTAG_GET_ANGLE)
   DO_INIT(ACSE_MTAG_GET_CEILINGZ)
   DO_INIT(ACSE_MTAG_GET_CLASSIFICATION)
   DO_INIT(ACSE_MTAG_GET_FLOORZ)
   DO_INIT(ACSE_MTAG_GET_INVENTORY)
   DO_INIT(ACSE_MTAG_GET_LIGHTLEVEL)
   DO_INIT(ACSE_MTAG_GET_PITCH)
   DO_INIT(ACSE_MTAG_GET_X)
   DO_INIT(ACSE_MTAG_GET_Y)
   DO_INIT(ACSE_MTAG_GET_Z)
   DO_INIT(ACSE_MTAG_MORPH)
   DO_INIT(ACSE_MTAG_SET)
   DO_INIT(ACSE_MTAG_SET_ANGLE)
   DO_INIT(ACSE_MTAG_SET_CAMERATEXTURE)
   DO_INIT(ACSE_MTAG_SET_MARINESPRITE)
   DO_INIT(ACSE_MTAG_SET_MARINEWEAPON)
   DO_INIT(ACSE_MTAG_SET_PITCH)
   DO_INIT(ACSE_MTAG_SET_SPECIAL)
   DO_INIT(ACSE_MTAG_SET_STATE)
   DO_INIT(ACSE_MTAG_SET_XYZ)
   DO_INIT(ACSE_MTAG_SUB_INVENTORY)
   DO_INIT(ACSE_MTAG_UNMORPH)
   DO_INIT(ACSE_MTAG_USEINVENTORY)

   DO_INIT(ACSE_PLAYER_GET_CAMERA)
   DO_INIT(ACSE_PLAYER_GET_CLASS)
   DO_INIT(ACSE_PLAYER_GET_INFO)
   DO_INIT(ACSE_PLAYER_GET_INGAME)
   DO_INIT(ACSE_PLAYER_GET_INPUT)
   DO_INIT(ACSE_PLAYER_GET_ISBOT)

   DO_INIT(ACSE_SCREEN_FADE_STOP)
   DO_INIT(ACSE_SCREEN_FADE_RANGE)
   DO_INIT(ACSE_SCREEN_FADE_START)
   DO_INIT(ACSE_SCREEN_GET_HEIGHT)
   DO_INIT(ACSE_SCREEN_GET_WIDTH)
   DO_INIT(ACSE_SCREEN_SET_HUDSIZE)

   DO_INIT(ACSE_SCRIPT_RETURN)
   DO_INIT(ACSE_SCRIPT_SETRETURN)

   DO_INIT(ACSE_SOUND_THING)
   DO_INIT(ACSE_SOUND_AMBIENTLOCAL)

   DO_INIT(ACSE_SPAWN_POINT)
   DO_INIT(ACSE_SPAWN_POINT_IMM)
   DO_INIT(ACSE_SPAWN_PROJECTILE_SID)
   DO_INIT(ACSE_SPAWN_PROJECTILE_STR)
   DO_INIT(ACSE_SPAWN_SPOT)
   DO_INIT(ACSE_SPAWN_SPOT_ANGLE)
   DO_INIT(ACSE_SPAWN_SPOT_ANGLE_IMM)

   DO_INIT(ACSE_SPECIAL_EXEC5_RETN1)

   //
   // Sector Tag
   //
   DO_INIT(ACSE_STAG_DAMAGE)
   DO_INIT(ACSE_STAG_GET_LIGHTLEVEL)
   DO_INIT(ACSE_STAG_GET_THINGCOUNT_SID)
   DO_INIT(ACSE_STAG_GET_THINGCOUNT_STR)
   DO_INIT(ACSE_STAG_GET_Z_CEILING)
   DO_INIT(ACSE_STAG_GET_Z_FLOOR)
   DO_INIT(ACSE_STAG_SET_TRIGGER_CEILING)
   DO_INIT(ACSE_STAG_SET_TRIGGER_FLOOR)

   DO_INIT(ACSE_STRING_COPY_GLOBALRANGE)
   DO_INIT(ACSE_STRING_COPY_MAPRANGE)
   DO_INIT(ACSE_STRING_COPY_WORLDRANGE)
   DO_INIT(ACSE_STRING_GET_LENGTH)
   DO_INIT(ACSE_STRING_TAG)

   //
   // Activator Thing
   //
   DO_INIT(ACSE_THING_ADD_INVENTORY)
   DO_INIT(ACSE_THING_ADD_INVENTORY_IMM)
   DO_INIT(ACSE_THING_CHK_WEAPON)
   DO_INIT(ACSE_THING_CLR_INVENTORY)
   DO_INIT(ACSE_THING_GET_AMMOCAP)
   DO_INIT(ACSE_THING_GET_ARMOR)
   DO_INIT(ACSE_THING_GET_FRAGS)
   DO_INIT(ACSE_THING_GET_HEALTH)
   DO_INIT(ACSE_THING_GET_INVENTORY)
   DO_INIT(ACSE_THING_GET_INVENTORY_IMM)
   DO_INIT(ACSE_THING_GET_MID)
   DO_INIT(ACSE_THING_GET_MTAG)
   DO_INIT(ACSE_THING_GET_PLAYERNUMBER)
   DO_INIT(ACSE_THING_GET_SIGIL)
   DO_INIT(ACSE_THING_GET_TEAM)
   DO_INIT(ACSE_THING_SET_AMMOCAP)
   DO_INIT(ACSE_THING_SET_MID)
   DO_INIT(ACSE_THING_SET_MUGSHOT)
   DO_INIT(ACSE_THING_SET_WEAPON)
   DO_INIT(ACSE_THING_SUB_INVENTORY)
   DO_INIT(ACSE_THING_SUB_INVENTORY_IMM)
   DO_INIT(ACSE_THING_USEINVENTORY)

   DO_INIT(ACSE_TRANSLATION_END)
   DO_INIT(ACSE_TRANSLATION_PALETTE)
   DO_INIT(ACSE_TRANSLATION_RGB)
   DO_INIT(ACSE_TRANSLATION_START)

   DO_INIT(ACSE_TRIG_VECTORANGLE)


   //-------------------------------------------------------------------------|
   // ACS Printing                                                            |
   //

   DO_INIT(ACSP_CHARACTER)

   DO_INIT(ACSP_END)
   DO_INIT(ACSP_END_BOLD)
   DO_INIT(ACSP_END_ERROR)
   DO_INIT(ACSP_END_HUD)
   DO_INIT(ACSP_END_HUD_BOLD)
   DO_INIT(ACSP_END_LOG)
   DO_INIT(ACSP_END_OPT)
   DO_INIT(ACSP_END_STR)

   DO_INIT(ACSP_KEYBIND)

   DO_INIT_OP_TYPE(ACSP_NUM_BIN)
   DO_INIT_OP_TYPE(ACSP_NUM_DEC)
   DO_INIT_OP_TYPE(ACSP_NUM_HEX)

   DO_INIT(ACSP_PLAYER_NAME)

   DO_INIT(ACSP_SET_FONT)
   DO_INIT(ACSP_SET_FONT_IMM)

   DO_INIT(ACSP_START)
   DO_INIT(ACSP_START_OPT)

   DO_INIT(ACSP_STR)
   DO_INIT(ACSP_STR_GBLARR)
   DO_INIT(ACSP_STR_GBLRNG)
   DO_INIT(ACSP_STR_LOCALIZED)
   DO_INIT(ACSP_STR_MAPARR)
   DO_INIT(ACSP_STR_MAPRNG)
   DO_INIT(ACSP_STR_WLDARR)
   DO_INIT(ACSP_STR_WLDRNG)

#undef DO_INIT_OP_AREA
#undef DO_INIT_OP_TYPE
#undef DO_INIT_OP_SIZE

// EOF

