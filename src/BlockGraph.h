#ifndef BLOCKGRAPH_H
#define BLOCKGRAPH_H

#include "Block.h"

class BlockGraph
{
  public:  //---------- public member functions ----------//

  // basics
  BlockGraph();
  BlockGraph(const int nBlock);
  BlockGraph(const BlockGraph& rhs);
  const BlockGraph& operator=(const BlockGraph& rhs);
  ~BlockGraph();

  int resize(int nBlock);

  int get_block_pt(int blkID, Block** ptBlk);

  int get_workload(int& work);

  int get_number_faces(int &numberoffaces, int* nsurface);

  int view();

  int copy_coord(const BlockGraph& prntBg);
  /*----------------------------------------------------------
    Copy coordinates from parent BG to current BG

    In:  prntBg  -  parent block graph

    Method: Each sub block find its parent block and copy the
            coordinates.
  ----------------------------------------------------------*/

  int fread_mesh_plot3d(const string& fname, int nHalo = 1);

  int fread_map(const string& fname);

  int get_bc(int* bclist);

  int fread_partition(const string& fname);

  int fwrite_decomp_2d(const string& fname);

  int fwrite_mesh_tecplot(const string& fname);

  int get_range(int* range);

  int set_mesh(int nBlock, const int* il, const int* jl, const int* kl, int nHalo = 1);

  int get_ID(int np, int* partiID);

  void get_bc_name(string* bcstring);
  /*----------------------------------------------------------
    Set mesh blocks' structure and alloc blocks accordingly in
    BlockGraph.

    In:  nBlock  -  #blocks in mesh
         sizes   -  sizes[nBlock][3], 1st dimension for #blocks
                    2nd for x, y, z.
         nHalo   -  optional, #halo layers
  ----------------------------------------------------------*/

  int cmpt_comm(int& edgeCut, int& commVol);

  int reset_parent();

  int read_bc(int* NbcInt, int length, string* bcString, int* bcInt);

  private: //---------- private attributes      ----------//

  int    nBlk;
  Block *blks;

  friend class BlockEditor;
  friend class BlockPart;

};


class BlockCut
{
  public:  //---------- public member functions ----------//
  BlockCut();
  bool operator<(const BlockCut& rhs) const;
  bool operator>(const BlockCut& rhs) const;

  public:  //---------- public attributes       ----------//
  int    blkID;    // block ID
  int    axis;     // x, y, z
  int    pos;      // cut index
  double msgIncr;  // increase of msgload due to cut, can be negative
};


#endif
