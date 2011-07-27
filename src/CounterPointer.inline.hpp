/* Copyright (C) 2010, 2011 David Hill
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

/* CounterPointer.inline.hpp
**
** Defines inline functions for CounterPointer.hpp.
*/

#ifndef HPP_CounterPointer_
#error "Do not include this file directly."
#endif



template<typename T> inline CounterPointer<T>::CounterPointer() : _p(NULL)
{

}
template<typename T> inline CounterPointer<T>::CounterPointer(CounterPointer<T> const & p) : _p(p._p)
{
	if (_p) ++_p->_referenceCount;
}
template<typename T> template<typename T2> inline CounterPointer<T>::CounterPointer(CounterPointer<T2> const & p) : _p((T2*)p)
{
	if (_p) ++_p->_referenceCount;
}
template<typename T> template<typename T2> inline CounterPointer<T>::CounterPointer(T2 * const p) : _p(p)
{
	if (_p) ++_p->_referenceCount;
}
template<typename T> inline CounterPointer<T>::CounterPointer(T * const p) : _p(p)
{
	if (_p) ++_p->_referenceCount;
}
template<typename T> inline CounterPointer<T>::~CounterPointer()
{
	if (_p && (--_p->_referenceCount == 0))
		delete _p;
}

template<typename T> inline size_t CounterPointer<T>::count() const
{
	return _p ? _p->_referenceCount : 0;
}

template<typename T> inline CounterPointer<T>::operator T * () const
{
	return _p;
}

template<typename T> inline CounterPointer<T> & CounterPointer<T>::operator = (CounterPointer<T> const & p)
{
	T * const old = _p;

	_p = p._p;

	if (_p) ++_p->_referenceCount;

	if (old && (--old->_referenceCount == 0))
		delete old;

	return *this;
}
template<typename T> inline CounterPointer<T> & CounterPointer<T>::operator = (T * const p)
{
	T * const old = _p;

	_p = p;

	if (_p) ++_p->_referenceCount;

	if (old && (--old->_referenceCount == 0))
		delete old;

	return *this;
}

template<typename T> inline bool CounterPointer<T>::operator == (CounterPointer<T> const & p) const
{
	return _p == p._p;
}
template<typename T> inline bool CounterPointer<T>::operator == (T const * const p) const
{
	return _p == p;
}
template<typename T> inline bool CounterPointer<T>::operator != (CounterPointer<T> const & p) const
{
	return _p != p._p;
}
template<typename T> inline bool CounterPointer<T>::operator != (T const * const p) const
{
	return _p != p;
}

template<typename T> inline T * CounterPointer<T>::operator -> () const
{
	return _p;
}
template<typename T> inline T & CounterPointer<T>::operator * () const
{
	return *_p;
}

inline Counter::Counter() : _referenceCount(0)
{

}
inline Counter::Counter(Counter const & c) : _referenceCount(0)
{
	(void)c;
}
inline Counter::~Counter()
{

}

inline Counter & Counter::operator = (Counter const & c)
{
	(void)c;

	return *this;
}



