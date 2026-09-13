#ifndef AGENTS_H
#define AGENTS_H

const char *get_graph_json(void);
const char *scan_face_response(void);
void register_biometric(const char *payload);
const char *register_response(void);

#endif
