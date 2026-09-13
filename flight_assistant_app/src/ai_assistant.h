/* ai_assistant.h - simple suggestion engine API (demo) */

#ifndef AI_ASSISTANT_H
#define AI_ASSISTANT_H

int ai_get_suggestion(const char *flight_id, char *out, size_t out_len);

#endif
