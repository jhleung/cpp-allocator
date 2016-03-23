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
            // <your code>
            return !(N < sizeof(T) + (2 * sizeof(int)));
        }

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * https://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
         */
        FRIEND_TEST(TestAllocator2, index);
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
            // <your code>
            // Find first fit; if no fit -> n is invalid?
            // set sentinels 
            // if free block cannot fit another allocatioin, allocate more than needed 
            // return pointer to first elem after sentinel
          for (int i = 0; i < N - 1; i++) {
            int blockSize = (*this)[i];
            //int& sentinelB = (*this)[i+blockSize+4];
            if (blockSize > 0 && blockSize < N) {
              if ((*this)[i+blockSize+4] == blockSize) {
                //allocate
                int dataSize = n * sizeof(T);
                int blockSizeNeeded = dataSize + 2 * sizeof(int);
                int allocatedBlockSize = blockSizeNeeded;
                if (blockSize >= blockSizeNeeded) {
                  if (blockSize - blockSizeNeeded < sizeof(T) + 2 * sizeof(int))
                    blockSizeNeeded = blockSize;

                  (*this)[i] = -1 * (n * sizeof(T));
                  (*this)[i+4+dataSize] = -1 * dataSize;

                  //set new sentinels for new free block
                  (*this)[i+4+dataSize+4] = blockSize - blockSizeNeeded;
                  (*this)[i+blockSize+4]= blockSize - blockSizeNeeded;
                  
                  int& firstData = (*this)[i+4];
                  pointer ptr = reinterpret_cast<pointer>(firstData);
                  return ptr;
                }
              }
            }
            if (blockSize < 0 && -1 * blockSize < N && (*this)[i+(-1*blockSize)+4] == blockSize) {
              //skip over these
              i += (-1*blockSize) + 8;
            }
          }          
          return nullptr;}             // replace!

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
            // <your code>
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
