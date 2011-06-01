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

/* SourceBlockC.cpp
**
** Defines the SourceBlockC methods.
*/

#include "SourceBlockC.hpp"

#include "print_debug.hpp"
#include "SourceStream.hpp"
#include "SourceTokenizerC.hpp"



SourceBlockC::SourceBlockC()
{

}
SourceBlockC::SourceBlockC(SourceTokenizerC * const in)
{
	while (true)
	{
		// Check for any more tokens in the stream.
		try
		{
			in->unget(in->get());
		}
		catch (SourceStream::EndOfStream & e)
		{
			return;
		}

		_blocks.push_back(SourceBlockC());
		_blocks.back().readBlock(in);
	}
}

SourceTokenC const & SourceBlockC::getToken(uintptr_t const index) const
{
	// TODO: Out of bounds access handling.
	return _tokens[index];
}

std::vector<SourceTokenC> const & SourceBlockC::getTokens() const
{
	return _tokens;
}

void SourceBlockC::readBlock(SourceTokenizerC * const in)
{
	while (true)
	{
		_tokens.push_back(in->get());
		SourceTokenC const & token(_tokens.back());

		if (token.getType() == SourceTokenC::TT_OP_SEMICOLON)
			return;

		if (token.getType() == SourceTokenC::TT_OP_BRACE_O)
		{
			readBlocks(in);

			_tokens.push_back(in->get(SourceTokenC::TT_OP_BRACE_C));
		}
	}
}

void SourceBlockC::readBlocks(SourceTokenizerC * const in)
{
	while (true)
	{
		SourceTokenC token(in->peek());

		if (token.getType() == SourceTokenC::TT_OP_BRACE_C)
			return;

		_blocks.push_back(SourceBlockC());
		_blocks.back().readBlock(in);
	}
}



void print_debug(std::ostream * const out, SourceBlockC const & block)
{
	*out << "SourceBlockC(";
		*out << "_blocks=(";
		print_debug(out, block._blocks);
		*out << ")";

		*out << ", ";

		*out << "_tokens=(";
		print_debug(out, block._tokens);
		*out << ")";
	*out << ")";
}



