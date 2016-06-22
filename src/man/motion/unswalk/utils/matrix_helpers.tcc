template <typename T>
boost::numeric::ublas::matrix<T> translateMatrix(T x, T y, T z) {
   boost::numeric::ublas::matrix<T> r(4, 4);
   r(0, 0) = 1;
   r(0, 1) = 0;
   r(0, 2) = 0;
   r(0, 3) = x;

   r(1, 0) = 0;
   r(1, 1) = 1;
   r(1, 2) = 0;
   r(1, 3) = y;

   r(2, 0) = 0;
   r(2, 1) = 0;
   r(2, 2) = 1;
   r(2, 3) = z;

   r(3, 0) = 0;
   r(3, 1) = 0;
   r(3, 2) = 0;
   r(3, 3) = 1;
   return r;
}


template <typename T>
boost::numeric::ublas::matrix<T> rotateZMatrix(T theta) {
   boost::numeric::ublas::matrix<T> r(4, 4);
   r(0, 0) = cos(theta);
   r(0, 1) = -sin(theta);
   r(0, 2) = 0;
   r(0, 3) = 0;

   r(1, 0) = sin(theta);
   r(1, 1) = cos(theta);
   r(1, 2) = 0;
   r(1, 3) = 0;

   r(2, 0) = 0;
   r(2, 1) = 0;
   r(2, 2) = 1;
   r(2, 3) = 0;

   r(3, 0) = 0;
   r(3, 1) = 0;
   r(3, 2) = 0;
   r(3, 3) = 1;
   return r;
}
template <typename T>
boost::numeric::ublas::matrix<T> createDHMatrix(T a, T alpha,
                                                T d, T theta) {
   boost::numeric::ublas::matrix<T> m(4, 4);
   m(0, 0) = cos(theta);
   m(0, 1) = -sin(theta);
   m(0, 2) = 0;
   m(0, 3) = a;

   m(1, 0) = sin(theta) * cos(alpha);
   m(1, 1) = cos(theta) * cos(alpha);
   m(1, 2) = -sin(alpha);
   m(1, 3) = -sin(alpha) * d;

   m(2, 0) = sin(theta) * sin(alpha);
   m(2, 1) = cos(theta) * sin(alpha);
   m(2, 2) = cos(alpha);
   m(2, 3) = cos(alpha) * d;

   m(3, 0) = 0;
   m(3, 1) = 0;
   m(3, 2) = 0;
   m(3, 3) = 1;
   return m;
}

/*
// inverse of 4*4 set formula
   typedef boost::numeric::ublas::permutation_matrix<std::size_t> pmatrix;
   // create a working copy of the input
   boost::numeric::ublas::matrix<T> A(input);

   // create a permutation matrix for the LU-factorization
   pmatrix pm(A.size1());

   // perform LU-factorization
   int res = boost::numeric::ublas::lu_factorize(A, pm);
   if (res != 0)
      return false;

   // create identity matrix of "inverse"
   inverse.assign(boost::numeric::ublas::identity_matrix<T> (A.size1()));

   // backsubstitute to get the inverse
   boost::numeric::ublas::lu_substitute(A, pm, inverse);

   return true;
}
*/

template <typename T>
bool invertMatrix(const boost::numeric::ublas::matrix<T>& input,
                  boost::numeric::ublas::matrix<T>& inv) {
   T det;
   T m[16];
   int i;

   m[0] = input(0, 0);
   m[1] = input(0, 1);
   m[2] = input(0, 2);
   m[3] = input(0, 3);

   m[4] = input(1, 0);
   m[5] = input(1, 1);
   m[6] = input(1, 2);
   m[7] = input(1, 3);

   m[8] = input(2, 0);
   m[9] = input(2, 1);
   m[10] = input(2, 2);
   m[11] = input(2, 3);

   m[12] = input(3, 0);
   m[13] = input(3, 1);
   m[14] = input(3, 2);
   m[15] = input(3, 3);

   inv(0, 0) =   m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15]
               + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
   inv(1, 0) =  -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15]
               - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
   inv(2, 0) =   m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15]
               + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
   inv(3, 0) = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14]
               - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
   inv(0, 1) =  -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15]
               - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
   inv(1, 1) =   m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15]
               + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
   inv(2, 1) =  -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15]
               - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
   inv(3, 1) =  m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14]
               + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
   inv(0, 2) =   m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15]
               + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
   inv(1, 2) =  -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15]
               - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
   inv(2, 2) =  m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15]
               + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
   inv(3, 2) = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14]
               - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
   inv(0, 3) =  -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11]
               - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
   inv(1, 3) =   m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11]
               + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
   inv(2, 3) = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11]
               - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
   inv(3, 3) =  m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10]
               + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

   det = m[0] * inv(0, 0) + m[1] * inv(1, 0) + m[2] * inv(2, 0) + m[3] * inv(3, 0);
   if (det == 0)
      return false;

   det = 1.0 / det;

   for (i = 0; i < 16; i++)
      inv(i / 4, i % 4) = inv(i / 4, i % 4) * det;

   return true;
}

template <typename T>
boost::numeric::ublas::matrix<T> projectionMatrix(T ex, T ey, T ez) {
   boost::numeric::ublas::matrix<T> projection(4, 4);
   projection(0, 0) = 1;
   projection(0, 1) = 0;
   projection(0, 2) = 0;
   projection(0, 3) = -ex;

   projection(1, 0) = 0;
   projection(1, 1) = 1;
   projection(1, 2) = 0;
   projection(1, 3) = -ey;

   projection(2, 0) = 0;
   projection(2, 1) = 0;
   projection(2, 2) = 1;
   projection(2, 3) = 0;

   projection(3, 0) = 0;
   projection(3, 1) = 0;
   projection(3, 2) = 1.0 / ez;
   projection(3, 3) = 0;
   return projection;
}
