#include <cmath>
#include "Utility.h"

int sort_insert(int val, int arr[], int& nElem)
{
  for(int i=0; i<nElem; i++){
    if(val == arr[i]){
      return(0);
    }
    else if(val < arr[i]){
      for(int j=nElem; j>i; j--){
        arr[j] = arr[j-1];
      }
      arr[i] = val;
      nElem++;
      return(0);
    }
  }

}


int sign(int val)
{
  if(val > 0)
    return(1);
  else if(val < 0)
    return(-1);
  else
    return(0);
}


int find_divisible(int  numerator, double  denominator, \
                   int& lowFactor, int&    upFactor)
{
  lowFactor = (int)ceil(denominator);
  while(numerator % lowFactor != 0){
    lowFactor--;
  }

  upFactor  = (int)floor(denominator);
  while(numerator % upFactor != 0){
    upFactor++;
  }

  return(0);
}
