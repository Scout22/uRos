#include "fastsim_c.h"

#include "robot.hpp"
#include "map.hpp"
#include "display.hpp"

// interface Robot
robot_t *new_robot(float x, float y)
{
  robot_t *r = new Robot(15, Posture(x, y, 0));
  r->add_laser(Laser(M_PI / 4.0, 100.0f));
  r->add_laser(Laser(-M_PI / 4.0, 100.0f));
  r->add_laser(Laser(0.0f, 100.0f));
  return r;
}

void move_robot(robot_t *r, map_t *m, float v1, float v2)
{
  r->move(v1, v2, m);
}

int get_left_bumper(robot_t *r)
{
  return r->get_left_bumper();
}

int get_right_bumper(robot_t *r)
{
  return r->get_right_bumper();
}

int get_laser(robot_t *r, int k)
{
  assert(k < 3);
  return r->get_lasers()[k].get_dist();
}

void delete_robot(robot_t *r)
{
  delete r;
}

// interface Map
map_t *new_map(char *fname)
{
  return new Map(fname, 400);
}
// interface Map
void delete_map(map_t* map)
{
  delete map;
}


// interface Display
display_t *new_display(map_t *m, robot_t *r)
{
  return new Display(m, *r);
}

void update_display(display_t *d)
{
  d->update();
}

void delete_display(display_t* d)
{
  delete d;
}
