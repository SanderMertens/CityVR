#include <include/CityVR.h>

#define GRID_MAX_X 40
#define GRID_MAX_Y 40
#define MIN_HEIGHT 0.1

float rnd() { return (float)rand() / (float)RAND_MAX; }

int cortomain(int argc, char *argv[]) {
    int x, y;
    const float X_CTR = GRID_MAX_X / 2 - 1, Y_CTR = GRID_MAX_Y / 2 - 1;
    float MAX_D = sqrt((X_CTR * X_CTR) + (Y_CTR * Y_CTR));
    corto_object data_o = corto_lookup(root_o, "data");

    for (x = 0; x < GRID_MAX_X; x ++) {
        for (y = 0; y < GRID_MAX_Y; y ++) {
            float x_c = X_CTR - x, y_c = Y_CTR - y;
            float distance = sqrt((x_c * x_c) + (y_c * y_c)) / MAX_D;
            float height = 1.0 - distance + rnd() * 0.7 + MIN_HEIGHT;

            if (distance > 0.5)
                height *= 0.5;
            else if (rnd() > 0.9)
                if (distance < 0.1)
                    height *= 5;
                else if (distance < 0.2)
                    height *= 3;
                else
                    height *= 2;

            Building b = corto_declare(data_o, NULL, Building_o);
            b->x = x - X_CTR;
            b->y = y - Y_CTR;
            b->width = 0.7;
            b->height = height;
            b->depth = 0.7;
            corto_define(b);
        }
    }

    while (true) {
        corto_sleep(1, 0);
    }

    return 0;
}
