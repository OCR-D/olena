// Copyright (C) 2008, 2009 EPITA Research and Development Laboratory (LRDE)
//
// This file is part of Olena.
//
// Olena is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, version 2 of the License.
//
// Olena is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Olena.  If not, see <http://www.gnu.org/licenses/>.
//
// As a special exception, you may use this file as part of a free
// software project without restriction.  Specifically, if other files
// instantiate templates or use macros or inline functions from this
// file, or you compile this file and link it with other files to produce
// an executable, this file does not by itself cause the resulting
// executable to be covered by the GNU General Public License.  This
// exception does not however invalidate any other reasons why the
// executable file might be covered by the GNU General Public License.

#include <mln/core/image/image2d.hh>
#include <mln/core/alias/neighb2d.hh>
#include <mln/value/int_u8.hh>
#include <mln/data/fill.hh>
#include <mln/debug/println.hh>
#include <mln/opt/at.hh>
#include <mln/data/compare.hh>

#include <mln/transform/distance_and_closest_point_geodesic.hh>

unsigned dmap_ref[] = { 4, 4, 4, 3, 2, 3, 4, 4, 4,
			4, 4, 3, 2, 1, 2, 3, 4, 4,
			4, 3, 2, 1, 0, 1, 2, 3, 4,
			3, 2, 1, 2, 1, 2, 3, 4, 4,
			2, 1, 0, 1, 2, 3, 4, 4, 4,
			3, 2, 1, 2, 3, 4, 4, 4, 4,
			4, 3, 2, 3, 4, 4, 4, 4, 4,
			4, 4, 3, 4, 4, 4, 4, 4, 4,
			4, 4, 4, 4, 4, 4, 4, 4, 4 };


unsigned cp_idx_ref[] = { 2, 2, 0, 0, 0, 0, 0, 2, 2,
			  2, 0, 0, 0, 0, 0, 0, 0, 2,
			  1, 0, 0, 0, 0, 0, 0, 0, 0,
			  1, 1, 1, 0, 0, 0, 0, 0, 2,
			  1, 1, 1, 1, 0, 0, 0, 2, 2,
			  1, 1, 1, 1, 0, 0, 2, 2, 2,
			  1, 1, 1, 1, 1, 2, 2, 2, 2,
			  2, 1, 1, 1, 2, 2, 2, 2, 2,
			  2, 2, 1, 2, 2, 2, 2, 2, 2 };


int main()
{
  using namespace mln;
  using value::int_u8;

  image2d<bool> input(9, 9);
  data::fill(input, false);
  opt::at(input, 2, 4) = true;
  opt::at(input, 4, 2) = true;

  {
    typedef util::couple<image2d<int_u8>, image2d<point2d> > output_t;
    output_t output = transform::distance_and_closest_point_geodesic(input,
								     c4(),
								     int_u8(4));
    mln_assertion(output.first() == make::image2d(dmap_ref));
    ///FIXME: test the closest point image here.
  }

  {
    p_array<point2d> arr;
    arr.insert(point2d(2, 4));
    arr.insert(point2d(4, 2));
    typedef util::couple<image2d<int_u8>, image2d<unsigned> > output_t;
    output_t output = transform::distance_and_closest_point_geodesic(arr,
								     input.domain(),
								     c4(),
								     int_u8(4));
    mln_assertion(output.first() == make::image2d(dmap_ref));
    mln_assertion(output.second() == make::image2d(cp_idx_ref));
  }
}
