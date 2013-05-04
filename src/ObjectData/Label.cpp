//-----------------------------------------------------------------------------
//
// Copyright(C) 2012-2013 David Hill
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
// Object-level label data.
//
//-----------------------------------------------------------------------------

#include "../ObjectData.hpp"

#include "../ObjectArchive.hpp"
#include "../ObjectExpression.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

typedef std::vector<ObjectData::Label> LabelTable;
typedef LabelTable::iterator LabelIter;


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static LabelTable Table;


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace ObjectData
{

//
// ObjectData::Label::Add
//
std::string const &Label::Add(std::string const &label)
{
   Table.push_back(Label());
   Label &data = Table.back();

   data.label = label;

   std::ostringstream oss;
   oss << ObjectExpression::get_filename() << "::0l::" << Table.size();
   data.name = oss.str();

   ObjectExpression::add_symbol(data.name, ObjectExpression::ET_UNS);

   return data.name;
}

//
// ObjectData::Label::GenerateSymbols
//
void Label::GenerateSymbols()
{
   ObjectExpression::Pointer expr;
   bigsint i = 0;

   for(auto const &itr : Table)
   {
      expr = ObjectExpression::CreateValueUNS(i++, SourcePosition::none());
      ObjectExpression::add_symbol(itr.name, expr);
   }
}

//
// ObjectData::Label::Iterate
//
void Label::Iterate(IterFunc iterFunc, std::ostream *out)
{
   for(auto const &itr : Table)
      iterFunc(out, itr);
}

//
// ObjectData::Label::Load
//
ObjectLoad &Label::Load(ObjectLoad &arc)
{
   return arc >> Table;
}

//
// ObjectData::Label::Save
//
ObjectSave &Label::Save(ObjectSave &arc)
{
   return arc << Table;
}

}

//
// operator ObjectSave << ObjectData::Label
//
ObjectSave &operator << (ObjectSave &arc, ObjectData::Label const &data)
{
   return arc << data.label << data.name;
}

//
// operator ObjectLoad >> ObjectData::Label
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectData::Label &data)
{
   return arc >> data.label >> data.name;
}

// EOF

