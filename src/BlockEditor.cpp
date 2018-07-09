#include <fstream>
#include <cstring>
#include <cmath>
#include "BlockEditor.h"
#include "Utility.h"

using namespace std;

int BlockEditor::split_bc(const BlockGraph& g, BlockGraph& sg)
{
  create_bc_cut(g);
  split_cut(g, sg);
  match_inner_face(sg);
  match_bc(g, sg); 
  destroy_cut();

  return(0);
}


int BlockEditor::split_proc(const BlockGraph& g, BlockGraph& sg)
{
  create_proc_cut_2d(g);
  split_cut(g, sg);
  match_inner_face(sg);
  match_bc(g, sg); 
  destroy_cut();
  return(0);
}


int BlockEditor::split_proc_rb(const BlockGraph& g, BlockGraph& sg)
{
  // set # level
  int maxNP = 0;
  for(int iBlk=0; iBlk<g.nBlk; iBlk++)
    if(maxNP < g.blks[iBlk].nProc)  maxNP = g.blks[iBlk].nProc;
  int nLevel = ceil(log2(maxNP));

  // recursively split the blocks
  BlockGraph tmpBg;
  sg = g;
  for(int i=0; i<nLevel; i++){
    create_bicut(sg);
    split_bicut(sg, tmpBg);
    match_inner_face(tmpBg);
    match_bc(sg, tmpBg); 
    destroy_cut();

    sg = tmpBg;
  }

  return(0);
}


int BlockEditor::split_cut(const BlockGraph& g, BlockGraph& sg)
{
  sg.nBlk = 0;
  for(int iBlk=0; iBlk<g.nBlk; iBlk++){
    sg.nBlk += (nCuts[iBlk][0]-1) * (nCuts[iBlk][1]-1) \
             * (nCuts[iBlk][2]-1);
  }
  if(sg.blks)  delete [] sg.blks;
  sg.blks = new Block [sg.nBlk];
  
  // split the block
  for(int iBlk=0; iBlk<g.nBlk; iBlk++)
  {
    for(int iCut=0; iCut < (nCuts[iBlk][0]-1); iCut++){
      for(int jCut=0; jCut < (nCuts[iBlk][1]-1); jCut++){
        for(int kCut=0; kCut < (nCuts[iBlk][2]-1); kCut++){

          // find id of current subblock
          int iSB = iCut * (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1) \
                  + jCut * (nCuts[iBlk][2]-1) + kCut + offsets[iBlk];
          
          // set parenet id and id 
          sg.blks[iSB].blkID  = iSB;
          sg.blks[iSB].pBlkID = g.blks[iBlk].pBlkID;

          // set subblock ranges
          sg.blks[iSB].iRngs[0] = cuts[iBlk][0][iCut];
          sg.blks[iSB].iRngs[1] = cuts[iBlk][1][jCut];
          sg.blks[iSB].iRngs[2] = cuts[iBlk][2][kCut];
          sg.blks[iSB].iRngs[3] = cuts[iBlk][0][iCut+1];
          sg.blks[iSB].iRngs[4] = cuts[iBlk][1][jCut+1];
          sg.blks[iSB].iRngs[5] = cuts[iBlk][2][kCut+1];

        }
      }
    }
  }

  return 0;
}


int BlockEditor::split_bicut(const BlockGraph& g, BlockGraph& sg)
{
  split_cut(g, sg);

  int iSB = 0;
  for(int iBlk=0; iBlk<g.nBlk; iBlk++){
    if(g.blks[iBlk].nProc > 1){ 
      sg.blks[iSB].nProc   = g.blks[iBlk].nProc / 2;
      sg.blks[iSB+1].nProc = g.blks[iBlk].nProc - sg.blks[iSB].nProc;
      iSB += 2;
    }
    else
    {
      sg.blks[iSB].nProc = 1;
      iSB++;
    }
  }

  return(0);
}


int BlockEditor::create_bc_cut(const BlockGraph& g)
{
  BlockFace fc; 
  int       max_ncut = 6;
  int       rngs[6];

  nBlk    = g.nBlk;
  nCuts   = new int*  [nBlk];
  cuts    = new int** [nBlk];
  // ...
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    nCuts[iBlk] = new int  [3];
     cuts[iBlk] = new int* [3];
    for( int iDir=0; iDir<3; iDir++){
      nCuts[iBlk][iDir]    = 2;
       cuts[iBlk][iDir]    = new int [max_ncut]; 
       cuts[iBlk][iDir][0] = g.blks[iBlk].iRngs[iDir];
       cuts[iBlk][iDir][1] = g.blks[iBlk].iRngs[iDir+3];
    }
  }

  for(int iBlk=0; iBlk<nBlk; iBlk++){
    for(int iFc=0; iFc<6; iFc++){
      g.blks[iBlk].get_face(iFc, fc);
      g.blks[iBlk].get_range(rngs);

      if(fc.nmap > 1){
        for(int imp=0; imp<fc.nmap; imp++){
          for(int iDir=0; iDir<3; iDir++){
            bool isCut =    (fc.iRngs[imp][iDir+3] > rngs[iDir]) \
                         && (fc.iRngs[imp][iDir+3] < rngs[iDir+3]); 
            if(isCut){
              sort_insert(fc.iRngs[imp][iDir+3], cuts[iBlk][iDir], \
                          nCuts[iBlk][iDir]);
            }
          }
        }
      }
    }
  }

  // set offsets
  offsets    = new int [nBlk];
  offsets[0] = 0;
  for(int iBlk=0; iBlk<nBlk-1; iBlk++){
    offsets[iBlk+1] = (nCuts[iBlk][0]-1) * (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1) \
                    + offsets[iBlk];
  }
  
  return 0;
}


int BlockEditor::create_proc_cut_2d(const BlockGraph& bg)
{
  // alloc space for cuts
  nBlk    = bg.nBlk;
  nCuts   = new int*  [nBlk];
  cuts    = new int** [nBlk];
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    nCuts[iBlk] = new int  [3];
     cuts[iBlk] = new int* [3];
  }

  for(int iBlk=0; iBlk<nBlk; iBlk++){
    // size of each dimension
    int nCells[3], rngs[6];
    bg.blks[iBlk].get_range(rngs);
    for(int i=0; i<2; i++)
      nCells[i] = rngs[i+3] - rngs[i];
    
    // find decomposition that minimize the comm
    int    blkNP   = bg.blks[iBlk].nProc;
    double pxIdeal = sqrt((double)nCells[0] * (double)blkNP / (double)nCells[1]);
    int    pxLo, pxUp;
    find_divisible(blkNP, pxIdeal, pxLo, pxUp);
    double commUp = (pxUp-1)*nCells[1] + (blkNP/pxUp-1)*nCells[0];
    double commLo = (pxLo-1)*nCells[1] + (blkNP/pxLo-1)*nCells[0];
    int    np[2];
    np[0] = pxLo;
    if(commUp < commLo)  np[0] = pxUp;
    np[1] = blkNP / np[0];

    // compute the cuts of the decomposition
    for(int iDir=0; iDir<2; iDir++){
      // create cuts
      nCuts[iBlk][iDir] = np[iDir] + 1;
       cuts[iBlk][iDir] = new int [np[iDir] + 1];
      // set cuts
      cuts[iBlk][iDir][0] = rngs[iDir];
      // ... 
      int stepFloo = (int)floor((double)nCells[iDir]/(double)np[iDir]);
      int npFloo   = np[iDir] * (stepFloo + 1) - nCells[iDir];
      // ...  cuts by ceil step
      for(int i=1; i <= (np[iDir]-npFloo); i++)
        cuts[iBlk][iDir][i] = cuts[iBlk][iDir][i-1] + (stepFloo + 1);
      // ... cut by floor step 
      for(int i=(np[iDir]-npFloo+1); i<=np[iDir]; i++)
        cuts[iBlk][iDir][i] = cuts[iBlk][iDir][i-1] + stepFloo;
    }

    // z direction
    nCuts[iBlk][2]    = 2;
     cuts[iBlk][2]    = new int [2];
     cuts[iBlk][2][0] = rngs[2];
     cuts[iBlk][2][1] = rngs[5];
  }

  // set offsets
  offsets    = new int [nBlk];
  offsets[0] = 0;
  for(int iBlk=0; iBlk<nBlk-1; iBlk++)
    offsets[iBlk+1] = (nCuts[iBlk][0]-1) * (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1) \
                    + offsets[iBlk];

  return(0);
}


int BlockEditor::create_bicut(const BlockGraph& bg)
{

  // alloc space for cuts
  nBlk    = bg.nBlk;
  nCuts   = new int*  [nBlk];
  cuts    = new int** [nBlk];
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    nCuts[iBlk] = new int  [3];
     cuts[iBlk] = new int* [3];
     for(int iDir=0; iDir<3; iDir++)
       cuts[iBlk][iDir] = new int [3];
  }

  for(int iBlk=0; iBlk<nBlk; iBlk++){
    // size of each dimension
    int sizes[3], rngs[6];
    bg.blks[iBlk].get_range(rngs);
    sizes[0] = rngs[3] - rngs[0] + 1;
    sizes[1] = rngs[4] - rngs[1] + 1;
    sizes[2] = rngs[5] - rngs[2] + 1;

    // init cuts
    for(int iDir=0; iDir<3; iDir++){
      nCuts[iBlk][iDir]    = 2;
       cuts[iBlk][iDir][0] = rngs[iDir];
       cuts[iBlk][iDir][1] = rngs[iDir+3];
    }

    // if assign with only 1 proc then nothing to be done
    int blkNP = bg.blks[iBlk].nProc;
    if(blkNP == 1)  continue;

    // cut the block along longest edge in two according to #proc
    int iLong = 0;
    for(int iDir=0; iDir<3; iDir++)
      if(sizes[iLong] < sizes[iDir])  iLong = iDir;
    // ...
    nCuts[iBlk][iLong]++;
    int nPCut = blkNP / 2;
    cuts[iBlk][iLong][1] = rngs[iLong] + sizes[iLong]*nPCut/blkNP;
    cuts[iBlk][iLong][2] = rngs[iLong+3];
  }

  // set offsets
  offsets    = new int [nBlk];
  offsets[0] = 0;
  for(int iBlk=0; iBlk<nBlk-1; iBlk++)
    offsets[iBlk+1] = (nCuts[iBlk][0]-1) * (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1) \
                    + offsets[iBlk];
  return(0);
}


int BlockEditor::match_inner_face(BlockGraph& sg)
{
  int rngs[6], toRngs[6];

  for(int iBlk=0; iBlk<nBlk; iBlk++)
  {
    // X
    if(nCuts[iBlk][0] > 2){
      for(int iCut=0; iCut < (nCuts[iBlk][0]-2); iCut++){
        for(int jCut=0; jCut < (nCuts[iBlk][1]-1); jCut++){
          for(int kCut=0; kCut < (nCuts[iBlk][2]-1); kCut++){
            // find id of current subblock
            int iSBm = iCut * (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1) \
                     + jCut * (nCuts[iBlk][2]-1) + kCut + offsets[iBlk];
            int iSBp = iSBm + (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1);
            // index ranges on face
            memcpy(rngs,   sg.blks[iSBm].iRngs, 6*sizeof(int));
            memcpy(toRngs, sg.blks[iSBp].iRngs, 6*sizeof(int));
            rngs[0]   = rngs[3];
            toRngs[3] = toRngs[0];
            // set connections
            sg.blks[iSBm].add_bc(3, "Block2Block", rngs,   iSBp, 0, toRngs, false);
            sg.blks[iSBp].add_bc(0, "Block2Block", toRngs, iSBm, 3, rngs,   false);
          }
        }
      }
    }

    // Y
    if(nCuts[iBlk][1] > 2){
      for(int iCut=0; iCut < (nCuts[iBlk][0]-1); iCut++){
        for(int jCut=0; jCut < (nCuts[iBlk][1]-2); jCut++){
          for(int kCut=0; kCut < (nCuts[iBlk][2]-1); kCut++){
            // find id of current subblock
            int iSBm = iCut * (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1) \
                     + jCut * (nCuts[iBlk][2]-1) + kCut + offsets[iBlk];
            int iSBp = iSBm + (nCuts[iBlk][2]-1);
            // index ranges on face
            memcpy(rngs,   sg.blks[iSBm].iRngs, 6*sizeof(int));
            memcpy(toRngs, sg.blks[iSBp].iRngs, 6*sizeof(int));
            rngs[1]   = rngs[4];
            toRngs[4] = toRngs[1];
            // set connections
            sg.blks[iSBm].add_bc(4, "Block2Block", rngs,   iSBp, 1, toRngs, false);
            sg.blks[iSBp].add_bc(1, "Block2Block", toRngs, iSBm, 4, rngs,   false);
          }
        }
      }
    }

    // Z
    if(nCuts[iBlk][2] > 2){
      for(int iCut=0; iCut < (nCuts[iBlk][0]-1); iCut++){
        for(int jCut=0; jCut < (nCuts[iBlk][1]-1); jCut++){
          for(int kCut=0; kCut < (nCuts[iBlk][2]-2); kCut++){
            // find id of current subblock
            int iSBm = iCut * (nCuts[iBlk][1]-1) * (nCuts[iBlk][2]-1) \
                     + jCut * (nCuts[iBlk][2]-1) + kCut + offsets[iBlk];
            int iSBp = iSBm + 1;
            // index ranges on face
            memcpy(rngs,   sg.blks[iSBm].iRngs, 6*sizeof(int));
            memcpy(toRngs, sg.blks[iSBp].iRngs, 6*sizeof(int));
            rngs[2]   = rngs[5];
            toRngs[5] = toRngs[2];
            // set connections
            sg.blks[iSBm].add_bc(5, "Block2Block", rngs,   iSBp, 2, toRngs, false);
            sg.blks[iSBp].add_bc(2, "Block2Block", toRngs, iSBm, 5, rngs,   false);
          }
        }
      }
    }
  }

  return(0);
}


int BlockEditor::match_bc(const BlockGraph& g, BlockGraph& sg)
{
  BlockFace face, toFace;
  int sbRngs[6], toSBRngs[6], rngs[6], toRngs[6];

  for(int iBlk=0; iBlk<nBlk; iBlk++)
  {
    for(int iFc=0; iFc<6; iFc++){
      g.blks[iBlk].get_face(iFc, face);
      for(int iMp=0; iMp<face.nmap; iMp++)
      {
        find_subblock(face.iRngs[iMp], iBlk, sbRngs);

        for(int iSB = sbRngs[0]; iSB <= sbRngs[3]; iSB++){
          for(int jSB = sbRngs[1]; jSB <= sbRngs[4]; jSB++){
            for(int kSB = sbRngs[2]; kSB <= sbRngs[5]; kSB++){
              // current subblock id
              int id = iSB * (nCuts[iBlk][1] - 1) * (nCuts[iBlk][2] - 1) \
                     + jSB * (nCuts[iBlk][2] - 1) + kSB + offsets[iBlk];
              // face range of subblock
              for(int iDir=0; iDir<3; iDir++){
                rngs[iDir]   = max(sg.blks[id].iRngs[iDir],   face.iRngs[iMp][iDir]);
                rngs[iDir+3] = min(sg.blks[id].iRngs[iDir+3], face.iRngs[iMp][iDir+3]);
              }

              // check if it is b2b bc
              if(face.bcType[iMp].compare("Block2Block") == 0) // if b2b
              {
                // find face range on the target block
                map_range(face, iMp, rngs, toRngs);
                // find target block 
                int toBlkID = face.toBlk[iMp];
                // find target subblocks involved with target face range
                find_subblock(toRngs, toBlkID, toSBRngs);

                for(int iToSB = toSBRngs[0]; iToSB <= toSBRngs[3]; iToSB++){
                  for(int jToSB = toSBRngs[1]; jToSB <= toSBRngs[4]; jToSB++){
                    for(int kToSB = toSBRngs[2]; kToSB <= toSBRngs[5]; kToSB++){
                      // current target subblock
                      int toID = iToSB * (nCuts[toBlkID][1] - 1) * (nCuts[toBlkID][2] - 1) \
                               + jToSB * (nCuts[toBlkID][2] - 1) + kToSB + offsets[toBlkID];
                      // find target subblock face range
                      int sbToRngs[6];
                      for(int iDir=0; iDir<3; iDir++){
                        sbToRngs[iDir]   = max(sg.blks[toID].iRngs[iDir],\
                                               toRngs[iDir]);
                        sbToRngs[iDir+3] = min(sg.blks[toID].iRngs[iDir+3],\
                                               toRngs[iDir+3]);
                      }
                      // map the target subblock face range back to sourse subblock
                      map_range_back(face, iMp, sbToRngs, rngs);
                      // inverse the target face range if necessary
                      for(int iDir=0; iDir<3; iDir++){
                        if(face.toiRngs[iMp][iDir] > face.toiRngs[iMp][iDir+3]){
                          int tmp = sbToRngs[iDir];
                          sbToRngs[iDir]   = sbToRngs[iDir+3];
                          sbToRngs[iDir+3] = tmp;
                        }
                      }
                      // add b2b bc
                      sg.blks[id].add_bc(iFc, "Block2Block", rngs, toID, face.toFace[iMp], 
                                         sbToRngs, false);
                      
                    } // end for kToSB
                  } // end for jToSB
                } // end for iToSB
              } // end if b2b

              else // if it is physical bc
              { 
                sg.blks[id].add_bc(iFc, face.bcType[iMp], rngs);
              }

            } // end for kSB
          } // end for jSB
        } // end for iSB

      } // end for iMp
    } // end for iFc
  } // end for iBlk

  return(0);
}


int BlockEditor::destroy_cut()
{
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    delete [] nCuts[iBlk];
    for(int iDir=0; iDir<3; iDir++){
      delete [] cuts[iBlk][iDir];
    }
    delete [] cuts[iBlk];
  }
  delete [] offsets;

  return 0;
}


int BlockEditor::find_subblock(const int rngs[6], int blk, int sbRngs[6])
{
  for(int iDir=0; iDir<3; iDir++){ 
    search_cut_l(rngs[iDir],   blk, iDir, sbRngs[iDir]);
    search_cut_r(rngs[iDir+3], blk, iDir, sbRngs[iDir+3]);
  }

  return(0);
}


int BlockEditor::search_cut_l(int val, int blk, int dir, int& interval)
{
  int  is, ie, im;

  if(nCuts[blk][dir] == 2){
      interval = 0;
      return(0);
  }

  if(val == cuts[blk][dir][nCuts[blk][dir]-1]){
    interval = nCuts[blk][dir] - 2;
    return(0);
  }

  is      = 0;
  ie      = nCuts[blk][dir] - 2;
  while(is <= ie){
    im = (is + ie) / 2;
    if(val >= cuts[blk][dir][im+1]){
      is = im + 1;
    }
    else if(val < (cuts[blk][dir][im])){
      ie = im - 1;
    }
    else{
      interval = im;
      break;
    }
  }
   
  return(0);
}


int BlockEditor::search_cut_r(int val, int blk, int dir, int& interval)
{
  int  is, ie, im;

  if(nCuts[blk][dir] == 2){
      interval = 0;
      return(0);
  }

  if(val == cuts[blk][dir][0]){
    interval = 0;
    return(0);
  }

  is      = 0;
  ie      = nCuts[blk][dir] - 2;
  while(is <= ie){
    im = (is + ie) / 2;
    if(val > cuts[blk][dir][im+1]){
      is = im + 1;
    }
    else if(val <= (cuts[blk][dir][im])){
      ie = im - 1;
    }
    else{
      interval = im;
      break;
    }
  }
   
  return(0);
}


int BlockEditor::map_range(const BlockFace& face, int mapID, \
                           const int rngs[6],     int toRngs[6])
{
  int dirs[2], toDirs[2];
  for(int i=0; i<2; i++){
    dirs[i]   = EdgeMap3d[face.ID%3][i];
    toDirs[i] = EdgeMap3d[face.toFace[mapID]%3][i];
  }

  if(face.isFlip[mapID]){
    int tmp   = toDirs[0];
    toDirs[0] = toDirs[1];
    toDirs[1] = tmp;
  }

  // set target range
  for(int iDir=0; iDir<3; iDir++){
    toRngs[iDir]   = face.toiRngs[mapID][iDir];
    toRngs[iDir+3] = face.toiRngs[mapID][iDir+3];
  }
  //...
  for(int i=0; i<2; i++){ 
    if(face.toiRngs[mapID][toDirs[i]+3] > face.toiRngs[mapID][toDirs[i]]){
      toRngs[toDirs[i]]   = face.toiRngs[mapID][toDirs[i]] + rngs[dirs[i]] \
                          - face.iRngs[mapID][dirs[i]];
      toRngs[toDirs[i]+3] = face.toiRngs[mapID][toDirs[i]] + rngs[dirs[i]+3] \
                          - face.iRngs[mapID][dirs[i]];
    }
    else{
      toRngs[toDirs[i]]   = face.toiRngs[mapID][toDirs[i]] - rngs[dirs[i]+3] \
                          + face.iRngs[mapID][dirs[i]];
      toRngs[toDirs[i]+3] = face.toiRngs[mapID][toDirs[i]] - rngs[dirs[i]] \
                          + face.iRngs[mapID][dirs[i]];
    }
  }

  return(0);
}


int BlockEditor::map_range_back(const BlockFace& face, int mapID, \
                                const int toRngs[6],   int rngs[6])
{
  int dirs[2], toDirs[2];
  for(int i=0; i<2; i++){
    dirs[i]   = EdgeMap3d[face.ID%3][i];
    toDirs[i] = EdgeMap3d[face.toFace[mapID]%3][i];
  }

  if(face.isFlip[mapID]){
    int tmp   = toDirs[0];
    toDirs[0] = toDirs[1];
    toDirs[1] = tmp;
  }

  // set target range
  for(int iDir=0; iDir<3; iDir++){
    rngs[iDir]   = face.iRngs[mapID][iDir]; 
    rngs[iDir+3] = face.iRngs[mapID][iDir+3]; 
  }
  //...
  for(int i=0; i<2; i++){ 
    if(face.toiRngs[mapID][toDirs[i]+3] > face.toiRngs[mapID][toDirs[i]]){
      rngs[dirs[i]]   = face.iRngs[mapID][dirs[i]] + toRngs[toDirs[i]] \
                      - face.toiRngs[mapID][toDirs[i]];
      rngs[dirs[i]+3] = face.iRngs[mapID][dirs[i]] + toRngs[toDirs[i]+3] \
                      - face.toiRngs[mapID][toDirs[i]];
    }
    else{
      rngs[dirs[i]]   = face.iRngs[mapID][dirs[i]] - toRngs[toDirs[i]+3] \
                      + face.toiRngs[mapID][toDirs[i]];
      rngs[dirs[i]+3] = face.iRngs[mapID][dirs[i]] - toRngs[toDirs[i]] \
                      + face.toiRngs[mapID][toDirs[i]];
    }
  }

  return(0);
}


int BlockEditor::view()
{
  cout << "# Blocks " << nBlk << endl;
  
  for(int iBlk=0; iBlk<nBlk; iBlk++){
    cout << "Offsets  " << offsets[iBlk] << endl;

    for(int iDir=0; iDir<3; iDir++){ 
      cout << "nCuts " << iDir << ": " << nCuts[iBlk][iDir] << endl;
      cout << "cuts  " << iDir << ": ";
      for(int iCut=0; iCut < nCuts[iBlk][iDir]; iCut++){
        cout << cuts[iBlk][iDir][iCut] << " ";
      }
      cout << endl;
    }
  }

  return(0);
}
