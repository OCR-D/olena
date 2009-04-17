#include <mln/core/image/image2d.hh>
#include <mln/value/label_8.hh>
#include <mln/labeling/blobs.hh>
#include <mln/core/alias/neighb2d.hh>
#include <mln/geom/bbox.hh>

int main()
{
  using namespace mln;
  using namespace mln::value;

  image2d<bool> ima(200,300);
  label_8 nlabels;
  // \{
  // ...
  trace::quiet = false;

  labeling::blobs(ima, c4(), nlabels);

  trace::quiet = true;

  geom::bbox(ima);
  // ...
  // \}
}