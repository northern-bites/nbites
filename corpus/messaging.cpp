
// This file is part of Man, a robotic perception, locomotion, and
// team strategy application created by the Northern Bites RoboCup
// team of Bowdoin College in Brunswick, Maine, for the Aldebaran
// Nao robot.
//
// Man is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Man is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser Public License for more details.
//
// You should have received a copy of the GNU General Public License
// and the GNU Lesser Public License along with Man.  If not, see
// <http://www.gnu.org/licenses/>.

#include <assert.h>

#include "messaging.h"

template <class T>
void MessagePost<T>::post(T &copy)
{
    assert(lock());
    data = copy;
    assert(release());
}

template <class T>
T MessagePost<T>::retrieve()
{
    T copy;
    assert(lock());
    copy = data;
    assert(release());
    return copy;
}

template <class T>
void MessageList<T>::append(T &copy)
{
    assert(lock());
    data.push_back(copy);
    assert(release());
}

template <class T>
T MessageList<T>::retrieve()
{
    T copy;
    assert(lock());
    copy = data.pop_front();
    assert(release());
    return copy;
}
