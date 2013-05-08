//-----------------------------------------------------------------------------
//
// Copyright(C) 2013 David Hill
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
// Member function type handling.
//
//-----------------------------------------------------------------------------

#ifndef Type__MemFunc_H__
#define Type__MemFunc_H__

#include "Class.hpp"

#include "Function.hpp"


//----------------------------------------------------------------------------|
// Types                                                                      |
//

//
// Type_MemberFunction
//
class Type_MemberFunction : public Type_Function
{
   CounterPreamble(Type_MemberFunction, Type_Function);

public:
   // Type information.
   virtual bigsint getCallWords() const;

   // Type classification.
   virtual bool isMemberFunctionType() const {return true;}


   friend TypeCR Clas::getMemberFunctionType(Type const *retn, Parm const *parm, CallCon conv) const;

   static MFunCR LoadType(ObjectLoad &load);

protected:
   Type_MemberFunction(Type_MemberFunction const &type);
   Type_MemberFunction(Clas const *clas, Type const *retn, Parm const *parm, CallCon conv);
   virtual ~Type_MemberFunction();

   virtual ObjectSave &saveObject(ObjectSave &save) const;

   ClasCR const clas;

private:
   virtual void getNameMangleBase(std::ostream &out, NameMangleStyle mangle) const;

   Type_MemberFunction *mfnNext, *mfnPrev;
};

#endif//Type__MemFunc_H__

