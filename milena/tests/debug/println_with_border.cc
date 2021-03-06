// Copyright (C) 2007, 2008, 2009 EPITA Research and Development Laboratory (LRDE)
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
#include <mln/core/image/image1d.hh>
#include <mln/data/fill.hh>
#include <mln/debug/println.hh>
#include <mln/debug/println_with_border.hh>
#include <mln/opt/at.hh>


using namespace mln;


int main()
{
  border::thickness = 3;

  {
    image2d<bool> msk(3, 3);
    opt::at(msk, 0, 0) = true;
    opt::at(msk, 1, 0) = true;
    opt::at(msk, 2, 0) = true;

    opt::at(msk, 0, 1) = true;
    opt::at(msk, 1, 1) = false;
    opt::at(msk, 2, 1) = true;

    opt::at(msk, 0, 2) = true;
    opt::at(msk, 1, 2) = true;
    opt::at(msk, 2, 2) = true;

    debug::println(msk);
    debug::println_with_border(msk);
  }

  {
    image1d<bool> msk(3);
    opt::at(msk, 0) = false;
    opt::at(msk, 1) = true;
    opt::at(msk, 2) = false;

    debug::println(msk);
    debug::println_with_border(msk);

  }

}
