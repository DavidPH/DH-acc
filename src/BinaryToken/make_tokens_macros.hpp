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

/* BinaryToken/make_tokens_macros.hpp
**
** Defines macros for token making.
*/

#ifndef HPP_make_tokens_macros__BinaryToken_
#define HPP_make_tokens_macros__BinaryToken_

// Needed deinfitions:
// TOKEN_CLASS - Define to the name of the BinaryToken class.

#define PUSH_ARGS2(START,STOP)       \
   for (int i(START); i < STOP; ++i) \
      args.push_back(object.getArg(i))

#define PUSH_ARGS1(STOP) \
   PUSH_ARGS2(0, STOP)

#define PUSH_TOKEN(BCODE)                                                \
   instructions->push_back(TOKEN_CLASS(BCODE, position, *labels, args)); \
   args.clear();                                                         \
   labels = &nolabels;

#define PUSH_TOKEN_ARGS1(BCODE,STOP) \
   PUSH_ARGS1(STOP);                 \
   PUSH_TOKEN(BCODE)

#define PUSH_TOKEN_ARGS2(BCODE,START,STOP) \
   PUSH_ARGS2(START, STOP);                \
   PUSH_TOKEN(BCODE)


#define CASE_REMAP(OCODE, BCODE)                                   \
   case OCODE_##OCODE:                                             \
      PUSH_TOKEN_ARGS1(BCODE_##BCODE, _arg_counts[BCODE_##BCODE]); \
      break

#define CASE_REMAP_PRE(PREFIX,OCODE,BCODE) \
   CASE_REMAP(PREFIX##_##OCODE, PREFIX##_##BCODE)

#define CASE_MAP_ACS(CODE) \
   CASE_REMAP(ACS_##CODE, CODE)

#define CASE_MAP_ACSE(CODE) \
   CASE_REMAP(ACSE_##CODE, CODE)

#define CASE_MAP_ACSP(CODE) \
   CASE_REMAP(ACSP_##CODE, PRINT_##CODE)
#define CASE_REMAP_ACSP(OCODE, BCODE) \
   CASE_REMAP(ACSP_##OCODE, PRINT_##BCODE)



#endif//HPP_make_tokens_macros__BinaryToken_

