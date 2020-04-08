// Copyright (C) 2009, 2010, 2011, 2012, 2013 EPITA Research and
// Development Laboratory (LRDE)
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


#ifndef SCRIBO_BINARIZATION_SAUVOLA_MS_HH
# define SCRIBO_BINARIZATION_SAUVOLA_MS_HH

// Setup default Sauvola's formula parameters values at different scales.
//
// Values are set according to the following reference:
// Lazzara, G. and Géraud, T.: Efficient multiscale Sauvola’s binarization.
// IJDAR 2013, DOI 10.1007/s10032-013-0209-0
//
// "According to our experiment using k2 = 0.2, k3 = 0.3 and k4 = 0.5
//  gives good results."
# define SCRIBO_DEFAULT_SAUVOLA_MS_K2 0.2
# define SCRIBO_DEFAULT_SAUVOLA_MS_K3 0.3
# define SCRIBO_DEFAULT_SAUVOLA_MS_K4 0.5

/// \file
///
/// \brief Binarize an image using a multi-scale implementation of
/// Sauvola's algoritm.
///
/// FIXME: Use the integral image for successive subsampling.


# include <mln/core/alias/neighb2d.hh>
# include <mln/data/fill.hh>
# include <mln/data/compare.hh>

# include <mln/subsampling/antialiased.hh>

# include <mln/transform/influence_zone_geodesic.hh>

# include <mln/data/split.hh>

# include <mln/value/int_u8.hh>
# include <mln/border/mirror.hh>
# include <mln/border/adjust.hh>
# include <mln/border/resize.hh>

# include <mln/core/box_runend_piter.hh>

# include <mln/util/couple.hh>

# include <mln/extension/adjust.hh>

# include <scribo/subsampling/integral_single_image.hh>

# include <scribo/core/macros.hh>

# include <scribo/binarization/internal/sauvola_ms_functor.hh>

# include <scribo/canvas/integral_browsing.hh>

# include <scribo/util/init_integral_image.hh>
# include <scribo/util/integral_sub_sum_sum2_functor.hh>
# include <scribo/util/compute_sub_domains.hh>

# include <scribo/debug/logger.hh>
# ifdef SCRIBO_LOCAL_THRESHOLD_DEBUG
#  include <scribo/binarization/internal/local_threshold_debug.hh>
#  include <mln/io/pgm/save.hh>
#  include <mln/io/dump/save.hh>
#  include <mln/debug/filename.hh>
#  include <mln/labeling/compute.hh>
#  include <mln/accu/math/count.hh>
# endif // ! SCRIBO_LOCAL_THRESHOLD_DEBUG


namespace scribo
{

  namespace binarization
  {

    using namespace mln;
    using value::int_u8;


    /*! \brief Binarize an image using a multi-scale implementation of
        Sauvola's algoritm.

      \param[in] input_1 A grayscale image.
      \param[in] w_1 The window size used to compute stats.
      \param[in] s The scale factor used for the first subscaling
      (usually 2 or 3 is enough).
      \param[in] k2 Sauvola's K parameter for the lowest scale.
      \param[in] k3 Sauvola's K parameter for the medium scales.
      \param[in] k4 Sauvola's K parameter for the highest scale.
      \param[out] integral_sum_sum_2 Integral image of sum and squared
                                     sum.

      \p w_1 and \p lambda_min_1 are expressed according to the image
      at scale 0, i.e. the original size.

      \return A Boolean image.

      \ingroup grpalgobinsauvola

      In this implementation, the input image is subscaled at three
      different scales.  For each subscale image, Sauvola's is applied
      and relevant objects are preserved. The results of the three
      scales is merged and areas of the input image are marked with
      the appropriate scale to use for final binarization.  Finally,
      the input image is binarized, using, for each pixel, the correct
      window size of the scale where it has been retrieved.

      This implementation is very useful for document image with
      heterogeneous object sizes.

      Its implementation is meant to be the fastest as possible. For
      instance, statistics computation is based on integral images
      \cite shafait2008drr.

     */
    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1,
	       unsigned s, double k2, double k3, double k4,
	       image2d<mln::util::couple<double,double> >& integral_sum_sum_2);

    /// \overload
    /// The integral image is not returned.
    /// Sauvola's K is set to the default on each scale, i.e.
    /// 0.2 (lowest) / 0.3 (medium) / 0.5 (highest).
    ///
    /// \ingroup grpalgobinsauvola
    //
    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1, unsigned s);

    /// \overload
    /// The integral image is not returned.
    /// Sauvola's K parameter is set to the default on each scale,
    /// i.e. 0.2 (lowest) / 0.3 (medium) / 0.5 (highest).
    /// s is set to 3.
    ///
    /// \ingroup grpalgobinsauvola
    //
    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1);

    /// \overload
    /// Sauvola's K parameter is set to the default on each scale,
    /// i.e. 0.2 (lowest) / 0.3 (medium) / 0.5 (highest).
    //
    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1, unsigned s);

    /// \overload
    /// Allow to specify the same value for Sauvola's K parameter
    /// on all scales.
    //
    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1, unsigned s,
	       double all_k);


# ifndef MLN_INCLUDE_ONLY


    // Routines

    namespace internal
    {

      using namespace mln;


      template <typename V>
      V my_find_root(image2d<V>& parent, const V& x)
      {
	if (parent.element(x) == x)
	  return x;
	return parent.element(x) = my_find_root(parent,
						parent.element(x));
      }


      inline
      image2d<int_u8>
      compute_t_n_and_e_2(const image2d<int_u8>& sub, image2d<int_u8>& e_2,
			  unsigned lambda_min, unsigned lambda_max,
			  unsigned s, double k,
			  unsigned q, unsigned i, unsigned w,
			  const image2d<mln::util::couple<double,double> >& integral_sum_sum_2)
      {
	typedef image2d<int_u8> I;
	typedef point2d P;

	// Cast to float is needed on MacOS X.
	unsigned ratio = unsigned(std::pow(float(q), float(i - 2u)));  // Ratio in comparison to e_2

	unsigned
	  w_local = w * ratio,
	  w_local_h = w_local,
	  w_local_w = w_local;

	// 1st pass
	scribo::binarization::internal::sauvola_ms_functor< image2d<int_u8> >
	  f(sub, k, SCRIBO_DEFAULT_SAUVOLA_R, e_2, i, q);
	scribo::canvas::integral_browsing(integral_sum_sum_2,
					  ratio,
					  w_local_w, w_local_h,
					  s,
					  f);

	// 2nd pass
	{
	  mln::util::array<mln_value_(I) *> ptr(ratio);
	  unsigned nrows = geom::nrows(e_2);

	  mln_box_runend_piter_(I) sp(sub.domain()); // Backward.
	  unsigned ncols = sp.run_length();
	  for_all(sp)
	  {
	    unsigned p = &sub(sp) - sub.buffer(); // Offset
	    P site = sp;

	    {
	      P tmp = site * ratio;

	      // FIXME: to be removed!
	      if (tmp.row() + ratio >= nrows)
		ptr.resize(nrows - tmp.row());

	      ptr(0) = &e_2(tmp);
	      // FIXME: pointers could just be updated with an offset.
	      for (unsigned j = 1; j < ptr.size(); ++j)
	      {
		tmp[0] += 1;
		ptr(j) = & e_2(tmp);
	      }
	    }

	    for (unsigned j = 0; j < ncols; ++j)
	    {
	      if (f.msk.element(p))
	      {
		mln_site_(I) sq = site * ratio;

		if (f.parent.element(p) == p)
		{
		  // test over the component cardinality
		  f.msk.element(p) = f.card.element(p) > lambda_min
		    && f.card.element(p) < lambda_max;
#  ifdef SCRIBO_LOCAL_THRESHOLD_DEBUG
		  f.full_msk.element(p) = true;


		  unsigned area = f.card.element(p) * ratio * s;
		  if (area_histo[i - 2].find(area) != area_histo[i - 2].end())
		    ++area_histo[i - 2][area];
		  else
		    area_histo[i - 2][area] = 1;

		  for (unsigned l = 0; l < ratio; ++l)
		    for (unsigned k = 0; k < ratio; ++k)
		      debug_scale_proba(point3d(i - 2, sq.row() + l, sq.col() + k)) = f.card.element(p);
#  endif // ! SCRIBO_LOCAL_THRESHOLD_DEBUG

		  if (f.msk.element(p) && e_2(sq) == 0u)
		  {
		    for (unsigned l = 0; l < ptr.size(); ++l)
		      std::memset(ptr(l), i, ratio * sizeof(mln_value_(I)));
		  }

		}
		else
		{
		  // Propagation
		  f.msk.element(p) = f.msk.element(f.parent.element(p));
#  ifdef SCRIBO_LOCAL_THRESHOLD_DEBUG
		  f.full_msk.element(p) = f.full_msk.element(f.parent.element(p));

		  point2d sqp = f.parent.point_at_offset(f.parent.element(p)) * ratio;
		  unsigned v = debug_scale_proba(point3d(i - 2, sqp.row(), sqp.col()));

		  for (unsigned l = 0; l < ratio; ++l)
		    for (unsigned k = 0; k < ratio; ++k)
		      debug_scale_proba(point3d(i - 2, sq.row() + l, sq.col() + k)) = v;
#  endif // ! SCRIBO_LOCAL_THRESHOLD_DEBUG

		  if (f.msk.element(p) && e_2(sq) == 0u)
		  {
		    for (unsigned l = 0; l < ptr.size(); ++l)
		      std::memset(ptr(l), i, ratio * sizeof(mln_value_(I)));
		  }

		}
	      }

	      for (unsigned l = 0; l < ptr.size(); ++l)
		ptr(l) -= ratio;

	      --site[1];
	      --p;
	    }

	  }
	} // end of 2nd pass


#  ifdef SCRIBO_LOCAL_THRESHOLD_DEBUG
	if (internal::threshold_image_output)
	  io::pbm::save(f.msk,
			mln::debug::filename(internal::threshold_image_output));
	if (internal::full_threshold_image_output)
	  io::pbm::save(f.full_msk,
			mln::debug::filename(internal::full_threshold_image_output));
#  endif // ! SCRIBO_LOCAL_THRESHOLD_DEBUG

	return f.t_sub;
      }



      template <typename I, typename J, typename K>
      mln_ch_value(I, bool)
      multi_scale_binarization(const I& in, const J& e2,
			       const mln::util::array<K>& t_ima,
			       unsigned s)
      {
	mln_ch_value(I,bool) out;
	initialize(out, in);

	typedef const mln_value(K)* ptr_type;

	// Warning: if there are pixels with value different from 2, 3
	// or 4 in e2, it will crash because of that array...
	ptr_type ptr_t[5];
	ptr_t[2] = & t_ima[2].at_(0, 0);
	ptr_t[3] = & t_ima[3].at_(0, 0);
	ptr_t[4] = & t_ima[4].at_(0, 0);


	const mln_value(J)* ptr_e2   = & e2.at_(0, 0);
	const mln_value(I)* ptr__in = & in.at_(0, 0);
	bool*    ptr__out = & out.at_(0, 0);


	// Since we iterate from a smaller image in the largest ones and
	// image at scale 1 does not always have a size which can be
	// divided by (4*s), some sites in the border may not be processed
	// and we must skip them.
	int more_offset = - ((4 * s) - in.ncols() % (4 * s));

	if (more_offset == - (static_cast<int>(4*s)))
	  more_offset = 0; // No offset needed.

	const int
	  nrows4 = t_ima[4].nrows(),
	  ncols4 = t_ima[4].ncols(),

	  delta1  = in.delta_offset(dpoint2d(+1, -(s - 1))),
	  delta1b = in.delta_offset(dpoint2d(+1, -(s + s - 1))),
	  delta1c = in.delta_offset(dpoint2d(-(s + s - 1), +1)),
	  delta1d = in.delta_offset(dpoint2d(+1, -(s * 4 - 1))),
	  delta1e = in.delta_offset(dpoint2d(-(s * 4 - 1), +1)),
	  delta1f = in.delta_offset(dpoint2d(-(s - 1), +1)),

	  delta2  = t_ima[2].delta_offset(dpoint2d(+1, -1)),
	  delta2b = t_ima[2].delta_offset(dpoint2d(+1, -3)),
	  delta2c = t_ima[2].delta_offset(dpoint2d(-3, +1)),

	  delta3  = t_ima[3].delta_offset(dpoint2d(+1, -1)),

	  eor1 = in.delta_offset(dpoint2d(+4 * s, - in.ncols())) + more_offset,
	  eor2 = t_ima[2].delta_offset(dpoint2d(+4,- t_ima[2].ncols())),
	  eor3 = t_ima[3].delta_offset(dpoint2d(+2,- t_ima[3].ncols())),
	  eor4 = t_ima[4].delta_offset(dpoint2d(+1,- t_ima[4].ncols()));

	mln_value(J) threshold;
	for (int row4 = 0; row4 < nrows4; ++row4)
	{
	  for (int col4 = 0; col4 < ncols4; ++col4)
	  {
	    // top left  1
	    {
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;
	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1b; ptr__in += delta1b;
	      }

	      ptr_t[2] += delta2; ptr_e2 += delta2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;

	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1c; ptr__in += delta1c;
	      }

	      ptr_t[2] -= delta2; ptr_e2 -= delta2;
	    }

	    // top right 1
	    ptr_t[3] += 1;
	    {
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;
	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1b; ptr__in += delta1b;
	      }

	      ptr_t[2] += delta2; ptr_e2 += delta2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;
	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1d; ptr__in += delta1d;
	      }

	      ptr_t[2] += delta2b; ptr_e2 += delta2b;
	    }

	    // bot left  1
	    ptr_t[3] += delta3;
	    {
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;
	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1b; ptr__in += delta1b;
	      }

	      ptr_t[2] += delta2; ptr_e2 += delta2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;
	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1c; ptr__in += delta1c;
	      }

	      ptr_t[2] -= delta2; ptr_e2 -= delta2;
	    }

	    // bot right 1
	    ptr_t[3] += 1;
	    {
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;
	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1b; ptr__in += delta1b;
	      }

	      ptr_t[2] += delta2; ptr_e2 += delta2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1f; ptr__in += delta1f;
	      }

	      ++ptr_t[2]; ++ptr_e2;
	      threshold = *ptr_t[*ptr_e2];
	      {
		for (unsigned i = 1; i < s; ++i)
		{
		  for (unsigned j = 1; j < s; ++j)
		  {
		    *ptr__out = *ptr__in <= threshold;
		    ++ptr__out; ++ptr__in;
		  }

		  *ptr__out = *ptr__in <= threshold;
		  ptr__out += delta1; ptr__in += delta1;
		}

		for (unsigned j = 1; j < s; ++j)
		{
		  *ptr__out = *ptr__in <= threshold;
		  ++ptr__out; ++ptr__in;
		}
		*ptr__out = *ptr__in <= threshold;
		ptr__out += delta1e; ptr__in += delta1e;
	      }
	    }

	    // bot right -> next top left
	    ptr_t[2] += delta2c; ptr_e2 += delta2c;
	    ptr_t[3] = ptr_t[3] - delta3;
	    ptr_t[4] += 1;
	  }

	  // eof -> next bof
	  ptr__out += eor1; ptr__in  += eor1;
	  ptr_t[2] += eor2; ptr_e2 += eor2;
	  ptr_t[3] += eor3;
	  ptr_t[4] += eor4;
	}

	return out;
      }

    } // end of namespace scribo::binarization::internal



    // Implementation

    namespace impl
    {

      namespace generic
      {

	template <typename I>
	mln_ch_value(I,bool)
	sauvola_ms(const Image<I>& input_1_, unsigned w_1,
		   unsigned s, double k2, double k3, double k4,
		   image2d<mln::util::couple<double,double> >& integral_sum_sum_2)
	{
	  mln_trace("scribo::binarization::sauvola_ms");

	  const I& input_1 = exact(input_1_);
	  typedef mln_value(I) V;

	  mlc_is_a(mln_value(I), value::Scalar)::check();
	  mln_precondition(input_1.is_valid());

	  dpoint2d none(0, 0);

	  // Number of subscales.
	  unsigned nb_subscale = 3;

	  // Window size.
	  unsigned w_work = w_1 * s;        // Scale 2


	  // Subscale step.
	  unsigned q = 2;


	  /*==========================
	    == Step 1 - Subsampling ==
	    ========================*/

	  mln::util::array<I> t_ima;

	  // Make sure t_ima indexes start from 2.
	  {
	    I dummy(1,1);
	    for (unsigned i = 0; i < nb_subscale + 2; ++i)
	      t_ima.append(dummy);
	  }

	  mln::util::array<I> sub_ima;

	  // Make sure sub_ima indexes start from 2.
	  {
	    I dummy(1,1);
	    sub_ima.append(dummy);
	    sub_ima.append(dummy);
	  }

	  mln::util::array<mln::util::couple<box2d, unsigned> >
	    sub_domains = scribo::util::compute_sub_domains(input_1,
							    nb_subscale, s);

	  border::adjust(input_1, sub_domains(1).second());
	  border::mirror(input_1);


	  // Resize input and compute integral images.
	  typedef image2d<mln::util::couple<double,double> > integral_t;
//	  integral_t integral_sum_sum_2;

	  scribo::debug::logger().start_time_logging();

	  // Subsampling from scale 1 to 2.
	  {
	    scribo::util::integral_sub_sum_sum2_functor<I, double>
	      fi(s, sub_domains[2].first(), sub_domains[2].second());

	    integral_sum_sum_2 = scribo::util::init_integral_image(input_1, s, fi,
								   sub_domains[2].first(),
								   sub_domains[2].second());
	    sub_ima.append(fi.sub);
	  }

	  scribo::debug::logger().stop_time_logging("1. subsampling and integral -");
	  scribo::debug::logger().start_time_logging();

	  // Subsampling to scale 3 and 4.
	  //
	  // FIXME: we may use the integral image to compute
	  // subsampled images -> faster and more precise.
	  for (unsigned i = 3; i <= nb_subscale + 1; ++i)
	    sub_ima.append(mln::subsampling::antialiased(sub_ima[i - 1], q,
							 sub_domains[i].first(),
							 sub_domains[i].second()));

	  scribo::debug::logger().stop_time_logging("2. More subsampling -");
	  scribo::debug::logger().start_time_logging();

	  // Compute threshold images.
	  image2d<int_u8> e_2;
	  initialize(e_2, sub_ima[2]);
	  data::fill(e_2, 0u);


# ifdef SCRIBO_LOCAL_THRESHOLD_DEBUG
	  internal::debug_scale_proba = image3d<double>(3,
							integral_sum_sum_2.nrows(),
							integral_sum_sum_2.ncols(),
							integral_sum_sum_2.border());
# endif // ! SCRIBO_LOCAL_THRESHOLD_DEBUG

	  /*=============================================
	    == Step 2 - Object selection at each scale ==
	    ===========================================*/

	  float
	    min_coef = 0.8,
	    max_s2 = (w_1 * w_1) / (s * s) * 0.7,
	    q_2 = q * q;


	  // Highest scale -> no maximum component size.
	  {
	    int i = sub_ima.size() - 1;
	    t_ima[i] = internal::compute_t_n_and_e_2(sub_ima[i], e_2,
						     (max_s2 * q_2) / (q_2) * min_coef,
						     mln_max(unsigned),
						     s, k4,
						     q, i, w_work,
						     integral_sum_sum_2);
	  }

	  // Other scales -> maximum and minimum component size.
	  {
	    for (int i = sub_ima.size() - 2; i > 2; --i)
	    {
	      t_ima[i] = internal::compute_t_n_and_e_2(sub_ima[i], e_2,
						       max_s2 / (q_2) * min_coef,
						       max_s2 * q_2,
						       s, k3,
						       q, i, w_work,
						       integral_sum_sum_2);
	    }
	  }

	  // Lowest scale -> no minimum component size.
	  {
	    t_ima[2] = internal::compute_t_n_and_e_2(sub_ima[2], e_2,
            // FIXME: was '0'. '2' is to avoid too much noise with k=0.2.
						     2,
						     max_s2,
						     s, k2,
                                                     1, 2, w_work,
						     integral_sum_sum_2);
	  }

	  scribo::debug::logger().stop_time_logging("3. Multi-scale processing -");
	  scribo::debug::logger().start_time_logging();


#  ifdef SCRIBO_LOCAL_THRESHOLD_DEBUG
	  if (internal::scale_image_output)
	    io::pgm::save(e_2,
			  mln::debug::filename(internal::scale_image_output));
#  endif // ! SCRIBO_LOCAL_THRESHOLD_DEBUG

	  /*==============================
	    == Step 3 - Results Merging ==
	    ============================*/

	  // Propagate scale values.
	  e_2 = transform::influence_zone_geodesic(e_2, c8());

	  scribo::debug::logger().stop_time_logging("4. Influence Zone on Scale image -");
	  scribo::debug::logger().start_time_logging();


#  ifdef SCRIBO_LOCAL_THRESHOLD_DEBUG
	  internal::debug_e_2 = e_2;
 	  if (internal::scale_iz_image_output)
 	    io::pgm::save(e_2,
			  mln::debug::filename(internal::scale_iz_image_output));

	  // Computing scale ratios.
	  mln::util::array<unsigned>
	    count = labeling::compute(accu::meta::math::count(), e_2, 4);
	  unsigned npixels = e_2.domain().nsites();
	  scribo::debug::logger() << "Scale ratios: 2 ("
				  << count[2] / (float)npixels * 100
				  << ") - 3 ("
				  << count[3] / (float)npixels * 100
				  << ") - 4 ("
				  << count[4] / (float)npixels * 100 << ")"
				  << std::endl;

 	  if (internal::scale_proba_output)
 	    io::dump::save(internal::debug_scale_proba,
			   mln::debug::filename(internal::scale_proba_output));
#  endif // ! SCRIBO_LOCAL_THRESHOLD_DEBUG

	  /*=================================
	    == Step 4 - Final Binarization ==
	    ===============================*/

	  // Binarize
	  image2d<bool>
	    output = internal::multi_scale_binarization(input_1, e_2, t_ima, s);

	  scribo::debug::logger().stop_time_logging("5. Final binarization -");

	  return output;
	}

      } // end of namespace scribo::binarization::impl::generic


    } // end of namespace scribo::binarization::impl


    // Facade

    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1_, unsigned w_1, unsigned s,
               double k2, double k3, double k4,
	       image2d<mln::util::couple<double,double> >& integral_sum_sum_2)
    {
      mln_trace("scribo::binarization::sauvola_ms");

      mln_precondition(exact(input_1_).is_valid());
      mln_precondition(s > 1);

      // Gray level images ONLY.
      mlc_is_not_a(mln_value(I), value::Vectorial)::check();
      mlc_is_not(mln_value(I), bool)::check();

      mln_ch_value(I,bool)
	output = impl::generic::sauvola_ms(exact(input_1_), w_1, s,
                                           k2, k3, k4,
					   integral_sum_sum_2);

      return output;
    }

    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1_, unsigned w_1, unsigned s,
               double k2, double k3, double k4)
    {
      mln_trace("scribo::binarization::sauvola_ms");

      mln_precondition(exact(input_1_).is_valid());
      mln_precondition(s > 1);

      // Gray level images ONLY.
      mlc_is_not_a(mln_value(I), value::Vectorial)::check();
      mlc_is_not(mln_value(I), bool)::check();

      typedef image2d<mln::util::couple<double,double> > integral_t;
      integral_t integral_sum_sum_2;

      mln_ch_value(I,bool)
	output = sauvola_ms(input_1_, w_1, s, k2, k3, k4, integral_sum_sum_2);

      return output;
    }


    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1)
    {
      return sauvola_ms(input_1, w_1, 3,
                        SCRIBO_DEFAULT_SAUVOLA_MS_K2,
                        SCRIBO_DEFAULT_SAUVOLA_MS_K3,
                        SCRIBO_DEFAULT_SAUVOLA_MS_K4);
    }


    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1, unsigned s)
    {
      return sauvola_ms(input_1, w_1, s,
                        SCRIBO_DEFAULT_SAUVOLA_MS_K2,
                        SCRIBO_DEFAULT_SAUVOLA_MS_K3,
                        SCRIBO_DEFAULT_SAUVOLA_MS_K4);
    }


    template <typename I>
    mln_ch_value(I,bool)
    sauvola_ms(const Image<I>& input_1, unsigned w_1, unsigned s,
	       double all_k)
    {
      return sauvola_ms(input_1, w_1, s, all_k, all_k, all_k);
    }


# endif // ! MLN_INCLUDE_ONLY


  } // end of namespace scribo::binarization

} // end of namespace scribo


#endif // SCRIBO_BINARIZATION_SAUVOLA_MS_HH
