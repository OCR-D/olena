// Copyright (C) 2007, 2008, 2009 EPITA Research and Development Laboratory
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

#ifndef MLN_CANVAS_MORPHO_CONNECTED_FILTER_HH
# define MLN_CANVAS_MORPHO_CONNECTED_FILTER_HH

/// \file mln/canvas/morpho/connected_filter.hh
///
/// Connected filters dispatch (algebraic & leveling filters).


# include <mln/core/concept/image.hh>
# include <mln/core/concept/neighborhood.hh>
# include <mln/core/concept/accumulator.hh>

# include <mln/trait/accumulators.hh>

# include <mln/level/sort_psites.hh>
# include <mln/level/sort_offsets.hh>

# include <mln/util/pix.hh>

# include <mln/data/fill.hh>

namespace mln {
  namespace canvas {
    namespace morpho {

      // Facade Fwd Declaration
      template <typename I, typename N, typename A>
      mln_concrete(I)
      connected_filter(const Image<I>& input, const Neighborhood<N>& nbh,
		       const Accumulator<A>& a, const typename A::result& lambda,
		       bool increasing);

      template <typename I, typename N, typename A>
      mln_concrete(I)
      leveling_filter(const Image<I>& input, const Neighborhood<N>& nbh,
		      const Accumulator<A>& a, const typename A::result& lambda,
		      bool increasing);

      template <typename I, typename N, typename A>
      mln_concrete(I)
      algebraic_filter(const Image<I>& input, const Neighborhood<N>& nbh,
		       const Accumulator<A>& a, const typename A::result& lambda,
		       bool increasing);

# ifndef MLN_INCLUDE_ONLY

      namespace impl {

	template <typename A, typename I>
	void take_as_init_fastest (trait::accumulator::when_pix::use_none, A& accu,
			   const I& input, const unsigned p)
	{
	  (void)input;
	  (void)p;
	  accu.take_as_init ();
	}

	template <typename A, typename I, typename P>
	void take_as_init (trait::accumulator::when_pix::use_p, A& accu,
			   const I& input, const P& p)
	{
	  (void)input;
	  accu.take_as_init (p);
	}

	template <typename A, typename I, typename P>
	void take_as_init (trait::accumulator::when_pix::use_none, A& accu,
			   const I& input, const P& p)
	{
	  (void)input;
	  accu.take_as_init (p);
	}

	template <typename A, typename I, typename P>
	void take_as_init (trait::accumulator::when_pix::use_pix, A& accu,
			   const I& input, const P& p)
	{
	  accu.take_as_init (make::pix(input, p));
	}

	template <typename A, typename I, typename P>
	void take_as_init (trait::accumulator::when_pix::use_v, A& accu,
			   const I& input, const P& p)
	{
	  accu.take_as_init (make::pix(input, p));
	}

	template <typename A, typename I>
	void take_as_init_fastest (trait::accumulator::when_pix::use_v, A& accu,
				   const I& input, const unsigned p)
	{
	  accu.take_as_init (input.element (p));
	}


	template <typename A, typename I, typename P>
	void take_as_init (A& accu, const I& input, const P& p)
	{
	  take_as_init (mln_trait_accumulator_when_pix(A)(), accu, input, p);
	}

	template <typename A, typename I, typename P>
	void take_as_init_fastest (A& accu, const I& input, const P& p)
	{
	  take_as_init_fastest (mln_trait_accumulator_when_pix(A)(), accu, input, p);
	}


	namespace generic {


	  ////////////////////////
	  /// Generic version. ///
	  ////////////////////////


	  template <typename I>
	  static inline
	  mln_psite(I)
	  find_root(I& parent, const mln_psite(I) & x)
	  {
	    if (parent(x) == x)
	      return x;
	    else
	      return parent(x) = find_root(parent, parent(x));
	  }

	  template <typename I, typename N, typename S, typename A>
	  mln_concrete(I)
	  connected_filter (const Image<I>& input_,
			    const Neighborhood<N>& nbh_,
			    const Site_Set<S>& s_,
			    const Accumulator<A>& a_,
			    const typename A::result& lambda)
	  {
	    trace::entering("canvas::morpho::impl::generic::connected_filter");
	    // FIXME: Test?!

	    const I& input = exact(input_);
	    const N& nbh   = exact(nbh_);
	    const S& s     = exact(s_);
	    (void)a_; // To avoid warning at compilation

	    mln_concrete(I) output;
	    initialize(output, input);

	    // Local type.
	    typedef mln_psite(I) P;


	    // Auxiliary data.
	    mln_ch_value(I, bool) deja_vu;
	    mln_ch_value(I, bool) activity;
	    mln_ch_value(I, P)    parent;
	    mln_ch_value(I, A)    data;

	    // Initialization.
	    {
	      initialize(deja_vu, input);
	      data::fill(deja_vu, false);
	      initialize(activity, input);
	      data::fill(activity, true);
	      initialize(parent, input);
	      initialize(data, input);
	      //a.init(); // init required.
	    }

	    // First pass.
	    {
	      mln_fwd_piter(S) p(s); // s required.
	      mln_niter(N) n(nbh, p);

	      for_all(p)
	      {
		// Make set.
		{
		  parent(p) = p;

		  // Check accumulator trait to handle argument type (Pix or Site).
		  take_as_init (data(p), input, p);
		}

		for_all(n)
		  if (input.domain().has(n) && deja_vu(n))
		    {
		      //do_union(n, p);
		      P r = find_root(parent, n);
		      if (r != p)
			{
			  if (input(r) == input(p) || (activity(r) && (data(r) < lambda)))     // Equiv(r, p)
			    // Either a flat zone or the component of r is still growing.
			    {
			      /* FIXME: Same remark as above concerning the
			      initialization of data(p); instead of

			      data(p).take(data(r));

			      we should (or could) have

			      unite_data(p, r);

			      so as to keep the generic aspect of this canvas
			      (as long as the set of acceptable types for the
			      template parameter A is not bound).  */

			      data(p).take(data(r));
			      parent(r) = p;
			      if (activity(r) == false)
				activity(p) = false;
			    }
			  else
			    {
			      activity(p) = false;
			    }
			}
		    }
		deja_vu(p) = true;
	      }
	    }

	    // Second pass.
	    {
	      mln_bkd_piter(S) p(s);
	      for_all(p)
		if (parent(p) == p) // p is root.
		  output(p) = input(p);
		else
		  output(p) = output(parent(p));
	    }

	    trace::exiting("canvas::morpho::impl::generic::connected_filter");
	    return output;
	  }

	} // end of namespace mln::canvas::morpho::impl::generic


	  ////////////////////////
	  /// Fastest version. ///
	  ////////////////////////

	template <typename I>
	inline
	unsigned
	find_root_fastest(I& parent, unsigned x)
	{
	  if (parent.element(x) == x)
	    return x;
	  else
	    return parent.element(x) = find_root_fastest(parent, parent.element(x));
	}

	template <typename I, typename N, typename A>
	mln_concrete(I)
	connected_filter_fastest(const Image<I>& input_,
				 const Neighborhood<N>& nbh_,
				 const util::array<unsigned>& s,
				 const Accumulator<A>& a_,
				 const typename A::result& lambda)
	{
	  trace::entering("canvas::morpho::impl::connected_filter_fastest");
	  // FIXME: Tests?

	  const I& input = exact(input_);
	  const N& nbh = exact(nbh_);
	  (void)a_;

	  mln_concrete(I) output;
	  initialize(output, input);

	  // Local type.
	  typedef mln_psite(I) P;

	  // Auxiliary data.
	  mln_ch_value(I, bool)  deja_vu;
	  mln_ch_value(I, bool)  activity;
	  mln_ch_value(I, unsigned) parent;
	  mln_ch_value(I, A)     data;

	  // Initialization.
	  {
	    initialize(deja_vu, input);
	    data::fill(deja_vu, false);
	    initialize(activity, input);
	    data::fill(activity, true);
	    initialize(parent, input);
	    data::fill(parent, 0);
	    initialize(data, input);
	  }

	  util::array<int> dp = offsets_wrt(input, nbh);
	  const unsigned n_nbhs = dp.nelements();
	  const unsigned n_points = s.nelements();

	  // First pass.
	  {
	    for (unsigned i = 0; i < n_points; ++i)
	      {
		unsigned p = s[i]; // An offset.

		// Make set.
		parent.element(p) = p;

		// Check accumulator trait to handle argument type (Value or None).
		take_as_init_fastest (data.element(p), input, p);

		for (unsigned j = 0; j < n_nbhs; ++j)
		  {
		    unsigned n = p + dp[j];
		    if (!deja_vu.element(n))
		      continue;

		    unsigned r = find_root_fastest(parent, n);
		    if (r != p)
		      {
			if (input.element(r) == input.element(p)
			    || (activity.element(r)
				&& (data.element(r) < lambda)))
			  {
			    data.element(p).take(data.element(r));
			    parent.element(r) = p;
			    if (activity.element(r) == false)
			      activity.element(p) = false;
			  }
			else
			  activity.element(p) = false;
		      }
		  }

		deja_vu.element(p) = true;
	      }
	  }


	  // Second pass.
	  {
	    for (int i = n_points - 1; i >= 0 ; --i)
	      {
		unsigned p = s[i];
		if (parent.element(p) == p) // p is root.
		  output.element(p) = input.element(p);
		else
		  output.element(p) = output.element(parent.element(p));
	      }
	  }

	  trace::exiting("canvas::morpho::impl::connected_filter_fastest");
	  return output;
	}


      } // end of namespace mln::canvas::morpho::impl




	// Dispatch.


      namespace internal
      {
	// Leveling
	template <typename I, typename N, typename A>
	mln_concrete(I)
	leveling_filter_dispatch(metal::false_,
				 const Image<I>& input,
				 const Neighborhood<N>& nbh,
				 const Accumulator<A>& a,
				 const typename A::result& lambda,
				 bool increasing)
	{
	  p_array < mln_psite(I) > s =
	    increasing ?
	    level::sort_psites_increasing(input) :
	    level::sort_psites_decreasing(input);
	  return impl::generic::connected_filter(input, nbh, s, a, lambda);
	}

	template <typename I, typename N, typename A>
	mln_concrete(I)
	leveling_filter_dispatch(metal::true_,
				 const Image<I>& input,
				 const Neighborhood<N>& nbh,
				 const Accumulator<A>& a,
				 const typename A::result& lambda,
				 bool increasing)
	{
	  util::array<unsigned> s =
	    increasing ?
	    level::sort_offsets_increasing(input) :
	    level::sort_offsets_decreasing(input);
	  return impl::connected_filter_fastest(input, nbh, s, a, lambda);
	}

	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	leveling_filter_dispatch(const Image<I>& input,
				 const Neighborhood<N>& nbh,
				 const Accumulator<A>& a,
				 const typename A::result& lambda,
				 bool increasing)
	{
	  mlc_or(mlc_equal(mln_trait_accumulator_when_pix(A),
			   trait::accumulator::when_pix::use_pix),
		 mlc_equal(mln_trait_accumulator_when_pix(A),
			   trait::accumulator::when_pix::use_v))::check();

	  enum
	  {
	    test = mlc_equal(mln_trait_image_speed(I),
			     trait::image::speed::fastest)::value
	    && mlc_equal(mln_trait_accumulator_when_pix(A),
			 trait::accumulator::when_pix::use_v)::value
	    && mln_is_simple_neighborhood(N)::value
	  };
	  return leveling_filter_dispatch(metal::bool_<test>(),
					  input, nbh, a, lambda, increasing);
	}

	// Alegebraic
	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	algebraic_filter_dispatch(metal::false_,
				  const Image<I>& input,
				  const Neighborhood<N>& nbh,
				  const Accumulator<A>& accu,
				  const mln_result(A)& lambda,
				  bool increasing)
	{
	  p_array<mln_psite(I)> s = increasing ?
	    level::sort_psites_increasing(input) :
	    level::sort_psites_decreasing(input);
	  return impl::generic::connected_filter(input, nbh, s, accu, lambda);
	}

	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	algebraic_filter_dispatch(metal::true_,
				  const Image<I>& input,
				  const Neighborhood<N>& nbh,
				  const Accumulator<A>& accu,
				  const mln_result(A)& lambda,
				  bool increasing)
	{
	  util::array<unsigned> s = increasing ?
	    level::sort_offsets_increasing(input) :
	    level::sort_offsets_decreasing(input);
	  return impl::connected_filter_fastest(input, nbh, s, accu, lambda);
	}

	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	algebraic_filter_dispatch(const Image<I>& input,
				  const Neighborhood<N>& nbh,
				  const Accumulator<A>& accu,
				  const mln_result(A)& lambda,
				  bool increasing)
	{
	  mlc_or(mlc_equal(mln_trait_accumulator_when_pix(A),
			   trait::accumulator::when_pix::use_none),
		 mlc_equal(mln_trait_accumulator_when_pix(A),
			   trait::accumulator::when_pix::use_p))::check();

	  enum {
	    test = (mlc_equal(mln_trait_image_speed(I),
			      trait::image::speed::fastest)::value &&
		    mln_is_simple_neighborhood(N)::value &&
		    mlc_equal(mln_trait_accumulator_when_pix(A),
			      trait::accumulator::when_pix::use_none)::value)
	  };
	  return algebraic_filter_dispatch(metal::bool_<test>(), input, nbh,
					   accu, lambda, increasing);
	}



	//connected
	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	connected_filter_dispatch(trait::accumulator::when_pix::use_none,
				  const Image<I>& input,
				  const Neighborhood<N>& nbh,
				  const Accumulator<A>& a,
				  const typename A::result& lambda,
				  bool increasing)
	{
	  return algebraic_filter_dispatch(input, nbh, a, lambda, increasing);
	}

	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	connected_filter_dispatch(trait::accumulator::when_pix::use_p,
				  const Image<I>& input,
				  const Neighborhood<N>& nbh,
				  const Accumulator<A>& a,
				  const typename A::result& lambda,
				  bool increasing)
	{
	  return algebraic_filter_dispatch(input, nbh, a, lambda, increasing);
	}

	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	connected_filter_dispatch(trait::accumulator::when_pix::use_v,
				  const Image<I>& input,
				  const Neighborhood<N>& nbh,
				  const Accumulator<A>& a,
				  const typename A::result& lambda,
				  bool increasing)
	{
	  return leveling_filter_dispatch(input, nbh, a, lambda, increasing);
	}

	template <typename I, typename N, typename A>
	inline
	mln_concrete(I)
	connected_filter_dispatch(trait::accumulator::when_pix::use_pix,
				  const Image<I>& input,
				  const Neighborhood<N>& nbh,
				  const Accumulator<A>& a,
				  const typename A::result& lambda,
				  bool increasing)
	{
	  return leveling_filter_dispatch(input, nbh, a, lambda, increasing);
	}
      } // end of namespace mln::canvas::morpho::internal


# endif // ! MLN_INCLUDE_ONLY

      // Facade.

      template <typename I, typename N, typename A>
      mln_concrete(I)
      connected_filter(const Image<I>& input,
		       const Neighborhood<N>& nbh,
		       const Accumulator<A>& a,
		       const typename A::result& lambda,
		       bool increasing)
      {
	return internal::connected_filter_dispatch(mln_trait_accumulator_when_pix(A)(),
						   input, nbh, a, lambda, increasing);
      }

      template <typename I, typename N, typename A>
      mln_concrete(I)
      algebraic_filter(const Image<I>& input,
		       const Neighborhood<N>& nbh,
		       const Accumulator<A>& a,
		       const typename A::result& lambda,
		       bool increasing)
      {
	return internal::algebraic_filter_dispatch(input, nbh, a, lambda, increasing);
      }

      template <typename I, typename N, typename A>
      mln_concrete(I)
      leveling_filter(const Image<I>& input,
		      const Neighborhood<N>& nbh,
		      const Accumulator<A>& a,
		      const typename A::result& lambda,
		      bool increasing)
      {
	return internal::leveling_filter_dispatch(input, nbh, a, lambda, increasing);
      }



    } // end of namespace mln::canvas::morpho
  } // end of namespace mln::canvas
} // end of namespace mln


#endif // ! MLN_CANVAS_MORPHO_CONNECTED_FILTER_HH
