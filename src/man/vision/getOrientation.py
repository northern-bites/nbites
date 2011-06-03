import math


# right_line and left_line are from the corner's
# perspective. right_line, left_line, robot_line will be 2 x 2 Arrays
# as in: [[x_1,y_1],[x_2,y_2]]. These will represent the two points
# defining each line. Assume that Pos has given us their position in
# relative 3-space.

#L-corner first
def getO(right_line, left_line, robot_line):

    #make lines into vectors

    right_i = right_line[1][0]-right_line[0][0]
    right_j = right_line[1][1]-right_line[0][1]

    left_i = left_line[1][0]-left_line[0][0]
    left_j = left_line[1][1]-left_line[0][1]

    robot_i = robot_line[1][0]-robot_line[0][0]
    robot_j = robot_line[1][1]-robot_line[0][1]


    #dot product manipulation to get cosine of angle between line pairs
    cos_angle_right = ((right_i * robot_i + right_j * robot_j)/ \
                           (math.sqrt(math.pow(robot_i,2) +
                                      math.pow(robot_j,2)) *
                            (math.sqrt(math.pow(right_i,2) +
                                       math.pow(right_j,2)))))

    cos_angle_left = ((left_i * robot_i + left_j * robot_j)/
                      (math.sqrt(math.pow(robot_i,2) +
                                 math.pow(robot_j,2)) *
                       (math.sqrt(math.pow(left_i,2) +
                                  math.pow(left_j,2)))))

    #inverse cosine to get actual angle
    angle_right = math.abs(math.acos(cos_angle_right))
    angle_left = math.abs(math.acos(cos_angle_left))

    #use angle information to determine orientation
    if angle_left >= angle_right :

        if  angle_left > 90:

            orientation = -45 - angle_right

        else :

            orientation = -45 + angle_right

    if angle_right > angle_left:

        if angle_right > 90:

            orientation = 45 + angle_left

        else:

            orientation = 45 - angle_left


    return orientation
