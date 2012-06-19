#ifndef VISUAL_OBSTACLE_H
#define VISUAL_OBSTACLE_H


class VisualObstacle {


 public:

  // Constructor
  VisualObstacle();

  
  // Initialization
  void init();
  
  // Setters
  void setLeft(bool _isLeft);
  void setRight(bool _isRight);
  void setOffField(bool _offField);
  void setPostLeft(bool _isPostLeft);
  void setPostRight(bool _isPostRight);
  // Getters
  bool onLeft() {return isLeft;}
  bool onRight() {return isRight;}
  bool postLeft() {return isPostLeft;}
  bool postRight() {return isPostRight;}
  bool offField() {return isOffField;}

 private:
  bool isLeft, isRight, isOffField;
  bool isPostLeft, isPostRight;




};

#endif
