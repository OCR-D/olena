//						       		-*- C++ -*-
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

/// \file image2d.i
/// \brief A wrapper of mln::image2d<T>.

%module image2d

%import "point2d.i"
%import "dpoint2d.i"
%import "box2d.i"

%{
#include "mln/core/image/image2d.hh"
%}

// Shortcuts macros.
%include "mln/core/macros.hh"

// Meta-expressions used in traits.
%include "mln/metal/equal.hh"
%include "mln/metal/if.hh"
%include "mln/metal/is_const.hh"

// Traits.
%include "mln/trait/value_.hh"
%include "mln/trait/images.hh"

// Concept.
%include "mln/core/concept/image.hh"

// Base classes.
%include "mln/core/internal/check/image_fastest.hh"
%include "mln/core/internal/check/image_all.hh"
%include "mln/core/internal/image_base.hh"
%include "mln/core/internal/image_primary.hh"

// mln::image2d definition.
%include "mln/core/image/image2d.hh"

// FIXME: Doc.
%define instantiate_image2d(I, T)
  // Instantiate base classes of mln::image2d<T> so that Swig knows it
  // derives from mln::Image.
  %template() mln::internal::image_primary< T, mln::box2d, mln::image2d< T > >;
  %template() mln::internal::image_base< T, mln::box2d, mln::image2d< T > >;
  %template() mln::internal::image_checked_< mln::image2d< T > >;
  // Instantiate mln::image2d<T>
  %template(I) mln::image2d< T >;
%enddef
