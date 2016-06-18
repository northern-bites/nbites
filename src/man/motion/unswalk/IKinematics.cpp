#include "IKinematics.hpp"

#include <cmath>

#include <Eigen/Eigen>

using namespace Eigen;

static float crop(float angle)
{
   while (angle <= -M_PI) angle += 2*M_PI;
   while (angle >   M_PI) angle -= 2*M_PI;

   return angle;
}

namespace IKinematics
{

/**
 * (Very) luckily for us, the nao's leg joint chain can be separated out
 * into reasonably simple independent chains. For simplicity, we assume that
 * the chain link from the ankle to the ground in negligible.
 *
 * Define the following:
 *   x:      axis extending forwards from the robot's torso (sagittal plane)
 *   y:      axis extending rightwards of the robot's torso (coronal plane )
 *   z:      axis extending upwards of the robot's torso
 *   rot:    rotation of the foot relative to the x axis, 0 is forward
 *
 * When rot is 0, the x and y chains are easily separable as only the
 * thigh roll can affects the y axis. Therefore
 *
 *    thighRoll = atan2(z, y)
 *
 * Two joints affect the x axis, namely thighPitch and kneePitch. Note
 * however that the chain has been rotated however, and the new target z'
 * is now the hypotenuse of the triangle created by target z y.
 *
 *    z' = cos(thighPitch + kneePitch)thighL + cos(kneePitch)shinL
 *    x  = sin(thighPitch + kneePitch)thighL + sin(kneePitch)shinL
 *
 * It's pretty obvious that there is a periodic solution to the above
 * equations. However, it is a hell of a lot easier just to use the
 * law of cosines to get the angles.
 *
 *  |\        -     |  We know `thigh', `shin' and the third length of
 *  |A\ thigh |     |  the triangle is simply hypot(z', x). Villa, solve
 *   | \      | z'  |  for B and you're done.
 *   | B\     |     |
 *    |C/     |     |  Note that thigh pitch is not the same as A. In
 *    |/ shin -     |  actual fact, it is a lot easier to calculate
 *                  |  thigh pitch by simply substituting the value of
 *  |-|             |  knee pitch into the above equations.
 *    x             |
 *
 * Relating the above logic back to the original equation, we note that
 * kneePos = pi + B. A rather nice result is that B is always positive
 * assuming we take the positive arc-cosine. We do not want to consider
 * the negative case because this would imply the knee is bending backwards.
 *
 * For rotation, note that the only way to rotate the foot in the lateral
 * plane is to alter the hip pitch (the joint is diagonal). Rotating the
 * chain causes the x and y chains to now depend on each other, as the pitch
 * and roll of each joint now affect both x and y. Easy solution, transform
 * the target using the rotation matrix of the hip. Now the x and y axes are
 * independent again.
 *
 * Now to deal with the negligible foot joint, it turns out its actually pretty
 * easy to add to the chain.
 *
 *    let a = thighPitch
 *        b = kneePitch
 *        c = anklePitch
 *
 *    0  = a + b + c
 *    z' = cos(a + b + c)footL + cos(a + b)shinL + cos(a)thighL
 *    x  = sin(a + b + c)footL + sin(a + b)shinL + sin(a)thighL
 *
 * cos(a + b + c) is a constant, so we can simply remove it both sides of
 * the equation and solve using the same triangle as above. Too easy. In
 * the same manner, we can add the ankle roll to the chain to reach our
 * y-target.
 */

NaoLegChain NaoSolve(const NaoFootTarget &target, float left)
{
   const float thighL = 0.10000;
   const float shinL  = 0.10290;
   const float footL  = 0.04519;

   NaoLegChain r;
   float x, y, z;

   /* For now assume hip yaw is exactly equal to foot yaw.
    * Fuck my life http://en.wikipedia.org/wiki/Universal_joint
    * Well, its actually reasonably linear:
    * http://www.wolframalpha.com/input/?i=plot+atan(tan(x)/cos(pi/4)))
    *
    * This is an expensive operation so dont do it unless we actually need to.
    */
   r.hip = target.yaw;
   if (target.yaw == 0) {
      x = target.x;
      y = target.y;
      z = target.z;
   } else {
      Vector3f v(target.x, target.y, target.z);
      AngleAxis<float> aa(-target.yaw, Vector3f(0, sqrtf(2)/2, sqrtf(2)/2));
      v = aa * v;
      x = v.x(); y = v.y(); z = v.z();
   }

   /* Subtract foot length from target. This can be seen as the vector
    *    target' = target - Rot_roll * Rot_pitch * foot
    *
    * The fact that pitch is applied before roll is due to the robot's
    * physical design.
    */ 

   x -= footL * sinf(target.pitch);
   y -= footL * cosf(target.pitch) * sinf(target.roll);
   z += footL * cosf(target.pitch) * cosf(target.roll);

   r.thighRoll = atan2f(y, -z);

   /* Solve for target.x and target.z */
   const float z_prime = z / cosf(r.thighRoll);
   const float h = hypotf(z_prime, x);

   /* A B and C are as above comment */
   const float A = acosf((shinL*shinL - h*h - thighL*thighL) / (-2*thighL*h));
   const float B = acosf((h*h - thighL*thighL - shinL*shinL) / (-2*thighL*shinL));
   r.thighPitch = A + asin(x/h) + 3*M_PI_2;
   r.kneePitch  = B + M_PI;

   /* Calculate the ankle pitch and roll */
   r.anklePitch = target.pitch - r.kneePitch - r.thighPitch;
   r.ankleRoll  = target.roll - r.thighRoll;

   /* Currently all angles are wound counter clockwise from the normal.
    * Joints on the actual robot each have their own normals. Basically,
    * when all joint values are set to zero the robot is standing upright.
    */
   r.thighPitch = 3*M_PI_2 - r.thighPitch;
   r.kneePitch  =          - r.kneePitch;
   r.anklePitch = 1*M_PI_2 - r.anklePitch;

   /* Finally make sure all angles are bound by (-pi, pi] */
   r.hip        = crop(r.hip       );
   r.thighRoll  = crop(r.thighRoll );
   r.thighPitch = crop(r.thighPitch);
   r.kneePitch  = crop(r.kneePitch );
   r.ankleRoll  = crop(r.ankleRoll );
   r.anklePitch = crop(r.anklePitch);

   return r;
}

std::ostream &operator <<(std::ostream &out, const NaoLegChain &chain)
{
   out << "chain.hip        = " << chain.hip        << std::endl;
   out << "chain.thighRoll  = " << chain.thighRoll  << std::endl;
   out << "chain.thighPitch = " << chain.thighPitch << std::endl;
   out << "chain.kneePitch  = " << chain.kneePitch  << std::endl;
   out << "chain.ankleRoll  = " << chain.ankleRoll  << std::endl;
   out << "chain.anklePitch = " << chain.anklePitch << std::endl;

   return out;
}


std::ostream &operator <<(std::ostream &out, const NaoFootTarget &target)
{
   out << "x     =" << target.x     << std::endl;
   out << "y     =" << target.y     << std::endl;
   out << "z     =" << target.z     << std::endl;
   out << "pitch =" << target.pitch << std::endl;
   out << "roll  =" << target.roll  << std::endl;
   out << "yaw   =" << target.yaw   << std::endl;

   return out;
}

} /* namespace IKinematics */

