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

/*! \file tests/io_pbm.cc
 *
 * \brief Test on mln::io::pbm::load and mln::io::pbm::save.
 */

#include <mln/core/image2d.hh>

#include <mln/value/int_u8.hh>
#include <mln/pw/all.hh>

#include <mln/io/pgm/load.hh>
#include <mln/io/pbm/load.hh>
#include <mln/io/pbm/save.hh>

#include <mln/level/compare.hh>
#include <mln/level/fill.hh>

#include <mln/display/show.hh>
#include <mln/display/save.hh>

#include <mln/border/thickness.hh>

int main()
{
  using namespace mln;
  using typename value::int_u8;

  typedef image2d< bool > I;

  border::thickness = 0;
  image2d< int_u8 >
    lena = io::pgm::load("../img/lena.pgm");


  I out(lena.domain());
  level::fill(out, pw::value(lena) > pw::cst(127));
  io::pbm::save(out, "out.pbm");

  {
    I lena = io::pbm::load("out.pbm");
    image2d<bool> out(lena.domain());

    io::pbm::save(lena, "out2.pbm");

    I lena2 = io::pbm::load("out2.pbm");

    io::pbm::save(lena2, "out3.pbm");

    mln_assertion(lena.domain() == lena2.domain());
    mln_assertion(lena == lena2);
  }
}
