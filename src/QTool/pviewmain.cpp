#include <QtGui>
#include <stdint.h>

#include "mainwindow.h"
#include "TreeModel.h"
#include "ProtoNode.h"
#include "/home/oneamtu/nbites/build/man/straight/memory/protos/Sensors.pb.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
//    MainWindow w;
//    w.show();

    memory::proto::PMotionSensors* ms =
            new memory::proto::PMotionSensors();
    ms->set_timestamp(42);
    ms->add_body_angles(23.2f);
    ms->add_body_angles(23.2f);
    ms->add_body_angles(23.2f);
    memory::proto::PSensors::PFSR* pfs = ms->add_fsrs();
    pfs->set_front_left(23.2f);
    ProtoView::ProtoNode* root = new ProtoView::ProtoNode(NULL, NULL, ms);
    ProtoView::TreeModel messageModel(root);


    QTreeView view;
    view.setModel(&messageModel);
    view.setWindowTitle(QObject::tr("Proto Viewer"));
    view.show();
    return app.exec();
}
