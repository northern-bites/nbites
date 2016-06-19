
#pragma once

#include <set>
#include <vector>
#include <iostream>

#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>

  


namespace Clustering{

  typedef float Coord;            // a coordinate
  typedef float Distance;         // distance
  typedef unsigned int Dimensions; // how many dimensions
  typedef unsigned int PointId;    // the id of this point
  typedef unsigned int ClusterId;  // the id of this cluster

  typedef std::vector<Coord> PointND;    // a point (a centroid)
  typedef std::vector<PointND> Points;   // collection of points

  typedef std::set<PointId> SetPoints; // set of points
  
  // ClusterId -> (PointId, PointId, PointId, .... )
  typedef std::vector<SetPoints> ClustersToPoints;
  // PointId -> ClusterId
  typedef std::vector<ClusterId> PointsToClusters; 
  // coll of centroids
  typedef std::vector<PointND> Centroids;

  //
  // Dump a point
  //
  std::ostream& operator << (std::ostream& os, PointND& p);

  //
  // distance between two points
  //
  Distance distance(const PointND & x, const PointND & y);

  //
  // Dump collection of Points
  //
  std::ostream& operator << (std::ostream& os, Points& cps);

  //
  // Dump a Set of points
  //
  std::ostream& operator << (std::ostream& os, SetPoints & sp);
    
  //
  // Dump centroids
  //
  std::ostream& operator << (std::ostream& os, Centroids & cp);
  
  //
  // Dump ClustersToPoints
  //
  std::ostream& operator << (std::ostream& os, ClustersToPoints & cp);

  //
  // Dump ClustersToPoints
  //
  std::ostream& operator << (std::ostream& os, PointsToClusters & pc);


  //
  // This class stores all the points available in the model
  //
  class PointsSpace{

    //
    // Dump collection of points
    //
    friend std::ostream& operator << (std::ostream& os, PointsSpace & ps){
      PointId i = 0;
      BOOST_FOREACH(Points::value_type p, ps.points__){     
	      os << "point["<<i++<<"]=" << p << std::endl;
      }
      return os;
    };

  public:

    PointsSpace(PointId num_points, Dimensions num_dimensions) 
      : num_points__(num_points), num_dimensions__(num_dimensions)
    {init_points();};

    PointsSpace(Points points){
      num_points__ = points.size();
      if(num_points__ > 0) num_dimensions__ = points[0].size();
      points__ = points;
    }

    inline const PointId getNumPoints() const {return num_points__;}
    inline const PointId getNumDimensions() const {return num_dimensions__;}
    inline const PointND& getPoint(PointId pid) const { return points__[pid];}
 
  private:
    //
    // Init collection of points
    //
    void init_points();
    
    PointId num_points__;
    Dimensions num_dimensions__;
    Points points__;

  };

  // 
  //  This class represents a cluster
  // 
  class Clusters {

  private:
   
    ClusterId num_clusters__;    // number of clusters
    PointsSpace& ps__;           // the point space
    Dimensions num_dimensions__; // the dimensions of vectors
    PointId num_points__;        // total number of points
    ClustersToPoints clusters_to_points__;
    PointsToClusters points_to_clusters__;
    Centroids centroids__;

    //
    // Zero centroids
    //
    void zero_centroids();     

    //
    // Compute centroids, returns true if successful, false if one or more clusters has no points in it
    // (and therefore the centroid is invalid)
    //
    bool compute_centroids();      
    //
    // Initial partition points randomly among available clusters
    //
    void initial_partition_points();

    //
    // Initial partition points among available clusters using Subset Furthest First
    //
    void SFF_partition_points();

  public:

    Centroids getCentroids(){
      return centroids__;
    }
   
    //
    // Dump ClustersToPoints
    //
    friend std::ostream& operator << (std::ostream& os, Clusters & cl){
      /*
      ClusterId cid = 0;
      BOOST_FOREACH(ClustersToPoints::value_type set, cl.clusters_to_points__){
	      os << "Cluster["<<cid<<"]=(";
	      BOOST_FOREACH(SetPoints::value_type pid, set){
	        Point p = cl.ps__.getPoint(pid);
	        os << "(" << p << ")";
	      }
	      os << ")" ;
	      cid++;
      }
      */
      os << cl.centroids__ << std::endl;
      return os;
    }
    

    Clusters(ClusterId num_clusters, PointsSpace & ps) : 
      num_clusters__(num_clusters), ps__(ps), 
	    num_dimensions__(ps.getNumDimensions()),
	    num_points__(ps.getNumPoints()),
	    points_to_clusters__(num_points__, 0) {

      ClusterId i = 0;
      Dimensions dim;
      for ( ; i < num_clusters; i++){
	      PointND point;   // each centroid is a point
	      for (dim=0; dim<num_dimensions__; dim++) point.push_back(0.0);
	      SetPoints set_of_points;

	      // init centroids
	      centroids__.push_back(point);  

	      // init clusterId -> set of points
	      clusters_to_points__.push_back(set_of_points);
	      // init point <- cluster
      }
    };
    
    //
    // k-means
    //
    void k_means (void);
  };

};

