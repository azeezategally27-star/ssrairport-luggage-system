#include "xray_simulator.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void xray_init(){ }
void xray_shutdown(){ }

// simple simulated radiograph: integrates a few shapes to create a realistic-looking image
void xray_render_current(xray_frame_t *out){
    int w = 256, h = 256; out->w = w; out->h = h;
    out->pixels = malloc(w*h);
    if(!out->pixels) return;
    // background gradient
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            float gx = (float)y / (h-1);
            unsigned char val = (unsigned char)(200 - 80*gx);
            out->pixels[y*w + x] = val;
        }
    }
    // add a few synthetic high-attenuation blobs (metal)
    int centers[3][3] = {{60,80,18},{140,120,24},{200,60,14}};
    for(int k=0;k<3;k++){
        int cx = centers[k][0]; int cy = centers[k][1]; int rad = centers[k][2];
        for(int yy=cy-rad; yy<=cy+rad; yy++){
            if(yy<0||yy>=h) continue;
            for(int xx=cx-rad; xx<=cx+rad; xx++){
                if(xx<0||xx>=w) continue;
                int dx=xx-cx, dy=yy-cy;
                if(dx*dx+dy*dy <= rad*rad){ out->pixels[yy*w + xx] = 30 + k*20; }
            }
        }
    }
}
