template <hist_mask_t hmask, edge_weights_t edge_weights>
FoveaT<hmask, edge_weights>::FoveaT(
      const BBox &bb,
      int density,
      float rotation,
      bool top)
   : bb(bb), density(density), rotation(rotation), top(top),

     _colour(new Colour[bb.width() * bb.height()]),
     _grey  (edge_weights ? new int  [bb.width() * bb.height()] : NULL),
     _edge  (edge_weights ? new Point[bb.width() * bb.height()] : NULL),

     xhistogram(bb.width()), yhistogram(bb.height()),
     fovea(bb, density, rotation, top, hmask, edge_weights,
           xhistogram, yhistogram, _colour, _grey, _edge)

{
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
FoveaT<hmask, edge_weights>::~FoveaT()
{
   delete[] _colour;
   delete[] _grey;
   delete[] _edge;
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
void FoveaT<hmask, edge_weights>::actuate(const VisionFrame &frame)
{
   makeColour(frame);
   if (edge_weights) {
      blurGrey();
      makeEdge();
   }
}

// j is the number of columns
// i is the number of rows
template <hist_mask_t hmask, edge_weights_t edge_weights>
void FoveaT<hmask, edge_weights>::makeColour (const VisionFrame &frame)
{
   typedef int (*histogram_ptr_t)[cNUM_COLOURS];

   const uint16_t COLS = (top) ? TOP_IMAGE_COLS : BOT_IMAGE_COLS;

   /* Image coords of saliency bounding box */
   const BBox BB(bb.a * density, bb.b * density);

   /* Set up some pointers for pointer arithmetic use */
   const uint8_t *currentFramePixel;
   if (top) {
      currentFramePixel = frame.topImage + 2 *(BB.a.y()*COLS + BB.a.x());
   } else {
      currentFramePixel = frame.botImage + 2 *(BB.a.y()*COLS + BB.a.x());
   }

   Colour *saliencyPixel = _colour;
   int    *greyPixel     = _grey;

   int u, v;
   const int *stop;
   if (top) {
      stop = frame.cameraToRR.topEndScanCoords + bb.a.x() * density;
   } else { 
      stop = frame.cameraToRR.botEndScanCoords + bb.a.x() * density;
   }
   const Colour *const saliencyEnd = _colour + bb.height() * bb.width();


   /* Only use the histograms if hmask is set */
   if (hmask) {
      memset(xhistogram._counts, 0, sizeof(*xhistogram._counts) * bb.width());
      memset(yhistogram._counts, 0, sizeof(*yhistogram._counts) * bb.height());
   }

   histogram_ptr_t xHistogram = &xhistogram._counts[0];

   do {
      const int bodyRow = std::min(bb.height(),
                                   std::max(*stop / density - bb.a.y(), 0));

      const Colour *const saliencyRowBodyStart = saliencyPixel + bodyRow;
      const Colour *const saliencyRowEnd       = saliencyPixel + bb.height();

      histogram_ptr_t yHistogram = &yhistogram._counts[0];

      while (saliencyPixel < saliencyRowBodyStart) {
         /* Copy the value to the grey image */
         if (edge_weights) {
            *greyPixel = 0;
         }
         if (edge_weights & 0xFF0000) {
            *greyPixel += ((edge_weights >> 16) & 0xFF) * (*currentFramePixel);
         }
         if (edge_weights & 0x00FF00) {
            u = *(currentFramePixel + ((size_t)currentFramePixel & 2 ? 1 : 3));
            *greyPixel += ((edge_weights >>  8) & 0xFF) * u;
         }
         if (edge_weights & 0x0000FF) {
            v = *(currentFramePixel + ((size_t)currentFramePixel & 2 ? 3 : 1));
            *greyPixel += ((edge_weights >>  0) & 0xFF) * v;
         }

         /* if the saliency density is even, then optimise and always use
          * fastest classification function
          */
         Colour c;
         if (density % 2) {
            if (top) {
               c = frame.topNnmc.classify(currentFramePixel);
            } else {
               c = frame.botNnmc.classify(currentFramePixel);
            }  
         } else {
            if (top) {
               c = frame.topNnmc.classify_UYV(currentFramePixel);
            } else {
               c = frame.botNnmc.classify_UYV(currentFramePixel);
            }
         }
         *saliencyPixel = c;

         /* Update the histograms. Inactive histograms will be optimised out */
         if (hmask & hBall) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hGoalBlue) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hGoalYellow) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hRobotBlue) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hRobotRed) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hFieldGreen) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hWhite) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hBlack) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }
         if (hmask & hBackground) {
            ++ (*xHistogram)[c];
            ++ (*yHistogram)[c];
         }

         /* increment for the next pixel down */
         ++ saliencyPixel;
         ++ greyPixel;
         ++ yHistogram;

         currentFramePixel += density * COLS * 2;
      }
      while (saliencyPixel < saliencyRowEnd) {
         *saliencyPixel = cBODY_PART;
         ++ saliencyPixel;

         if (edge_weights) {
            *greyPixel = 0;
            ++ greyPixel;
         }
      }

      // increment for the next pixel to the right
      currentFramePixel -= bodyRow * density * COLS * 2;
      currentFramePixel += density * 2;

      ++ xHistogram;

      stop += density;
   } while (saliencyPixel < saliencyEnd);
}


template <hist_mask_t hmask, edge_weights_t edge_weights>
void FoveaT<hmask, edge_weights>::blurGrey()
{
   int grey_tmp[bb.width()][bb.height()];

   int x, y;

   for (y = 0; y < bb.height(); ++ y) {
      x = 0;
      grey_tmp[x][y] = 3 * grey(x+0, y) +
                       1 * grey(x+1, y);

      x = bb.width() - 1;
      grey_tmp[x][y] = 3 * grey(x-0, y) + 
                       1 * grey(x-1, y);

   }

   for (y = 0; y < bb.height(); ++ y) {
      for (x = 1; x < bb.width() - 1; ++ x) {
         grey_tmp[x][y] = 1 * grey(x-0, y) +
                          2 * grey(x+0, y) +
                          1 * grey(x+1, y);

      }
   }

   for (x = 0; x < bb.width(); ++ x) {
      y = 0;
      grey(x, y) = 3 * grey_tmp[x][y] + grey_tmp[x][y+1];

      y = bb.height() - 1;
      grey(x, y) = 3 * grey_tmp[x][y] + grey_tmp[x][y-1];
   }

   for (y = 1; y < bb.height() - 1; ++ y) {
      for (x = 0; x < bb.width(); ++ x) {
         grey(x, y) = 1 * grey_tmp[x][y-1] +
                      2 * grey_tmp[x][y+0] +
                      1 * grey_tmp[x][y+1];
      }
   }
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
void FoveaT<hmask, edge_weights>::makeEdge ()
{
   int x, y;
   int a, b;
   int dx, dy;

   for (y = 0; y < bb.height() - 1; ++ y) {
      for (x = 0; x < bb.width() - 1; ++ x) {
         a  = grey(x  , y  );
         a -= grey(x+1, y+1);

         b  = grey(x  , y+1);
         b -= grey(x+1, y  );

         dx  = (724 * (a + b)) / 1024;
         dy  = (724 * (a - b)) / 1024;

         edge(x, y)[0] = dx;
         edge(x, y)[1] = dy;
      }
      edge(x, y)[0] = 0;
      edge(x, y)[1] = 0;
   }

   for (x = 0; x < bb.width(); ++ x) {
      edge(x, y)[0] = 0;
      edge(x, y)[1] = 0;
   }
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
const Fovea &FoveaT<hmask, edge_weights>::asFovea() const
{
   return fovea;
}

/* FoveaT versions */
template <hist_mask_t hmask, edge_weights_t edge_weights>
const Colour &FoveaT<hmask, edge_weights>::colour(int x, int y) const
{
   return _colour[x * bb.height() + y];
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
const int &FoveaT<hmask, edge_weights>::grey(int x, int y) const
{
   return _grey[x * bb.height() + y];
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
const Point &FoveaT<hmask, edge_weights>::edge(int x, int y) const
{
   return _edge[x * bb.height() + y];
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
const Colour &FoveaT<hmask, edge_weights>::colour(Point p) const
{
   return colour(p.x(), p.y());
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
const int &FoveaT<hmask, edge_weights>::grey(Point p) const
{
   return grey(p.x(), p.y());
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
const Point &FoveaT<hmask, edge_weights>::edge(Point p) const
{
   return edge(p.x(), p.y());
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
Point FoveaT<hmask, edge_weights>::mapImageToFovea(Point p) const
{
   if (!top) p.y() -= TOP_IMAGE_ROWS;
   return (p / density) - bb.a;
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
Point FoveaT<hmask, edge_weights>::mapFoveaToImage(Point p) const
{
   if (!top) {
      Point temp = (p + bb.a) * density;
      temp.y() += TOP_IMAGE_ROWS;
      return temp;
   }
   return (p + bb.a) * density;
}

/* Private non const versions */
template <hist_mask_t hmask, edge_weights_t edge_weights>
int &FoveaT<hmask, edge_weights>::grey(int x, int y)
{
   return _grey[x * bb.height() + y];
}

template <hist_mask_t hmask, edge_weights_t edge_weights>
Point &FoveaT<hmask, edge_weights>::edge(int x, int y)
{
   return _edge[x * bb.height() + y];
}

/* Fovea versions */
const Colour &Fovea::colour(int x, int y) const
{
   return _colour[x * bb.height() + y];
}

const int &Fovea::grey(int x, int y) const
{
   return _grey[x * bb.height() + y];
}

const Point &Fovea::edge(int x, int y) const
{
   return _edge[x * bb.height() + y];
}

const Colour &Fovea::colour(Point p) const
{
   return colour(p.x(), p.y());
}

const int &Fovea::grey(Point p) const
{
   return grey(p.x(), p.y());
}

const Point &Fovea::edge(Point p) const
{
   return edge(p.x(), p.y());
}

Point Fovea::mapImageToFovea(Point p) const
{
   if (!top) p.y() -= TOP_IMAGE_ROWS;
   return (p / density) - bb.a;
}

Point Fovea::mapFoveaToImage(Point p) const
{
   if (!top) {
      Point temp = (p + bb.a) * density;
      temp.y() += TOP_IMAGE_ROWS;
      return temp;
   }
   return (p + bb.a) * density;
}

