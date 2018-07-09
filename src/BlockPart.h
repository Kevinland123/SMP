#ifndef BLOCKPART_H
#define BLOCKPART_H

#include "Block.h"
#include "BlockGraph.h"
#include "BlockEditor.h"

const int PART_DEFAULT = 0; // closest division
const int PART_RB      = 1; // recursive bisection

class BlockPart
{
  public:  //---------- public member functions ----------//
  BlockPart();
  BlockPart(int nProc, const BlockGraph& bg, bool isCutBC = false);
  BlockPart(const BlockPart& rhs);
  const BlockPart& operator=(const BlockPart& rhs);
  ~BlockPart();

  int set_mesh(BlockGraph blkGraph);

  int set_proc(int nProc);

  int set_comm(int nHalo, int dataSize, double alpha, double beta);

  int assign_proc();

  int decompose(BlockGraph& subGraph, int method = PART_DEFAULT);

  int view();

  private: //---------- private attributes      ----------//

  int         nProc;
  int         workSum;
  int         *compLds;
  double      workIdeal;
  double      alpha, beta;
  int         *bandLds, *ltncLds, nHalo, dataSize;
  BlockGraph  bg;
  BlockEditor be;
};

#endif
