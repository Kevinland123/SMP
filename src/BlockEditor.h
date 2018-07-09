#ifndef BLOCKEDITOR_H
#define BLOCKEDITOR_H

#include "Block.h"
#include "BlockGraph.h"

class BlockEditor
{
  public:  //---------- public member functions ----------//

  int split_bc(const BlockGraph& graph, BlockGraph& subGraph);

  int split_proc(const BlockGraph& graph, BlockGraph& subGraph);

  int split_proc_rb(const BlockGraph& graph, BlockGraph& subGraph);

  int split_cut(const BlockGraph& graph, BlockGraph& subGraph);

  int split_bicut(const BlockGraph& graph, BlockGraph& subGraph);

  int create_bc_cut(const BlockGraph& graph);
  /*----------------------------------------------------------
    Create cuts according the bc on block's faces

    IN:  graph    -  input graph, blocks are connected

    Pre: The map info of input graph is well set.

    Method: Check the bc on each face, if the bc range cuts 
            the face, save the range in cuts array.
            Note that x range of all face bcs are saved in the
            same array since the cut planes are normal to x axis.
         
    Result: cuts, ncut array are set up
  ----------------------------------------------------------*/

  int create_proc_cut_2d(const BlockGraph& graph);

  int create_bicut(const BlockGraph& graph);

  int match_inner_face(BlockGraph& subGraph);

  int match_bc(const BlockGraph& graph, BlockGraph& subGraph);
  /*----------------------------------------------------------
    Match the bc of subblocks from connected blocks.

    IN:  graph    -  input graph, blocks are connected
    OUT: subGraph -  graph of sublocks split from input graph

    Pre: * The map info of input graph is well set.
         * The cuts of blocks are set.

    Method: Traverse the map of each block in graph. For each
            map find the involved subBlocks and the involved 
            face range of each subBlock. Map the this range to
            to target block and locate the target range.
            Note that the target range may involve several
            subBlocks from the target block i.e. target subBlocks
            Find the involved range on each each target subBlock
            and map the range back to the source subBlock.
            Set the bc for range mapped back.
         
    Result: The blocks in the subGraph are connected by the 
            connection between blocks in graph. 
  ----------------------------------------------------------*/

  int destroy_cut();

  int find_subblock(const int rngs[6], int blk, int subBlkRngs[6]);
  /*----------------------------------------------------------
    Find the sub blocks involved with an input range 

    IN:  rngs       -  input index range
         blk        -  ID of block where the input range belong
    OUT: subBlkRngs -  index range of sub blocks involved with 
                       the input range

    Pre: * the cuts are well set.
         * the block is already cut into sub blocks.

    NOte: It doesn't matter which face the input range is on.
  ----------------------------------------------------------*/

  int search_cut_l(int val, int blk, int dir, int& interval);
  /*----------------------------------------------------------
    locate the interval containing the input 'val' in 'cuts'.
    Each interval stands for an index for sub blocks in one
    direction.

    IN:  val      -  input index, typically a bound of a range
         blk      -  block ID whose 'cuts' to search
         dir      -  direction to search
    OUT: interval -  the id of interval containing 'val'

    Method: Given the block ID and direction, use binary search
            in the specific 'cuts'. Each interval is bewteen 
            two cuts. During the binvary, compare 'val' with
            bounding cuts of the interval.

    Pre: * the cuts are well set.
  ----------------------------------------------------------*/

  int search_cut_r(int val, int blk, int dir, int& interval);

  int map_range(const BlockFace& face, int mapID, \
                const int rngs[6],     int toRngs[6]);
  /*----------------------------------------------------------
    Compute target range of a given range of a face. 
    Note that the given range may be a subset the range involved
    in the input map.

    IN:  face   -  input block face
         mapID  -  ID of the map of the input face
         rngs   -  input index range
    OUT: toRngs -  target range

    Method: First find the direction involved in the map, then
            match the range.

    Note: toRngs is stored in the natural order. Starting
          index is always smaller than the ending index.

    Pre: * The map info of input face is well set.
  ----------------------------------------------------------*/

  int map_range_back(const BlockFace& face, int mapID, \
                     const int toRngs[6],   int rngs[6]);

  int view();


  private: //---------- private attributes      ----------//

  int nBlk;
  int *offsets;
  int **nCuts;
  int ***cuts;

};

#endif
