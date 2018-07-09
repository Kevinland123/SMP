#include <iostream>
#include <cmath>
#include "BlockPart.h"

using namespace std;

BlockPart::BlockPart()
{
  compLds   = NULL;
  bandLds   = NULL;
  ltncLds   = NULL;
  nProc     = 0;
  workSum   = 0;
  workIdeal = 0.0;
}


BlockPart::BlockPart(int nProcess, const BlockGraph& blkGraph, bool isCutBC)
{
  if(isCutBC){
    be.split_bc(blkGraph, bg);
  }
  else{
    bg = blkGraph;
  }

  nProc = nProcess;

  compLds = new int [nProc];
  ltncLds = new int [nProc];
  bandLds = new int [nProc];
  for(int i=0; i<nProc; i++){
    compLds[i] = 0;
    ltncLds[i] = 0;
    bandLds[i] = 0;
  }

  bg.get_workload(workSum);
  workIdeal = (double)workSum / (double)nProc;
}


BlockPart::BlockPart(const BlockPart& rhs)
{
  bg        = rhs.bg;
  nProc     = rhs.nProc;
  workSum   = rhs.workSum;
  workIdeal = rhs.workIdeal;

  if(rhs.compLds){
    compLds = new int [nProc];
    ltncLds = new int [nProc];
    bandLds = new int [nProc];
    for(int i=0; i<nProc; i++){
      compLds[i] = rhs.compLds[i];
      ltncLds[i] = rhs.ltncLds[i];
      bandLds[i] = rhs.bandLds[i];
    }
  }
}


const BlockPart& BlockPart::operator=(const BlockPart& rhs)
{
  if(this != &rhs){
    bg        = rhs.bg;
    nProc     = rhs.nProc;
    workSum   = rhs.workSum;
    workIdeal = rhs.workIdeal;

    if(compLds){
      delete [] compLds;
      compLds = NULL;
    }
    if(rhs.compLds){
      compLds = new int [nProc];
      for(int i=0; i<nProc; i++)
        compLds[i] = rhs.compLds[i];
    }

    if(ltncLds){
      delete [] ltncLds;
      ltncLds = NULL;
    }
    if(rhs.ltncLds){
      ltncLds = new int [nProc];
      for(int i=0; i<nProc; i++)
        ltncLds[i] = rhs.ltncLds[i];
    }

    if(bandLds){
      delete [] bandLds;
      bandLds = NULL;
    }
    if(rhs.bandLds){
      bandLds = new int [nProc];
      for(int i=0; i<nProc; i++)
        bandLds[i] = rhs.bandLds[i];
    }
  }

  return(*this);
}


BlockPart::~BlockPart()
{
  if(compLds)  delete [] compLds;
  if(ltncLds)  delete [] ltncLds;
  if(bandLds)  delete [] bandLds;
}


int BlockPart::set_mesh(BlockGraph blkGraph)
{
  bg = blkGraph;
  bg.get_workload(workSum);

  return(0);
}


int BlockPart::set_proc(int nProcess)
{
  nProc = nProcess;
  workIdeal = (double)workSum / (double)nProc;

  if(compLds)  delete [] compLds;
  if(ltncLds)  delete [] ltncLds;
  if(bandLds)  delete [] bandLds;

  compLds = new int [nProc];
  ltncLds = new int [nProc];
  bandLds = new int [nProc];

  for(int i=0; i<nProc; i++){
    compLds[i] = 0;
    ltncLds[i] = 0;
    bandLds[i] = 0;
  }
  
  return(0);
}


int BlockPart::set_comm(int nHalo_, int dataSize_, double alpha_, double beta_)
{
  nHalo_   = nHalo;
  dataSize = dataSize_;
  alpha    = alpha_;
  beta     = beta_;

  return 0;
}


int BlockPart::assign_proc()
{
  int rngs[6];

  // celing # proc for each block
  double *npFloat = new double [bg.nBlk];
  int    *np      = new int    [bg.nBlk];
  bool   *isNpSet = new bool   [bg.nBlk];

  for(int iBlk=0; iBlk<bg.nBlk; iBlk++){
    // work of current block, # grid cells
    bg.blks[iBlk].get_range(rngs);
    int work = (rngs[3] - rngs[0]) * (rngs[4] - rngs[1]) \
             * max(rngs[5] - rngs[2], 1);
    // ideal # proc
    npFloat[iBlk] = (double)work / (double)workSum * nProc;
    // celing # proc
    np[iBlk] = ceil(npFloat[iBlk]);
    // init not set
    isNpSet[iBlk] = false;

  }

  // exceeding # proc
  int npExtra = 0;
  for(int iBlk=0; iBlk<bg.nBlk; iBlk++){
    npExtra += np[iBlk];
  }
  npExtra -= nProc;

  // adjust the exceeding proc so that the sum of blk'proc fit the total
  for(int i=npExtra; i>0; i--)
  {
    double minExtra    = 10.0;
    int    blkMinExtra = 0;
    // ...
    for(int iBlk=0; iBlk<bg.nBlk; iBlk++)
    {
      if(np[iBlk] == 1 || isNpSet[iBlk])  continue;

      double extra = npFloat[iBlk]/(np[iBlk]-1);
      if(extra < minExtra){
        minExtra    = extra;
        blkMinExtra = iBlk;
      }
    }
    //reduce one extra proc
    np[blkMinExtra]--;
    isNpSet[blkMinExtra] = true;
  }

  // set each blk's #proc
  for(int iBlk=0; iBlk<bg.nBlk; iBlk++){
    bg.blks[iBlk].set_nproc(np[iBlk]);
  }

  delete [] npFloat;
  delete [] np;
  delete [] isNpSet;

  return(0);
}


int BlockPart::decompose(BlockGraph& sg, int method)
{
  // assign # proc to each block
  assign_proc();

  // split each block
  switch(method){
    case PART_DEFAULT:
      be.split_proc(bg, sg);    break;
    case PART_RB:
      be.split_proc_rb(bg, sg); break;
  }

  // set the partition
  for(int iBlk=0; iBlk<sg.nBlk; iBlk++)
    sg.blks[iBlk].set_partition(iBlk);

  // compute the work load per proc
  for(int iBlk=0; iBlk<sg.nBlk; iBlk++)
  {
    int work, partID;
    sg.blks[iBlk].get_partition(partID);
    sg.blks[iBlk].get_workload(work);
    compLds[partID] += work;
  }

  // compute the communication load
  for(int iBlk=0; iBlk<sg.nBlk; iBlk++)
  {
    int partID;
    sg.blks[iBlk].get_partition(partID);
    // ...
    for(int iFc=0; iFc<6; iFc++){
      BlockFace face;
      sg.blks[iBlk].get_face(iFc, face);
      // ...
      for(int iMp=0; iMp<face.nmap; iMp++){
        // ...
        if(face.bcType[iMp].compare("Block2Block") == 0){
          int toID = face.toBlk[iMp], toPartID;
          sg.blks[toID].get_partition(toPartID);
          // ...
          // check if the connected block is in same proc
          if(partID != toPartID){
            ltncLds[partID]++;
            bandLds[partID] += max(face.iRngs[iMp][3] - face.iRngs[iMp][0], 1) \
                             * max(face.iRngs[iMp][4] - face.iRngs[iMp][1], 1) \
                             * max(face.iRngs[iMp][5] - face.iRngs[iMp][2], 1);
            //bandLds[partID] *= nHalo * dataSize;
          }
        }
      }
    }
  }

  return(0);
}


int BlockPart::view()
{
  // find the most overload and underload proc
  double loadMax = 0.0, loadMin = 0.0, toler = 0.04, std = 0.0;
  int    procLdMax = PARTITION_NULL, procLdMin = PARTITION_NULL;
  int    nProcOL = 0, nProcUL = 0;
  // ...
  for(int i=0; i<nProc; i++){
    double err = (compLds[i] - workIdeal) / workIdeal;
    std += (compLds[i] - workIdeal) * (compLds[i] - workIdeal);
    // check overload
    if(err > loadMax || err > toler){
      loadMax   = err;
      procLdMax = i;
      nProcOL ++;
    }
    // check underload
    if(err < loadMin || err < -toler){
      loadMin   = err;
      procLdMin = i;
      nProcUL ++;
    }
  }

  std = sqrt(std / (nProcOL + nProcUL + 1.0E-10));

  cout << "------Block Partition Info------" << endl;
  cout << "----computation" << endl;
  cout << "# Proc:        " << nProc << endl;
  cout << "Idead load:    " << workIdeal << endl;
  cout << "overload %:    " << double(nProcOL) / nProc << endl;
  cout << "Max overload:  " << procLdMax << " " << loadMax << endl;
  cout << "underload %:   " << double(nProcUL) / nProc << endl;
  cout << "Min underload: " << procLdMin << " " << loadMin << endl;
  cout << "std:           " << std << endl;

  cout << "----latency" << endl;

  // latency statistics
  double loadAvg = 0.0;
  loadMax = 0.0;  loadMin = 0.0;
  std     = 0.0;
  //.. average
  for(int i=0; i<nProc; i++)
    loadAvg += (double)ltncLds[i];
  loadAvg = loadAvg / nProc;
  //.. max, min
  for(int i=0; i<nProc; i++){
    double err = (ltncLds[i] - loadAvg) / loadAvg;
    std += (ltncLds[i] - loadAvg) * (ltncLds[i] - loadAvg);
    // check overload
    if(err > loadMax){
      loadMax   = err;
      procLdMax = i;
    }
    // check underload
    if(err < loadMin){
      loadMin   = err;
      procLdMin = i;
    }
  }
  //.. std
  std = sqrt(std / nProc);
  //.. print
  cout << "Avg latency:   " << loadAvg << endl;
  cout << "Max  overload: " << procLdMax << " " << loadMax << endl;
  cout << "Min underload: " << procLdMin << " " << loadMin << endl;
  cout << "std:           " << std << endl;

  // bandwidth statistics
  loadAvg = 0.0;  loadMax = 0.0;  loadMin = 0.0;
  std     = 0.0;
  //.. average
  for(int i=0; i<nProc; i++)
    loadAvg += (double)bandLds[i];
  loadAvg = loadAvg / nProc;
  //.. max, min
  for(int i=0; i<nProc; i++){
    double err = (bandLds[i] - loadAvg) / loadAvg;
    std += (bandLds[i] - loadAvg) * (bandLds[i] - loadAvg);
    // check overload
    if(err > loadMax){
      loadMax   = err;
      procLdMax = i;
    }
    // check underload
    if(err < loadMin){
      loadMin   = err;
      procLdMin = i;
    }
  }
  //.. std
  std = sqrt(std / nProc);
  //.. print
  cout << "Avg message:   " << loadAvg << endl;
  cout << "Max  overload: " << procLdMax << " " << loadMax << endl;
  cout << "Min underload: " << procLdMin << " " << loadMin << endl;
  cout << "std:           " << std << endl;

  return(0);
}
