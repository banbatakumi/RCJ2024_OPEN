#ifndef ON_LINE_HPP
#define ON_LINE_HPP

#include "robot.hpp"

#define BACK_TO_INSIDE_TIME 200  // ms

void OnLine(Robot* robot);

extern Timer back_to_inside_timer;

extern bool is_pre_on_line;
extern float pre_speed;
// extern bool is_pre_leftside;
// extern bool is_pre_rightside;

#endif