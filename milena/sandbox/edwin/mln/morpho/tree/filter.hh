// Copyright (C) 2009 EPITA Research and Development Laboratory
// (LRDE)
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

#ifndef MLN_MORPHO_TREE_FILTER_HH_
# define MLN_MORPHO_TREE_FILTER_HH_

/**
** @file   mln/morpho/tree/filter.hh
**
** @brief Methods to handle component tree filtering strategies with
** non-increasing attribute. Nevertheless, it works on increasing
** predicate as well. In this case, all strategies have the same
** result but min filter or direct filter should be used in term
** of performance. If a predicate test is not enough fast, then
** prefer the min filter that minimizes calls to predicate.
*/

# include <mln/core/concept/function.hh>
# include <mln/morpho/tree/data.hh>
# include <mln/morpho/tree/propagate_if.hh>

namespace mln {
  namespace morpho {
    namespace tree {
      namespace filter {


	template <typename T, typename F, typename P2B>
	inline
	void
	filter(const T& tree, Image<F>& f_, const Function_v2b<P2B>& pred_, const mln_value(F)& v);


# ifndef MLN_INCLUDE_ONLY

	template <typename T, typename F, typename P2B>
	inline
	void
	filter(const T& tree, Image<F>& f_, const Function_v2b<P2B>& pred_, const mln_value(F)& v)
	{
	  F& f = exact(f_);
	  const P2B& pred = exact(pred_);

	  trace::entering("mln::morpho::tree::filter::filter");

	  mln_ch_value(F, bool) mark;
	  initialize(mark, f);
	  mln::data::fill(mark, false);

	  mln_dn_node_piter(T) n(tree);
	  for_all(n)
	    if (mark(tree.parent(n)) || !pred(n))
	      {
		f(n) = v;
		mark(n) = true;
	      }

	  trace::exiting("mln::morpho::tree::filter::filter");
	}

# endif /* !MLN_INCLUDE_ONLY */

      } // end of namespace mln::morpho::tree::filter
    } // end of namespace mln::morpho::tree
  } // end of namespace mln::morpho
} // end of namespace mln


#endif /* !MLN_MORPHO_TREE_FILTER_HH_ */
