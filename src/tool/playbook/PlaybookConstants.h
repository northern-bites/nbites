
// This acts as a constants file for all structs used
// in the Playbook module of the Tool.

#pragma once

namespace tool {
namespace playbook {

enum Role {
    DEFENDER = 0,
    OFFENDER,
    MIDDIE,
    GOALIE
};

static const Qt::GlobalColor roleColors [] = {Qt::blue, Qt::yellow,
                                              Qt::cyan, Qt::magenta};

struct PlaybookPosition
{
    short x;
    short y;
    short h;
    short role;
    PlaybookPosition(int x_, int y_, int h_, char r) :
        x(x_), y(y_), h(h_), role(r)
    {}
};

static const short defaultRoleList [] = {0,1,2,0,1,0};

} // namespace playbook
} // namespace tool
