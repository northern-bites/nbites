#include "gtest/gtest.h"

#include <iostream>

#include "../LineSystem.h"
#include "../LocStructs.h"

using namespace man::localization;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST (LocalizationTest, ConfirmPointOnLine) {
    Point start(0.f,0.f);
    Point end(4.f, 4.f);
    Line l(start, end);

    Point on (2.f, 2.f);
    Point off(1.f, 3.f);
    ASSERT_TRUE(l.containsPoint( on));
    ASSERT_FALSE(l.containsPoint(off));

    Point end_(0.f, 4.f);
    Line vert(start, end_);

    Point onV(0.f, 2.f);
    Point offV(0.f, 5.f);
    Point offV_(1.f, 2.f);

    ASSERT_TRUE(vert.containsPoint(onV));
    ASSERT_FALSE(vert.containsPoint(offV));
    ASSERT_FALSE(vert.containsPoint(offV_));
}

TEST (LocalizationTest, ConfirmClosestPointTo) {
    Point a(-2.f, 2.f);
    Point b( 2.f, 2.f);
    Line l(a,b);

    Point toA1(-3.f, 0.f);
    Point toA2(-3.f, 3.f);
    Point toA3(-2.f, 2.f);
    Point toB1(3.f, 0.f);
    Point toB2(3.f, 3.f);
    Point toB3(2.f, 2.f);
    Point to02(0.f, 0.f);
    Point to02_(0.f, 3.f);

    ASSERT_EQ(a.x, l.closestPointTo(toA1).x);
    ASSERT_EQ(a.y, l.closestPointTo(toA1).y);

    ASSERT_EQ(a.x, l.closestPointTo(toA2).x);
    ASSERT_EQ(a.y, l.closestPointTo(toA2).y);

    ASSERT_EQ(a.x, l.closestPointTo(toA3).x);
    ASSERT_EQ(a.y, l.closestPointTo(toA3).y);


    ASSERT_EQ(b.x, l.closestPointTo(toB1).x);
    ASSERT_EQ(b.y, l.closestPointTo(toB1).y);

    ASSERT_EQ(b.x, l.closestPointTo(toB2).x);
    ASSERT_EQ(b.y, l.closestPointTo(toB2).y);

    ASSERT_EQ(b.x, l.closestPointTo(toB3).x);
    ASSERT_EQ(b.y, l.closestPointTo(toB3).y);

    ASSERT_EQ(0.f, l.closestPointTo(to02 ).x);
    ASSERT_EQ(2.f, l.closestPointTo(to02 ).y);
    ASSERT_EQ(0.f, l.closestPointTo(to02_).x);
    ASSERT_EQ(2.f, l.closestPointTo(to02_).y);

    Line l_(b,a);

    ASSERT_EQ(a.x, l_.closestPointTo(toA1).x);
    ASSERT_EQ(a.y, l_.closestPointTo(toA1).y);

    ASSERT_EQ(a.x, l_.closestPointTo(toA2).x);
    ASSERT_EQ(a.y, l_.closestPointTo(toA2).y);

    ASSERT_EQ(a.x, l_.closestPointTo(toA3).x);
    ASSERT_EQ(a.y, l_.closestPointTo(toA3).y);

    ASSERT_EQ(b.x, l_.closestPointTo(toB1).x);
    ASSERT_EQ(b.y, l_.closestPointTo(toB1).y);

    ASSERT_EQ(b.x, l_.closestPointTo(toB2).x);
    ASSERT_EQ(b.y, l_.closestPointTo(toB2).y);

    ASSERT_EQ(b.x, l_.closestPointTo(toB3).x);
    ASSERT_EQ(b.y, l_.closestPointTo(toB3).y);

    ASSERT_EQ(0.f, l_.closestPointTo(to02 ).x);
    ASSERT_EQ(2.f, l_.closestPointTo(to02 ).y);
    ASSERT_EQ(0.f, l_.closestPointTo(to02_).x);
    ASSERT_EQ(2.f, l_.closestPointTo(to02_).y);

}

TEST(LocalizationTest, ShiftDownLine)
{
    Point a(-2.f,-2.f);
    Point b( 2.f, 2.f);

    Point origin(0.f, 0.f);

    Line l(a,b);

    float distToOrigin = std::sqrt(8.f);
    Point shifted = l.shiftDownLine(a, distToOrigin);

    bool worked = (abs(shifted.x - origin.x) < .01f);

    ASSERT_TRUE(abs(shifted.x -origin.x) < .01f);
    ASSERT_TRUE(abs(shifted.y -origin.y) < .01f);

    shifted = l.shiftDownLine(b, -distToOrigin);

    ASSERT_TRUE(abs(shifted.x -origin.x) < .01f);
    ASSERT_TRUE(abs(shifted.y -origin.y) < .01f);
}

TEST(LocalizationTest, PartialScoring)
{
    Point a(-2.f, 1.f);
    Point b( 2.f, 4.f);
    Line l(a,b); // line to project onto

    Point x(-1.5f, 0.f);
    Point y( 1.f, 0.f);
    Line ob(x,y);
    float obsvDist = 2.5f;

    Point initProj = l.closestPointTo(x);

    // Note: High precision error for close projections? Ask Bill...
    ASSERT_TRUE(abs(initProj.x - a.x) < .2f);
    ASSERT_TRUE(abs(initProj.y - a.y) < .2f);

    // Shift down line obsvDist and see if on line
    if(x.x < y.x)
        obsvDist = abs(obsvDist);
    else
        obsvDist = -abs(obsvDist);
    Point segmentMatchEnd = l.shiftDownLine(initProj, obsvDist);

    ASSERT_TRUE(abs(segmentMatchEnd.x - 0.f) < 0.2f);
    ASSERT_TRUE(abs(segmentMatchEnd.y - 2.5f) < 0.2f);
}

TEST(LocalizationTest, TestErrorCalc)
{
    // Try a square
    Point a(0.f, 0.f);
    Point b(0.f, 4.f);
    Point c(4.f, 4.f);
    Point d(4.f, 0.f);

    Line top(b,c);
    Line bot(a,d);

    float four = top.getError(bot);

    ASSERT_TRUE(abs(four - 4.f) < .2f);

    // Same line
    ASSERT_TRUE(abs(top.getError(top)) < .2f);

    // Change line defs
    Line top_(c,b);
    Line bot_(a,d);

    four = top_.getError(bot);
    ASSERT_TRUE(abs(four - 4.f) < .0001f);

    four = top.getError(bot_);
    ASSERT_TRUE(abs(four - 4.f) < .0001f);

    four = top_.getError(bot_);
    ASSERT_TRUE(abs(four - 4.f) < .0001f);

    four = bot_.getError(top);
    ASSERT_TRUE(abs(four - 4.f) < .0001f);

    four = bot.getError(top_);
    ASSERT_TRUE(abs(four - 4.f) < .0001f);

    four = bot_.getError(top_);
    ASSERT_TRUE(abs(four - 4.f) < .0001f);
}

TEST(LocalizationTest, TestLineSystem)
{
    LineSystem testSystem;

    // Give a line segment that matches a line
    Point bottomLeftCorner(450.f,451.f);
    Point slightToRight(450.f, 451.f);
    Line perfectObsv(bottomLeftCorner, slightToRight);

    std::cout << "OK score:\t" << testSystem.scoreObservation(perfectObsv) << std::endl;
}
