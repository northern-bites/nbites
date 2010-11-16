#ifndef _ColorParams_h_DEFINED
#define _ColorParams_h_DEFINED

struct ColorParams
{                           // words:      3         2         1         0
long long int  yZero;              // | y0 << 2 | y0 << 2 | y0 << 2 | y0 << 2 |
long long int  ySlope;             // |    0    |   yk    |    0    |   yk    |
long long int  yLimit;             // |  yn-1   |  yn-1   |  yn-1   |  yn-1   |

long long int  uvZero;             // | v0 << 1 | u0 << 1 | v0 << 1 | u0 << 1 |
long long int  uvSlope;            // |   vk    |   uk    |   vk    |   uk    |
long long int  uvLimit;            // |  vn-1   |  un-1   |  vn-1   |  un-1   |
long long int  uvDim;              // | un * yn |   yn    | un * yn |   yn    |
};

#endif /* _ColorParams_h_DEFINED */
