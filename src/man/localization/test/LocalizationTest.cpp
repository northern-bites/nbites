#include "gtest/gtest.h"

#include <iostream>
#include <cmath>

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

TEST(LocalizationTest, ProjectionVert)
{
    Point a(1.f,-1.f);
    Point b(1.f, 4.f);

    Line l(a,b);

    Point s(0.f,-1.f);

    Point proj = l.closestPointTo(s);

    ASSERT_EQ( 1.f, proj.x);
    ASSERT_EQ(-1.f, proj.y);

    Point t(0.f, 0.f);
    proj = l.closestPointTo(t);

    ASSERT_TRUE( std::fabs(0.f - proj.y) < .001f);
}

TEST(LocalizationTest, ShiftDownLine)
{
    Point a(-2.f,-2.f);
    Point b( 2.f, 2.f);

    Point origin(0.f, 0.f);

    Line l(a,b);

    float distToOrigin = std::sqrt(8.f);
    Point shifted = l.shiftDownLine(a, distToOrigin);

    bool worked = (std::fabs(shifted.x - origin.x) < .01f);

    ASSERT_TRUE(std::fabs(shifted.x -origin.x) < .01f);
    ASSERT_TRUE(std::fabs(shifted.y -origin.y) < .01f);

    shifted = l.shiftDownLine(b, -distToOrigin);

    ASSERT_TRUE(std::fabs(shifted.x -origin.x) < .01f);
    ASSERT_TRUE(std::fabs(shifted.y -origin.y) < .01f);
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
    ASSERT_TRUE(std::fabs(initProj.x - a.x) < .2f);
    ASSERT_TRUE(std::fabs(initProj.y - a.y) < .2f);

    // Shift down line obsvDist and see if on line
    if(x.x < y.x)
        obsvDist = std::fabs(obsvDist);
    else
        obsvDist = -std::fabs(obsvDist);
    Point segmentMatchEnd = l.shiftDownLine(initProj, obsvDist);

    ASSERT_TRUE(std::fabs(segmentMatchEnd.x - 0.f) < 0.2f);
    ASSERT_TRUE(std::fabs(segmentMatchEnd.y - 2.5f) < 0.2f);
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

    // float four = top.getError(bot).error;

    // ASSERT_EQ(std::fabs(four - 4.f), 0.f);

    // // Same line
    // ASSERT_TRUE(std::fabs(top.getError(top).error) < .2f);

    // Change line defs
    Line top_(c,b);
    Line bot_(a,d);

    // four = top_.getError(bot).error;
    // ASSERT_TRUE(std::fabs(four - 4.f) < .0001f);

    // four = top.getError(bot_).error;
    // ASSERT_TRUE(std::fabs(four - 4.f) < .0001f);

    // four = top_.getError(bot_).error;
    // ASSERT_TRUE(std::fabs(four - 4.f) < .0001f);

    // four = bot_.getError(top).error;
    // ASSERT_TRUE(std::fabs(four - 4.f) < .0001f);

    // four = bot.getError(top_).error;
    // ASSERT_TRUE(std::fabs(four - 4.f) < .0001f);

    // four = bot_.getError(top_).error;
    // ASSERT_TRUE(std::fabs(four - 4.f) < .0001f);


    //Try intersecting lines
    d = Point(6.f, -2.f);

    Line negSlope(b,d);
    Line posSlope(a,c);

    ASSERT_EQ(negSlope.getError(posSlope).error, 2.f);
}

TEST(LocalizationTest, TestLineSystem)
{
    LineSystem testSystem;

    // Give a line segment that matches a line
    Point bottomLeftCorner(700.f,300.f);
    Point slightToRight(705.f, 400.f);
    Line perfectObsv(bottomLeftCorner, slightToRight);
}

TEST(LocalizationTest, IntersectionTest)
{

    // TEST PARALLEL
    Point a(-2.f, 2.f);
    Point b( 2.f, 2.f);
    Point c(-2.f, 0.f);
    Point d( 2.f, 0.f);

    Point noInt(-1000000.f, -1000000.f);

    Line ab(a,b);
    Line ba(b,a);
    Line cd(c,d);
    Line dc(d,c);

    ASSERT_EQ(ab.intersect(cd).x, noInt.x);
    ASSERT_EQ(ab.intersect(cd).y, noInt.y);
    ASSERT_EQ(ab.intersect(dc).x, noInt.x);
    ASSERT_EQ(ab.intersect(dc).y, noInt.y);
    ASSERT_EQ(ba.intersect(cd).x, noInt.x);
    ASSERT_EQ(ba.intersect(cd).y, noInt.y);
    ASSERT_EQ(ba.intersect(dc).x, noInt.x);
    ASSERT_EQ(ba.intersect(dc).y, noInt.y);

    // TEST VERTICAL
    a = Point(-2.f, 0.f);
    b = Point( 2.f, 0.f);

    ab = Line(a,b);
    ba = Line(b,a);
    cd = Line(c,d);
    dc = Line(d,c);

    Point origin(0.f, 0.f);
    ASSERT_TRUE((ab.intersect(cd).x - origin.x) < .0001f);
    ASSERT_TRUE((ab.intersect(cd).y - origin.y) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).x - origin.x) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).y - origin.y) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).x - origin.x) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).y - origin.y) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).x - origin.x) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).y - origin.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).x - origin.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).y - origin.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).x - origin.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).y - origin.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).x - origin.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).y - origin.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).x - origin.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).y - origin.y) < .0001f);

    // NO VERTICAL INTERSECTION
    a = Point(-2.f,-2.f);
    b = Point( 2.f, 2.f);

    ab = Line(a,b);
    ba = Line(b,a);
    cd = Line(c,d);
    dc = Line(d,c);

    ASSERT_TRUE((ab.intersect(cd).x - origin.x) < .0001f);
    ASSERT_TRUE((ab.intersect(cd).y - origin.y) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).x - origin.x) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).y - origin.y) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).x - origin.x) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).y - origin.y) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).x - origin.x) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).y - origin.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).x - origin.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).y - origin.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).x - origin.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).y - origin.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).x - origin.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).y - origin.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).x - origin.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).y - origin.y) < .0001f);


    // SHIFTED
    a.x += 2.f;
    b.x += 2.f;
    c.x += 2.f;
    d.x += 2.f;

    ab = Line(a,b);
    ba = Line(b,a);
    cd = Line(c,d);
    dc = Line(d,c);

    Point newInt(origin.x+2.f, origin.y);

    ASSERT_TRUE((ab.intersect(cd).x - newInt.x) < .0001f);
    ASSERT_TRUE((ab.intersect(cd).y - newInt.y) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).x - newInt.x) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).y - newInt.y) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).x - newInt.x) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).y - newInt.y) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).x - newInt.x) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).y - newInt.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).x - newInt.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).y - newInt.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).x - newInt.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).y - newInt.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).x - newInt.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).y - newInt.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).x - newInt.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).y - newInt.y) < .0001f);


    // LINES INTERSECT, SEGMENTS DONT
    d.x = 1.f;
    cd = Line(c,d);
    dc = Line(d,c);

    // intersect should reject by giving noInt
    ASSERT_TRUE((ab.intersect(cd).x - noInt.x) < .0001f);
    ASSERT_TRUE((ab.intersect(cd).y - noInt.y) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).x - noInt.x) < .0001f);
    ASSERT_TRUE((ab.intersect(dc).y - noInt.y) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).x - noInt.x) < .0001f);
    ASSERT_TRUE((ba.intersect(cd).y - noInt.y) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).x - noInt.x) < .0001f);
    ASSERT_TRUE((ba.intersect(dc).y - noInt.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).x - noInt.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ab).y - noInt.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).x - noInt.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ab).y - noInt.y) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).x - noInt.x) < .0001f);
    ASSERT_TRUE((cd.intersect(ba).y - noInt.y) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).x - noInt.x) < .0001f);
    ASSERT_TRUE((dc.intersect(ba).y - noInt.y) < .0001f);

}
