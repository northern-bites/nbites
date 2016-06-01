template <class T, class G, class A>
bool RANSAC::Ransac<T, G, A>::operator () (
      const G                   &generator,
      const std::vector<PointI>  &points,
      std::vector<bool>        **cons,
      T                         &result,
      unsigned int               k,
      float                      e,
      unsigned int               n,
      std::vector<bool>          cons_buf[2],
      unsigned int              *seed
     )
{
   if (points.size() < n)
   {
      return false;
   }

   /* error of best line found so far */
   float minerr = std::numeric_limits<float>::max();

   std::vector<bool> *best_concensus, *this_concensus;
   best_concensus = &cons_buf[0];

   unsigned int i, j;
   for (i = 0; i < k; ++ i) {
      /* Generate a model */
      T model;
      if (! generator(model, points, seed)) {
         break;
      }

      /* Choose the currently unused concensus buffer */
      if (best_concensus == &cons_buf[0]) {
         this_concensus = &cons_buf[1];
      } else {
         this_concensus = &cons_buf[0];
      }

      unsigned int n_concensus_points = 0;
      A acceptor(model, e);

      for (j = 0; j < points.size(); ++ j) {
         const PointI &p = points[j];
         if (acceptor.accept(p)) {
            ++ n_concensus_points;
            (*this_concensus)[j] = true;
         } else {
            (*this_concensus)[j] = false;
         }
      }

      acceptor.finalise();

      model.var /= (n_concensus_points / 2);
      model.var -= (((float)n_concensus_points) / points.size());

      if (model.var < minerr && n_concensus_points >= n) {
         minerr = model.var;
         result = model;
         best_concensus = this_concensus;
      }
   }

   if (minerr < std::numeric_limits<float>::max()) {
      *cons = best_concensus;
      return true;
   } else {
      return false;
   }
}
