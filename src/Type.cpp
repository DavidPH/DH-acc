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
// Source- and Object-level type handling.
//
//
// Type classification.
//
// Where semantic differences between languages exist, the line is marked by
// that language, indicating that the type has the properties of that
// relative position in the chart only for that language. For example, enum
// is an integer type in C, but merely a scalar type in C++.
//
// And now, a chart of types:
//   function types
//   +-member function types
//   +-static function types
//   object types
//   +-accum types
//   | +-signed accum types
//   | | +-short _Accum
//   | | +-_Accum
//   | | +-long _Accum
//   | | +-long long _Accum
//   | | +-_Sat short _Accum
//   | | +-_Sat _Accum
//   | | +-_Sat long _Accum
//   | | +-_Sat long long _Accum
//   | +-unsigned accum types
//   |   +-unsigned short _Accum
//   |   +-unsigned _Accum
//   |   +-unsigned long _Accum
//   |   +-unsigned long long _Accum
//   |   +-_Sat unsigned short _Accum
//   |   +-_Sat unsigned _Accum
//   |   +-_Sat unsigned long _Accum
//   |   +-_Sat unsigned long long _Accum
//   +-aggregate types
//   | +-array types
//   | +-structure types
//   | +-tuple types
//   +-character types
//   | +-char
//   | +-signed char
//   | +-unsigned char
//   +-fract types
//   | +-signed fract types
//   | | +-short _Fract
//   | | +-_Fract
//   | | +-long _Fract
//   | | +-long long _Fract
//   | | +-_Sat short _Fract
//   | | +-_Sat _Fract
//   | | +-_Sat long _Fract
//   | | +-_Sat long long _Fract
//   | +-unsigned fract types
//   |   +-unsigned short _Fract
//   |   +-unsigned _Fract
//   |   +-unsigned long _Fract
//   |   +-unsigned long long _Fract
//   |   +-_Sat unsigned short _Fract
//   |   +-_Sat unsigned _Fract
//   |   +-_Sat unsigned long _Fract
//   |   +-_Sat unsigned long long _Fract
//   +-real types
//   | +-fixed-point types
//   | +-integer types
//   | +-real floating types
//   +-scalar types
//   | +-arithmetic types
//   | | +-fixed-point types
//   | | | +-primary fixed-point types
//   | | | | +-primary signed fixed-point types
//   | | | | | +-short _Accum
//   | | | | | +-_Accum
//   | | | | | +-long _Accum
//   | | | | | +-long long _Accum
//   | | | | | +-short _Fract
//   | | | | | +-_Fract
//   | | | | | +-long _Fract
//   | | | | | +-long long _Fract
//   | | | | +-primary unsigned fixed-point types
//   | | | |   +-unsigned short _Accum
//   | | | |   +-unsigned _Accum
//   | | | |   +-unsigned long _Accum
//   | | | |   +-unsigned long long _Accum
//   | | | |   +-unsigned short _Fract
//   | | | |   +-unsigned _Fract
//   | | | |   +-unsigned long _Fract
//   | | | |   +-unsigned long long _Fract
//   | | | +-saturating fixed-point types
//   | | |   +-saturating signed fixed-point types
//   | | |   | +-_Sat short _Accum
//   | | |   | +-_Sat _Accum
//   | | |   | +-_Sat long _Accum
//   | | |   | +-_Sat long long _Accum
//   | | |   | +-_Sat short _Fract
//   | | |   | +-_Sat _Fract
//   | | |   | +-_Sat long _Fract
//   | | |   | +-_Sat long long _Fract
//   | | |   +-saturating unsigned fixed-point types
//   | | |     +-_Sat unsigned short _Accum
//   | | |     +-_Sat unsigned _Accum
//   | | |     +-_Sat unsigned long _Accum
//   | | |     +-_Sat unsigned long long _Accum
//   | | |     +-_Sat unsigned short _Fract
//   | | |     +-_Sat unsigned _Fract
//   | | |     +-_Sat unsigned long _Fract
//   | | |     +-_Sat unsigned long long _Fract
//   | | +-floating types
//   | | | +-complex types
//   | | | | +-short float _Complex
//   | | | | +-float _Complex
//   | | | | +-double _Complex
//   | | | | +-long double _Complex
//   | | | +-imaginary types
//   | | | | +-short float _Imaginary
//   | | | | +-float _Imaginary
//   | | | | +-double _Imaginary
//   | | | | +-long double _Imaginary
//   | | | +-real floating types
//   | | |   +-short float
//   | | |   +-float
//   | | |   +-double
//   | | |   +-long double
//   | | +-integer types
//   | |   +-bool (C++)
//   | |   +-char
//   | |   +-char16_t (C++)
//   | |   +-char32_t (C++)
//   | |   +-enumerated types (C)
//   | |   +-signed integer types
//   | |   | +-extended signed integer types
//   | |   | +-standard signed integer types
//   | |   |   +-signed char
//   | |   |   +-short int
//   | |   |   +-int
//   | |   |   +-long int
//   | |   |   +-long long int
//   | |   +-unsigned integer types
//   | |   | +-extended unsigned integer types
//   | |   | +-standard unsigned integer types
//   | |   |   +-unsigned char
//   | |   |   +-unsigned short int
//   | |   |   +-unsigned int
//   | |   |   +-unsigned long int
//   | |   |   +-unsigned long long int
//   | |   |   +-_Bool (C)
//   | |   +-wchar_t (C++)
//   | +-enumerated types (C++)
//   | | +-scoped enumeration types
//   | | +-unscoped enumeration types
//   | +-pointer types
//   | +-pointer to member types
//   | +-std::nullptr_t
//   | +-__fastbool
//   | +-__string
//   +-extended integer types
//   | +-extended signed integer types
//   | +-extended unsigned integer types
//   +-standard integer types
//   | +-standard signed integer types
//   | +-standard unsigned integer types
//   +-union types
//   +-void
//   reference types
//   +-lvalue reference types
//   +-rvalue reference types
//   __label
//
//
// Name mangling.
//
//   <mangled name> ::= <type-base> <qualifier>*
//
//   <qualifier> ::= a          # atomic
//               ::= aF         # address space: far
//               ::= aGA <name> # address space: global array
//               ::= aGR        # address space: global register
//               ::= aGS        # address space: global arrays
//               ::= aL         # address space: local
//               ::= aLR        # address space: local register
//               ::= aMA <name> # address space: map array
//               ::= aMR        # address space: map register
//               ::= aMS        # address space: map arrays
//               ::= aN         # address space: near
//               ::= aSA <name> # address space: string array
//               ::= aSS        # address space: string arrays
//               ::= aVA        # address space: va_addr
//               ::= aWA <name> # address space: world array
//               ::= aWR        # address space: world register
//               ::= aWS        # address space: world arrays
//               ::= c          # const
//               ::= r          # restrict
//               ::= v          # volatile
//
//   <type-base> ::= A <number> <type-single> # array type
//               ::= B <builtin-type>         # builtin type
//               ::= E <name>                 # enumerated type
//               ::= F <function-list>        # static function type
//               ::= L <type-single>          # lvalue reference type
//               ::= M <name> <function-list> # member function type
//               ::= O <name> <type-single>   # pointer to member type
//               ::= P <type-single>          # pointer type
//               ::= R <type-single>          # rvalue reference type
//               ::= S <name>                 # structure type
//               ::= T <type-list>            # tuple type
//               ::= U <name>                 # union type
//
//   <function-list> ::= <call-type>? <type-list> <type-single>
//
//   <type-list> ::= ( type-list-list )
//
//   <type-list-list> ::= <type-list-part>
//                    ::= <type-list-list> , <type-list-part>
//
//   <type-list-part> ::= <mangled name>
//                    ::= ...
//
//   <type-single> ::= ( <mangled name> )
//
//   <builtin-type> ::= B    # bool (C++)
//                  ::= BC   # bool (C)
//                  ::= BF   # fast bool
//                  ::= C    # char
//                  ::= CD   # char16_t
//                  ::= CQ   # char32_t
//                  ::= CS   # signed char
//                  ::= CU   # unsigned char
//                  ::= CW   # wchar_t
//                  ::= F    # float
//                  ::= FC   # float complex
//                  ::= FCD  # long float complex
//                  ::= FCH  # short float complex
//                  ::= FCQ  # long long float complex
//                  ::= FD   # long float
//                  ::= FH   # short float
//                  ::= FI   # float imaginary
//                  ::= FID  # long float imaginary
//                  ::= FIH  # short float imaginary
//                  ::= FIQ  # long long float imaginary
//                  ::= FQ   # long long float
//                  ::= I    # int
//                  ::= ID   # long int
//                  ::= IH   # short int
//                  ::= IQ   # long long int
//                  ::= IU   # unsigned int
//                  ::= IUD  # unsigned long int
//                  ::= IUH  # unsigned short int
//                  ::= IUQ  # unsigned long long int
//                  ::= K    # accum
//                  ::= KD   # long accum
//                  ::= KH   # short accum
//                  ::= KQ   # long long accum
//                  ::= KS   # sat accum
//                  ::= KSD  # sat long accum
//                  ::= KSH  # sat short accum
//                  ::= KSQ  # sat long long accum
//                  ::= KSU  # sat unsigned accum
//                  ::= KSUD # sat unsigned long accum
//                  ::= KSUH # sat unsigned short accum
//                  ::= KSUQ # sat unsigned long long accum
//                  ::= KU   # unsigned accum
//                  ::= KUD  # unsigned long accum
//                  ::= KUH  # unsigned short accum
//                  ::= KUQ  # unsigned long long accum
//                  ::= L    # label
//                  ::= N    # nullptr
//                  ::= R    # fract
//                  ::= RD   # long fract
//                  ::= RH   # short fract
//                  ::= RQ   # long long fract
//                  ::= RS   # sat fract
//                  ::= RSD  # sat long fract
//                  ::= RSH  # sat short fract
//                  ::= RSQ  # sat long long fract
//                  ::= RSU  # sat unsigned fract
//                  ::= RSUD # sat unsigned long fract
//                  ::= RSUH # sat unsigned short fract
//                  ::= RSUQ # sat unsigned long long fract
//                  ::= RU   # unsigned fract
//                  ::= RUD  # unsigned long fract
//                  ::= RUH  # unsigned short fract
//                  ::= RUQ  # unsigned long long fract
//                  ::= S    # string
//                  ::= V    # void
//
//   <call-type> ::= aFUN # __action (action function)
//               ::= aSPE # __special (action special)
//               ::= bFUN # __asmfunc (builtin)
//               ::= cFUN # __native (callfunc)
//               ::= lACS # extern "ACS"
//               ::= lASM # extern "asm"
//               ::= lAXX # extern "ACS++"
//               ::= lC   # extern "C"
//               ::= lCXX # extern "C++"
//               ::= lDS  # extern "DS"
//               ::= sFNI # __scripti
//               ::= sFNS # __scripts
//               ::= sFUN # __script
//
//   <name> ::= { <name-part>+ }
//
//   <name-part> ::= <name-prefix> <alphanumber>+
//
//   <name-prefix> ::= ::
//
//   <number> ::= [ <digit>+ ]
//            ::= [ ]
//            ::= [ * ]
//
//-----------------------------------------------------------------------------

#include "Type.hpp"

#include "ost_type.hpp"
#include "SourceException.hpp"
#include "SourceExpression.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

//
// AddressSpace::getNameMangle
//
void AddressSpace::getNameMangle(std::ostream &out, NameMangleStyle mangle) const
{
   switch(base)
   {
   case ADDR_Generic: break;

   case ADDR_Copy:    Error_p("internal error: ADDR_Copy");    break;
   case ADDR_Literal: Error_p("internal error: ADDR_Literal"); break;
   case ADDR_Stack:   Error_p("internal error: ADDR_Stack");   break;
   case ADDR_Void:    Error_p("internal error: ADDR_Void");    break;

   case ADDR_Far:            out << "aF";  break;
   case ADDR_GlobalArrays:   out << "aGR"; break;
   case ADDR_GlobalRegister: out << "aGS"; break;
   case ADDR_Local:          out << "aL";  break;
   case ADDR_LocalRegister:  out << "aLR"; break;
   case ADDR_MapArrays:      out << "aMS"; break;
   case ADDR_MapRegister:    out << "aMR"; break;
   case ADDR_Near:           out << "aN";  break;
   case ADDR_StringArrays:   out << "aSS"; break;
   case ADDR_Varg:           out << "aVA"; break;
   case ADDR_WorldArrays:    out << "aWS"; break;
   case ADDR_WorldRegister:  out << "aWR"; break;

   case ADDR_GlobalArray: Type::GetNameMangle(name, out << "aGA", mangle); break;
   case ADDR_MapArray:    Type::GetNameMangle(name, out << "aMA", mangle); break;
   case ADDR_StringArray: Type::GetNameMangle(name, out << "aSA", mangle); break;
   case ADDR_WorldArray:  Type::GetNameMangle(name, out << "aWA", mangle); break;
   }
}

//
// TypeQual::getNameMangle
//
void TypeQual::getNameMangle(std::ostream &out, NameMangleStyle mangle) const
{
   addressSpace.getNameMangle(out, mangle);

   if(accessAtomic)   out << 'a';
   if(accessConst)    out << 'c';
   if(accessRestrict) out << 'r';
   if(accessVolatile) out << 'v';
}

//
// Type::Type
//
Type::Type() : qualNone{this}, qualNext{this}, qualPrev{this}, arrType{nullptr},
   arrType0{nullptr}, arrTypeVLA{nullptr}, arrTypeVLA0{nullptr}, lvrType{nullptr},
   ptrType{nullptr}, rvrType{nullptr}
{
   // We hold a reference to this, and so must not count that reference.
   // During destruction, the reference count will be decremented one last time.
   // Because the count will already be at 0, the decrement result will not be 0.
   --refCount;
}

//
// Type::Type
//
Type::Type(Type const &type) : Super{type}, qualNone{type.getQualType()},
   qualNext{type.qualNext}, qualPrev{qualNext->qualPrev}, arrType{nullptr},
   arrType0{nullptr}, arrTypeVLA{nullptr}, arrTypeVLA0{nullptr}, lvrType{nullptr},
   ptrType{nullptr}, rvrType{nullptr}
{
   qualNext->qualPrev = this;
   qualPrev->qualNext = this;
}

//
// Type::~Type
//
Type::~Type()
{
   qualNext->qualPrev = qualPrev;
   qualPrev->qualNext = qualNext;

   // As an addendum to the default constructor comment: Some type objects get
   // allocated statically and set their reference count to 1. Set it to 0 now
   // so that qualNone doesn't delete this.
   refCount = 0;
}

//
// Type::getBase
//
auto Type::getBase() const -> TypeCR
{
   Error_Np("internal error");
}

//
// Type::getCallConv
//
auto Type::getCallConv() const -> CallCon
{
   Error_Np("internal error");
}

//
// Type::getName
//
auto Type::getName() const -> Keyword
{
   Error_Np("internal error");
}

//
// Type::getNameMangle
//
void Type::getNameMangle(std::ostream &out, NameMangleStyle mangle) const
{
   getNameMangleBase(out, mangle);

   quals.getNameMangle(out, mangle);
}

//
// Type::getSizeBitsF
//
bigsint Type::getSizeBitsF() const
{
   Error_Np("internal error");
}

//
// Type::getSizeBitsI
//
bigsint Type::getSizeBitsI() const
{
   Error_Np("internal error");
}

//
// Type::getSizeBytesVM
//
auto Type::getSizeBytesVM() const -> SrcExpR
{
   Error_Np("stub");
 //return SourceExpression::GetSizeLiteral(getSizeBytes());
}

//
// Type::getSizePtrVM
//
auto Type::getSizePtrVM() const -> SrcExpR
{
   Error_Np("stub");
 //return SourceExpression::GetSizeLiteral(getSizePtr());
}

//
// Type::getSizeWordsVM
//
auto Type::getSizeWordsVM() const -> SrcExpR
{
   Error_Np("stub");
 //return SourceExpression::GetSizeLiteral(getSizeWords());
}

//
// Type::getQualType
//
auto Type::getQualType(TypeQual newQuals) const -> TypeCR
{
   if(quals == newQuals) return static_cast<TypeCR>(this);

   for(Type *type = qualNext; type != this; type = type->qualNext)
      if(type->quals == newQuals) return static_cast<TypeCR>(type);

   auto type = clone();
   type->quals = newQuals;
   return type;
}

//
// Type::GetAlignmentHWord
//
bigsint Type::GetAlignmentHWord()
{
   if(Target == TARGET_MageCraft) return 2;

   return 1;
}

//
// Type::GetAlignmentWord
//
bigsint Type::GetAlignmentWord()
{
   if(Target == TARGET_MageCraft) return 4;

   return 1;
}

//
// Type::GetNameMangle <call-type>
//
void Type::GetNameMangle(CallCon conv, std::ostream &out, NameMangleStyle)
{
   switch(conv)
   {
   case CALL_Action:  out << "aFUN"; break;
   case CALL_AsmFunc: out << "bFUN"; break;
   case CALL_LangACS: out << "lACS"; break;
   case CALL_LangASM: out << "lASM"; break;
   case CALL_LangAXX: out << "lAXX"; break;
   case CALL_LangC:   out << "lC";   break;
   case CALL_LangCXX: out << "lCXX"; break;
   case CALL_LangDS:  out << "lDS";  break;
   case CALL_Native:  out << "cFUN"; break;
   case CALL_Script:  out << "sFUN"; break;
   case CALL_ScriptI: out << "sFNI"; break;
   case CALL_ScriptS: out << "sFNS"; break;
   case CALL_Special: out << "aSPE"; break;
   }
}

//
// Type::GetNameMangle <name>
//
void Type::GetNameMangle(ContextKey ctxk, std::ostream &out, NameMangleStyle mangle)
{
   out << '{'; GetNameMangleCTXK(ctxk, out, mangle); out << '}';
}

//
// Type::GetNameMangle <number>
//
void Type::GetNameMangle(bigsint numb, std::ostream &out, NameMangleStyle)
{
   out << '[' << numb << ']';
}

//
// Type::GetNameMangle <type-single>
//
void Type::GetNameMangle(Type const *type, std::ostream &out, NameMangleStyle mangle)
{
   out << '('; type->getNameMangle(out, mangle); out << ')';
}

//
// Type::GetNameMangleCTXK
//
void Type::GetNameMangleCTXK(ContextKey ctxk, std::ostream &out, NameMangleStyle mangle)
{
   if(!ctxk) return;

   GetNameMangleCTXK(ctxk.getBase(), out, mangle);

   out << "::" << ctxk.getKeyword().getString();
}

//
// Type::GetSizeByte
//
bigsint Type::GetSizeByte()
{
   if(Target == TARGET_MageCraft)
      return 8;

   return 32;
}

//
// Type::GetSizeHWord
//
bigsint Type::GetSizeHWord()
{
   if(Target == TARGET_MageCraft)
      return 2;

   return 1;
}

//
// Type::GetSizeWord
//
bigsint Type::GetSizeWord()
{
   if(Target == TARGET_MageCraft)
      return 4;

   return 1;
}

//
// Value::Value
//
Value::Value(Type const *type_, SrcExp *addr_) : type{type_}, addr{addr_}
{
}

//
// Value::~Value
//
Value::~Value()
{
}

//
// Value::Create
//
auto Value::Create(Type const *type, SrcExp *addr) -> Reference
{
   return static_cast<Reference>(new Value(type, addr));
}

// EOF

