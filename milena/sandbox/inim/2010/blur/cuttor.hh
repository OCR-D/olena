#ifndef CUTTOR_HH
# define CUTTOR_HH

# include <sys/types.h>
# include <sys/stat.h>

# include <mln/core/image/image2d.hh>
# include <mln/io/pgm/load.hh>
# include <mln/io/pgm/save.hh>
# include <mln/io/ppm/save.hh>
# include <mln/value/all.hh>

template<typename Value>
class Cuttor
{
public:
  // Eat the filepath to the image to be treated.
  Cuttor (std::string filepath);

  // Lauch all the treatments needed at once on the image
  void start ();

  // Draw the separation line which starts at the given position
  void find_line (mln::image2d<Value> water, int y_start, int x_start = 0);
private:
  // TODO Private methods called by start.

  // directory of the file
  std::string filedir_;
  // filename (no extension which should be .pgm)
  std::string filename_;

  // The original image
  mln::image2d<Value> img_;

  // The line separation image
  mln::image2d<Value> lined_;
};

template <typename Value>
void save (mln::image2d<Value>& ima, std::string filename, std::string postfix);

# include "cuttor.hxx"

#endif /* !CUTTOR_HH */
