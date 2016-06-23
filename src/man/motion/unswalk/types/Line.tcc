template <typename T>
__Line_Generic<T>::__Line_Generic(Eigen::Matrix<T, 2, 1> p,
                                  Eigen::Matrix<T, 2, 1> v)
   : p(p), v(v)
{
}

template <typename T> 
bool __Line_Generic<T>::intersect(const __Line_Generic &l,
                                  Eigen::Matrix<T, 2, 1> &intersection)
const
{

   Eigen::Matrix2f A;
   Eigen::Vector2f b, x;

   A << v.x(), -l.v.x(),
        v.y(), -l.v.y();

   b << l.p.x() - p.x(),
        l.p.y() - p.y();

   if (! A.lu().solve(b, &x)) {
      return false;
   } else {
      intersection = (p + x[0] * v).cast<T>();
      return true;
   }
}

template <typename T>
T __Line_Generic<T>::distToPoint(Eigen::Matrix<T, 2, 1> p) const
{
   __Line_Generic<T> perp(p, Eigen::Matrix<T, 2, 1>(v.y(), -v.x()));
   Eigen::Matrix<T, 2, 1> intersection = Eigen::Matrix<T,2,1>(0,0);
   intersect(perp, intersection);
   return (p - intersection).cast<float>().norm();
}

template <typename T>
template <typename cast_T>
__Line_Generic<cast_T> __Line_Generic<T>::cast() const
{
   return __Line_Generic<cast_T>(p.cast<cast_T>(), v.cast<cast_T>);
}

template <typename T>
template<class Archive>
void __Line_Generic<T>::serialize(Archive &ar, const unsigned int version)
{
   ar & p & v;
}

