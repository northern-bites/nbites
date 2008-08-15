/* Pose.cc

   -through a bunch of matrix transformations:
        -derives the camera focal point's xyz coordinate from the
	center of the body
	-derives the height of the center body (origin) using back leg info
	-derives a pose-estimated horizon line on the screen

   -at the vision system's request Pose.cc will:
        -calculate 'estimate' from the center
	of the body to a single pixel in the camera.
	-recalculate an 'estimate' from the focal point of the camera
	to an object to an 'estimate' of measurements from the center
	of the body to the object

   index:
   - 'estimate' is a struct consisting of distance, bearing, and elevation
   - 'point <int>' is a struct consisting of an x and a y int
   - 'point3 <float>' is a struct consisting of x,y,z integers
 */

#include <stdio.h>

#include "Pose.h"
#include "debug.h"
#include "VisionDef.h"

// define abs for either ints or floats
#undef abs
#define abs(x) ((x) < 0 ? -(x) : (x))

//#define DEBUG_MDH 1

// class constructur
Pose::Pose()
{
    //initialize the main matrices
    camera_to_world = identity4D();

    //define the edges of the screen

    cameraEdgePoints.push_back(point4D(FOCAL_LENGTH_MM,
                                       IMAGE_MM_WIDTH/2,
                                       IMAGE_MM_HEIGHT/2));
    cameraEdgePoints.push_back(point4D(FOCAL_LENGTH_MM,
                                       IMAGE_MM_WIDTH/2,
                                       -IMAGE_MM_HEIGHT/2));
    cameraEdgePoints.push_back(point4D(FOCAL_LENGTH_MM,
                                       -IMAGE_MM_WIDTH/2,
                                       IMAGE_MM_HEIGHT/2));
    cameraEdgePoints.push_back(point4D(FOCAL_LENGTH_MM,
                                       -IMAGE_MM_WIDTH/2,
                                       -IMAGE_MM_HEIGHT/2));
}
Pose::~Pose()
{
    fmfree(camera_to_world);
}


Matrix * Pose::origin4D()
{
    Matrix * origin= fmatrix(4,1);
    float originContents[] = {0,0,0,1.0};
    memcpy(origin->data,originContents, 4*sizeof(float));
    return origin;
}

Matrix * Pose::identity4D()
{
    return fmidentity(4);
}

Matrix * Pose::rotation4D(const int axis,const float angle)
{
    Matrix * rot = identity4D();
    float sinAngle = sin(angle);
    float cosAngle = cos(angle);
    if(angle == 0.0){ //OPTIMIZAION POINT
        return rot;
    }
    switch(axis){
    case X_AXIS:
        fmset(rot,Y_AXIS,Y_AXIS, cosAngle);
        fmset(rot,Y_AXIS,Z_AXIS,-sinAngle);
        fmset(rot,Z_AXIS,Y_AXIS, sinAngle);
        fmset(rot,Z_AXIS,Z_AXIS, cosAngle);
        break;
    case Y_AXIS:
        fmset(rot,X_AXIS,X_AXIS, cosAngle);
        fmset(rot,X_AXIS,Z_AXIS, sinAngle);
        fmset(rot,Z_AXIS,X_AXIS,-sinAngle);
        fmset(rot,Z_AXIS,Z_AXIS, cosAngle);
        break;
    case Z_AXIS:
        fmset(rot,X_AXIS,X_AXIS, cosAngle);
        fmset(rot,X_AXIS,Y_AXIS,-sinAngle);
        fmset(rot,Y_AXIS,X_AXIS, sinAngle);
        fmset(rot,Y_AXIS,Y_AXIS, cosAngle);
        break;
    }
    return rot;
}
Matrix * Pose::translation4D(float dx, float dy, float dz)
{
    Matrix * trans = identity4D();
    fmset(trans,X_AXIS,W_AXIS,dx);
    fmset(trans,Y_AXIS,W_AXIS,dy);
    fmset(trans,Z_AXIS,W_AXIS,dz);
    return trans;
}

Matrix * Pose::point3D(float x, float y, float z)
{
    Matrix * p = fmatrix(3,1);
    float pContents[] = {x,y,z};
    memcpy(p->data,pContents, 3*sizeof(float));
    return p;
}

Matrix * Pose::point4D(float x, float y, float z, float w )
{
    Matrix * p = fmatrix(4,1);
    float pContents[] = {x,y,z,w};
    memcpy(p->data,pContents, 4*sizeof(float));
    return p;
}


/*
 * Calculates the translation matrix M for a chain using
 * Modified Denavit Hartenberg  parameters approach.
 * See the Nao Kinematics files for details or the wiki.
 */
Matrix * Pose::calcChainTransforms(const int id,
                                   const vector<float> *chainAngles)
{
    Matrix * fT = identity4D(); //fullTransformation

    //base transforms
    vector<Matrix *> * baseTrans = baseTransforms.at(id);
    vector<Matrix *>::iterator base = baseTrans->begin();
    for(; base != baseTrans->end(); base++){
        Matrix * bT = *base;
        fmmuleq(fT,bT);
    }
#ifdef DEBUG_MDH
    printf("base:\n %s \n",fmtostring(fT));
#endif

    //mDH parameters
    vector<const mDHParam*> * chainMDHParams = mDHParams.at(id);
    vector<const mDHParam*>::iterator mDH = chainMDHParams->begin();
    for(int i = 0 ; mDH != chainMDHParams->end(); mDH++){

        const mDHParam * p = *mDH;
        const float angle = chainAngles->at(i++);

        Matrix * xRot = rotation4D(X_AXIS,p->xRot);
        Matrix * xTrans = translation4D(p->xTrans,0.0f,0.0f);
        Matrix * zRot = rotation4D(Z_AXIS,p->zRot+angle);
        Matrix * zTrans = translation4D(0.0f,0.0f,p->zTrans);

        fmmuleq(fT,xRot); fmfree(xRot);
        fmmuleq(fT,xTrans); fmfree(xTrans);
        fmmuleq(fT,zRot); fmfree(zRot);
        fmmuleq(fT,zTrans); fmfree(zTrans);

#ifdef DEBUG_MDH
        printf(" %g, \t %g, \t %g, \t %g \n",
               p->xRot,p->xTrans,p->zRot+angle,p->zTrans);
        printf("mDH %d :\n %s \n",(i - 1),fmtostring(fT));
#endif
    }
#ifdef DEBUG_MDH
    printf("mDH :\n %s \n",fmtostring(fT));
#endif

    //end transforms:
    vector<Matrix *> * endTrans = endTransforms.at(id);
    vector<Matrix *>::iterator end = endTrans->begin();
    for(; end != (endTrans->end()); end++){
        Matrix * eT = *end;
        fmmuleq(fT,eT);
    }
#ifdef DEBUG_MDH
    printf("end: \n%s\n",fmtostring(fT));
#endif

    return fT;
}
// transformation of camera focal point to body center
// ORDER MATTERS EXTREMELY.  READ AustinVilla04.pdf for more.
point3 <float> Pose::calcFocalPointInBodyFrame()
{
    /*
      for (int chain = 0; chain < NUM_CHAINS; chain++ ){
      //get mdh params for this chain

      }
    */


    //OLD STUFF:
    // clears matrix values each time method is called (once per frame)
    image_matrix.clear();

    image_matrix.rotateY(body_roll_angle); // rotate body roll around y axis
    image_matrix.rotateX(body_tilt_angle); // rotate body tilt around x axis
    //translate based on offsets between neck joint base and center body (0,0,0)
    image_matrix.translate(0,NECK_BASE_TO_CENTER_Y,NECK_BASE_TO_CENTER_Z);
    image_matrix.rotateX(neck_angle); // rotate neck angle around x axis
    image_matrix.rotateZ(pan_angle); // rotate pan angle around z axis
    // translate about neck's actual length on z-axis
    image_matrix.translate(0, 0, NECK_LENGTH);
    image_matrix.rotateX(yaw_angle); // rotate yaw angle around x axis
    // translate about yaw joint to camera focal point offsets in y, z axis
    image_matrix.translate(0,CAMERA_YAW_Y_LENGTH,-CAMERA_YAW_Z_LENGTH);

    // stores focal point's x,y,z point in the body frame into
    // focal_point_body_frame point3 <float>
    point3 <float> focal_point (image_matrix.getX(), image_matrix.getY(),
                                image_matrix.getZ());
    return focal_point;
}

// transforms left and right leg joints to find xyz points of each leg's toe.
// then calculates body center using the taller of the two.
float Pose::calcBodyCenterHeight()
{
    // clears the left and right leg matrices each time method is called
    // (once every frame)
    left_leg_matrix.clear();
    right_leg_matrix.clear();

    // TRANSFORM TO FIND XYZ POINT OF LEFT REAR LEG'S TOE
    // rotate body roll angle about y axis
    left_leg_matrix.rotateY(body_roll_angle);
    // rotate body tilt angle about x axis
    left_leg_matrix.rotateX(body_tilt_angle);
    // translates along x axis and y axis offsets
    left_leg_matrix.translate(-REAR_LEG_TO_CENTER_BODY_X,
                              REAR_LEG_TO_CENTER_BODY_Y,0);
    // rotate shoulder angle along x
    left_leg_matrix.rotateX(left_rear_shoulder_angle);
    // rotate hip angle along y axis
    left_leg_matrix.rotateY(left_rear_hip_angle);
    // translate from shoulder top to elbow
    left_leg_matrix.translate(ELBOW_TO_ELBOW_X,-ELBOW_TO_ELBOW_Y,
                              -SHOULDER_TO_ELBOW_LENGTH);
    // rotate knee angle along x axis
    left_leg_matrix.rotateX(left_rear_knee_angle);
    // translate from elbow to toe
    left_leg_matrix.translate(0,0,-REAR_LEG_LENGTH);

    // TRANSFORM TO FIND XYZ POINT OF RIGHT REAR LEG'S TOE
    // same as left_leg above, just -1* the x offsets
    right_leg_matrix.rotateY(body_roll_angle);
    right_leg_matrix.rotateX(body_tilt_angle);
    right_leg_matrix.translate(REAR_LEG_TO_CENTER_BODY_X,
                               REAR_LEG_TO_CENTER_BODY_Y,0);
    right_leg_matrix.rotateX(right_rear_shoulder_angle);
    right_leg_matrix.rotateY(right_rear_hip_angle);
    right_leg_matrix.translate(-ELBOW_TO_ELBOW_X,-ELBOW_TO_ELBOW_Y,
                               -SHOULDER_TO_ELBOW_LENGTH);
    right_leg_matrix.rotateX(right_rear_knee_angle);
    right_leg_matrix.translate(0,0,-REAR_LEG_LENGTH);

    // compares the height of either leg and chooses the higher value
    // looks at the z value of the left and right leg transformation matrices
    if (abs(left_leg_matrix.getZ()) > abs(right_leg_matrix.getZ())) {
        //cout << "left leg is higher: " << -1*left_leg_matrix.getZ() << endl;
        return abs(left_leg_matrix.getZ());
    }
    else {
        //cout << "right leg is higher: " << -1*right_leg_matrix.getZ() << endl;
        return abs(right_leg_matrix.getZ());
    }
    return 0.0;
}


// Calculates a horizon line for real image via the camera matrix
void Pose::calcImageHorizonLine(Matrix * camera_to_world)
{
    //moving the camera frame to the center of the body
    //lets us compare the rotation of the camera frame
    //relative to the world frame
    Matrix * camera_to_horizon = fmatrix3(camera_to_world);
    fmset(camera_to_horizon,X_AXIS,W_AXIS,0.0f);
    fmset(camera_to_horizon,Y_AXIS,W_AXIS,0.0f);
    fmset(camera_to_horizon,Z_AXIS,W_AXIS,0.0f);

    //optimization note: it is likely that the inverse in this case
    //is just the transpose ..  hmmm?
    Matrix * horizon_to_camera = fminvert(camera_to_horizon);

    vector<Matrix *> left_edge, right_edge; //need to be freed

    left_edge.push_back(fmmul(camera_to_horizon,cameraEdgePoints[0]));
    left_edge.push_back(fmmul(camera_to_horizon,cameraEdgePoints[1]));

    right_edge.push_back(fmmul(camera_to_horizon,cameraEdgePoints[2]));
    right_edge.push_back(fmmul(camera_to_horizon,cameraEdgePoints[3]));

    // These are intersection points in the horizon plane.
    Matrix * intersection_left = intersectLineWithXYPlane(&left_edge);
    Matrix * intersection_right = intersectLineWithXYPlane(&right_edge);

    // Now they are in the camera frame. Result still stored in intersection1,2
    fmmuleq2(horizon_to_camera, intersection_left);
    fmmuleq2(horizon_to_camera, intersection_right);

    //we are only interested in the height (z axis), not the width
    //width_mm = intersect.get(1,0)
    //width_pix = -width_mm*MM_TO_PIX_X + IMAGE_WIDTH/2
    const float height_mm_left = *fmget(intersection_left,2,0);
    const float height_mm_right = *fmget(intersection_right,2,0);

    const float height_pix_left = -height_mm_left*MM_TO_PIX_Y + IMAGE_HEIGHT/2;
    const float height_pix_right = -height_mm_right*MM_TO_PIX_Y +
        IMAGE_HEIGHT/2;

    fmfree(camera_to_horizon);  fmfree(horizon_to_camera);
    fmfree(left_edge[0]);       fmfree(left_edge[1]);
    fmfree(right_edge[0]);      fmfree(right_edge[1]);
    fmfree(intersection_left);  fmfree(intersection_right);

    horizon_left_2d.x = 0;
    horizon_left_2d.y = static_cast<int>(height_pix_left);
    horizon_right_2d.x = IMAGE_WIDTH - 1;
    horizon_right_2d.y = static_cast<int>(height_pix_right);

    /*

    // calculates horizon left/right z's which are the z coordinates
    // in body frame space if you project a 2d plane in the x,y axises
    // at the focal point.

    // subtract focal point z value from transformed point3 <float>'s z value.
    float horizon_left_z = focal_point_body_frame.z-
    image_matrix.transform(HORIZON_LEFT_3D).z;

    // do same for right side
    float horizon_right_z = focal_point_body_frame.z
    -image_matrix.transform(HORIZON_RIGHT_3D).z;

    // convert horizon_left_z and horizon_right_z to (x,y) coordinates in the
    // image plane.  converts the z mm value to a y pixel value.
    horizon_left_2d.x = 0;
    horizon_left_2d.y = (int)(-((horizon_left_z/MM_TO_PIX_Y_PLANE)
                                -IMAGE_CENTER_Y));
    horizon_right_2d.x = IMAGE_WIDTH-1;
    horizon_right_2d.y = (int)(-((horizon_right_z/MM_TO_PIX_Y_PLANE)
                                 -IMAGE_CENTER_Y));

    */

    // calculate slope of horizon + perpen
    horizon_slope = ((float)(horizon_right_2d.y - horizon_left_2d.y) /
                     (float)(IMAGE_WIDTH-1));
    if (horizon_slope != 0) {
        perpen_slope = -1/horizon_slope;
    }
    else {
        // need to include INFINITY macro here
    }
}

Matrix * Pose::intersectLineWithXYPlane(vector<Matrix *> * aLine)
{
    Matrix *l1 = aLine->at(0);
    Matrix *l2 = aLine->at(1);

    //points on the plane level with the ground in the horizon coord frame
    //normally need 3 points, but since one is the origin, it can get ignored
    Matrix * horizonUnitX = point4D(1,0,0);
    Matrix * horizonUnitY = point4D(0,1,0);

    //we now solve the point of intersection using linear algebra
    //Ax=b, where b is the target, x is the solution of weights (t,u,v)
    //to solve l1 + (l2 -l1)t = o1*u + o2*v
    //Note!: usually a plane is defined by three vectors, but in this
    //case, the horizon plane goes through the origin of the horizon
    //frame, so one of the vectors is the zero vector, so we ignore it
    //See http://en.wikipedia.org/wiki/Line-plane_intersection for detail
    float contents[] =
        {*fmget(l1,0,0) - *fmget(l2,0,0),    *fmget(horizonUnitX,0,0),
         *fmget(horizonUnitY,0,0),

         *fmget(l1,1,0) - *fmget(l2,1,0),    *fmget(horizonUnitX,1,0),
         *fmget(horizonUnitY,1,0),

         *fmget(l1,2,0) - *fmget(l2,2,0),    *fmget(horizonUnitX,2,0),
         *fmget(horizonUnitY,2,0)
        };
    Matrix *eqSystem = fmatrix(3,3);
    memcpy(eqSystem->data,contents,3*3*sizeof(float));

    // Solve for the solution of the weights.
    // Now usually we would fmsolve(eqSystem,l1), but l1 is defined in
    // homogeneous coordiantes. We need it to be a 3 by 1 vector to solve the
    // system of equations.
    Matrix *target = fmatrix4(l1,0,3,0,1);
    Matrix *result = fmsolve(eqSystem,target);
    float t = *fmget(result,0,0);
    fmfree(result);  fmfree(eqSystem);
    fmfree(horizonUnitX); fmfree(horizonUnitY);

    //solution.get(0,0) contains the value of the parameter t
    //such that the point l1 + (l2 -l1)t is on the horizon plane
    //NOTE: this intersection is still in the horizon frame though
    Matrix *intersection = fmsub(l2,l1);
    fmscaleeq(intersection,t);
    fmaddeq(intersection,l1);

    return intersection;
}

// returns 'estimate' (dist,bearing,elevation) to a pixel x,y in centimeters
// from body center
const estimate Pose::pixEstimate(const int pixel_x,const int pixel_y,
                                 const float object_height)
{
    // verifies that pixel is on screen. if it isn't, null estimate
    if (pixel_x > IMAGE_WIDTH || pixel_x < 0 ||
        pixel_y > IMAGE_HEIGHT || pixel_y < 0) {
        //print("pixel out of range, x: %d y: %d", pixel_x, pixel_y);
        return NULL_ESTIMATE;
    }

    // declare x,y,z coord of pixel in relation to focal point
    point3 <float> pixel_in_camera_frame;

    // calculate camera frame x,y,z on a specific pixel
    pixel_in_camera_frame.x = pixel_x - IMAGE_CENTER_X;
    pixel_in_camera_frame.y = FOCAL_LENGTH_MM;
    pixel_in_camera_frame.z = IMAGE_CENTER_Y - pixel_y;

    // convert x, z camera frame values to mm space
    pixel_in_camera_frame.x = (MM_TO_PIX_X_PLANE)*pixel_in_camera_frame.x;
    pixel_in_camera_frame.z = (MM_TO_PIX_Y_PLANE)*pixel_in_camera_frame.z;

    // declare x,y,z coord of pixel in relation to body center
    point3 <float> pixel_in_body_frame;

    // transform camera coordinates to body frame coordinates for a test pixel
    pixel_in_body_frame = image_matrix.transform(pixel_in_camera_frame);

    // find object z from body center (need to convert object_height to mms)
    float object_z_in_body_frame = -body_center_height+(object_height*10);

    // declare 3d line parametric multiplier t from object to body center (orig)
    float object_to_origin_t = 0;

    // calculate t knowing object_z_in_body_frame
    if ((focal_point_body_frame.z-pixel_in_body_frame.z) != 0) {
        object_to_origin_t = ((object_z_in_body_frame-pixel_in_body_frame.z)/
                              (focal_point_body_frame.z-pixel_in_body_frame.z));
    }

    // declare 3d coord of the object's xyz point in body frame
    point3 <float> object_in_body_frame;

    // calculate x,y,z using parametric form of 3d vector
    object_in_body_frame.x = pixel_in_body_frame.x +
        (focal_point_body_frame.x-pixel_in_body_frame.x)*object_to_origin_t;
    object_in_body_frame.y = pixel_in_body_frame.y +
        (focal_point_body_frame.y-pixel_in_body_frame.y)*object_to_origin_t;
    object_in_body_frame.z = pixel_in_body_frame.z +
        (focal_point_body_frame.z-pixel_in_body_frame.z)*object_to_origin_t;

    // declare null estimate
    estimate pix_est = {0,0,0,0,0};

    // SANITY CHECKS
    // if the object height > focal point and pixel > focal point in image plane
    if (object_height*10 < body_center_height+focal_point_body_frame.z &&
        pixel_in_body_frame.z > focal_point_body_frame.z) {
        return pix_est;
    }

    // calculate in degs the bearing of the object from the center of the body
    if (object_in_body_frame.x/object_in_body_frame.y != 0) {
        pix_est.bearing = -RAD2DEG(atan(object_in_body_frame.x/
                                        object_in_body_frame.y));
    }

    // NEED TO CALCULATE ELEVATION

    // calculate dist from body center in 3 dimensions
    //pix_est.dist = get3dDist(object_in_body_frame, ZERO_COORD)/10.0;

    // get dist from origin just on the plane of x/y
    pix_est.dist = getHypotenuse(object_in_body_frame.x,
                                 object_in_body_frame.y)/10.0;
    // Convert from mm to cm
    pix_est.x = object_in_body_frame.x / 10;
    pix_est.y = object_in_body_frame.y / 10;

    /*//////////////////////////////////////////////////
      Corrections based on curve fitting (use only one). -yz
      The first one is the better one to uses.
      Both are commented out for now, since the distance to corners were always
      overestimated and there's code that handled this problem some where else.
      Right now, fixing the distance estimation will cause the dog to back into
      the goal area whenever he sees the ball.
      (he localizes himself correctly when he does not see the ball though)
      need to change that piece of code before we can use this correction.
    */
    // Yi's
    //pix_est.dist = (2.86618 + 0.937351 * pix_est.dist -
    //                0.0020501 * pix_est.dist * pix_est.dist);

    // Harrison's regression

    //pix_est.dist = .96962065 * pix_est.dist - 5.79841029;
    //pix_est.dist = (5.30471 + 0.723296 * pix_est.dist + 0.00124533 *
    //                (pix_est.dist * pix_est.dist));

    //pix_est.dist = (8.0977 + 0.692808 * pix_est.dist +
    //                0.000428233 * (pix_est.dist * pix_est.dist));
    pix_est.dist = (-3.3893113 + 0.9859228*pix_est.dist +
                    -0.0007460*pix_est.dist*pix_est.dist);


    // Yi's second one
    // pix_est.dist = (8.22999 + 0.879145*pix_est.dist -
    //                 0.00204601*pix_est.dist*pix_est.dist + 19.9248*yaw_angle);
    /////////////////////////////////////////////////-end of yz
    return pix_est;
}

// this takes an already calculated object distance (via blob height/width)
// to the camera focal point plus its center x,y on the screen
// and returns the distance,bearing, and elevation to the center of the body.
// no sanity checks done in this method
const estimate Pose::bodyEstimate(const int pixel_x, const int pixel_y,
                                  const float obj_dist)
{
    // check if distance is ridiculous
    if (obj_dist <= 0.0)
        return NULL_ESTIMATE;

    // get bearing in image plane
    float object_bearing = DEG2RAD((IMAGE_WIDTH/2 - pixel_x)/MAX_BEARING);
    // get elevation in image plane
    float object_elevation = DEG2RAD((IMAGE_HEIGHT/2 - pixel_y)/MAX_ELEVATION);
    // convert dist estimate to mm
    float object_dist = obj_dist*10;

    // declare (x,y,z) of object in the image frame
    point3 <float> object_in_image_frame;

    // get (x,y,z) of object in image plane
    object_in_image_frame.x = -object_dist*sin(object_bearing);
    object_in_image_frame.y = (object_dist*cos(object_bearing)*
                               cos(object_elevation));
    object_in_image_frame.z = (object_dist*cos(object_bearing)*
                               sin(object_elevation));

    // transform (x,y,z) to body center
    point3 <float> object_in_body_frame =
        image_matrix.transform(object_in_image_frame);

    //print("coords x: %g y: %g z: %g", coords.x, coords.y, coords.z);

    // declare estimate
    estimate est = {0,0,0,0,0};

    // get distnace from (x,y,z) of object to body center (origin);
    //est.dist = get3dDist(object_in_body_frame,ZERO_COORD);

    // get distance from just the x/y plane
    est.dist = getHypotenuse(object_in_body_frame.x,object_in_body_frame.y);

    // get elevation from body center
    est.elevation = RAD2DEG(asin(object_in_body_frame.z/est.dist));

    // we'll estimate bearing based off the shorter of x and y
    if (abs(object_in_body_frame.x) < abs(object_in_body_frame.y)) {
        est.bearing = RAD2DEG(asin(object_in_body_frame.x/est.dist));
    }
    else {
        est.bearing = RAD2DEG(acos(object_in_body_frame.y/est.dist));
        if (object_in_body_frame.x < 0) est.bearing = -est.bearing;
    }

    // convert dist back into cms
    est.dist = est.dist / 10.0;

    // print("coords x: %g y: %g z: %g body:: dist: %g elevation: %g bearing: %g",
    //       coords.x, coords.y, coords.z, est.dist/10.0, est.elevation,
    //       est.bearing);

    return est;
}


////
////  HELPER METHODS
////


// takes in two point3 <float>s, returns euclidian distance
float Pose::get3dDist(point3 <float> coord1, point3 <float> coord2)
{
    return sqrt(pow((coord1.x-coord2.x),2) +
                pow((coord1.y-coord2.y),2) +
                pow((coord1.z-coord2.z),2));
}

// takes in two point <int>s, returns euclidian distance
float Pose::get2dDist(point <int> coord1, point <int> coord2)
{
    return sqrt(pow(((float)(coord1.x-coord2.x)),2) +
                pow(((float)(coord1.y-coord2.y)),2));
}

// takes in two (x,y) coordinates, returns euclidian distance
float Pose::get2dDist(float x1, float y1, float x2, float y2)
{
    return sqrt(pow((x1-x2),2)+pow((y1-y2),2));
}

// takes in two sides of a triangle, returns hypotenuse
float Pose::getHypotenuse(float x, float y)
{
    return sqrt(pow(x,2)+pow(y,2));
}

// returns the y coord for a given x coord on the horizon line
const int Pose::getHorizonY(const int x) const
{
    return (int)(horizon_left_2d.y + (int)(horizon_slope * x));
}

// returns the x coord for a given y coord on the horizon line
const int Pose::getHorizonX(const int y) const
{
    return (int)((y - horizon_left_2d.y)/horizon_slope);
}

// takes in PoseMatrix instance, prints it prettily
void Pose::printMatrix(PoseMatrix matrix)
{
    printf("%g %g %g %g", RAD2DEG(matrix.get(0)), RAD2DEG(matrix.get(1)),
           RAD2DEG(matrix.get(2)), matrix.get(3));
    printf("%g %g %g %g", RAD2DEG(matrix.get(4)), RAD2DEG(matrix.get(5)),
           RAD2DEG(matrix.get(6)), matrix.get(7));
    printf("%g %g %g %g", RAD2DEG(matrix.get(8)), RAD2DEG(matrix.get(9)),
           RAD2DEG(matrix.get(10)), matrix.get(11));
}

void
Pose::transform (void)
{
}
