/*
 * ObservedPostsHistory.hpp
 *
 *  Created on: 10/06/2014
 *      Author: osushkov
 */

#pragma once

#include "PostType.hpp"
#include <list>

/**
 * Backstory:
 * One of the key processes of the multi-modal Kalman filter is merging similar modes (modes with
 * a similar mean vector and covariance matrices). However, 
 * we observed that if we often merge modes that in their recent history thought they observed
 * different goalposts for the same observation. This comes about because all goalposts look the
 * same. So when we see a single goalpost, for example, we create 4 modes to cover the 4 different
 * goalposts that could correspond to this observation. We noticed that our state estimation drifts
 * considerable and breaks down when we merge modes that observed 'different' posts in the last
 * few frames. There is probably a bug somewhere in our merge code, but we didnt end up getting
 * to the bottom of it
 * 
 * This class is a "hack" around this problem. We basically keep a history of the goalposts observed
 * by a given mode over the last few frames, and we do not allow merging two modes that have a
 * different goalposts observed history. This seemed to largely solve our merging issues. But this
 * is nonetheless a hack and time should be spent investigating the root cause of the merging problem.
 */
class ObservedPostsHistory {
public:
   ObservedPostsHistory(unsigned minFrameGapPostObservation);

   /**
    * Insert into the history that for this frame the current mode observed this type of post.
    */
   void addObservedPost(const PostType &postType);
   
   /**
    * Insert into the history that this frame this mode did not observe any goalposts.
    */
   void addNoObservedPosts(void);

   /**
    * Check if it is possible for the current mode to observe the given post type, given its
    * history of observed posts. For example, if in the last frame the current mode observed its
    * own goalpost, its impossible for it to observe the opponent's post the very next frame, due to
    * the max speed with which it can turn its head the needed amount).
    */
   bool canObservePostType(const PostType &postType) const;

   bool isSimilarTo(const ObservedPostsHistory &other) const;
   ObservedPostsHistory& mergedWith(const ObservedPostsHistory &other);

   ObservedPostsHistory createSymmetricHistory(void) const;
   void printHistory() const;

private:
   enum ObservedPostSide {
      OWN_POST_LEFT,
      OWN_POST_RIGHT,
      OPPONENT_POST_LEFT,
      OPPONENT_POST_RIGHT,
      NONE,
   };

   unsigned minFrameGapPostObservation;
   std::list<ObservedPostSide> observedPosts;
};
