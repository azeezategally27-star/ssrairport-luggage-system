#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

bool renderer_init();
void renderer_shutdown();
void renderer_frame();

// conveyor interface used by main
void conveyor_init();
void conveyor_update();
bool conveyor_bag_in_tunnel();
void conveyor_hold_current();
void conveyor_toggle_hold();

#endif
