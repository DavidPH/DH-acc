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

/* SourceTokenizerDS.hpp
**
** Defines the SourceTokenizerDS class.
*/

#ifndef HPP_SourceTokenizerDS_
#define HPP_SourceTokenizerDS_

#include "CounterPointer.hpp"
#include "SourceTokenC.hpp"

#include <map>
#include <stack>
#include <string>
#include <vector>

class ObjectExpression;
class SourceStream;



class SourceTokenizerDS
{
public:
	SourceTokenizerDS(SourceStream * const in);
	~SourceTokenizerDS();

	SourceTokenC const & get();
	SourceTokenC const & get(SourceTokenC::TokenType type);

	SourceTokenC const & peek();

	void unget(SourceTokenC const & token);

private:
	SourceTokenizerDS(SourceTokenizerDS const & tokenizer)/* = delete*/;

	void addDefine(std::string const & name, SourcePosition const & position, std::vector<SourceTokenC> const & tokens);
	void addSkip(bool skip);

	void doAssert(SourceTokenC::TokenType type);

	void doCommand();
	void doCommand_define();
	void doCommand_else();
	void doCommand_elif();
	void doCommand_endif();
	void doCommand_error();
	void doCommand_if();
	void doCommand_ifdef();
	void doCommand_ifndef();
	void doCommand_include();
	void doCommand_undef();

	bool getIf();
	CounterPointer<ObjectExpression> getIfMultiple();
	CounterPointer<ObjectExpression> getIfSingle();

	bool hasDefine();
	bool hasDefine(std::string const & name);

	bool isSkip();

	SourceTokenizerDS & operator = (SourceTokenizerDS const & tokenizer)/* = delete*/;

	void prep();
	void prep(SourceTokenC::TokenType type);
	void prepDefine();

	void remDefine();
	void remSkip();

	std::map<std::string, std::vector<SourceTokenC> > _defines;

	std::stack<SourceStream *> _in;
	std::stack<bool> _skipStack;
	std::stack<SourceTokenC> _ungetStack;
	std::stack<bool> _unskipStack;

	SourceTokenC _token;

	unsigned _canCommand : 1;
	unsigned _canExpand  : 1;
	unsigned _canSkip    : 1;
};



#endif /* HPP_SourceTokenizerDS_ */



