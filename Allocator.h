// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // ptrdiff_t, size_t
#include <new>       // bad_alloc, new
#include <stdexcept> // invalid_argument

// ---------
// Allocator
// ---------
using namespace std;

template <typename T, std::size_t N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef       value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef       value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}                                              // this is correct

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

    private:
        // ----
        // data
        // ----

        char a[N];

        // -----
        // valid
        // -----

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         */
        bool valid () const {
          for (int i = 0; i < N; i++) {
            int sentinelA = (*this)[i];
            if (sentinelA < 0 && abs(sentinelA) < N) {
              if ((*this)[i+abs(sentinelA)+4] != sentinelA)
                return false;
            }
            if (sentinelA > 0 && sentinelA < N) {
              if ((*this)[i+sentinelA+4] != sentinelA)
                return false;
            }
            //pass over non sentinels
            i+=abs(sentinelA)+8-1;
          }
          return true;
        }

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestAllocator2, index);
        FRIEND_TEST(TestAllocator4, constructor1);
        FRIEND_TEST(TestAllocator4, constructor2);
        FRIEND_TEST(TestAllocator5, allocate);
        FRIEND_TEST(TestAllocator5, allocate1);
        FRIEND_TEST(TestAllocator5, allocate2);
        FRIEND_TEST(TestAllocator5, allocate3);
        FRIEND_TEST(TestAllocator5, allocate4);
        FRIEND_TEST(TestAllocator6, deallocate);
        FRIEND_TEST(TestAllocator6, deallocate1);
        FRIEND_TEST(TestAllocator6, deallocate2);
        FRIEND_TEST(TestAllocator6, deallocate3);
        FRIEND_TEST(TestAllocator6, deallocate4);
        int& operator [] (int i) {
            return *reinterpret_cast<int*>(&a[i]);}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * throw a bad_alloc exception, if N is less than sizeof(T) + (2 * sizeof(int))
         */
        Allocator () {
            //(*this)[0] = 0; // replace!
            // <your code>
            if (!(N < sizeof(T) + (2 * sizeof(int)))) {
              (*this)[0] = N-8;
              (*this)[N-4] = N-8;
            }
            else {
              bad_alloc exception;
              throw exception;
            }              
            assert(valid());}

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         * throw a bad_alloc exception, if n is invalid
         */
        pointer allocate (size_type n) {
          if ((int)n < 0) {
            bad_alloc exception;
            throw exception;
          }

          for (int i = 0; i < N - 1; i++) {
            int blockSize = (*this)[i];
            if (blockSize > 0 && blockSize < N) {
              if ((*this)[i+blockSize+4] == blockSize) {
                int dataSize = n * sizeof(T);
                int blockSizeNeeded = dataSize + 2 * sizeof(int);
                int allocatedBlockSize = blockSizeNeeded;

                //entire array is free and allocation requests entire array
                if (blockSize == blockSizeNeeded) {
                    (*this)[i] = -1 * (n * sizeof(T)) - 8;
                    (*this)[i+blockSize+4] = -1 * (n * sizeof(T)) - 8;
                    pointer ptr = reinterpret_cast<pointer>(&a[i+4]);
                    return ptr;
                }
                
                //blocksize+8 to account for sentinels 
                if (blockSize + 8 >= blockSizeNeeded) {
                  //found an exact fit
                  if (blockSize + 8 - blockSizeNeeded == 0) {
                    (*this)[i] = -1 * (n * sizeof(T));
                    (*this)[i+blockSize+4] = -1 * (n * sizeof(T));
                    pointer ptr = reinterpret_cast<pointer>(&a[i+4]);
                    return ptr;
                  }
                  if (blockSize + 8 - blockSizeNeeded < sizeof(T) + 2 * sizeof(int)){
                    blockSizeNeeded = blockSize;
                  }

                  (*this)[i] = -1 * (n * sizeof(T));
                  (*this)[i+4+dataSize] = -1 * dataSize;

                  //set new sentinels for new free block
                  (*this)[i+4+dataSize+4] = blockSize - blockSizeNeeded;
                  (*this)[i+blockSize+4] = blockSize - blockSizeNeeded;
                  
                  pointer ptr = reinterpret_cast<pointer>(&a[i+4]);
                  return ptr;
                }
              }
            }
            if (blockSize < 0 && -1 * blockSize < N && (*this)[i+(-1*blockSize)+4] == blockSize) {
              //skip over these
              i += (-1*blockSize) + 8 - 1;
            }
          }          
          bad_alloc exception;
          throw exception;}

        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                               // this is correct and exempt
            assert(valid());}                           // from the prohibition of new

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * after deallocation adjacent free blocks must be coalesced
         * throw an invalid_argument exception, if p is invalid
         * <your documentation>
         */
        void deallocate (pointer p, size_type) {
          // change sentinels from negative to positive  
          // coalesce:
          //    both adj blocks are free
          //    left adj block is free
          //    right adj block is free
          int *pp = reinterpret_cast<int*>(p);
          int sentinelAPos = -1;
          int& sentinelA = *(pp+sentinelAPos);
          int sentinelBPos = (-1*(sentinelA))/4;
          int& sentinelB = *(pp+sentinelBPos);

          // is ptr given valid?
          if (sentinelA < 0 && sentinelA == sentinelB) {
            sentinelA *= -1;
            sentinelB *= -1;
            
            //check right adjacent block to deallocated block's sentinels
            int sentinelRAdjAPos = sentinelBPos + 1;
            int& sentinelRAdjA = *(pp + sentinelRAdjAPos);
            int sentinelRAdjBPos = (abs(sentinelRAdjA))/4 + sentinelRAdjAPos + 1;
            int& sentinelRAdjB = *(pp + sentinelRAdjBPos);
              
            //check left adj block...
            int sentinelLAdjBPos = sentinelAPos - 1;
            int& sentinelLAdjB = *(pp + sentinelLAdjBPos);
            int sentinelLAdjAPos = sentinelLAdjBPos - abs(sentinelLAdjB)/4 - 1;
            int& sentinelLAdjA = *(pp + sentinelLAdjAPos);
           
            // both adj blocks are also free after deallocation of requested block 
            if (sentinelRAdjA > 0 && sentinelRAdjA == sentinelRAdjB && sentinelLAdjA > 0 && sentinelLAdjA == sentinelLAdjB) {
              int coalesceSize = sentinelLAdjA + sentinelA + sentinelRAdjA + 16;
              sentinelLAdjA = coalesceSize;
              sentinelRAdjB = coalesceSize;

              //clear inner sentinels
              sentinelLAdjB = 0;
              sentinelA = 0;
              sentinelB = 0;
              sentinelRAdjA = 0;
            }
            else {
              // only right adj block is also free
              if (sentinelRAdjA > 0 && sentinelRAdjA == sentinelRAdjB) {
                int coalesceSize = sentinelA + sentinelRAdjA + 8;
                sentinelA = coalesceSize;
                sentinelRAdjB = coalesceSize;

                //clear inner sentinels
                sentinelB = 0;
                sentinelRAdjA = 0;
              }
              // only left adj block is also free
              if (sentinelLAdjA > 0 && sentinelLAdjA == sentinelLAdjB) {
                int coalesceSize = sentinelB + sentinelLAdjA + 8;
                sentinelB = coalesceSize;
                sentinelLAdjA = coalesceSize;
                
                //clear inner sentinels
                sentinelA = 0;
                sentinelLAdjB = 0;
              }
            }
          }
          else {
            throw std::invalid_argument("Recieved bad pointer");
          }
          assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         */
        void destroy (pointer p) {
            p->~T();               // this is correct
            assert(valid());}

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        const int& operator [] (int i) const {
            return *reinterpret_cast<const int*>(&a[i]);}};

#endif // Allocator_h
