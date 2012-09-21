/*
 * A panel with lots of spin boxes to control the placement of objects
 * in the world.
 */

#pragma once

#include <QSpinBox>

class Controls : public QWidget {
Q_OBJECT

public:
    Controls(QWidget* parent = 0);
    ~Controls() {};

signals:
    void robotMoved(float x, float y, float h);
    void headMoved(float yaw, float pitch);
    void ballMoved(float x, float y);

private slots:
    void sendRobotInfo();
    void sendHeadInfo();
    void sendBallInfo();

private:
    QSpinBox robotX;
    QSpinBox robotY;
    QSpinBox robotH;
    QSpinBox headYaw;
    QSpinBox headPitch;
    QSpinBox ballX;
    QSpinBox ballY;
};

