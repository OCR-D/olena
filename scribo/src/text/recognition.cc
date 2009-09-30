// Copyright (C) 2009 EPITA Research and Development Laboratory (LRDE)
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

#include <iostream>

#include <mln/core/image/image2d.hh>

#include <mln/util/array.hh>

#include <mln/io/txt/save.hh>
#include <mln/io/pbm/load.hh>

#include <mln/value/label_16.hh>

#include <mln/core/alias/neighb2d.hh>

#include <scribo/primitive/extract/objects.hh>

#include <scribo/primitive/group/apply.hh>
#include <scribo/primitive/link/with_several_left_links.hh>
#include <scribo/primitive/link/with_several_right_links.hh>
#include <scribo/primitive/group/from_double_link.hh>
#include <scribo/filter/objects_small.hh>
#include <scribo/filter/objects_thin.hh>
#include <scribo/text/recognition.hh>

#include <scribo/debug/usage.hh>

const char *args_desc[][2] =
{
  { "input.pbm", "A binary image. 'True' for objects, 'False'\
for the background." },
  { "out.txt",   "OCR's output." },
  {0, 0}
};



int main(int argc, char* argv[])
{
  using namespace scribo;
  using namespace mln;

  if (argc != 3)
    return scribo::debug::usage(argv,
				"Text extraction and recognition",
				"input.pbm out.txt",
				args_desc,
				"The text is printed on the standard output and stored in 'out.txt'");

  trace::entering("main");

  image2d<bool> input;
  io::pbm::load(input, argv[1]);

  /// Extract text.
  typedef mln_ch_value_(image2d<bool>,value::label_16) L;
  value::label_16 nbboxes;
  object_image(L)
    objects = scribo::primitive::extract::objects(input, c8(), nbboxes);

  /// Filter non interesting objects
  objects = filter::objects_small(objects, 4);
  objects = filter::objects_thin<L>(objects, 2);

  /// Group objects.
  object_links<L> left_link
	= primitive::link::with_several_left_links(objects, 30);
  object_links<L> right_link
	= primitive::link::with_several_right_links(objects, 30);
  object_groups<L>
    groups = primitive::group::from_double_link(objects, left_link, right_link);

  objects = primitive::group::apply(objects, groups);
  /// Try to recognize text in grouped objects.
  scribo::text::recognition(objects, "fra", argv[2]);

  trace::exiting("main");
}