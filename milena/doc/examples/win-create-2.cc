#include <mln/core/alias/window2d.hh>
#include <mln/convert/to.hh>

int main()
{
  using namespace mln;

  // \{
  bool b[9]	=   { 1, 0, 0,
		      1, 0, 0,
		      1, 0, 0 };

  bool b2[3][3] = { { 1, 0, 0 },
		    { 1, 0, 0 },
		    { 1, 0, 0 } };

  window2d win = convert::to<window2d>(b);
  window2d win2 = convert::to<window2d>(b2);
  // \}

}
