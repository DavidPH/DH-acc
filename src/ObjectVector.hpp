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
// ObjectToken set handling.
//
//-----------------------------------------------------------------------------

#ifndef HPP_ObjectVector_
#define HPP_ObjectVector_

#include "bignum.hpp"
#include "Counter.hpp"
#include "ObjectCode.hpp"
#include "ObjectToken.hpp"
#include "SourcePosition.hpp"

#include <string>
#include <vector>


//----------------------------------------------------------------------------|
// Types                                                                      |
//

class ObjectExpression;

//
// ObjectVector
//
class ObjectVector
{
private:
   //
   // ::basic_iterator
   //
   // Used to make iterator and const_iterator without duplicating code.
   //
   template<typename T> class basic_iterator
   {
   public:
      //
      // ::basic_iterator
      //
      basic_iterator() {}

      //
      // ::operator ++
      //
      basic_iterator &operator ++ () {p = p->next; return *this;}
      basic_iterator operator ++ (int)
      {
         basic_iterator iter = *this;
         p = p->next;
         return iter;
      }

      //
      // ::operator --
      //
      basic_iterator &operator -- () {p = p->prev; return *this;}
      basic_iterator operator -- (int)
      {
         basic_iterator iter = *this;
         p = p->prev;
         return iter;
      }

      //
      // ::operator *
      //
      T &operator * () const {return *p;}

      //
      // ::operator ->
      //
      T *operator -> () const {return p;}

      //
      // ::operator T*
      //
      operator T * () const {return p;}


      friend class ObjectVector;

   private:
      basic_iterator(T *_p) : p(_p) {}

      T *p;
   };

public:
   typedef basic_iterator<ObjectToken> iterator;
   typedef basic_iterator<ObjectToken const> const_iterator;

   ObjectVector();
   ~ObjectVector();

   void addLabel(std::string const &label) {head.addLabel(label);}
   void addLabel(std::vector<std::string> const &labels) {head.addLabel(labels);}

   void addToken(ObjectCode code);
   void addToken(ObjectCode code, std::vector<CounterPointer<ObjectExpression> > const &args);
   void addToken(ObjectCode code, ObjectExpression *arg0);
   void addToken(ObjectCode code, ObjectExpression *arg0, ObjectExpression *arg1);
   void addTokenPushZero();

   iterator begin() {return head.next;}
   iterator end() {return &head;}

   const_iterator begin() const {return head.next;}
   const_iterator end() const {return &head;}

   CounterPointer<ObjectExpression> getValue(bigreal f) const;
   CounterPointer<ObjectExpression> getValue(bigsint i) const;
   CounterPointer<ObjectExpression> getValue(double f) const;
   CounterPointer<ObjectExpression> getValue(int i) const;
   CounterPointer<ObjectExpression> getValue(ObjectExpression *expr) const;
   CounterPointer<ObjectExpression> getValue(std::string const &symbol) const;
   CounterPointer<ObjectExpression> getValue(unsigned int i) const;

   CounterPointer<ObjectExpression> getValueAdd
   (ObjectExpression *exprL, ObjectExpression *exprR) const;

   //
   // ::getValueAdd
   //
   template<typename T1, typename T2>
   CounterPointer<ObjectExpression> getValueAdd(T1 const &l, T2 const &r) const
   {
      return getValueAdd(static_cast<ObjectExpression *>(getValue(l)),
                         static_cast<ObjectExpression *>(getValue(r)));
   }

   void optimize();
   void optimize_branch_flip();
   void optimize_math_nop();
   void optimize_nop();
   void optimize_pushdrop();
   void optimize_pushpushswap();

   void setPosition(SourcePosition const &_pos) {head.pos = _pos;}


   friend void read_object(std::istream *in, ObjectVector *out);
   friend void write_object(std::ostream *out, ObjectVector const *in);

private:
   void addToken(ObjectToken *token);
   void remToken(ObjectToken *token);

   ObjectToken head;
};

#endif//HPP_ObjectVector_

