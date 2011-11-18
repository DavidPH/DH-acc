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

/* ACSP.hpp
**
** Constants relating to ACS+.
*/

#ifndef HPP_ACSP_
#define HPP_ACSP_



enum
{
	ACSP_EXPR_LITERAL,
	ACSP_EXPR_SYMBOL,
	ACSP_EXPR_ADD,
	ACSP_EXPR_SUB,
	ACSP_EXPR_MUL,
	ACSP_EXPR_DIV,
	ACSP_EXPR_MOD,
	ACSP_EXPR_BAND,
	ACSP_EXPR_BIOR,
	ACSP_EXPR_BXOR,
	ACSP_EXPR_BNOT,
	ACSP_EXPR_LAND,
	ACSP_EXPR_LIOR,
	ACSP_EXPR_LXOR,
	ACSP_EXPR_LNOT,
	ACSP_EXPR_NEGATE,
	ACSP_EXPR_FMUL,
	ACSP_EXPR_FDIV,
	ACSP_EXPR_I2F,
	ACSP_EXPR_F2I
};

enum
{
	ACSP_TOKEN_EOF,
	ACSP_TOKEN_INSTRUCTION,
	ACSP_TOKEN_SCRIPT,
	ACSP_TOKEN_STRING,
	ACSP_TOKEN_SYMBOL,
	ACSP_TOKEN_LABEL,
	ACSP_TOKEN_ALLOCATE
};



#endif//HPP_ACSP_

