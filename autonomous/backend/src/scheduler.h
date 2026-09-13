#ifndef SCHEDULER_H
#define SCHEDULER_H

void scheduler_init();
void scheduler_update(int ms);
void scheduler_send_command(int agent_id, const char *cmd);

#endif
