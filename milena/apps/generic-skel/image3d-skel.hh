// Copyright (C) 2011, 2013 EPITA Research and Development Laboratory (LRDE)
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

/// \file
/// \brief Routines used in the computation of binary skeletons of 3D
/// images.

#ifndef APPS_GENERIC_SKEL_IMAGE3D_SKEL_HH
# define APPS_GENERIC_SKEL_IMAGE3D_SKEL_HH

#include <mln/version.hh>

#include <mln/core/image/image3d.hh>

#include <mln/math/sqr.hh>
#include <mln/math/sqrt.hh>


/*------.
| I/O.  |
`------*/

// FIXME: Do not use a dedicated I/O routines.  Either convert the data
// to another format, or move thess routines into mln/io.

// -------------------- //
// PGM (3D extension).  //
// -------------------- //

inline
mln::image3d<bool>
load_pgm_3d(const std::string& filename)
{
  std::ifstream file(filename.c_str());

  std::string magic;
  std::getline(file, magic);
  mln_assertion(magic == "P5");

  const size_t ncomments = 3;
  std::string comment[ncomments];
  std::string expected[ncomments] = { "#xdim 1", "#ydim 1", "#zdim 1" };
  // Avoid a warning when NDEBUG is defined.
  (void) expected;
  for (size_t i = 0; i < ncomments; ++i)
    {
      std::getline(file, comment[i]);
      mln_assertion(comment[i] == expected[i]);
    }

  // FIXME: What is the right order?  Does the number of slices
  // appears first or last?
  int nslis, nrows, ncols;
  file >> nslis >> nrows >> ncols;

  // FIXME: We just ignore max_val.
  unsigned max_val;
  file >> max_val;

  mln::image3d<bool> ima(nslis, nrows, ncols);
  char val;
  mln::point3d p;
  for (p.sli() = 0; p.sli() < nslis; ++p.sli())
    for (p.row() = 0; p.row() < nrows; ++p.row())
      for (p.col() = 0; p.col() < ncols; ++p.col())
	{
	  file.read(&val, 1);
	  ima(p) = val;
	}
  return ima;
}

// ------------- //
// Raw 3D data.  //
// ------------- //

inline
void
save_raw_3d(const mln::image3d<bool>& ima, const std::string& filename)
{
  std::ofstream file(filename.c_str());
  mln_fwd_piter_(mln::image3d<bool>) p(ima.domain());
  for_all(p)
    file << static_cast<char>(ima(p) ? 0xff : 0x00);
}

// ----- //
// VTK.  //
// ----- //

// Save a binary 3D image as a VTK file where each voxel is
// represented by a cube decomposed in 6 square faces.
inline
void
save_vtk_polygons(const mln::image3d<bool>& ima,
		  const std::string& filename)
{
  std::ofstream file(filename.c_str());

  // Header.
  file
    << "# vtk DataFile Version 2.0" << std::endl
    << "Generated by " OLN_PACKAGE_STRING " (" OLN_PACKAGE_URL ")" << std::endl
    << "ASCII" << std::endl
    << std::endl;

  // Number of object (foreground) sites to represent as a cube.
  unsigned ncubes = 0;
  mln_piter_(mln::image3d<bool>) p(ima.domain());
  for_all(p)
    if (ima(p))
      ++ncubes;

  /* FIXME: The current approach duplicates some of the vertices (when
     they are shared by two cubes or more).  */

  // Points (locations).
  file << "DATASET POLYDATA" << std::endl;
  // Each site (voxel) is defined by the location of its 8 corners.
  file << "POINTS " << ncubes * 8 << " float" << std::endl;

  // ``Radius'' (half-length) of a cube.
  float r = 0.5;

  // Cubes (voxels).
  for_all(p)
    if (ima(p))
      file << p[0] - r << ' ' << p[1] - r << ' ' << p[2] - r << "    "
	   << p[0] + r << ' ' << p[1] - r << ' ' << p[2] - r << "    "
	   << p[0] + r << ' ' << p[1] + r << ' ' << p[2] - r << "    "
	   << p[0] - r << ' ' << p[1] + r << ' ' << p[2] - r << "    "
	   << p[0] - r << ' ' << p[1] - r << ' ' << p[2] + r << "    "
	   << p[0] + r << ' ' << p[1] - r << ' ' << p[2] + r << "    "
	   << p[0] + r << ' ' << p[1] + r << ' ' << p[2] + r << "    "
	   << p[0] - r << ' ' << p[1] + r << ' ' << p[2] + r
	   << std::endl;
  file << std::endl;

  /* FIXME: The current approach duplicates some of the faces (when
     they are shared by two squares).  */

  // Decompose each cube as a set of squares (polygons).
  file << "POLYGONS "
    // Each square has 6 faces
       << ncubes * 6 << ' '
    /* Each square face requires 5 parameters: 1 for the number of
       vertices of the polygon (always `4'), and 4 for the indices of
       the vertices of this polygon.  */
       << ncubes * 6 * (1 + 4) << std::endl;

  // ``Draw'' the 6 faces of each cube.
  for (unsigned i = 0; i < ncubes; ++i)
    {
      /* Vertices.  The 8 vertices of associated with the `i'-th cube
	 have indices `8 * i' to `8 * i + 7'.  */
      unsigned v[8] =
	{ 8 * i + 0,  8 * i + 1,  8 * i + 2,  8 * i + 3,
	  8 * i + 4,  8 * i + 5,  8 * i + 6,  8 * i + 7 };
      // Square faces.
      file
	<< "4   " << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3] << '\n'
	<< "4   " << v[4] << ' ' << v[5] << ' ' << v[6] << ' ' << v[7] << '\n'
	<< "4   " << v[0] << ' ' << v[1] << ' ' << v[5] << ' ' << v[4] << '\n'
	<< "4   " << v[2] << ' ' << v[3] << ' ' << v[7] << ' ' << v[6] << '\n'
	<< "4   " << v[0] << ' ' << v[4] << ' ' << v[7] << ' ' << v[3] << '\n'
	<< "4   " << v[1] << ' ' << v[2] << ' ' << v[6] << ' ' << v[5] << '\n'
	<< std::flush;
    }
}

// Save a binary 3D image as a VTK file where each voxel is
// represented by a cube (voxel polyhedron).  This routine generates
// smaller files than save_vtk_polygons.
inline
void
save_vtk_polyhedrons(const mln::image3d<bool>& ima,
		     const std::string& filename)
{
  std::ofstream file(filename.c_str());

  // Header.
  file
    << "# vtk DataFile Version 2.0" << std::endl
    << "Generated by " OLN_PACKAGE_STRING " (" OLN_PACKAGE_URL ")" << std::endl
    << "ASCII" << std::endl
    << std::endl;

  // Number of object (foreground) sites to represent as a cube.
  unsigned ncubes = 0;
  mln_piter_(mln::image3d<bool>) p(ima.domain());
  for_all(p)
    if (ima(p))
      ++ncubes;

  /* FIXME: The current approach duplicates some of the vertices (when
     they are shared by two cubes or more).  */

  // Points (locations).
  file << "DATASET UNSTRUCTURED_GRID" << std::endl;
  // Each site (voxel) is defined by the location of its 8 corners.
  file << "POINTS " << ncubes * 8 << " float" << std::endl;

  // ``Radius'' (half-length) of a cube.
  float r = 0.5;

  // Cubes (voxels).
  for_all(p)
    if (ima(p))
      file << p[0] - r << ' ' << p[1] - r << ' ' << p[2] - r << "    "
	   << p[0] + r << ' ' << p[1] - r << ' ' << p[2] - r << "    "
	   << p[0] + r << ' ' << p[1] + r << ' ' << p[2] - r << "    "
	   << p[0] - r << ' ' << p[1] + r << ' ' << p[2] - r << "    "
	   << p[0] - r << ' ' << p[1] - r << ' ' << p[2] + r << "    "
	   << p[0] + r << ' ' << p[1] - r << ' ' << p[2] + r << "    "
	   << p[0] + r << ' ' << p[1] + r << ' ' << p[2] + r << "    "
	   << p[0] - r << ' ' << p[1] + r << ' ' << p[2] + r
	   << std::endl;
  file << std::endl;

  // Create a polyhedron for each voxel.
  file << "CELLS " << ncubes << ' '
    /* Each cubes (VTK_VOXEL) requires 9 parameters: 1 for the number
       of vertices of the cube (always `8'), and 8 for the indices of
       the vertices of this polyhedron.  */
       << ncubes * (1 + 8) << std::endl;

  // ``Draw'' the 6 faces of each cube.
  for (unsigned i = 0; i < ncubes; ++i)
    {
      /* Vertices.  The 8 vertices of associated with the `i'-th cube
	 have indices `8 * i' to `8 * i + 7'.  */
      unsigned v[8] =
	{ 8 * i + 0,  8 * i + 1,  8 * i + 2,  8 * i + 3,
	  8 * i + 4,  8 * i + 5,  8 * i + 6,  8 * i + 7 };
      // The order of the vertices follows the conventions of the VTK
      // file format specification.
      file << "8   "
	   << v[0] << ' ' << v[1] << ' ' << v[3] << ' ' << v[2] << ' '
	   << v[4] << ' ' << v[5] << ' ' << v[7] << ' ' << v[6]
	   << std::endl;
    }
  file << std::endl;

  // Announce the type of cells used.
  file << "CELL_TYPES " << ncubes << std::endl;
  for (unsigned i = 0; i < ncubes; ++i)
    // The VTK_VERTEX cell type has number `11'.
    file << 11 << std::endl;
}


/*--------------.
| Subsampling.  |
`--------------*/

// FIXME: Again, this is a dedicated routine.  Use something from
// subsample.
inline
mln::image3d<bool>
subsampling_3d(const mln::image3d<bool>& input, unsigned factor)
{
  using namespace mln;
  typedef image3d<bool> I;
  /* This subsampling procedure may not take into account pixels at the
     end of each sloce/row/column if the corresponding dimension is
     not a multiple of FACTOR.  */
  I output(input.nslis() / factor,
	   input.nrows() / factor,
	   input.ncols() / factor);
  mln_piter_(I) po(output.domain());
  for_all(po)
  {
    // Bounds of the browsed box.
    point3d pi_min(po.sli() * factor,
		   po.row() * factor,
		   po.col() * factor);
    point3d pi_max((po.sli() + 1) * factor - 1,
		   (po.row() + 1) * factor - 1,
		   (po.col() + 1) * factor - 1);
    box3d sample_box(pi_min, pi_max);
    size_t sample_size = sample_box.nsites();

    // Count the number of `true' and `false' values and set OUTPUT(P)
    // to the prevalent value.
    unsigned ntrue_vals = 0;
    unsigned nfalse_vals = 0;
    mln_piter_(box3d) pi(sample_box);
    for_all(pi)
    {
      if (input(pi))
	++ntrue_vals;
      else
	++nfalse_vals;
      // Optimization: if one of the values (`true' or `false' has
      // absolute majority, stop here).
      if (   ntrue_vals  > sample_size / 2
	  || nfalse_vals > sample_size / 2)
	break;
    }
    output(po) = (ntrue_vals >= nfalse_vals);
  }
  return output;
}

/// \brief Draw a torus on a binary 3D image.
///
/// \param ima         The image to write into.
/// \param axis_dim    The dimentsion corresponding to the axis of
///                    the torus.
/// \param int_radius  Internal radius of the torus.
/// \param ext_radius  External radius of the torus.
inline
void
draw_torus(mln::image3d<bool>& ima, unsigned axis_dim,
	   unsigned int_radius, unsigned ext_radius)
{
  using namespace mln;

  // Check the axis dimension.
  mln_precondition(0 <= axis_dim && axis_dim < 3);
  // Dimensions other than the one of the axis.
  unsigned dim1 = (axis_dim + 1) % 3;
  unsigned dim2 = (axis_dim + 2) % 3;

  // Mid radius.
  const unsigned mid_radius = (ext_radius + int_radius) / 2;
  // Radius of the section of the torus.
  const unsigned torus_radius = (ext_radius - int_radius) / 2 ;

  point3d bb_min;
  bb_min[axis_dim] = -torus_radius;
  bb_min[dim1]     = -ext_radius;
  bb_min[dim2]     = -ext_radius;

  point3d bb_max;
  bb_max[axis_dim] = torus_radius;
  bb_max[dim1]     = ext_radius;
  bb_max[dim2]     = ext_radius;

  // Ensure the box of IMA is large enough.
  mln_assertion(ima.has(bb_min));
  mln_assertion(ima.has(bb_max));

  box3d bb(bb_min, bb_max);
  mln_piter_(box3d) p(bb);
  for_all(p)
  {
    unsigned x = math::sqrt((math::sqr(torus_radius) - math::sqr(p[axis_dim])));
    unsigned lo_radius = mid_radius - x;
    unsigned hi_radius = mid_radius + x;
    unsigned r2 = math::sqr(p[dim1]) + math::sqr(p[dim2]);
    if (math::sqr(lo_radius) < r2 && r2 < math::sqr(hi_radius))
      ima(p) = true;
  }
}

/// Create a 3D image containing three intersecting tori.
inline
mln::image3d<bool>
make_triple_torus(mln::def::coord half_len = 100)
{
  using namespace mln;

  box3d b = make::box3d(-half_len, -half_len, -half_len,
			+half_len, +half_len, +half_len);
  image3d<bool> ima(b);
  data::fill(ima, false);
  border::fill(ima, false);
  def::coord ext_radius = half_len;
  def::coord int_radius = ext_radius * 0.7;
  draw_torus(ima, 0, int_radius, ext_radius);
  draw_torus(ima, 1, int_radius, ext_radius);
  draw_torus(ima, 2, int_radius, ext_radius);
  return ima;
}

#endif // ! APPS_GENERIC_SKEL_IMAGE3D_SKEL_HH
