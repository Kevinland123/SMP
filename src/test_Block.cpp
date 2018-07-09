#include "Block.h"

int main()
{

  int       ranges[6] = {0,0,0,1,2,3};
  BlockFace fc0, fc1;

  fc0.add_map("Block2Block", ranges, 100, 1, ranges, true);
  fc0.view();
  
  fc1.add_map("Block2Block", ranges, 101, 1, ranges, true);
  fc1.add_map("EulerWall", ranges );
  fc1.view();

  fc0 = fc1;
  fc0.view();

  BlockFace fc2(fc1);
  fc2.view();

  Block b0;

  // data range
  ranges[0] =  0;  ranges[3] = 63;
  ranges[1] =  0;  ranges[4] = 31;
  ranges[2] =  0;  ranges[5] =  0;
  b0.set_range(ranges);
  // x- bc
  ranges[0] =  0;  ranges[3] =  0;
  ranges[1] =  0;  ranges[4] = 31;
  b0.add_bc(0, "FarField", ranges);
  // x+ bc
  ranges[0] = 63;  ranges[3] = 63;
  ranges[1] =  0;  ranges[4] = 31;
  b0.add_bc(3, "FarField", ranges);
  // y- bc
  ranges[0] =  0;  ranges[3] = 63;
  ranges[1] =  0;  ranges[4] =  0;
  b0.add_bc(1, "FarField", ranges);
  // y+ bc
  ranges[0] =  0;  ranges[3] = 63;
  ranges[1] = 31;  ranges[4] = 31;
  b0.add_bc(4, "FarField", ranges);

  b0.view();

  Block b1(b0);
  b1.view();

  Block b2;
  b2 = b1;
  b2.view();

  return(0);
}
