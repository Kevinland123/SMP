#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include "BlockGraph.h"

using namespace std;


BlockGraph::BlockGraph()
{
  nBlk = 0;
  blks = NULL;
}


BlockGraph::BlockGraph(const int nBlock)
{
  nBlk = nBlock;
  blks = new Block[nBlk];
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    blks[iBlk].set_blockID(iBlk);
    blks[iBlk].set_parentID(iBlk);
  }
}


BlockGraph::BlockGraph(const BlockGraph& rhs)
{
  nBlk = rhs.nBlk;
  blks = new Block[nBlk];
  for(int i=0; i<nBlk; i++)
    blks[i] = rhs.blks[i];
}


const BlockGraph& BlockGraph::operator=(const BlockGraph& rhs)
{
  if(this != &rhs)
  {
    if(blks) delete [] blks;

    nBlk = rhs.nBlk;
    blks = new Block[nBlk];

    for(int i=0; i<nBlk; i++)
      blks[i] = rhs.blks[i];
  }

  return(*this);
}


BlockGraph::~BlockGraph()
{
  delete [] blks;
  blks = NULL;
}


int BlockGraph::resize(const int nBlock)
{
  if(blks) delete [] blks;
  nBlk = nBlock;
  blks = new Block[nBlk];

  for(int iBlk=0; iBlk<nBlk; iBlk++){
    blks[iBlk].set_blockID(iBlk);
    blks[iBlk].set_parentID(iBlk);
  }

  return(0);
}


int BlockGraph::get_block_pt(int blkID, Block** ptBlk)
{
  *ptBlk = &(blks[blkID]);
  return(0);
}


int BlockGraph::get_workload(int& work)
{
  int blkWork;

  work = 0;
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    blks[iBlk].get_workload(blkWork);
    work += blkWork;
  }

  return(0);
}


int BlockGraph::view()
{
  cout << "\n Block Graph Info" << endl;
  cout << "# Blocks " << nBlk << endl;

  for(int iBlk=0; iBlk<nBlk; iBlk++){
    cout << "----Block " << iBlk << endl;
    blks[iBlk].view();
  }

  return(0);
}


int BlockGraph::copy_coord(const BlockGraph& bg)
{
  for(int iBlk=0; iBlk<nBlk; iBlk++) {
    int iPrnt;
    blks[iBlk].get_parent(iPrnt);

    blks[iBlk].alloc_coord();
    blks[iBlk].copy_coord(bg.blks[iPrnt]);
  }

  return(0);
}


int BlockGraph::fread_mesh_plot3d(const string& fname, int nHalo)
{
  ifstream input(fname);

  // get # blocks
  input >> nBlk;

  // alloc blocks
  if(blks) delete [] blks;
  blks = new Block [nBlk];
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    blks[iBlk].set_blockID(iBlk);
    blks[iBlk].set_parentID(iBlk);
  }

  // read block index ranges
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    int rngs[6] = {0, 0, 0, 0, 0, 0};
    input >> rngs[3] >> rngs[4] >> rngs[5];
    for(int i=3; i<6; i++)
      rngs[i]--;
    blks[iBlk].set_range(rngs);
  }

  // alloc and read coordinates
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    int rngs[6];
    blks[iBlk].get_range(rngs);
    blks[iBlk].alloc_coord(nHalo);
    //...
    int     ySize = rngs[4] - rngs[1] + 1 + 2*nHalo;
    int     zSize = rngs[5] - rngs[2] + 1 + 2*nHalo;
    double *coords;
    //...
    for(int iDir=0; iDir<3; iDir++){
      blks[iBlk].get_coord_pt(iDir, &coords);
      //...
      for(int k=rngs[2]; k<=rngs[5]; k++){
        for(int j=rngs[1]; j<=rngs[4]; j++){
          for(int i=rngs[0]; i<=rngs[3]; i++){
            int idx = (k - (rngs[2] - nHalo)) \
                    + (j - (rngs[1] - nHalo)) * zSize \
                    + (i - (rngs[0] - nHalo)) * ySize * zSize;
            input >> coords[idx];
          }
        }
      }
    }
  }

  return(0);
}


int BlockGraph::fread_map(const string& fname)
{
  ifstream input(fname);
  string line;

  // skip the title line.
  getline(input, line);

  // set bcs
  while(getline(input, line)){
    // read bc type
    string bcStr;
    istringstream strIn(line);
    getline(strIn, bcStr, ' ');

    // read block, face ID
    int blkID, fcID;
    strIn >> blkID >> fcID;
    blkID--;
    fcID--;

    // set the default face ranges, which is whole face
    int fcDir, dirs[2], rngs[6];
    fcDir   = fcID % 3;
    dirs[0] = EdgeMap3d[fcDir][0];
    dirs[1] = EdgeMap3d[fcDir][1];
    blks[blkID].get_range(rngs);
    rngs[fcDir]   = rngs[fcID];
    rngs[fcDir+3] = rngs[fcID];

    // check the bc type
    if(bcStr.compare("Block2Block") == 0){
      // set face range in this map
      strIn >> rngs[dirs[0]];   rngs[dirs[0]]--;
      strIn >> rngs[dirs[0]+3]; rngs[dirs[0]+3]--;
      strIn >> rngs[dirs[1]];   rngs[dirs[1]]--;
      strIn >> rngs[dirs[1]+3]; rngs[dirs[1]+3]--;
      // set targe block face
      int toBlkID, toFcID, toFcDir, toDirs[2], toRngs[6];
      strIn >> toBlkID >> toFcID;
      toBlkID--;
      toFcID--;
      // set target face range
      toFcDir   = toFcID % 3;
      toDirs[0] = EdgeMap3d[toFcDir][0];
      toDirs[1] = EdgeMap3d[toFcDir][1];
      //...
      blks[toBlkID].get_range(toRngs);
      toRngs[toFcDir]   = toRngs[toFcID];
      toRngs[toFcDir+3] = toRngs[toFcID];
      strIn >> toRngs[toDirs[0]];   toRngs[toDirs[0]]--;
      strIn >> toRngs[toDirs[0]+3]; toRngs[toDirs[0]+3]--;
      strIn >> toRngs[toDirs[1]];   toRngs[toDirs[1]]--;
      strIn >> toRngs[toDirs[1]+3]; toRngs[toDirs[1]+3]--;
      // flip directions or not
      string flipStr;
      strIn >> ws >> flipStr;
      bool   isFlip = (flipStr.compare("yes") == 0);
      // add bc
      blks[blkID].add_bc(fcID, bcStr, rngs, toBlkID, toFcID, toRngs, isFlip);
      //cout << blkID << "|" << fcID << "|" << toBlkID << "|" << toFcID << endl;
      // add reverse mapping
      for(int i=0; i<2; i++){
        if(toRngs[toDirs[i]] > toRngs[toDirs[i]+3]){
          swap(toRngs[toDirs[i]], toRngs[toDirs[i]+3]);
          swap(rngs[dirs[i]], rngs[dirs[i]+3]);
        }
      }
      blks[toBlkID].add_bc(toFcID, bcStr, toRngs, blkID, fcID, rngs, isFlip);
    }
    // physical bcs
    else{
      //cout << strIn.eof() << endl;
      strIn >> ws;
      //cout << strIn.eof() << endl;
      if(!strIn.eof()){
        strIn >> rngs[dirs[0]];   rngs[dirs[0]]--;
        strIn >> rngs[dirs[0]+3]; rngs[dirs[0]+3]--;
        strIn >> rngs[dirs[1]];   rngs[dirs[1]]--;
        strIn >> rngs[dirs[1]+3]; rngs[dirs[1]+3]--;
      }
      blks[blkID].add_bc(fcID, bcStr, rngs);
      //cout << blkID << "|" << fcID  << endl;
    }
  }

  return(0);
}


int BlockGraph::fread_partition(const string& fname)
{
  ifstream input(fname);
  int      partID;

  for(int iBlk=0; iBlk<nBlk; iBlk++){
    input >> partID;
    blks[iBlk].set_partition(partID);
  }

  return(0);
}


int BlockGraph::fwrite_decomp_2d(const string& fname)
{
  ofstream output(fname);
  int      rngs[6], partID;
  double   coords[3];

  for(int iBlk=0; iBlk<nBlk; iBlk++){
    blks[iBlk].get_range(rngs);
    rngs[0] = max(rngs[0]-1, 0);
    rngs[1] = max(rngs[1]-1, 0);

    blks[iBlk].get_partition(partID);

    // left lower corner
    blks[iBlk].get_coord(rngs[0], rngs[1], 0, coords);
    output << coords[0] << " " << coords[1] << " "
           << partID << endl;
    // left upper corner
    blks[iBlk].get_coord(rngs[0], rngs[4], 0, coords);
    output << coords[0] << " " << coords[1] << " "
           << partID << endl;
    output << endl;

    // right lower corner
    blks[iBlk].get_coord(rngs[3], rngs[1], 0, coords);
    output << coords[0] << " " << coords[1] << " "
           << partID << endl;
    // right upper corner
    blks[iBlk].get_coord(rngs[3], rngs[4], 0, coords);
    output << coords[0] << " " << coords[1] << " "
           << partID << endl;
    output << endl;
    output << endl;
  }

  return(0);
}


int BlockGraph::fwrite_mesh_tecplot(const string& fname)
{
  ofstream output(fname);

  // tecplot header
  output << "TITLE = \"Mesh Per Proc\"" << endl;
  output << "VARIABLES = \"X\", \"Y\", \"Z\", \"Proc\"" << endl;

  // output each block's coordinates
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    int rngs[6];
    blks[iBlk].get_range(rngs);
    //rngs[0] = max(0, rngs[0]-1);
    //rngs[1] = max(0, rngs[1]-1);
    //rngs[2] = max(0, rngs[2]-1);

    // zone title
    int sizes[3];
    for(int i=0; i<3; i++)
      sizes[i] = rngs[i+3] - rngs[i] + 1;
    int partID;
    blks[iBlk].get_partition(partID);
    output << "ZONE T=\"Proc " << partID
           << "\", I=" << sizes[0]
           << ", J="  << sizes[1]
           << ", K="  << sizes[2]
           << ", DATAPACKING=POINT" << endl;
    output << "ZONETYPE=Ordered" << endl;

    //output coordinates
    for( int k=rngs[2]; k<=rngs[5]; k++){
      for( int j=rngs[1]; j<=rngs[4]; j++){
        for( int i=rngs[0]; i<=rngs[3]; i++){
          double coords[3];
          blks[iBlk].get_coord(i, j, k, coords);
          output << coords[0] << " " << coords[1] << " "
                 << coords[2] << " " << partID << endl;
        }
      }
    }
  }

  return(0);
}


int BlockGraph::set_mesh(int nBlock, const int* il, const int* jl, const int* kl, int nHalo)
{
  // resize the blk array
  this->resize(nBlock);

  // read block index ranges
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    int rngs[6] = {0};
    rngs[3] = il[iBlk] - 1;
    rngs[4] = jl[iBlk] - 1;
    rngs[5] = kl[iBlk] - 1;
    blks[iBlk].set_range(rngs);
  }

  // alloc coordinates
  for(int iBlk=0; iBlk<nBlk; iBlk++)
    blks[iBlk].alloc_coord(nHalo);

  return(0);
}


int BlockGraph::cmpt_comm(int& edgeCut, int& commVol)
{
  edgeCut = 0;
  commVol = 0;

  for(int iBlk=0; iBlk<nBlk; iBlk++)
  {
    int partID;
    blks[iBlk].get_partition(partID);
    // ...
    for(int iFc=0; iFc<6; iFc++){
      BlockFace face;
      blks[iBlk].get_face(iFc, face);
      // ...
      for(int iMp=0; iMp<face.nmap; iMp++){
        // ...
        if(face.bcType[iMp].compare("Block2Block") == 0){
          int toID = face.toBlk[iMp], toPartID;
          blks[toID].get_partition(toPartID);
          // ...
          // check if the connected block is in same proc
          if(partID != toPartID){
            commVol++;
            edgeCut += max(face.iRngs[iMp][3] - face.iRngs[iMp][0], 1) \
                     * max(face.iRngs[iMp][4] - face.iRngs[iMp][1], 1) \
                     * max(face.iRngs[iMp][5] - face.iRngs[iMp][2], 1);
          }
        }
      }
    }
  }

  return(0);
}


int BlockGraph::reset_parent()
{
  for(int iBlk=0; iBlk<nBlk; iBlk++)
    blks[iBlk].set_parentID(iBlk);

  return(0);
}


int BlockGraph::read_bc(int* NbcInt, int length, string* bcString, int* bcInt){


  int count, next=0;
  for (int i = 0; i < length; i++) {
    int blkID, fcID, toBlkID, toFcID, toFcDir, toDirs[2], toRngs[6], fcDir, dirs[2], rngs[6];
    string bcStr;
    bool isFlip;

    bcStr=bcString[i];
    count=NbcInt[i];

    blkID=bcInt[i+next];
    fcID=bcInt[i+1+next];
    blkID--;
    fcID--;
  // set the default face ranges, which is whole face
    fcDir   = fcID % 3;
    dirs[0] = EdgeMap3d[fcDir][0];
    dirs[1] = EdgeMap3d[fcDir][1];
    blks[blkID].get_range(rngs);
    rngs[fcDir]   = rngs[fcID];
    rngs[fcDir+3] = rngs[fcID];

    if(count==13){
      rngs[dirs[0]]=bcInt[i+2+next];rngs[dirs[0]]--;
      rngs[dirs[0]+3]=bcInt[i+3+next];rngs[dirs[0]+3]--;
      rngs[dirs[1]]=bcInt[i+4+next];rngs[dirs[1]]--;
      rngs[dirs[1]+3]=bcInt[i+5+next];rngs[dirs[1]+3]--;

      toBlkID=bcInt[i+6+next];
      toFcID=bcInt[i+7+next];
      toBlkID--;
      toFcID--;

      toFcDir   = toFcID % 3;
      toDirs[0] = EdgeMap3d[toFcDir][0];
      toDirs[1] = EdgeMap3d[toFcDir][1];
      //...
      blks[toBlkID].get_range(toRngs);
      toRngs[toFcDir]   = toRngs[toFcID];
      toRngs[toFcDir+3] = toRngs[toFcID];

      toRngs[toDirs[0]]=bcInt[i+8+next];toRngs[toDirs[0]]--;
      toRngs[toDirs[0]+3]=bcInt[i+9+next];toRngs[toDirs[0]+3]--;
      toRngs[toDirs[1]]=bcInt[i+10+next];toRngs[toDirs[1]]--;
      toRngs[toDirs[1]+3]=bcInt[i+11+next];toRngs[toDirs[1]+3]--;

      int flipStr;
      flipStr=bcInt[i+12+next];
      if(flipStr==0){isFlip = false;}else{isFlip = true;}
      // add bc
      blks[blkID].add_bc(fcID, bcStr, rngs, toBlkID, toFcID, toRngs, isFlip);
      //cout << blkID << "|" << fcID << "|" << toBlkID << "|" << toFcID << endl;
      // add reverse mapping
      for(int i=0; i<2; i++){
        if(toRngs[toDirs[i]] > toRngs[toDirs[i]+3]){
          swap(toRngs[toDirs[i]], toRngs[toDirs[i]+3]);
          swap(rngs[dirs[i]], rngs[dirs[i]+3]);
        }
      }
      blks[toBlkID].add_bc(toFcID, bcStr, toRngs, blkID, fcID, rngs, isFlip);
      count--;
      next=next+count;
    }else if (count == 6) {

      rngs[dirs[0]]=bcInt[i+2+next];rngs[dirs[0]]--;
      rngs[dirs[0]+3]=bcInt[i+3+next];rngs[dirs[0]+3]--;
      rngs[dirs[1]]=bcInt[i+4+next];rngs[dirs[1]]--;
      rngs[dirs[1]+3]=bcInt[i+5+next];rngs[dirs[1]+3]--;

      blks[blkID].add_bc(fcID, bcStr, rngs);
      count--;
      next=next+count;
    } else {

      blks[blkID].add_bc(fcID, bcStr, rngs);
      count--;
      next=next+count;
    }


  }

  return(0);
}


int BlockGraph::get_ID(int np, int* partiID){
  for (int iBlk = 0; iBlk < nBlk; iBlk++) {
    int id;
    blks[iBlk].get_parent(id);
    partiID[iBlk]=id;
  }
  return(0);
}


void BlockGraph::get_bc_name(string* bcstring){
  int next1=0, next2=0, a, b;
  for (int iBlk = 0; iBlk < nBlk; iBlk++) {
    for (int ifc = 0; ifc < 6; ifc++){
      BlockFace face;
      blks[iBlk].get_face(ifc,face);
      string str;
      int nm=face.nmap;
      for (int i = 0; i<nm; i++){
      bcstring[iBlk+ifc+i+next1+next2]=face.get_bc_name(str,i);a=i;}
      next1=next1+a;
      b=ifc;
    }
    next2=next2+b;
  }

}


int BlockGraph::get_number_faces(int &numberoffaces, int* nsurface){
  int nf=0,nf2;
  for (int iBlk = 0; iBlk < nBlk; iBlk++) {
    nf2=0;
    for (int ifc = 0; ifc < 6; ifc++){
      BlockFace face;
      blks[iBlk].get_face(ifc,face);
      nf=nf+face.nmap;
      nf2=nf2+face.nmap;
    }
  nsurface[iBlk]=nf2;
  }
  numberoffaces=nf;
  return (0);
}


int BlockGraph::get_bc(int* bclist){
  const int bl=17;
  int cont=0, countint;
  for (int iBlk = 0; iBlk < nBlk; iBlk++) {
    int blkID;
    blkID=iBlk;
    blkID++;
    int nmapping;
    for (int ifc = 0; ifc < 6; ifc++){
      BlockFace face;
      blks[iBlk].get_face(ifc,face);
      nmapping=face.nmap;
      int* bcprop= new int [bl*nmapping];
      face.get_prop(bcprop,blkID,ifc,nmapping);
      countint=0;
      for (int i = 0; i < bl*nmapping; i++) {
        countint=countint+1;
        bclist[i+cont]=bcprop[i];
       }
       cont=cont+countint;
       delete[] bcprop;
      }
    }
    return(0);
}


int BlockGraph::get_range(int* range){
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    int rng[6];
    blks[iBlk].get_range(rng);
    for (int i = 0; i < 6; i++) {
      range[i+iBlk*6]=rng[i];
    }
    }
 return(0);
}


/*------------------------------------------------------------------------------
                               BlockCut Class
------------------------------------------------------------------------------*/


BlockCut::BlockCut()
{
  blkID   = BLOCK_NULL;
  axis    = RANGE_NULL;
  pos     = RANGE_NULL;
  msgIncr = 0.0;
}


bool BlockCut::operator<(const BlockCut& rhs) const
{
  return(msgIncr < rhs.msgIncr);
}


bool BlockCut::operator>(const BlockCut& rhs) const
{
  return(msgIncr > rhs.msgIncr);
}
