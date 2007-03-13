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

#ifndef OLN_CORE_2D_BOX2D_HH
# define OLN_CORE_2D_BOX2D_HH

# include <oln/core/2d/point2d.hh>


namespace oln
{

  // Fwd decl.
  template <typename P> class box_;


  // FIXME: box2d should be an actual type, not an alias...
  typedef box_<point2d> box2d;


  /// init__
  namespace internal
  {

    template <typename C>
    void init__(box2d& b,
		const initializer_< pair< nrows_t<C>, ncols_t<C> > >& data);

    template <typename C>
    void init__(box2d& b,
		const initializer_< triplet< from_t<point2d>, nrows_t<C>, ncols_t<C> > >& data);

  } // end of namespace oln::internal

} // end of namespace oln


# include <oln/core/gen/box.hh>


# ifndef OLN_INCLUDE_ONLY

namespace oln
{

  namespace internal
  {

    template <typename C>
    void init__(box2d& b,
		const initializer_< pair< nrows_t<C>, ncols_t<C> > >& data)
    {
      C nrows = data->value1.value;
      C ncols = data->value2.value;
      precondition(nrows > 0 and ncols > 0);
      b.pmin().row() = 0;
      b.pmin().col() = 0;
      b.pmax().row() = nrows - 1;
      b.pmax().col() = ncols - 1;
    }

    template <typename C>
    void init__(box2d& b,
		const initializer_< triplet< from_t<point2d>, nrows_t<C>, ncols_t<C> > >& data)
    {
      C nrows = data->value2.value;
      C ncols = data->value3.value;
      precondition(nrows > 0 and ncols > 0);
      b.pmin() = data->value1.value;
      b.pmax().row() = b.pmin().row() + nrows - 1;
      b.pmax().col() = b.pmin().col() + ncols - 1;
    }

  } // end of namespace oln::internal

} // end of namespace oln


# endif // OLN_INCLUDE_ONLY


#endif // ! OLN_CORE_2D_BOX2D_HH
