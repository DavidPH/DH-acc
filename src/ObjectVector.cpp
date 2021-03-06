//-----------------------------------------------------------------------------
//
// Copyright(C) 2011-2013 David Hill
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

#include "ObjectArchive.hpp"
#include "ObjectExpression.hpp"
#include "ObjectToken.hpp"
#include "option.hpp"


//----------------------------------------------------------------------------|
// Static Variables                                                           |
//

static option::option_data<bool> option_opt_branch_flip
('\0', "opt-branch-flip", "optimization",
 "Inverts conditional branches preceded by a NOT. On by default.", NULL, true);
static option::option_data<bool> option_opt_math_nop
('\0', "opt-math-nop", "optimization", "Strips mathematical no-ops.", NULL, false);
static option::option_data<bool> option_opt_nop
('\0', "opt-nop", "optimization", "Strips NOP instructions.", NULL, false);
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
   return ObjectExpression::CreateValueFIX(f, head.pos);
}

//
// ObjectVector::getValue
//
ObjectExpression::Pointer ObjectVector::getValue(bigsint i) const
{
   return ObjectExpression::CreateValueINT(i, head.pos);
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
   return ObjectExpression::CreateValueSymbol(symbol, head.pos);
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
   if(option_opt_nop.data) optimize_nop();

   // NOT JMP_NIL/JMP_TRU fixing.
   if(option_opt_branch_flip.data) optimize_branch_flip();

   // Mathematical no-op removal.
   if(option_opt_math_nop.data) optimize_math_nop();

   // PUSH/DROP removal.
   if(option_opt_pushdrop.data) optimize_pushdrop();

   // PUSH/PUSH/SWAP fixing.
   if(option_opt_pushpushswap.data) optimize_pushpushswap();
}

//
// ObjectVector::optimize_branch_flip
//
// NOT JMP_NIL/JMP_TRU fixing.
//
void ObjectVector::optimize_branch_flip()
{
   ObjectToken *arg0, *arg1;

   iterator token = begin();
   iterator stop = end();

   while(token != stop)
   {
      arg0 = token++;
      if(arg0->code != OCODE_NOT_STK_I && arg0->code != OCODE_NOT_STK_U &&
         arg0->code != OCODE_NOT_STK_X)
         continue;

      arg1 = token++;
      if((arg1->code != OCODE_JMP_NIL && arg1->code != OCODE_JMP_TRU) ||
         !arg1->labels.empty())
         continue;

      arg1->addLabel(arg0->labels);
      if(arg1->code == OCODE_JMP_NIL)
         arg1->code = OCODE_JMP_TRU;
      else
         arg1->code = OCODE_JMP_NIL;

      remToken(arg0);
   }
}

//
// ObjectVector::optimize_math_nop
//
// Mathematical no-op removal.
//  * PUSH 1 MUL
//  * PUSH 1 DIV
//  * PUSH 0 ADD
//  * PUSH 0 SUB
//
void ObjectVector::optimize_math_nop()
{
   ObjectToken *arg0, *arg1;

   iterator token = begin();
   iterator stop = end();

   while(token != stop)
   {
      arg0 = token++;
      if(arg0->code != OCODE_GET_IMM || !arg0->labels.empty() || !arg0->getArg(0)->canResolve())
         continue;

      arg1 = token++;
      if(!arg1->labels.empty())
         continue;

      switch(arg1->code)
      {
      case OCODE_ADD_STK_I:
      case OCODE_ADD_STK_U:
      case OCODE_SUB_STK_I:
      case OCODE_SUB_STK_U:
         if(arg0->getArg(0)->resolveINT() != 0)
            continue;
         break;

      case OCODE_ADD_STK_X:
      case OCODE_SUB_STK_X:
         if(arg0->getArg(0)->resolveFIX() != 0)
            continue;
         break;

      case OCODE_DIV_STK_I:
      case OCODE_DIV_STK_U:
      case OCODE_MUL_STK_I:
      case OCODE_MUL_STK_U:
         if(arg0->getArg(0)->resolveINT() != 1)
            continue;
         break;

      case OCODE_DIV_STK_X:
      case OCODE_MUL_STK_X:
         if(arg0->getArg(0)->resolveFIX() != 1)
            continue;
         break;

      default:
         continue;
      }

      token->addLabel(arg0->labels);
      token->addLabel(arg1->labels);

      remToken(arg0);
      remToken(arg1);
   }
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
   ObjectToken *arg0, *arg1;

   iterator token = begin();
   iterator stop = end();

   while(token != stop)
   {
      arg0 = token++;
      if(!ocode_is_push_noarg(arg0->code))
         continue;

      arg1 = token++;
      if(arg1->code != OCODE_STK_DROP || !arg1->labels.empty())
         continue;

      token->addLabel(arg0->labels);
      remToken(arg0);
      remToken(arg1);

      // Go back in case there was an earlier PUSH.
      if(token != begin()) --token;
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

   while(token != stop)
   {
      arg0 = token++;
      if(!ocode_is_push_noarg(arg0->code))
         continue;

      arg1 = token++;
      if(!ocode_is_push_noarg(arg1->code) || !arg1->labels.empty())
         continue;

      arg2 = token++;
      if(arg2->code != OCODE_STK_SWAP || !arg2->labels.empty())
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
// operator ObjectSave << ObjectVector
//
ObjectSave &operator << (ObjectSave &arc, ObjectVector const &data)
{
   arc << data.head.labels << data.head.pos;

   for(auto &token : data)
      arc << true << token;
   arc << false;

   return arc;
}

//
// operator ObjectLoad >> ObjectVector
//
ObjectLoad &operator >> (ObjectLoad &arc, ObjectVector &data)
{
   ObjectToken *token;
   bool b;

   arc >> data.head.labels >> data.head.pos;

   while(arc >> b, b)
   {
      arc >> *(token = new ObjectToken);
      data.addToken(token);
   }

   return arc;
}

// EOF

