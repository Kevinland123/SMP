#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>
using namespace std;

const int MAX_NMAP       = 24;
const int BLOCK_NULL     = -1;
const int FACE_NULL      = -1;
const int RANGE_NULL     = -1;
const int PARTITION_NULL = -1;

const int EdgeMap3d[3][2] = {{1,2}, {2,0}, {0,1}};


class BlockFace
{
  public:  //---------- public member functions ----------//

  // basics
  BlockFace();
  BlockFace(const BlockFace& rhs);
  const BlockFace& operator=(const BlockFace& rhs);
  ~BlockFace();

  int alloc_map(int num);
  /*----------------------------------------------------------
    Allocate map info

    In: num  -  # maps
  ----------------------------------------------------------*/

  int add_map(string bcType, const int ranges[6]);

  int get_prop(int* bc_char, int blockid, int faceid, int nm);

  string get_bc_name(string bcstr,int nmap);
  /*----------------------------------------------------------
    Add a map info for physical bc

    In:  bcType - physical bc
         ranges - local index range
  ----------------------------------------------------------*/

  int add_map(string bcType, const int ranges[6],   int  toBlk, \
              int    toFace, const int toRanges[6], bool isFilp );
  /*----------------------------------------------------------
    Add a map info for block to block communication

    In:  bcType   -  b2b boudnary condition
         ranges   -  local index range
         toBlk    -  target block
         toFace   -  target face of target block
         toRanges -  idx ranges of target block
         isFilp   -  flip the map or not

    TODO: output error if adding more map than nmap.
  ----------------------------------------------------------*/

  int view();
  /*----------------------------------------------------------
    print the map info on current face
  ----------------------------------------------------------*/


  public: //---------- public attributes      ----------//

  int      ID;
  int      nmap;
  string  *bcType;
  int    **iRngs;
  int     *toBlk;
  int     *toFace;
  int    **toiRngs;
  bool    *isFlip;
};



class Block
{
  public:  //---------- public member functions ----------//

  Block(int blockID = BLOCK_NULL);
  Block(const Block& rhs);
  const Block& operator=(const Block& rhs);
  ~Block();

  int set_partition(int partitionID);

  int set_blockID(int blockID);

  int set_parentID(int prntID);

  int set_range(const int rngs[6]);

  int set_nproc(int nProcess);

  int add_bc(int faceID, string boundaryType, const int ranges[6]);

  int add_bc(int  faceID,  string boundaryType, const int ranges[6],   \
             int  toBlock, int    toFace,       const int toRanges[6], \
             bool isFlip);

  int get_range(int ranges[6]);

  int get_parent(int& prntBlk);

  int get_face(int faceID, BlockFace& face);

  int get_partition(int& partitionID);

  int get_workload(int& work);

  int alloc_coord(int numHalo = 1);

  int set_coord(int i, int j, int k, double coords[3]);

  int copy_coord(const Block& blk);
  /*----------------------------------------------------------
    Copy the coordinates from parent block.

    In:  blk  -  parent block

    Pre: * The input blk is the parent block of present blk.
         * The index range of present blk is part of the
           the input blk's range.
  ----------------------------------------------------------*/

  int get_coord(int i, int j, int k, double coords[3]);

  int get_coord_pt(int dir, double** ptCoord);

  int view();


  private: //---------- private attributes      ----------//
  int       blkID, pBlkID;
  int       partID;
  int       nProc;
  int       iRngs[6];
  int       nHalo;
  double    *x, *y, *z;
  BlockFace faces[6];
  int       nedge;

  friend class BlockEditor;
};


#endif
