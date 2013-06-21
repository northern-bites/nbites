/**
 * The Playbook Tool class!
 */

#pragma once

#include <QMainWindow>

#include "playbook/PlaybookCreator.h"

namespace tool {

class PlaybookTool : public QMainWindow {
    Q_OBJECT;

public:
    PlaybookTool(const char* title = "PlaybookTool");
    ~PlaybookTool();

protected:
    playbook::PlaybookCreator playbookCreator;

    QRect* geometry;
};

}
