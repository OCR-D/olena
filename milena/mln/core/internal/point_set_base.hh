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

#ifndef MLN_CORE_INTERNAL_POINT_SET_BASE_HH
# define MLN_CORE_INTERNAL_POINT_SET_BASE_HH

/*! \file mln/core/internal/point_set_base.hh
 *
 * \brief Definition of a base class for point set classes.
 */

# include <mln/core/concept/point_set.hh>
# include <mln/core/grids.hh>


namespace mln
{

  namespace internal
  {

    /*! \internal A base class for point set classes.
     * \p P is a point site type.
     */
    template <typename P, typename E>
    struct point_set_base_ : public Site_Set<E>
    {
      /// Point_Site associated type.
      typedef P psite;

      /// Mesh associated type.
      typedef mln_mesh(P) mesh;

      /// Point associated type.
      typedef mln_point(P) point;

      /// Dpoint associated type.
      typedef mln_dpoint(point) dpoint;

    protected:
      point_set_base_();
    };


# ifndef MLN_INCLUDE_ONLY

    template <typename S, typename E>
    inline
    point_set_base_<S,E>::point_set_base_()
    {
    }

# endif // ! MLN_INCLUDE_ONLY

  } // end of namespace mln::internal

} // end of namespace mln


#endif // ! MLN_CORE_INTERNAL_POINT_SET_BASE_HH
