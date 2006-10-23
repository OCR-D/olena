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

/// Test the stack morpher.

#include <cassert>
#include <oln/basics2d.hh>
#include <oln/morpher/stack.hh>


int main()
{
  using namespace oln;
  typedef image2d<int> image_t;

  image_t ima(3,3);
  morpher::stack<2,image_t> ima_v = stack(ima, ima);

  point2d p(2, 2);
  ima(p) = 51;

  assert(ima(p) == ima_v.image(1)(p));
  assert(ima_v.at(2, 2) == ima_v(p));
}
