#include <stdio.h>

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

float point_segment_distance(
    float x, float y, float px, float py, float qx, float qy)
{
    float pqx = qx - px;
    float pqy = qy - py;
    float dx = x - px;
    float dy = y - py;
    float d = pqx * pqx + pqy * pqy;
    float t = pqx * dx + pqy * dy;
    if (d > 0) {
        t /= d;
    }
    if (t < 0) {
        t = 0;
    } else if (t > 1) {
        t = 1;
    }
    dx = px + t * pqx - x;
    dy = py + t * pqy - y;
    return dx * dx + dy * dy;
}

void cubic_bezier(
    float x1, float y1, float x2, float y2,
    float x3, float y3, float x4, float y4,
    float tol, int depth)
{
    if (depth > 12) {
        return;
    }
    float x12 = (x1 + x2) / 2;
    float y12 = (y1 + y2) / 2;
    float x23 = (x2 + x3) / 2;
    float y23 = (y2 + y3) / 2;
    float x34 = (x3 + x4) / 2;
    float y34 = (y3 + y4) / 2;
    float x123 = (x12 + x23) / 2;
    float y123 = (y12 + y23) / 2;
    float x234 = (x23 + x34) / 2;
    float y234 = (y23 + y34) / 2;
    float x1234 = (x123 + x234) / 2;
    float y1234 = (y123 + y234) / 2;
    float d = point_segment_distance(x1234, y1234, x1, y1, x4, y4);
    if (d > tol * tol) {
        cubic_bezier(x1, y1, x12, y12, x123, y123, x1234, y1234, tol, depth + 1);
        cubic_bezier(x1234, y1234, x234, y234, x34, y34, x4, y4, tol, depth + 1);
    } else {
        printf("%g,%g ", x4, y4);
    }
}

void handle_path(float *pts, int npts, char closed, float tol) {
    printf("%g,%g ", pts[0], pts[1]);
    for (int i = 0; i < npts - 1; i += 3) {
        float *p = &pts[i * 2];
        cubic_bezier(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], tol, 0);
    }
    if (closed) {
        printf("%g,%g ", pts[0], pts[1]);
    }
    printf("\n");
}

int main(int argc, char **argv) {
    NSVGimage *image = nsvgParseFromFile(argv[1], "px", 96.0f);
    if (image == NULL) {
        printf("Could not open SVG image.\n");
        return -1;
    }
    for (NSVGshape *shape = image->shapes; shape != NULL; shape = shape->next) {
        for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
            handle_path(path->pts, path->npts, path->closed, 1);
        }
    }
    nsvgDelete(image);
    return 0;
}
