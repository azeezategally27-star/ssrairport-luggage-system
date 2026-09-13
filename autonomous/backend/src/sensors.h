#ifndef SENSORS_H
#define SENSORS_H

void sensors_init();
void sensors_update(int ms);
float sensors_read_proximity(float x, float y);
float sensors_read_weight(float x, float y);

#endif
