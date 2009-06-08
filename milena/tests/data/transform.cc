// Copyright (C) 2007, 2008, 2009 EPITA Research and Development
// Laboratory (LRDE)
//
// This file is part of the Milena Library.  This library is free
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

/// \file tests/data/transform.cc
///
/// Tests on mln::data::transform.

#include <cmath>

#include <mln/core/image/image1d.hh>
#include <mln/core/image/image2d.hh>
#include <mln/core/image/image3d.hh>
#include <mln/pw/image.hh>
#include <mln/core/image/flat_image.hh>
#include <mln/core/image/vmorph/cast_image.hh>
#include <mln/core/image/dmorph/image_if.hh>
#include <mln/core/image/dmorph/sub_image.hh>
#include <mln/core/image/dmorph/extension_val.hh>


#include <mln/data/fill.hh>
#include <mln/data/transform.hh>
#include <mln/data/paste.hh>

#include <mln/core/var.hh>

#include <mln/fun/p2b/chess.hh>
#include <mln/fun/p2v/iota.hh>

#include <mln/debug/iota.hh>
#include <mln/debug/println.hh>


struct mysqrt : mln::Function_v2v<mysqrt>
{
  typedef unsigned short result;
  template <typename T>
  result operator()(T c) const
  {
    return static_cast<result>( std::sqrt(float(c)) );
  }
};



int main()
{
  using namespace mln;
  const unsigned size = 50;

  /// image 1d test
  {
    image1d<unsigned short> ima(size);
    image1d<unsigned short> out(size);

    debug::iota(ima);
    out = data::transform(ima, mysqrt());

    box_fwd_piter_<point1d> p(out.domain());
    for_all(p)
      mln_assertion((unsigned short)std::sqrt(ima(p)) == out(p));
  }


  /// image 2d test
  {
    image2d<unsigned short> ima(size, size);
    image2d<unsigned short> out(size, size);

     debug::iota(ima);
     out = data::transform(ima, mysqrt());

     box_fwd_piter_<point2d> p(out.domain());
     for_all(p)
       mln_assertion((unsigned short)std::sqrt(ima(p)) == out(p));
  }

  /// Another image2d test
  {
     image2d<unsigned short> ima(size, size);

     data::fill_with_value(ima, 51);
     data::transform(ima, mysqrt());

  }

  /// image 3d test
  {
    image3d<unsigned short> ima(size, size, size);
    image3d<unsigned short> out(size, size, size);

    debug::iota(ima);
    out = data::transform(ima, mysqrt());

    box_fwd_piter_<point3d> p(out.domain());
    for_all(p)
      mln_assertion((unsigned short)std::sqrt(ima(p)) == out(p));
  }

  /// pw image test
  {
    const pw::image<fun::p2v::iota, box2d> ima(fun::p2v::iota(),
                                                 make::box2d(2,2, 5,5));
    image2d<unsigned short> out(8, 8);

    data::fill(out, (short unsigned int)0);
    out = data::transform(ima, mysqrt());
  }

  // flat image test
  {
    flat_image<short, box2d> ima(5, make::box2d(size, size));
    image2d<unsigned short> out(size, size);

    data::fill_with_value(ima, 51);
    out = data::transform(ima, mysqrt());

    box2d::piter p(out.domain());
    for_all(p)
      mln_assertion((unsigned short)std::sqrt(ima(p)) == out(p));
  }

  // image if test
  {
    typedef image2d<unsigned short> I;
    typedef image_if<I, fun::p2b::chess> II;

    I ima(size, size);
    II ima_if = ima | fun::p2b::chess();

    data::fill_with_value(ima, 0);
    debug::iota(ima);
    II out = data::transform(ima_if, mysqrt());

    II::piter p(ima_if.domain());
    for_all(p)
      mln_assertion((unsigned short)std::sqrt(ima_if(p)) == out(p));
  }

  // cast image test
  {
    typedef image2d<unsigned short> I;
    typedef cast_image_<int, I> II;
    typedef image2d<unsigned short> III;

    I in(size, size);
    II cast(in);
    III out(size, size);

    data::fill(in, (unsigned short)51);
    data::fill(out, (unsigned short)42);

    out = data::transform(cast, mysqrt());

    II::piter p(cast.domain());
    for_all(p)
      mln_assertion((unsigned short)std::sqrt(cast(p)) == out(p));
  }

  // sub_image test
  {
    typedef image2d<int> I;
    typedef sub_image< image2d<int>, box2d > II;
    typedef sub_image< image2d<unsigned short>, box2d > III;

    I ima(size, size);
    II sub_ima(ima, make::box2d(4,4, 10,10));

    data::fill(ima, 51);
    III out = data::transform(sub_ima, mysqrt());

    II::piter p(sub_ima.domain());
    for_all(p)
      mln_assertion((unsigned short)std::sqrt(sub_ima(p)) == out(p));
  }

  // extended image test
  {
    typedef image2d<int> I;
    typedef extension_val< image2d<int> > II;
    typedef extension_val< image2d<unsigned short> > III;

    I ima(size, size);
    II extend_ima(ima, 5);

    data::fill(ima, 51);
    III out = data::transform(extend_ima, mysqrt());

    II::piter p(extend_ima.domain());
    for_all(p)
      mln_assertion((unsigned short)std::sqrt(extend_ima(p)) == out(p));
  }
}