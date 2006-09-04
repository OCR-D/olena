// Copyright (C) 2006 EPITA Research and Development Laboratory
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

/// Test oln::abstract::image_entry.

// FIXME: Check also with a (possibly fake) ntg::int_u<1, B> type, to
// stress image_typeness.hh.  This might be done in another test, of
// course.

#include <oln/core/1d/grid1d.hh>
#include <oln/core/abstract/image_entry.hh>


namespace my
{
  // Forward declaration.
  class image;
}

namespace oln
{
  // Warning, this sugar might be removed in the future.
  stc_set_super(my::image, abstract::image_entry<my::image>);

  /// Virtual types associated to my::image.
  template<>
  struct vtypes_in_category<category::image, my::image>
  {
    // The switch for image_dimension (above image_entry) plugs the
    // inheritance relation to the right abstract::image_dimension
    // class using the sole grid information (the grid can be seen
    // here as a ``tag'').
    typedef oln::grid1d grid_type;

    // Likewise, for the switch of image_typeness using the type of
    // the value of the image.
    typedef char value_type;
  };
}

namespace my
{
  // A very simple 1-D image.
  class image : public oln::set_super_type<my::image>::ret
  {
  public:
    typedef image self_type;
    // Internal vtypes.
    typedef oln_type_of_(self_type, grid) grid_type;
    typedef oln_type_of_(self_type, value) value_type;
    // External vtypes.
    typedef oln_type_of_(self_type, image_dimension) image_dimension_type;
    typedef oln_type_of_(self_type, image_typeness) image_typeness_type;
  };
}

int
main()
{
  // Check its internally defined vtypes.
  mlc::assert_< mlc_eq(my::image::grid_type, oln::grid1d) >::check();
  mlc::assert_< mlc_eq(my::image::value_type, char) >::check();
  // Check its externally defined vtypes.
  mlc::assert_<
    mlc_eq( my::image::image_dimension_type,
	    stc::abstraction_as_type<oln::abstract::image1d> )
    >::check();
  mlc::assert_<
    mlc_eq( my::image::image_typeness_type,
	    stc::abstraction_as_type<oln::abstract::grey_level_image> )
    >::check();

  // Check its image dimension abstraction.
  mlc::assert_< mlc_is_a_(my::image, oln::abstract::image1d) >::check();
  // Check its image ``typeness'' abstraction.
  mlc::assert_< mlc_is_a_(my::image,
			  oln::abstract::grey_level_image) >::check();

  // Ensure we can instantiate it.
  my::image i;

  // Print my::image's vtypes.
  // FIXME: To be removed in the final version.
  oln::packed_vtypes_in_category< oln::category::image,
                                  my::image >::echo (std::cout);
}
