// Copyright (C) 2009 EPITA Research and Development Laboratory (LRDE)
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
// License.
// reasons why the executable file might be covered by the GNU General
// Public License.

#ifndef MLN_MAKE_RAG_AND_ALBL_HH
# define MLN_MAKE_RAG_AND_ALBL_HH

/// \file mln/make/rag_and_labeled_wsl.hh
///
/// Create a region adjacency graph and a label image of the watershed line
/// from a watershed image.
///
/// \sa morpho::watershed::flooding.
///
/// \todo Do not use mln_max() as background label!

# include <map>

# include <mln/core/concept/image.hh>
# include <mln/core/concept/neighborhood.hh>
# include <mln/core/image/image2d.hh>
# include <mln/core/alias/box2d.hh>
# include <mln/extension/adjust_fill.hh>
# include <mln/util/graph.hh>
# include <mln/util/couple.hh>


namespace mln
{

  namespace make
  {

    /// Create a region adjacency graph and a label image of the watershed line
    /// from a watershed image.
    ///
    /// \param[in] wshd Watershed image.
    /// \param[in] nbh Neighborhood
    /// \param[in] nbasins Number of influence zone in \p wshd.
    ///
    /// \return A couple. First element is the graph, second element is an
    /// image with a labeled watershed line.
    template <typename I, typename N>
    util::couple<util::graph,mln_concrete(I)>
    rag_and_labeled_wsl(const Image<I>& wshd_,
			const Neighborhood<N>& nbh_,
			const mln_value(I)& nbasins);



# ifndef MLN_INCLUDE_ONLY


    namespace internal
    {

      template <typename I, typename N>
      inline
      void
      rag_and_labeled_wsl_tests(const Image<I>& wshd,
				const Neighborhood<N>& nbh,
				const mln_value(I)&)
      {
	mln_precondition(exact(wshd).is_valid());
	mln_precondition(exact(nbh).is_valid());
	(void) wshd;
	(void) nbh;
      }

    } // end of namespace mln::make::internal


    namespace impl
    {

      namespace generic
      {

	template <typename I, typename N>
	inline
	util::couple<util::graph,mln_concrete(I)>
	rag_and_labeled_wsl(const Image<I>& wshd_,
			    const Neighborhood<N>& nbh_,
			    const mln_value(I)& nbasins)
	{
	  trace::entering("make::impl::generic::rag_and_labeled_wsl");

	  internal::rag_and_labeled_wsl_tests(wshd_, nbh_, nbasins);
	  const I& wshd = exact(wshd_);
	  const N& nbh = exact(nbh_);
	  typedef mln_value(I) L;

	  mln::image2d<bool> adj(mln::box2d(nbasins.next(), nbasins.next()));
	  data::fill(adj, false);
	  extension::adjust_fill(wshd, nbh, 0u);

	  mln_concrete(I) output(wshd.domain());

	  // FIXME: we would like to not use mln_value() but 0 instead.
	  // We can't do that because graph edges are numbered from 0.
	  data::fill(output, mln_max(mln_value(I)).prev());

	  std::map<util::couple<L,L>, mln_value(I)> c2la;
	  util::array<util::couple<L,L> > la2c;
	  mln_value(I) nlabels = literal::zero;

	  L l1, l2;
	  mln_piter(I) p(wshd.domain());
	  mln_niter(N) n(nbh, p);
	  for_all(p)
	  {
	    if (wshd(p) != 0u)
	      continue;
	    // p is in the watershed line.
	    l1 = l2 = 0;
	    for_all(n)
	      if (wshd.has(n) && wshd(n) != 0u)
	      {
		if (l1 == 0u) // First label to be stored.
		  l1 = wshd(n);
		else
		  if (wshd(n) != l1) // Useless: && l2 == 0)
		  { // Second label to be stored.
		    mln_invariant(l2 == 0u);
		    l2 = wshd(n);
		    break;
		  }
	      }
	    if (l2 == 0u || l1 == 0u)
	      continue;
	    if (l2 < l1)
	      std::swap(l1, l2);

	    // adjacency l1 l2
	    util::couple<L,L> c = make::couple(l2,l1);
	    if (c2la.find(c) == c2la.end())
	    {
	      c2la[c] = nlabels++;
	      la2c.append(c);
	    }
	    output(p) = c2la[c];

	  }

	  // Construct graph.
	  util::graph g;
	  g.add_vertices(nbasins.next());
	  for (unsigned i = 0; i < la2c.nelements(); ++i)
	    g.add_edge(la2c[i].first(), la2c[i].second());

	  trace::exiting("make::impl::generic::rag_and_labeled_wsl");
	  return make::couple(g, output);
	}

      } // end of namespace mln::make::impl::generic

    } // end of namespace mln::make::impl



    namespace internal
    {

      template <typename I, typename N>
      inline
      util::couple<util::graph,mln_concrete(I)>
      rag_and_labeled_wsl_dispatch(const Image<I>& wshd,
				   const Neighborhood<N>& nbh,
				   const mln_value(I)& nbasins)
      {
	return make::impl::generic::rag_and_labeled_wsl(wshd, nbh, nbasins);
      }

    } // end of namespace mln::make::internal



    // Facade

    template <typename I, typename N>
    inline
    util::couple<util::graph,mln_concrete(I)>
    rag_and_labeled_wsl(const Image<I>& wshd,
			const Neighborhood<N>& nbh,
			const mln_value(I)& nbasins)
    {
      trace::entering("make::rag_and_labeled_wsl");

      internal::rag_and_labeled_wsl_tests(wshd, nbh, nbasins);

      util::couple<util::graph,mln_concrete(I)>
	result = internal::rag_and_labeled_wsl_dispatch(wshd, nbh, nbasins);

      trace::exiting("make::rag_and_labeled_wsl");
      return result;
    }


# endif // ! MLN_INCLUDE_ONLY


  } // end of namespace mln::make

} // end of namespace mln


#endif // ! MLN_MAKE_RAG_AND_ALBL_HH
