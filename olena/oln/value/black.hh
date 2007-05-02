// Copyright (C) 2007 EPITA Research and Development Laboratory
//
// This file is part of the Olena Library.  This library is free
// software; you can redistribute it and/or modify it under the terms
// of the GNU General Public License version 2 as published by the
// Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this library; see the file COPYING.  If not, write to
// the Free Software Foundation, 51 Franklin Street, Fifth Floor,
// Boston, MA 02111-1307, USA.
//
// As a special exception, you may use this file as part of a free
// software library without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to
// produce an executable, this file does not by itself cause the
// resulting executable to be covered by the GNU General Public
// License.  This exception does not however invalidate any other
// reasons why the executable file might be covered by the GNU General
// Public License.

#ifndef OLN_CORE_VALUE_ZERO_HH
# define OLN_CORE_VALUE_ZERO_HH

# include <oln/core/concept/literal.hh>


namespace oln
{

  struct black_t : public Literal< black_t >
  {

    template <typename T>
    operator T() const;

    // builtins:
    operator unsigned() const;
    operator int() const;
    operator float() const;
    operator double() const;
  }

  black;



  // Default unary minus operator.

  template <typename T>
  T
  operator - (const Any<T>& rhs); // Use binary -.



# ifndef OLN_INCLUDE_ONLY

  template <typename T>
  black_t::operator T() const
  {
    mlc::assert_< mlc_is_a(T, Any) >::check(); // FIXME: Add err msg.
    T tmp;
    tmp.assign_(*this);
    return tmp;
  }

  black_t::operator unsigned() const { return 0u;  }
  black_t::operator int()      const { return 0;   }
  black_t::operator float()    const { return 0.f; }
  black_t::operator double()   const { return 0.;  }


  template <typename T>
  T
  operator - (const Any<T>& rhs)
  {
    return black - exact(rhs);
  }

# endif // ! OLN_INCLUDE_ONLY

} // end of namespace oln


#endif // ! OLN_CORE_VALUE_BLACK_HH
