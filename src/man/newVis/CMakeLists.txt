########################### Project Name
PROJECT( NEW_VISION )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )


########################### Sources
SET( rle_SRCS   RLE/Encoder )
SET( mod_SRCS   VisionModule )

SET( newVis_SRCS newVision
                 Blackboard
		 ${rle_SRCS}
		 ${mod_SRCS}
		 )

########################### Libraries
ADD_LIBRARY( newVis ${newVis_SRCS})

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  newVis
  portals
  common
  messages
  )