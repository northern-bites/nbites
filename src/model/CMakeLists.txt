####### subproject model

project( model )
cmake_minimum_required(VERSION 2.8.0)

####### sources

set( model_srcs VisionCorner.cpp
  World.cpp
  Image.cpp
)

add_library(model
  STATIC
  ${model_srcs} )
