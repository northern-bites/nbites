#pragma once

#include "VisionDefs.hpp"
#include "VisionFrame.hpp"

#include "types/BBox.hpp"
#include "utils/Histogram.hpp"

/* Masks need to be or'd together. The result of an `or' operation is
 * an int, which does not type match an enum in c++
 */
typedef int hist_mask_t;
enum {
   hNone          = 0x00,
   hBall          = 0x01,
   hGoalBlue      = 0x02,
   hGoalYellow    = 0x04,
   hGoals         = hGoalBlue | hGoalYellow,
   hRobotBlue     = 0x02,
   hRobotRed      = 0x08,
   hFieldGreen    = 0x10,
   hWhite         = 0x20,
   hBlack         = 0x40,
   hBackground    = 0x80
};

/* 0x00yyuuvv weightings for edge calcultations */
enum edge_weights_t
{
   eNone          = 0x00000000,
   eGrey          = 0x00010000,
   eBall          = 0x00010100
};

/* Non templated wrapper for a FoveaT */
struct Fovea
{
   const BBox           bb;
   const int            density;
   const float          rotation;
   const bool           top;
   const hist_mask_t    hmask;
   const edge_weights_t edge_weights;;

   inline const Colour  &colour(int x, int y) const;
   inline const Colour  &colour(Point p)      const;

   inline const int     &grey  (int x, int y) const;
   inline const int     &grey  (Point p)      const;

   inline const Point   &edge  (int x, int y) const;
   inline const Point   &edge  (Point p)      const;

   const Histogram<int, cNUM_COLOURS> &xhistogram;
   const Histogram<int, cNUM_COLOURS> &yhistogram;

   /* Provided in the public interface for sequencial access */
   const Colour *const _colour;
   const int    *const _grey;
   const Point  *const _edge;

   /**
    * Convert image coord to fovea coord
    */
   inline Point mapImageToFovea(Point p) const;

   /**
    * Convert fovea coord to image coord
    */
   inline Point mapFoveaToImage(Point p) const;

   /* This constructor should not be called directly. See FoveaT asFovea() */
   Fovea(BBox bb, int density, float rotation, bool top,
         hist_mask_t hmask, edge_weights_t edge_weights,
         const Histogram<int, cNUM_COLOURS> &xhistogram,
         const Histogram<int, cNUM_COLOURS> &yhistogram,
         const Colour *colour, const int *grey, const Point *edge)
      : bb          (bb          ),
        density     (density     ),
        rotation    (rotation    ),
        top         (top         ),
        hmask       (hmask       ),
        edge_weights(edge_weights),
        xhistogram  (xhistogram  ),
        yhistogram  (yhistogram  ),
        _colour     (colour      ),
        _grey       (grey        ),
        _edge       (edge        )
   {}
};

/* Class representing a subsampled area of the image.
 * Tight loops are used for processing image regions,
 * and for speed reasons templating is used to allow
 * the optimizer to work its magic
 */
template <hist_mask_t hmask, edge_weights_t edge_weights>
class FoveaT
{
   public:
      const BBox bb;

      const int   density;
      const float rotation;
      const bool  top;

      FoveaT(const BBox &bb, int density, float rotation = 0, bool top = true);
      virtual ~FoveaT();

      void actuate(const VisionFrame &frame);

      /**
       * C-Space representation of the fovea
       */
      inline const Colour &colour(int x, int y) const;
      inline const Colour &colour(Point p)      const;

      /**
       * Blurred grey scale image. Values are 16 times normal intensity
       */
      inline const int    &grey  (int x, int y) const;
      inline const int    &grey  (Point p)      const;

      /**
       * Edge information stored as [dx, dy]. 
       */
      inline const Point  &edge  (int x, int y) const;
      inline const Point  &edge  (Point p)      const;

      /**
       * Convert image coord to fovea coord
       */
      inline Point mapImageToFovea(Point p) const;

      /**
       * Convert fovea coord to image coord
       */
      inline Point mapFoveaToImage(Point p) const;

      Colour *_colour;
      int    *_grey;
      Point  *_edge;

      /**
       * Histograms indicating how many pixels of each colour appear
       * in each row and column of the fovea
       */
      Histogram<int, cNUM_COLOURS> xhistogram;
      Histogram<int, cNUM_COLOURS> yhistogram;

      const Fovea &asFovea() const;

   private:

      const Fovea fovea;

      /* Private non const versions for calculating the grey scale image */
      inline int   &grey (int x, int y);
      inline Point &edge (int x, int y);

      void makeColour(const VisionFrame &frame); 
      void blurGrey  ();
      void makeEdge  ();

};

#include "Fovea.tcc"

