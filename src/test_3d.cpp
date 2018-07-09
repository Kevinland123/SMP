#include <stdlib.h>
#include <iostream>
#include "Block.h"
#include "BlockGraph.h"
#include "BlockEditor.h"
#include "BlockPart.h"

using namespace std;

int main(int argc, char* argv[])
{
  if(argc != 2){
    cerr << "Wrong number of command arguments.\n" \
         << "The correct usage is ./[executable] [#proc]." << endl;
    exit(-1);
  }

  int         nProc = atoi(argv[1]);
  BlockGraph  bg0, bg1;

  bg0.fread_mesh_plot3d("mesh3d.x");
  bg0.fread_map("mapfile");

  // partition
  BlockPart   bp(nProc, bg0);
  bp.decompose(bg1, PART_RB);
  bp.view();

  int edgeCut, commVol;
  bg1.cmpt_comm(edgeCut, commVol);
  cout << "\n" \
    << "Edge Cut: " << edgeCut << endl \
    << "Comm Vol: " << commVol << endl;

  bg1.copy_coord(bg0);
  bg1.fwrite_mesh_tecplot("decomp.dat");

  return(0);
}
