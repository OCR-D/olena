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

#ifndef MLN_CANVAS_ALGEBRAIC_HH
# define MLN_CANVAS_ALGEBRAIC_HH

/// \file mln/canvas/algebraic.hh
///
/// Apply algebraic connected filter to images.
///
/// \todo Make a fastest version.

# include <mln/core/concept/image.hh>
# include <mln/data/fill.hh>
# include <mln/literal/zero.hh>
# include <mln/convert/to_upper_window.hh>
# include <mln/extension/adjust_fill.hh>

# include <mln/level/sort_psites.hh>
# include <mln/level/sort_offsets.hh>
#include "../edwin/accu_trait.hh"

namespace mln
{

  namespace canvas
  {

    template < typename I, typename N, typename A, typename L>
    void
    algebraic(const Image<I>& input, const Neighborhood<N>& nbh,
              Accumulator<A>& a, L lambda, bool increasing);

# ifndef MLN_INCLUDE_ONLY

    // Implementations.

    namespace impl
    {

      namespace generic
      {

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

        template < typename I, typename N, typename S, typename A, typename L>
        void
        algebraic(const Image<I>& input_, const Neighborhood<N>& nbh_,
                  const Site_Set<S>& s_, Accumulator<A>& a_, L lambda)
        {
          trace::entering("canvas::impl::generic::algebraic");

          // FIXME: Test?!

          const I& input = exact(input_);
          const N& nbh   = exact(nbh_);
          const S& s     = exact(s_);
          const A& a     = exact(a_);

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
            mln::data::fill(deja_vu, false);
            initialize(activity, input);
            mln::data::fill(activity, true);
            initialize(parent, input);
            initialize(data, input);
            a.init(); // init required.
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
                data(p).take_as_init(p);
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
          
          trace::exiting("canvas::morpho::impl::generic::algebraic");
        }
        
      } // end of namespace mln::canvas::impl::generic
      
    } // end of namespace mln::canvas::impl

    // Dispatch.
    namespace internal
    {
      
      // Algebraic
      template < typename I, typename N, typename S, typename A, typename L>
      void
      algebraic_dispatch(metal::false_,
                          const Image<I>& input, const Neighborhood<N>& nbh,
                          Accumulator<A>& a, L lambda, bool increasing)
      {
        p_array < mln_psite(I) > s =
          increasing ?
          level::sort_psites_increasing(input) :
          level::sort_psites_decreasing(input);
        impl::generic::algebraic(input, nbh, s, a, lambda);
      }

      template < typename I, typename N, typename S, typename A, typename L>
      void
      algebraic_dispatch(metal::true_,
                          const Image<I>& input, const Neighborhood<N>& nbh,
                          Accumulator<A>& a, L lambda, bool increasing)
      {
        util::array<unsigned> s =
          increasing ?
          level::sort_offsets_increasing(input) :
          level::sort_offsets_decreasing(input);
        //impl::algebraic_fastest(input, nbh, s, a, lambda);
      }

      template < typename I, typename N, typename S, typename A, typename L>
      inline
      void
      algebraic_dispatch(const Image<I>& input, const Neighborhood<N>& nbh,
                          Accumulator<A>& a, L lambda, bool increasing)
      {
        enum
        {
          test = mlc_equal(mln_trait_image_speed(I),
                            trait::image::speed::fastest)::value
              && mlc_equal(mln_trait_accu_when_pix(I),
                            trait::accu::when_pix::use_whatever)::value
              && mln_is_simple_neighborhood(N)::value
        };
        algebraic_dispatch(metal::bool_<test>(),
                            input, nbh, a, lambda, increasing);
      }


    } // end of namespace mln::canvas::internal

    // Facades.
    template < typename I, typename N, typename A, typename L>
    inline
    void
    algebraic(const Image<I>& input, const Neighborhood<N>& nbh,
              Accumulator<A>& a, L lambda, bool increasing)
    {
      trace::entering("canvas::algebraic");

//      FIXME: Do we need to check input validity ?

      internal::algebraic_dispatch(input, nbh, a, lambda, increasing);

      trace::exiting("canvas::algebraic");
    }

# endif // ! MLN_INCLUDE_ONLY

  } // end of namespace mln::canvas

} // end of namespace mln


#endif // ! MLN_CANVAS_ALGEBRAIC_HH

int main()
{

}