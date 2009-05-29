#include "my_xyz.hh"
#include "rgb_to_xyz.hh"

#include <cmath>

#include <mln/core/image/image2d.hh>
#include <mln/value/rgb.hh>

#include <mln/io/ppm/load.hh>
#include <mln/io/ppm/save.hh>
#include <mln/math/round.hh>
#include <mln/data/transform.hh>

#include "rgb_to_xyz.hh"
#include "get_red.hh"

int main()
{
//    typedef mln::value::xyz_d Col;
   using namespace mln::fun::v2v;

   mln::value::xyz_d col;
   mln::image2d<mln::value::rgb8> lena;
   mln::io::ppm::load(lena, "../../../img/lena.ppm");

  mln::image2d<mln::value::xyz_f> lena_xyz = mln::data::transform(lena,
						    mln::fun::v2v::f_rgb_to_xyz_f_t());

  mln::image2d<mln::value::rgb8> lena_rgb = mln::data::transform(lena_xyz,
								  f_xyz_get_red ());
}
