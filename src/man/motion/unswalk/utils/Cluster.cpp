//#include <math.h>
//#include <stdlib.h>
#include "Cluster.hpp"


namespace Clustering{

 
 //
  // Dump a point
  //
  std::ostream& operator << (std::ostream& os, PointND& p){
    
    BOOST_FOREACH(PointND::value_type d, p){ os << d << " "; }    
    return os;
  }

  //
  // distance between two points
  //
  Distance distance(const PointND& x, const PointND& y)
  {
    Distance total = 0.0;
    Distance diff;
    
    PointND::const_iterator cpx=x.begin(); 
    PointND::const_iterator cpy=y.begin();
    PointND::const_iterator cpx_end=x.end();
    for(;cpx!=cpx_end;++cpx,++cpy){
      diff = *cpx - *cpy;
      total += (diff * diff); 
    }
    return total;  // no need to take sqrt, which is monotonic
  }

  //
  // Dump collection of Points
  //
  std::ostream& operator << (std::ostream& os, Points& cps){
    BOOST_FOREACH(Points::value_type p, cps){ os<<p <<std::endl;}
    return os;
  }

  //
  // Dump a Set of points
  //
  std::ostream& operator << (std::ostream& os, SetPoints & sp){
    
    BOOST_FOREACH(SetPoints::value_type pid, sp){     
      os << "pid=" << pid << " " ;
    }
    return os;
  };

  //
  // Dump ClustersToPoints
  //
  std::ostream& operator << (std::ostream& os, ClustersToPoints & cp){
    ClusterId cid = 0;
    BOOST_FOREACH(ClustersToPoints::value_type set, cp){
      os << "clusterid["  << cid << "]" << "=(" << set << ")" << std::endl; 
      cid++;
    }
    return os;
  }

  //
  // Dump ClustersToPoints
  //
  std::ostream& operator << (std::ostream& os, PointsToClusters & pc){
    PointId pid = 0;
    BOOST_FOREACH(PointsToClusters::value_type cid, pc){
      
      std::cout << "pid[" << pid << "]=" << cid << std::endl;      
      pid ++;
    }
    return os;
  }


  //
  // Init collection of points
  //  
  void PointsSpace::init_points() { 
    
    for (PointId i=0; i < num_points__; i++){
      PointND p;
			srand ( time(NULL) );/* initialize random seed: */
      for (Dimensions d=0 ; d < num_dimensions__; d++)
	      { p.push_back( rand() % 100 ); }     
      points__.push_back(p);
    }
  }  

  //
  // Zero centroids
  //
  void Clusters::zero_centroids() {
 
    BOOST_FOREACH(Centroids::value_type& centroid, centroids__){
      BOOST_FOREACH(PointND::value_type& d, centroid){
	      d = 0.0;
      }
    }
  }

  //
  // Compute Centroids
  //
  bool Clusters::compute_centroids() {

    Dimensions i;
    ClusterId cid = 0;
    PointId num_points_in_cluster;
    zero_centroids();

    // For each centroid
    BOOST_FOREACH(Centroids::value_type& centroid, centroids__){

      num_points_in_cluster = 0;

      // For earch PointId in this set
      BOOST_FOREACH(SetPoints::value_type pid, clusters_to_points__[cid]){
	
	      PointND p = ps__.getPoint(pid);
	      for (i=0; i<num_dimensions__; i++) centroid[i] += p[i];	
	      num_points_in_cluster++;
      }
 
      // If no point in cluster, how to calculate centroid, it would go to infinite (NaN)
      // It is possible for a cluster to "lose" all of its members during the initial
      //  phase of clustering; that's just how the K-Means algorithm works. The usual
      // reason why this happens is a bad choice of starting cluster centroids
      if(num_points_in_cluster == 0) return false;      
      for (i=0; i<num_dimensions__; i++) centroid[i] /= num_points_in_cluster;	
      cid++;
    }
    return true;  // centroids are valid if we got this far
  }

  //
  // Initial partition points randomly among available clusters
  //
  void Clusters::initial_partition_points(){

    ClusterId cid;
    
    for (PointId pid = 0; pid < ps__.getNumPoints(); pid++){
      
      cid = pid % num_clusters__;

      points_to_clusters__[pid] = cid;
      clusters_to_points__[cid].insert(pid);
    }

  };

  //
  // Partition points among available clusters using Subset Furthest First
  //
  void Clusters::SFF_partition_points(){
    
    // Subset size chosen as 2*k ln k
    ClusterId subset_size = (ClusterId) (log((float)num_clusters__)*2.0*num_clusters__);
    subset_size = std::min(subset_size, num_points__);

    // Sample a random subset of points without replacement
    Points subset;
  	srand ( time(NULL) );/* initialize random seed: */
    while(subset.size() < subset_size){
      PointND point = ps__.getPoint( rand() % ps__.getNumPoints());
      subset.push_back(point); // fails if it's already in the set
    }

    // Now initialise each centroid using furthest first
    PointId pid;
    ClusterId cid, to_cluster = 0;
    Distance d, min;    

    // Choose first element at random
    centroids__.clear();
    centroids__.push_back( subset.back() );
    subset.pop_back();

    while(centroids__.size() < num_clusters__){
      // choose the furthest as the next element
      Distance furthest = 0.0;
      unsigned int best = 0;    

      for(unsigned int k=0; k< subset.size(); k++){

          PointND p = subset.at(k);
          min = std::numeric_limits<float>::max( );
         
          BOOST_FOREACH(Centroids::value_type centroid, centroids__){
            
            d = Clustering::distance(centroid, p);
            if(d < min) min = d;
          }  

          if(min > furthest){
            furthest = min;
            best = k;
          }    
       }
       centroids__.push_back(subset.at(best));
       subset.erase(subset.begin()+best);
    }


    // Allocate each point to its closest cluster
    for (pid=0; pid<num_points__; pid++){

      min = std::numeric_limits<float>::max( );
    	cid = 0; 
      BOOST_FOREACH(Centroids::value_type c, centroids__){	  
        d = Clustering::distance(c, ps__.getPoint(pid));
        if (d < min){
          min = d;
          to_cluster = cid;
        }
        cid++;
    	}
      points_to_clusters__[pid] = to_cluster;
      clusters_to_points__[to_cluster].insert(pid);
    }      

  };

  //
  // k-means
  //
  void Clusters::k_means(void){
    
    bool move;
    bool some_point_is_moving = true;
    unsigned int num_iterations = 0;
    PointId pid;
    ClusterId cid, to_cluster =0;
    Distance d, min;
   
    SFF_partition_points();
    //initial_partition_points();

    // Until not converge
    while (some_point_is_moving){

      some_point_is_moving = false;

      while(!compute_centroids()){
        // compute centroids, but if one becomes empty then redo initial partition
        //std::cout << "redo partition, this is bad\n";
        SFF_partition_points();
      }

      //std::cout << "Centroids are: " << centroids__;

      // for each point
      //std::cout << "\nclusters to points before" << clusters_to_points__;
      //std::cout << "\npoints to cluster before" << points_to_clusters__;
      //std::cout << "\nMoving points: ";
      for (pid=0; pid<num_points__; pid++){
	
	      // distance from current cluster
	      min = Clustering::distance(centroids__[points_to_clusters__[pid]], ps__.getPoint(pid));

      	// foreach centroid
      	cid = 0; 
	      move = false;
	      BOOST_FOREACH(Centroids::value_type c, centroids__){	  
	        d = Clustering::distance(c, ps__.getPoint(pid));
	        if (d < min){
           //std::cout << pid << ", distance from " << points_to_clusters__[pid] <<
            // ": " << min << ", better dist: " << d << " to " << cid << ", ";
	          min = d;
	          move = true;
	          to_cluster = cid;

	          // remove from current cluster
	          clusters_to_points__[points_to_clusters__[pid]].erase(pid);
	          some_point_is_moving = true;
	        }
	        cid++;
      	}
	
	      // move towards a closer centroid 
	      if (move){ 
	        // insert
	        points_to_clusters__[pid] = to_cluster;
	        clusters_to_points__[to_cluster].insert(pid);
	      }
      }      
      //std::cout << "\nclusters to points after" << clusters_to_points__;
      //std::cout << "\npoints to cluster after" << points_to_clusters__;
      
      num_iterations++;
      //std::cout << "kmeans iterations: " << num_iterations;
    } 
  }

  
};
