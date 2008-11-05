// Copyright (C) 2007, 2008 EPITA Research and Development Laboratory
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

#ifndef MLN_PW_IMAGE_HH
# define MLN_PW_IMAGE_HH

/*! \file mln/pw/image.hh
 *
 * \brief FIXME.
 *
 * \todo Relax Function_p2v into Function_v2v.
 */

# include <mln/core/internal/image_primary.hh>
# include <mln/core/concept/function.hh>
# include <mln/value/set.hh>
# include <mln/metal/unqualif.hh>


namespace mln
{

  // Fwd decl.
  namespace pw { template <typename F, typename S> struct image; }



  /*! \brief FIXME
   *
   */
  template <typename F, typename S>
  pw::image<F,S>
  operator | (const Function_p2v<F>& f, const Site_Set<S>& ps);



  namespace internal
  {

    /// Data structure for mln::pw::image
    template <typename F, typename S>
    struct data< mln::pw::image<F,S> >
    {
      data(const F& f, const S& ps);
      F f_;
      S pset_;
    };

  } // end of namespace mln::internal



  namespace trait
  {

    // FIXME: the result type should *not* be qualified

    template <typename F, typename S>
    struct image_< pw::image<F,S> > : default_image_< mlc_unqualif(mln_result(F)),
						      pw::image<F,S> >
    {
      // misc
      typedef trait::image::category::primary category;
      typedef trait::image::speed::fast       speed;
      typedef trait::image::size::regular     size;

      // value
      typedef trait::image::vw_io::none                    vw_io;
      typedef trait::image::vw_set::none                   vw_set;
      typedef trait::image::value_access::computed         value_access;
      typedef trait::image::value_storage::disrupted       value_storage;
      typedef trait::image::value_browsing::site_wise_only value_browsing;
      typedef trait::image::value_alignement::irrelevant   value_alignement;
      typedef trait::image::value_io::read_only            value_io;

      // site / domain
      typedef trait::image::pw_io::read         pw_io;
      typedef /* FIXME: depends on S */  undef  localization;
      typedef /* FIXME: depends on S */  undef  dimension;

      // extended domain
      typedef trait::image::ext_domain::none      ext_domain;
      typedef trait::image::ext_value::irrelevant ext_value;
      typedef trait::image::ext_io::irrelevant    ext_io;
    };

  } // end of namespace mln::trait


  namespace pw
  {

    /*! \brief FIXME
     *
     */
    template <typename F, typename S>
    struct image :
      public internal::image_primary<mln_result(F), S, image<F,S> >
    {
      /// Skeleton.
      typedef image< tag::function_<F>, tag::pset_<S> > skeleton;


      /// Value associated type.
      typedef mln_result(F) value;

      /// Return type of read-only access.
      typedef mln_result(F) rvalue;

      /// Return type of read-write access.
      typedef rvalue lvalue;

      /// Constructor without argument.
      image();

      /// Constructor.
      image(const Function_p2v<F>& f, const Site_Set<S>& ps);

      /// Initialize an empty image.
      void init_(const Function_p2v<F>& f, const Site_Set<S>& ps);

      /// Give the definition domain.
      const S& domain() const;

      /// Return the function which computes a site value.
      F function() const;

      /// Read-only access of pixel value at point site \p p.
      mln_result(F) operator()(const mln_psite(S)& p) const;

      /// Read-write access is present but return a temporary value.
      mln_result(F) operator()(const mln_psite(S)&);
    };

  } // end of namespace mln::pw

  template <typename F, typename S>
  void init_(tag::function_t, Function_p2v<F>& target,
	      const mln::pw::image<F,S>& model);

  template <typename F, typename S, typename J>
  void init_(tag::image_t, mln::pw::image<F,S>& target, const J& model);


# ifndef MLN_INCLUDE_ONLY

  // init_

  template <typename F, typename S>
  void init_(tag::function_t, Function_p2v<F>& target,
	      const mln::pw::image<F,S>& model)
  {
    target = model.function();
  }

  template <typename F, typename S, typename J>
  void init_(tag::image_t, mln::pw::image<F,S>& target, const J& model)
  {
    F f;
    init_(tag::function, f, model);
    S s;
    init_(tag::domain, s, model);
    target.init_(f, s);
  }

  // Operator.

  template <typename F, typename S>
  inline
  pw::image<F,S>
  operator | (const Function_p2v<F>& f, const Site_Set<S>& ps)
  {
    pw::image<F,S> tmp(f, ps);
    return tmp;
  }

  // internal::data< pw::image<F,S> >

  namespace internal
  {

    template <typename F, typename S>
    inline
    data< mln::pw::image<F,S> >::data(const F& f, const S& ps)
      : f_(f),
	pset_(ps)
    {
    }

  }

  // pw::image<F,S>

  namespace pw
  {

    template <typename F, typename S>
    inline
    image<F,S>::image()
    {
    }

    template <typename F, typename S>
    inline
    image<F,S>::image(const Function_p2v<F>& f, const Site_Set<S>& ps)
    {
      this->data_ = new internal::data< pw::image<F,S> >(exact(f), exact(ps));
    }

    template <typename F, typename S>
    inline
    void
    image<F,S>::init_(const Function_p2v<F>& f, const Site_Set<S>& ps)
    {
      this->data_ = new internal::data< image<F,S> >(exact(f), exact(ps));
    }

    template <typename F, typename S>
    inline
    const S&
    image<F,S>::domain() const
    {
      return this->data_->pset_;
    }

    template <typename F, typename S>
    inline
    F
    image<F,S>:: function() const
    {
      return this->data_->f_;
    }


    template <typename F, typename S>
    inline
    mln_result(F)
    image<F,S>::operator()(const mln_psite(S)& p) const
    {
      mln_precondition(this->data_->pset_.has(p));
      return this->data_->f_(p);
    }

    template <typename F, typename S>
    inline
    mln_result(F)
    image<F,S>::operator()(const mln_psite(S)& p)
    {
      mln_precondition(this->data_->pset_.has(p));
      return this->data_->f_(p);
    }

  } // end of namespace mln::pw

# endif // ! MLN_INCLUDE_ONLY

} // end of namespace mln


#endif // ! MLN_PW_IMAGE_HH
