#include "VisualObstacle.h"
using namespace std;

namespace man {
namespace vision {

VisualObstacle::VisualObstacle()
{
  init();
}

void VisualObstacle::init() {

  isLeft = false;
  isRight = false;
  isOffField = false;
  isPostLeft = false;
  isPostRight = false;

}

void VisualObstacle::setLeft(bool _isLeft) {
  isLeft = _isLeft;
}

void VisualObstacle::setRight(bool _isRight) {
  isRight = _isRight;
}

void VisualObstacle::setPostLeft(bool _isPostLeft) {
  isPostLeft = _isPostLeft;
  if (isPostLeft == true) isLeft = true;
}

void VisualObstacle::setPostRight(bool _isPostRight) {
  isPostRight = _isPostRight;
  if (isPostRight == true) isRight = true;
}

void VisualObstacle::setOffField(bool _offField) {
  isOffField = _offField;
}

}
}
