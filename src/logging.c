#include "logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char g_dir[512] = "demo/xray_evidence";

static void ensure_dir(){ mkdir(g_dir, 0700); }

void logging_init(const char *dir){ if(dir) strncpy(g_dir, dir, sizeof(g_dir)-1); ensure_dir(); }
void logging_shutdown(){ }

void logging_save_evidence(const xray_frame_t *frame, const ai_result_t *res, const sensors_readout_t *s){
    static int id = 0; char imgpath[1024]; char meta[1024];
    snprintf(imgpath, sizeof(imgpath), "%s/evidence_%03d.pgm", g_dir, id);
    FILE *f = fopen(imgpath, "wb"); if(!f) return;
    fprintf(f, "P5\n%d %d\n255\n", frame->w, frame->h);
    fwrite(frame->pixels, 1, frame->w*frame->h, f);
    fclose(f);
    // metadata JSON
    snprintf(meta, sizeof(meta), "%s/metadata_%03d.json", g_dir, id);
    FILE *m = fopen(meta, "w"); if(!m){ free((void*)frame->pixels); return; }
    fprintf(m, "{\n  \"id\": %d,\n  \"threat_score\": %.3f,\n  \"sensors\": { \"metal\": %s, \"weight\": %.3f },\n  \"detections_count\": %d\n}\n",
            id, res->threat_score, s->metal_detected?"true":"false", s->weight_anomaly, res->count);
    fclose(m);
    printf("[LOG] Saved evidence %s and %s (detections=%d, threat=%.2f)\n", imgpath, meta, res->count, res->threat_score);
    id++;
    free((void*)frame->pixels);
}
