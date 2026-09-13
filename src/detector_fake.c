#include "detector_fake.h"

int detect_fake(const unsigned char *frame, int w, int h) {
    // fake detection return count of 'people' (random small number)
    (void)frame; (void)w; (void)h;
    return 3; // deterministic for demo
}
