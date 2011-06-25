
#include "COMPreview.h"

COMPreview::COMPreview()
    : x(0.0l), y(0.0l), dx(0.0l), dy(0.0l)
{
    pthread_mutex_init(&mutex, NULL);
}

COMPreview::~COMPreview() {
    pthread_mutex_destroy(&mutex);
}

// the setter is thread safe, probably overkill...
void COMPreview::update(double _x, double _y, double _dx, double _dy) {
    pthread_mutex_lock(&mutex);
    x = _x;
    y = _y;
    dx = _dx;
    dy = _dy;
    pthread_mutex_unlock(&mutex);
}
