#include "scene_state.h"
#include <pthread.h>
#include <string.h>

static ai_result_t g_current;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

void scene_state_set_result(const ai_result_t *res){
    pthread_mutex_lock(&g_mutex);
    memcpy(&g_current, res, sizeof(ai_result_t));
    pthread_mutex_unlock(&g_mutex);
}

void scene_state_get_result(ai_result_t *out){
    pthread_mutex_lock(&g_mutex);
    memcpy(out, &g_current, sizeof(ai_result_t));
    pthread_mutex_unlock(&g_mutex);
}
