//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:56:45 taubin>
//------------------------------------------------------------------------
//
// Partition.hpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//     * Neither the name of the Brown University nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GABRIEL
// TAUBIN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#ifndef _PARTITION_HPP_
#define _PARTITION_HPP_

#include <vector>

using namespace std;

class Partition {

  // this class implements the Fast Union-Find data structure
  //
  // Reference
  // https://en.wikipedia.org/wiki/Disjoint-set_data_structure
  
public:

  // create a partition of the N elements {0,1,2,...,N-1} where
  // every element is a singleton {0},{1},{2},...,{N-1}
          Partition(const int nElements);

  // delete the current partition and create a new partition of the N
  // elements {0,1,2,...,N-1} where every element is a singleton
  // {0},{1},{2},...,{N-1}; the number of elements N can be different
  // from the one previously set by the constructor or by a previous
  // call to this method
  virtual void reset(const int nElements);

  // returns the current number of elements; i.e. the value of the
  // parameter N passed to the constructore or to the reset(N) method 
  int     getNumberOfElements()          const;

  // returns the current number of parts; immediately after
  // constructed or reset, the the number of parts should be equal to
  // the number of elements because each element becomes a singleton
  int     getNumberOfParts()             const;

  // the class assigns each part a unique non-negative ID number; this
  // method return the part ID number of the part containing element i;
  // if the element index is out of range this method returns -1
  int     find(const int i);

  // if elements i and j belong to the same part, this method returns
  // the ID of the part containing the two elements; otherwise, the
  // two parts are joined into a single part, and the ID of the new
  // part is returned; the ID of the new part resulting from joining
  // the previous two is not guaranteed to have any relation with the
  // IDs of the original two parts; the old IDs are not longer valid;
  // if either one of the two element indices is out of range this
  // method returns -1
  virtual int join(const int i, const int j);

  // returns the number of elements in the part containing the element
  // i; if the element index is out of range this method returns 0
  int     getSize(const int i)           const;
  
protected: // so that they accesible to SplittablePartition methods

  int         _nParts;
  vector<int> _parent;
  vector<int> _size;

};

#endif /* _PARTITION_HPP_ */
