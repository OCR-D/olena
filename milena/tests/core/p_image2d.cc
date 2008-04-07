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

/*! \file tests/core/p_image2d.cc
 *
 * \brief Tests on mln::p_image2d.
 */

#include <mln/core/p_image2d.hh>

int main()
{
  using namespace mln;

  trace::quiet = false;
  p_image2d<point2d> ps(20,20);
  ps
    .insert(make::point2d(6, 9))
    .insert(make::point2d(4, 2))
    .insert(make::point2d(4, 2))
    .insert(make::point2d(5, 1));
  mln_assertion(ps.npoints() == 3);

  ps.remove(make::point2d(5, 1));
  ps.remove(make::point2d(5, 1));

  ps.remove(make::point2d(6, 9));
  ps.remove(make::point2d(4, 2));

  mln_assertion(ps.npoints() == 0);
  mln_assertion(ps.is_empty());

  std::cout << ps << std::endl;

  mln_fwd_piter_(box2d) p(inplace(make::box2d(13,13,19,15)));
  for_all(p)
  {
    ps.insert(p);
  }
  ps.clear();
  for_all(p)
  {
    mln_assertion(!ps.has(p));
  }

}
