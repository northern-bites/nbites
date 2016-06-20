/*
 * ObservedPostsHistory.cpp
 *
 *  Created on: 11/06/2014
 *      Author: osushkov
 */

#include "ObservedPostsHistory.hpp"
#include <iostream>


ObservedPostsHistory::ObservedPostsHistory(unsigned minFrameGapPostObservation):
      minFrameGapPostObservation(minFrameGapPostObservation) {}


void ObservedPostsHistory::addObservedPost(const PostType &postType) {
   if (postType == MY_LEFT) {
      observedPosts.push_front(OWN_POST_LEFT);
   } else if (postType == MY_RIGHT) {
      observedPosts.push_front(OWN_POST_RIGHT);
   } else if (postType == OPPONENT_LEFT) {
      observedPosts.push_front(OPPONENT_POST_LEFT);
   } else if (postType == OPPONENT_RIGHT) {
      observedPosts.push_front(OPPONENT_POST_RIGHT);
   } else {
      observedPosts.push_front(NONE);
   }
   
   while (observedPosts.size() > minFrameGapPostObservation) {
      observedPosts.pop_back();
   }
}


void ObservedPostsHistory::addNoObservedPosts(void) {
   observedPosts.push_front(NONE);
   while (observedPosts.size() > minFrameGapPostObservation) {
      observedPosts.pop_back();
   }
}


bool ObservedPostsHistory::canObservePostType(const PostType &postType) const {
   ObservedPostSide disallowedObservationLeft;
   ObservedPostSide disallowedObservationRight;
   if (postType == MY_LEFT || postType == MY_RIGHT) {
      disallowedObservationLeft  = OPPONENT_POST_LEFT;
      disallowedObservationRight = OPPONENT_POST_RIGHT;
   } else {
      disallowedObservationLeft  = OWN_POST_LEFT;
      disallowedObservationRight = OWN_POST_RIGHT;
   }

   std::list<ObservedPostSide>::const_iterator it = observedPosts.begin();
   while (it != observedPosts.end()) {
      if (*it == disallowedObservationLeft ||
          *it == disallowedObservationRight) {
         return false;
      }
      ++it;
   }
   
   return true;
}


bool ObservedPostsHistory::isSimilarTo(const ObservedPostsHistory &other) const {
   ObservedPostSide mySide = NONE;
   std::list<ObservedPostSide>::const_iterator it = observedPosts.begin();
   while (it != observedPosts.end()) {
      if (*it != NONE) {
         mySide = *it;
      }
      ++it;
   }
   
   ObservedPostSide otherSide = NONE;
   it = other.observedPosts.begin();
   while (it != other.observedPosts.end()) {
      if (*it != NONE) {
         otherSide = *it;
      }
      ++it;
   }
   
   // Check both sides are home (or not home)
   bool myHome = (mySide    == OWN_POST_LEFT || mySide    == OWN_POST_RIGHT);
   bool opHome = (otherSide == OWN_POST_LEFT || otherSide == OWN_POST_RIGHT);
   if (myHome != opHome) return false;

   // Check the last N posts match
   int n = 4;
   int i = 0;
   it = other.observedPosts.begin();
   std::list<ObservedPostSide>::const_iterator jt = observedPosts.begin();
   while (it != other.observedPosts.end() &&
          jt != observedPosts.end()       &&
          i < n) {
      if (*it != *jt) return false;
      ++i;
      ++it;
      ++jt;
   }

   return true;
}


ObservedPostsHistory& ObservedPostsHistory::mergedWith(const ObservedPostsHistory &other) {
   std::list<ObservedPostSide>::iterator it = observedPosts.begin();
   std::list<ObservedPostSide>::const_iterator other_it = other.observedPosts.begin();
   
   while (it != observedPosts.end() && other_it != other.observedPosts.end()) {
      if (*it == NONE) {
         *it = *other_it;
      }
      
      ++it;
      ++other_it;
   }
   
   return *this;
}

ObservedPostsHistory ObservedPostsHistory::createSymmetricHistory(void) const {
   ObservedPostsHistory newHistory(minFrameGapPostObservation);
   newHistory.observedPosts = observedPosts;
   
   std::list<ObservedPostSide>::iterator it = newHistory.observedPosts.begin();
   while (it != newHistory.observedPosts.end()) {
      if (*it == OWN_POST_LEFT) {
         *it = OPPONENT_POST_LEFT;
      } else if (*it == OWN_POST_RIGHT) {
         *it = OPPONENT_POST_RIGHT;
      } else if (*it == OPPONENT_POST_LEFT) {
         *it = OWN_POST_LEFT;
      } else if (*it == OPPONENT_POST_RIGHT) {
         *it = OWN_POST_RIGHT;
      } else {
         *it = NONE;
      }
      ++it;
   }
   
   return newHistory;
}

void ObservedPostsHistory::printHistory() const {
   std::list<ObservedPostSide>::const_iterator it = observedPosts.begin();
   std::cout << "history = ";
   while (it != observedPosts.end()) {
      std::cout << *it << " ";
      ++it;
   }
   std::cout << std::endl;
}
