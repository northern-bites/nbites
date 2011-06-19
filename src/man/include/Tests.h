#include <iostream>
#include <assert.h>
#include <stdio.h>

#define CLOSE_ENOUGH_EQ_FLOAT .0000001

#define EQ_FLOAT(x,y) (assert(x - CLOSE_ENOUGH_EQ_FLOAT < y \
                       && x + CLOSE_ENOUGH_EQ_FLOAT > y))
#define EQ_INT(x,y) assert(x == y)
// #define EQ_INT(x,y) if (x !=y ){ printf("%d != %d\n\t", x, y); fflush(stdout); assert(false);}

#define NE_INT(x,y) assert(x != y)
#define NE(x,y) assert(x != y)

#define LT(x,y) assert(x < y)
#define LTE(x,y) assert(x <= y)
#define GT(x,y) assert(x > y)
#define GTE(x,y) assert(x >= y)

#define TRUE(x) assert( (x) )
#define FALSE(x) assert( !(x) )

#define PASSED(x) std::cout << "Passed test:\n\t" << #x << std::endl;


