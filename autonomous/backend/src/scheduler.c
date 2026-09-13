#include "scheduler.h"
#include "agent_core.h"
#include <stdio.h>

void scheduler_init(){ }
void scheduler_update(int ms){ (void)ms; }
void scheduler_send_command(int agent_id, const char *cmd){ agents_send_command(agent_id, cmd); }
