#include <iomanip>
#include "Block.h"

/*------------------------------------------------------------------------------
   BlockFace class member functions
------------------------------------------------------------------------------*/


// constructor
BlockFace::BlockFace()
{
  nmap = 0;
  alloc_map(MAX_NMAP);
}


// copy constructor
BlockFace::BlockFace(const BlockFace& rhs)
{
  //BlockFace(rhs.nmap);
    nmap   = rhs.nmap;
    ID     = rhs.ID;
    alloc_map(MAX_NMAP);

    for(int i=0; i<nmap; i++){
      bcType[i] = rhs.bcType[i];
      toBlk[i]  = rhs.toBlk[i];
      toFace[i] = rhs.toFace[i];
      isFlip[i] = rhs.isFlip[i];

      for(int j=0; j<6; j++){
        iRngs[i][j]   = rhs.iRngs[i][j];
        toiRngs[i][j] = rhs.toiRngs[i][j];
      }
    }
}


// Assignment
const BlockFace& BlockFace::operator=(const BlockFace& rhs)
{
  if(this != &rhs){
    if(nmap != rhs.nmap)
      nmap = rhs.nmap;
      ID   = rhs.ID;

    for(int i=0; i<nmap; i++){
      bcType[i] = rhs.bcType[i];
      toBlk[i]  = rhs.toBlk[i];
      toFace[i] = rhs.toFace[i];
      isFlip[i] = rhs.isFlip[i];

      for(int j=0; j<6; j++){
        iRngs[i][j]   = rhs.iRngs[i][j];
        toiRngs[i][j] = rhs.toiRngs[i][j];
      }
    }
  }

  return(*this);
}


// Destructor
BlockFace::~BlockFace()
{
    delete [] bcType;
    delete [] toBlk;
    delete [] toFace;
    delete [] isFlip;

    for(int i=0; i<MAX_NMAP; i++){
      delete [] iRngs[i];
      delete [] toiRngs[i];
    }
    delete iRngs;
    delete toiRngs;
}


int BlockFace::alloc_map(int num)
{
  bcType = new string [num];
  toBlk  = new int    [num];
  toFace = new int    [num];
  isFlip = new bool   [num];

  iRngs   = new int* [num];
  toiRngs = new int* [num];
  for(int i=0; i<num; i++){
    iRngs[i]   = new int [6];
    toiRngs[i] = new int [6];
  }

  for(int i=0; i<num; i++){
    bcType[i] = "BC_NULL";
    toBlk[i]  = BLOCK_NULL;
    toFace[i] = FACE_NULL;
    isFlip[i] = false;
    for(int j=0; j<6; j++){
        iRngs[i][j] = RANGE_NULL;
      toiRngs[i][j] = RANGE_NULL;
    }
  }

  return(0);
}


// Add a map info for physical bc
int BlockFace::add_map(string boundaryType, const int ranges[6])
{
  if(nmap == MAX_NMAP){
    cout << "ERROR: Try to add more map info than specified" << endl;
    exit(-1);
  }

  if(boundaryType.compare("Block2Block") == 0){
    cout << "ERROR: B2B boundary lacks of map info." << endl;
    exit(-1);
  }

  bcType[nmap] = boundaryType;
  toBlk[nmap]  = BLOCK_NULL;
  toFace[nmap] = FACE_NULL;
  isFlip[nmap] = false;

  for(int i=0; i<6; i++){
    iRngs[nmap][i]   = ranges[i];
    toiRngs[nmap][i] = RANGE_NULL;
  }

  nmap++;

  return(0);
}


// Add a map info
int BlockFace::add_map(string boundaryType, const int  ranges[6], int targetBlock, \
                       int    targetFace,   const int targetRanges[6], bool isMapFilp  )
{
  if(nmap == MAX_NMAP){
    cout << "ERROR: Try to add more map info than specified" << endl;
    exit(-1);
  }

  if(boundaryType.compare("Block2Block") != 0){
    cout << "ERROR: Try to add B2B map info to non B2B boundary" << endl;
    exit(-1);
  }

  bcType[nmap] = boundaryType;
  toBlk[nmap]  = targetBlock;
  toFace[nmap] = targetFace;
  isFlip[nmap] = isMapFilp;

  for(int i=0; i<6; i++){
    iRngs[nmap][i]   = ranges[i];
    toiRngs[nmap][i] = targetRanges[i];
  }

  nmap++;

  return(0);
}


// view info
int BlockFace::view()
{
  cout << "# maps: " << nmap << endl;

  for(int i=0; i<nmap; i++){
    cout.width(11);
    cout.flags(ios::left);
    cout << bcType[i] << " | ";
    // ...
    for(int j=0; j<6; j++)
      cout << iRngs[i][j] << " ";
    // ...
    cout << "| ";
    cout << toBlk[i]  << " | ";
    cout << toFace[i] << " | ";
    // ...
    for(int j=0; j<6; j++)
      cout << toiRngs[i][j] << " ";
    // ...
    cout << "| " << isFlip[i] << endl;
  }

  return(0);
}


/*------------------------------------------------------------------------------
   Block class member functions
------------------------------------------------------------------------------*/


// constructor
Block::Block(int BlockID)
{
  blkID  = BlockID;
  pBlkID = BLOCK_NULL;
  nProc  = 1;
  nedge  = 0;
  x      = NULL;
  y      = NULL;
  z      = NULL;
  partID = PARTITION_NULL;

  for(int i=0; i<6; i++){
    iRngs[i] = RANGE_NULL;
    faces[i].ID = i;
  }
}


// copy constructor
Block::Block(const Block& rhs)
{
  blkID  = rhs.blkID;
  pBlkID = rhs.pBlkID;
  partID = rhs.partID;
  nProc  = rhs.nProc;
  nedge  = rhs.nedge;
  x      = NULL;
  y      = NULL;
  z      = NULL;

  for(int i=0; i<6; i++)
  {
    iRngs[i] = rhs.iRngs[i];
    faces[i]   = rhs.faces[i];
  }

  int n =  (iRngs[3] - iRngs[0] + 1) * (iRngs[4] - iRngs[1] + 1) \
         * (iRngs[5] - iRngs[2] + 1);
  // ...
  if(rhs.x){
    x = new double [n];
    for(int i=0; i<n; i++) x[i] = rhs.x[i];
  }
  // ...
  if(rhs.y){
    y = new double [n];
    for(int i=0; i<n; i++) y[i] = rhs.y[i];
  }
  // ...
  if(rhs.z){
    z = new double [n];
    for(int i=0; i<n; i++) z[i] = rhs.z[i];
  }
}


// assignment
const Block& Block::operator=(const Block& rhs)
{
  if(this!=&rhs){
    blkID  = rhs.blkID;
    pBlkID = rhs.pBlkID;
    partID = rhs.partID;
    nProc  = rhs.nProc;
    nedge  = rhs.nedge;

    for(int i=0; i<6; i++)
    {
      iRngs[i] = rhs.iRngs[i];
      faces[i] = rhs.faces[i];
    }

    int n =  (iRngs[3] - iRngs[0] + 1) \
           * (iRngs[4] - iRngs[1] + 1) \
           * (iRngs[5] - iRngs[2] + 1);
    // ...
    if(x) delete [] x; x = NULL;
    if(y) delete [] y; y = NULL;
    if(z) delete [] z; z = NULL;
    // ...
    if(rhs.x){
      x = new double [n];
      for(int i=0; i<n; i++) x[i] = rhs.x[i];
    }
    // ...
    if(rhs.y){
      y = new double [n];
      for(int i=0; i<n; i++) y[i] = rhs.y[i];
    }
    // ...
    if(rhs.z){
      z = new double [n];
      for(int i=0; i<n; i++) z[i] = rhs.z[i];
    }
  }

  return(*this);
}


// destructor
Block::~Block()
{
  // delete coordinate arrays
  if(x) delete [] x;
  if(y) delete [] y;
  if(z) delete [] z;
}


int Block::set_partition(int partitionID)
{
  partID = partitionID;
  return(0);
}


int Block::set_blockID(int blockID)
{
  blkID = blockID;
  return(0);
}


int Block::set_parentID(int prntID)
{
  pBlkID = prntID;
  return(0);
}


// set the data range
int Block::set_range(const int ranges[6])
{
  for(int i=0; i<6; i++)
     iRngs[i] = ranges[i];

  return(0);
}


int Block::set_nproc(int nProcess)
{
  nProc = nProcess;
  return(0);
}


int Block::add_bc(int faceID, string boundaryType, const int ranges[6])
{
  faces[faceID].add_map(boundaryType, ranges);

  return(0);
}


int Block::add_bc(int  faceID,  string boundaryType, const int ranges[6],   \
                  int  toBlock, int    toFace,       const int toRanges[6], \
                  bool isFlip)
{
  faces[faceID].add_map(boundaryType, ranges, toBlock, toFace, toRanges, isFlip);
  nedge++;

  return(0);
}


// get the data range
int Block::get_range(int ranges[6])
{
  for(int i=0; i<6; i++)
     ranges[i] = iRngs[i];

  return(0);
}


int Block::get_parent(int& prntBlk)
{
  prntBlk = pBlkID;
  return(0);
}


int Block::get_face(int faceID, BlockFace& face)
{
  face = faces[faceID];
  return(0);
}


int Block::get_partition(int& partitionID)
{
  partitionID = partID;
  return(0);
}


int Block::get_workload(int& work)
{
  work = (iRngs[3] - iRngs[0]) * (iRngs[4] - iRngs[1]) \
       * max(iRngs[5] - iRngs[2], 1);
  return(0);
}


int Block::alloc_coord(int numHalo)
{
  nHalo = numHalo;
  int size = 1;
  for(int i=0; i<3; i++){
     size *= (iRngs[i+3] - iRngs[i] + 1 + 2*nHalo);
  }

  if(x) delete [] x;
  x = new double [size];

  if(y) delete [] y;
  y = new double [size];

  if(z) delete [] z;
  z = new double [size];

  return(0);
}


int Block::set_coord(int i, int j, int k, double coords[3])
{
  int iSizes[3];
  for(int i=0; i<3; i++){
    iSizes[i] = iRngs[i+3] - iRngs[i] + 1 + 2*nHalo;
  }
  int idx =  k - (iRngs[2] - nHalo) + (j - (iRngs[1] - nHalo)) * iSizes[2]
          + (i - (iRngs[0] - nHalo)) * iSizes[1] * iSizes[2];
  x[idx]  = coords[0];
  y[idx]  = coords[1];
  z[idx]  = coords[2];

  return 0;
}


int Block::get_coord(int i, int j, int k, double coords[3])
{
  int iSizes[3];
  for(int i=0; i<3; i++){
    iSizes[i] = iRngs[i+3] - iRngs[i] + 1 + 2*nHalo;
  }
  int idx =  k - (iRngs[2] - nHalo) + (j - (iRngs[1] - nHalo)) * iSizes[2]
          + (i - (iRngs[0] - nHalo)) * iSizes[1] * iSizes[2];
   coords[0] = x[idx];
   coords[1] = y[idx];
   coords[2] = z[idx];

  return 0;
}


int Block::copy_coord(const Block& blk)
{
  int iSizes[3], toISizes[3];
  for(int i=0; i<3; i++){
    iSizes[i]   = iRngs[i+3] - iRngs[i] + 1 + 2*nHalo;
    toISizes[i] = blk.iRngs[i+3] - blk.iRngs[i] + 1 + 2*blk.nHalo;
  }

  for(int i=iRngs[0]-nHalo; i<iRngs[3]+nHalo; i++){
    for(int j=iRngs[1]-nHalo; j<iRngs[4]+nHalo; j++){
      for(int k=iRngs[2]-nHalo; k<iRngs[5]+nHalo; k++){
        int idx   = (i - (iRngs[0] - nHalo)) * iSizes[1] * iSizes[2] \
                  + (j - (iRngs[1] - nHalo)) * iSizes[2] \
                  +  k - (iRngs[2] - nHalo);
        //...
        int toIdx = (i - (blk.iRngs[0] - blk.nHalo)) * toISizes[1] * toISizes[2] \
                  + (j - (blk.iRngs[1] - blk.nHalo)) * toISizes[2] \
                  +  k - (blk.iRngs[2] - blk.nHalo);
        x[idx] = blk.x[toIdx];
        y[idx] = blk.y[toIdx];
        z[idx] = blk.z[toIdx];
      }
    }
  }

  return(0);
}


int Block::get_coord_pt(int dir, double** ptCoord)
{
  switch(dir){
    case 0:
      *ptCoord = x; break;
    case 1:
      *ptCoord = y; break;
    case 2:
      *ptCoord = z; break;
  }

  return 0;
}


int Block::view()
{
  cout << "Block    " << blkID  << endl;
  cout << "Parent   " << pBlkID << endl;
  cout << "Proc     " << partID << endl;
  cout << "Ranges X " << iRngs[0] << " - " << iRngs[3] << endl;
  cout << "Ranges Y " << iRngs[1] << " - " << iRngs[4] << endl;
  cout << "Ranges Z " << iRngs[2] << " - " << iRngs[5] << endl;
  cout << "Face info" << endl;
  for(int i=0; i<6; i++){
    faces[i].view();
  }

  return(0);
}


int BlockFace::get_prop(int* bc_char, int blockid, int faceid, int nm){
  int next=0;
  for(int i=0; i<nm; i++){
    int a=0,b=0;
    bc_char[next]=blockid;
    bc_char[next+1]=faceid;
    for(int j=0; j<6; j++){
    bc_char[2+j+next]=iRngs[i][j];a=j;}
    bc_char[a+3+next]=toBlk[i];
    bc_char[a+4+next]=toFace[i];
    for(int k=0; k<6; k++){
    bc_char[a+5+k+next]=toiRngs[i][k];b=k;}
    bc_char[a+b+6+next]=isFlip[i];
    next=next+17;
  }
  return (0);
}

string BlockFace::get_bc_name(string bcstr,int nmap){

  bcstr=bcType[nmap];
  return bcstr;
}
