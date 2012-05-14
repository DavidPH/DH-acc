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

#include "ObjectVector.hpp"

#include "ObjectExpression.hpp"
#include "object_io.hpp"
#include "ObjectToken.hpp"
#include "option.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_data<bool> option_opt_nop
('\0', "opt-nop", "optimization",
 "Strips NOP instructions.", NULL, false);
static option::option_data<bool> option_opt_pushdrop
('\0', "opt-pushdrop", "optimization",
 "Strips PUSH/DROP pairs. On by default.", NULL, true);
static option::option_data<bool> option_opt_pushpushswap
('\0', "opt-pushpushswap", "optimization",
 "Removes the SWAP from PUSH/PUSH/SWAP sets. On by default.", NULL, true);


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// ObjectVector::ObjectVector
//
ObjectVector::ObjectVector()
{
   head.next = &head;
   head.prev = &head;
}

//
// ObjectVector::~ObjectVector
//
ObjectVector::~ObjectVector()
{
   iterator token = begin();
   while (token != end())
      delete static_cast<ObjectToken *>(token++);
}

//
// ObjectVector::addToken
//
void ObjectVector::addToken(ObjectToken *token)
{
   token->prev = head.prev;
   head.prev->next = token;
   token->next = &head;
   head.prev = token;

   head.labels.clear();
}

//
// ObjectVector::addToken
//
void ObjectVector::addToken(ObjectCode code)
{
   static ObjectExpression::Vector const args;

   addToken(new ObjectToken(code, head.pos, head.labels, args));
}

//
// ObjectVector::addToken
//
void ObjectVector::addToken
(ObjectCode code, ObjectExpression::Vector const &args)
{
   addToken(new ObjectToken(code, head.pos, head.labels, args));
}

//
// ObjectVector::addToken
//
void ObjectVector::addToken(ObjectCode code, ObjectExpression *arg0)
{
   ObjectExpression::Vector args;

   args.push_back(arg0);

   addToken(new ObjectToken(code, head.pos, head.labels, args));
}

//
// ObjectVector::addToken
//
void ObjectVector::addToken
(ObjectCode code, ObjectExpression *arg0, ObjectExpression *arg1)
{
   ObjectExpression::Vector args;

   args.push_back(arg0);
   args.push_back(arg1);

   addToken(new ObjectToken(code, head.pos, head.labels, args));
}

//
// ObjectVector::addToken
//
void ObjectVector::addTokenPushZero()
{
   addToken(OCODE_GET_IMM, getValue(0));
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(bigreal f) const
{
   return ObjectExpression::create_value_float(f, head.pos);
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(bigsint i) const
{
   return ObjectExpression::create_value_int(i, head.pos);
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(double f) const
{
   return getValue((bigreal)f);
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(int i) const
{
   return getValue((bigsint)i);
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(ObjectExpression *expr) const
{
   return expr;
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(std::string const &symbol) const
{
   return ObjectExpression::create_value_symbol(symbol, head.pos);
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(unsigned int i) const
{
   return getValue((bigsint)i);
}

//
// ObjectVector::getValueAdd
//
CounterPointer<ObjectExpression> ObjectVector::getValueAdd
(ObjectExpression *exprL, ObjectExpression *exprR) const
{
   return ObjectExpression::create_binary_add(exprL, exprR, head.pos);
}

//
// ObjectVector::optimize
//
// Invokes all optimizations that have been requested by the user.
//
void ObjectVector::optimize()
{
   // NOP removal.
   // Off by default because NOPs do not normally get generated.
   if (option_opt_nop.data) optimize_nop();

   // PUSH/DROP removal.
   if (option_opt_pushdrop.data) optimize_pushdrop();

   // PUSH/PUSH/SWAP fixing.
   if (option_opt_pushpushswap.data) optimize_pushpushswap();
}

//
// ObjectVector::optimize_nop
//
// NOP removal.
//
void ObjectVector::optimize_nop()
{
   iterator token = begin();
   iterator stop = end(); --stop; // Need an extra token before end.

   while (token != stop)
   {
      if (token->code == OCODE_NOP)
      {
         token->next->addLabel(token->labels);
         remToken(token++);
      }
      else
      {
         ++token;
      }
   }
}

//
// ObjectVector::optimize_pushdrop
//
// PUSH DROP removal.
//
void ObjectVector::optimize_pushdrop()
{
   iterator token = begin();
   iterator stop = end();

   while (token != stop)
   {
      if (ocode_is_push_noarg(token->code) && token->labels.empty() &&
          token->next->code == OCODE_STK_DROP && token->next->labels.empty())
      {
         remToken(token++);
         remToken(token++);

         // Go back in case there was an earlier PUSH.
         if (token != begin()) --token;
      }
      else
      {
         ++token;
      }
   }
}

//
// ObjectVector::optimize_pushpushswap
//
// PUSH PUSH SWAP fixing.
//
void ObjectVector::optimize_pushpushswap()
{
   ObjectToken *arg0, *arg1, *arg2;

   iterator token = begin();
   iterator stop = end();

   while (token != stop)
   {
      arg0 = token++;
      if (!ocode_is_push_noarg(arg0->code) || !arg0->labels.empty())
         continue;

      arg1 = token++;
      if (!ocode_is_push_noarg(arg1->code) || !arg1->labels.empty())
         continue;

      arg2 = token++;
      if (arg2->code != OCODE_STK_SWAP || !arg2->labels.empty())
         continue;

      arg0->swapData(arg1);
      remToken(arg2);
   }
}

//
// ObjectVector::remToken
//
void ObjectVector::remToken(ObjectToken *token)
{
   token->prev->next = token->next;
   token->next->prev = token->prev;
   delete token;
}

//
// read_object<ObjectVector>
//
void read_object(std::istream *in, ObjectVector *out)
{
   ObjectToken *token;

   read_object(in, &out->head.labels);
   read_object(in, &out->head.pos);

   while (read_object_bit(in))
   {
      read_object(in, token = new ObjectToken);
      out->addToken(token);
   }
}

//
// write_object<ObjectVector>
//
void write_object(std::ostream *out, ObjectVector const *in)
{
   ObjectVector::const_iterator token;

   write_object(out, &in->head.labels);
   write_object(out, &in->head.pos);

   for (token = in->begin(); token != in->end(); ++token)
   {
      write_object_bit(out, true);
      write_object(out, token);
   }
   write_object_bit(out, false);
}

// EOF

