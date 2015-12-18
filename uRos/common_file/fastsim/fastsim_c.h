#ifndef FASTSIM_C_HPP_
#define FASTSIM_C_HPP_

#ifdef __cplusplus
extern "C" {
#endif
  // Display window structure
  struct Display;

  // Robot structure
  struct Robot;

  // Map structure
  struct Map;
  
  typedef struct Robot robot_t;
  typedef struct Display display_t;
  typedef struct Map map_t;
  
  // Create a new robot at coords (x,y)
  robot_t *new_robot(float x, float y);

  // Delete a robot
  void delete_robot(robot_t *r);

  /* Make robot r move one time step on map m, with right
     wheel speed v1 and right left wheel v2  */
  void move_robot(robot_t *r, map_t *m, float v1, float v2);

  // Gets state of the left bumper (1 = triggered ; 0 = not triggered)
  int get_left_bumper(robot_t *r);
  // Gets state of the lright bumper (1 = triggered ; 0 = not triggered)
  int get_right_bumper(robot_t *r);
  // Gets distance from the laser ranger number k
  int get_laser(robot_t* r, int k);

  // Creates new map from file fname
  map_t *new_map(char *fname);
  // Delete an existing map
  void delete_map(map_t* map);

  // Create a display window to display robot r on map m
  display_t *new_display(map_t *m, robot_t *r);
  // Update display d after the robot has moved
  void update_display(display_t *d);
  // Delete an existing display
  void delete_display(display_t* d);

#ifdef __cplusplus
}
#endif

#endif
