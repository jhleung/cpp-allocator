// ------------------------------------
// projects/allocator/TestAllocator1.c++
// Copyright (C) 2015
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// --------------
// TestAllocator1
// --------------

template <typename A>
struct TestAllocator1 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            std::allocator<int>,
            std::allocator<double>,
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_1;

TYPED_TEST_CASE(TestAllocator1, my_types_1);

TYPED_TEST(TestAllocator1, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator1, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type  x;
    const size_type       s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// --------------
// TestAllocator2
// --------------

TEST(TestAllocator2, const_index) {
    const Allocator<int, 100> x;
    ASSERT_EQ(92, x[0]);}

TEST(TestAllocator2, const_index2) {
    const Allocator<int, 100> x;
    ASSERT_EQ(92, x[96]);}

// --------------
// TestAllocator3
// --------------

template <typename A>
struct TestAllocator3 : testing::Test {
    // --------
    // typedefs
    // --------

    typedef          A             allocator_type;
    typedef typename A::value_type value_type;
    typedef typename A::size_type  size_type;
    typedef typename A::pointer    pointer;};

typedef testing::Types<
            Allocator<int,    100>,
            Allocator<double, 100> >
        my_types_2;

TYPED_TEST_CASE(TestAllocator3, my_types_2);

TYPED_TEST(TestAllocator3, test_1) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 1;
    const value_type     v = 2;
    const pointer        p = x.allocate(s);
    if (p != nullptr) {
        x.construct(p, v);
        ASSERT_EQ(v, *p);
        x.destroy(p);
        x.deallocate(p, s);}}

TYPED_TEST(TestAllocator3, test_10) {
    typedef typename TestFixture::allocator_type allocator_type;
    typedef typename TestFixture::value_type     value_type;
    typedef typename TestFixture::size_type      size_type;
    typedef typename TestFixture::pointer        pointer;

          allocator_type x;
    const size_type      s = 10;
    const value_type     v = 2;
    const pointer        b = x.allocate(s);
    if (b != nullptr) {
        pointer e = b + s;
        pointer p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        ASSERT_EQ(s, std::count(b, e, v));
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}}

// MY TESTS BEGIN

// --------------
// Constructor
// --------------

TEST(TestAllocator4, constructor1) {
  const Allocator<int, 12> x;
  ASSERT_EQ(x[0], 4);
  ASSERT_EQ(x[8], 4);
}

TEST(TestAllocator4, constructor2) {
  const Allocator<double, 16> x;
  ASSERT_EQ(x[0], 8);
  ASSERT_EQ(x[12], 8);
}

//Test for bad_alloc in constructor
TEST(TestAllocator4, constructor3) {
  try {
    const Allocator<int, 4> x;
    // shouldnt reach this line
    ASSERT_EQ(false, true);
  }
  catch (bad_alloc exception) {
    ASSERT_EQ(true, true);
  }
}

// --------------
// Allocate
// --------------

//simple allocation
TEST(TestAllocator5, allocate) {
  Allocator<int, 100> x;
  x.allocate(1);
  ASSERT_EQ(x[0], -4);
  ASSERT_EQ(x[8], -4);
  ASSERT_EQ(x[12], 80);
  ASSERT_EQ(x[96], 80);

  x.allocate(5);
  ASSERT_EQ(x[12], -20);
  ASSERT_EQ(x[36], -20);
  ASSERT_EQ(x[40], 52);
  ASSERT_EQ(x[96], 52);
  
  x.allocate(13);
  ASSERT_EQ(x[40], -52);
  ASSERT_EQ(x[96], -52);
}

//allocating more than needed if necessary
TEST(TestAllocator5, allocate1) {
  Allocator<int, 100> x;
  x.allocate(21);
  ASSERT_EQ(x[0], -92);
  ASSERT_EQ(x[96], -92);
}

// allocating exact fit
TEST(TestAllocator5, allocate2) {
  Allocator<int, 92> x;
  x.allocate(21);
  ASSERT_EQ(x[0], -84);
  ASSERT_EQ(x[88], -84);
}

// no free blocks available
TEST(TestAllocator5, allocate3) {
  Allocator<int, 12> x;
  x.allocate(1);
  ASSERT_EQ(x[0], -4);
  ASSERT_EQ(x[8], -4);

  try {
    x.allocate(1);
    // this shouldnt be reached if bad_alloc exception is thrown
    ASSERT_EQ(false, true);
  }
  catch(bad_alloc exception) {
    // good we caught the exception
    ASSERT_EQ(true, true);
  }
}

// bad value for n
TEST(TestAllocator5, allocate4) {
  Allocator<int, 100> x;
  try {
    x.allocate(-1 * 2);
    // this shouldnt be reached if bad_alloc exception is thrown
    ASSERT_EQ(false, true);
  }
  catch(bad_alloc exception) {
    // good we caught the exception
    ASSERT_EQ(true, true);
  }
}

// --------------
// Deallocate
// --------------

TEST(TestAllocator6, deallocate) {
  Allocator<int, 100> x;
  int* p = x.allocate(1);
  
  ASSERT_EQ(x[0], -4);
  ASSERT_EQ(x[8], -4);
  ASSERT_EQ(x[12], 80);
  ASSERT_EQ(x[96], 80);

  x.deallocate(p, (size_t) 1);

  ASSERT_EQ(x[0], 92);
  ASSERT_EQ(x[96], 92);
}

// both adj blocks are also free after deallocation
TEST(TestAllocator6, deallocate1) {
  Allocator<int, 100> x;
  int* p = x.allocate(1);
  
  ASSERT_EQ(x[0], -4);
  ASSERT_EQ(x[8], -4);
  ASSERT_EQ(x[12], 80);
  ASSERT_EQ(x[96], 80);

  int* q = x.allocate(5);
  ASSERT_EQ(x[12], -20);
  ASSERT_EQ(x[36], -20);
  ASSERT_EQ(x[40], 52);
  ASSERT_EQ(x[96], 52);
  
  int* r = x.allocate(13);
  ASSERT_EQ(x[40], -52);
  ASSERT_EQ(x[96], -52);

  x.deallocate(p, (size_t) 1);
  ASSERT_EQ(x[0], 4);
  ASSERT_EQ(x[8], 4);
  ASSERT_EQ(x[12], -20);
  ASSERT_EQ(x[96], -52);

  x.deallocate(r, (size_t) 13);
  ASSERT_EQ(x[40], 52);
  ASSERT_EQ(x[96], 52);
  ASSERT_EQ(x[12], -20);
  ASSERT_EQ(x[36], -20);

  x.deallocate(q, (size_t) 5);
  ASSERT_EQ(x[0], 92);
  ASSERT_EQ(x[96], 92);
}

// only left adjacent block is also a free block after deallocation
TEST(TestAllocator6, deallocate2) {
  Allocator<int, 100> x;
  int* p = x.allocate(1);
  
  ASSERT_EQ(x[0], -4);
  ASSERT_EQ(x[8], -4);
  ASSERT_EQ(x[12], 80);
  ASSERT_EQ(x[96], 80);

  int* q = x.allocate(5);
  ASSERT_EQ(x[12], -20);
  ASSERT_EQ(x[36], -20);
  ASSERT_EQ(x[40], 52);
  ASSERT_EQ(x[96], 52);
  
  int* r = x.allocate(13);
  ASSERT_EQ(x[40], -52);
  ASSERT_EQ(x[96], -52);

  x.deallocate(p, (size_t) 1);
  ASSERT_EQ(x[0], 4);
  ASSERT_EQ(x[8], 4);

  x.deallocate(q, (size_t) 5);
  ASSERT_EQ(x[0], 32);
  ASSERT_EQ(x[36], 32);

  x.deallocate(r, (size_t) 13);
  ASSERT_EQ(x[0], 92);
  ASSERT_EQ(x[96], 92);
}

TEST(TestAllocator6, deallocate3) {
  Allocator<int, 100> x;
  int* p = x.allocate(1);
  
  ASSERT_EQ(x[0], -4);
  ASSERT_EQ(x[8], -4);
  ASSERT_EQ(x[12], 80);
  ASSERT_EQ(x[96], 80);

  int* q = x.allocate(5);
  ASSERT_EQ(x[12], -20);
  ASSERT_EQ(x[36], -20);
  ASSERT_EQ(x[40], 52);
  ASSERT_EQ(x[96], 52);
  
  int* r = x.allocate(13);
  ASSERT_EQ(x[40], -52);
  ASSERT_EQ(x[96], -52);

  x.deallocate(r, (size_t) 13);
  ASSERT_EQ(x[40], 52);
  ASSERT_EQ(x[96], 52);

  x.deallocate(q, (size_t) 5);
  ASSERT_EQ(x[12], 80);
  ASSERT_EQ(x[96], 80);

  x.deallocate(p, (size_t) 1);
  ASSERT_EQ(x[0], 92);
  ASSERT_EQ(x[96], 92);
}

// given bad pointer
TEST(TestAllocator6, deallocate4) {
  Allocator<int, 100> x;
  int* p = x.allocate(1);
  int* q = &*(p+1);
  ASSERT_EQ(x[0], -4);
  ASSERT_EQ(x[8], -4);
  ASSERT_EQ(x[12], 80);
  ASSERT_EQ(x[96], 80);
  
  try{
    x.deallocate(q, (size_t) 1);
    //this shouldnt be reached
    ASSERT_EQ(false, true);
  }
  catch (invalid_argument exception) {
    //good caught the exception
    ASSERT_EQ(true, true);
  }
}

