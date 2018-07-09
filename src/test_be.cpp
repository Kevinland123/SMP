#include <stdlib.h>
#include <iostream>
#include "Block.h"
#include "BlockGraph.h"
#include "BlockEditor.h"

using namespace std;

const int    NX         = 257;
const int    NY         = 129;
const int    WALL_START = 80;
const int    WALL_END   = 176;
const double LX         = 2.0;
const double LY         = 1.0;

int main()
{
  int         rngs[6], toRngs[6];
  BlockEditor be; 
  BlockGraph  bg0(1), bg1;
  Block       *ptb;

  bg0.get_block_pt(0, &ptb);

  // set block data range
  rngs[0] =  0;  rngs[3] = NX-1;
  rngs[1] =  0;  rngs[4] = NY-1;
  rngs[2] =  0;  rngs[5] =  0;
  ptb->set_range(rngs);

  // set block coordinates
  double dx = LX / (double)(NX-1);
  double dy = LY / (double)(NY-1);
  ptb->alloc_coord();
  for(int i=rngs[0]-1; i<=rngs[3]+1; i++){
    for(int j=rngs[1]-1; j<=rngs[4]+1; j++){
      for(int k=rngs[2]-1; k<=rngs[5]+1; k++){
        double coords[3];
        coords[0] = i * dx;
        coords[1] = j * dy;
        coords[2] = 0.0;
        ptb->set_coord(i, j, k, coords);
      }
    }
  }

  // set block bc
  // x- bc
  rngs[0] =  0;  rngs[3] = 0;
  rngs[1] =  0;  rngs[4] = NY-1;
  ptb->add_bc(0, "FarField", rngs);
  // x+ bc
  rngs[0] = NX-1;  rngs[3] = NX-1;
  rngs[1] = 0;     rngs[4] = NY-1;
  ptb->add_bc(3, "FarField", rngs);
  // y- bc 1
  rngs[0]   = 0;     rngs[3]   = WALL_START;
  rngs[1]   = 0;     rngs[4]   = 0;
  toRngs[0] = NX-1;  toRngs[3] = WALL_END;
  toRngs[1] = 0;     toRngs[4] = 0;
  toRngs[2] = 0;     toRngs[5] = 0;
  ptb->add_bc(1, "Block2Block", rngs, 0, 1, toRngs, false);
  // y- bc 2
  rngs[0] = WALL_START;  rngs[3] = WALL_END;
  rngs[1] =  0;  rngs[4] =  0;
  ptb->add_bc(1, "EulerWall", rngs);
  // y- bc 3
  rngs[0]   = WALL_END;    rngs[3]   = NX-1;
  rngs[1]   = 0;           rngs[4]   = 0;
  toRngs[0] = WALL_START;  toRngs[3] = 0;
  toRngs[1] =  0;          toRngs[4] =  0;
  toRngs[2] =  0;          toRngs[5] =  0;
  ptb->add_bc(1, "Block2Block", rngs, 0, 1, toRngs, false);
  // y+ bc
  rngs[0] = 0;     rngs[3] = NX-1;
  rngs[1] = NY-1;  rngs[4] = NY-1;
  ptb->add_bc(4, "FarField", rngs);

  // cut by bc
  be.split_bc(bg0, bg1);
  bg1.copy_coord(bg0);
  bg1.view();

  return(0);
}
