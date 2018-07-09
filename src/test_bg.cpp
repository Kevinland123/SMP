#include <iostream>
#include "Block.h"
#include "BlockGraph.h"

using namespace std;

int main()
{
  int ranges[6];
  BlockGraph bg0(2);

  bg0.resize(1);

  Block *ptb;
  bg0.get_block_pt(0, &ptb);

  // set block data
  ranges[0] =  0;  ranges[3] = 63;
  ranges[1] =  0;  ranges[4] = 31;
  ranges[2] =  0;  ranges[5] =  0;
  ptb->set_range(ranges);

  // set block bc
  // x- bc
  ranges[0] =  0;  ranges[3] =  0;
  ranges[1] =  0;  ranges[4] = 31;
  ptb->add_bc(0, "FarField", ranges);
  // x+ bc
  ranges[0] = 63;  ranges[3] = 63;
  ranges[1] =  0;  ranges[4] = 31;
  ptb->add_bc(3, "FarField", ranges);
  // y- bc
  ranges[0] =  0;  ranges[3] = 63;
  ranges[1] =  0;  ranges[4] =  0;
  ptb->add_bc(1, "FarField", ranges);
  // y+ bc
  ranges[0] =  0;  ranges[3] = 63;
  ranges[1] = 31;  ranges[4] = 31;
  ptb->add_bc(4, "FarField", ranges);

  bg0.view();

  BlockGraph bg1(bg0);
  bg1.view();

  BlockGraph bg2(2);
  bg2 = bg1;
  bg2.view();

  return(0);
}
