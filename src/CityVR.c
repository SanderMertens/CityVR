#include <include/CityVR.h>

#define BUILDING_SIZE 1
#define GRID_MAX_X 10
#define GRID_MAX_Y 10
#define MIN_HEIGHT 0.1
#define MAX_CARS 20
#define HORIZON 1
static float X_CTR, Y_CTR;
static corto_object data_o;

float rnd() { return (float)rand() / (float)RAND_MAX; }

void reset_car(CityVR_FlyingCar c) {
    int speed = c->speed > 0 ? -1 : 1;
    if (c->move_avenue) {
        c->y = Y_CTR * speed * BUILDING_SIZE;
    } else {
        c->x = X_CTR * speed * BUILDING_SIZE;
    }
    c->offset = c->offset_orig;
}

void position_car(CityVR_FlyingCar c) {
    float pos_dir = (rnd() * GRID_MAX_X * BUILDING_SIZE * HORIZON - X_CTR * BUILDING_SIZE) - 0.5 * BUILDING_SIZE * HORIZON;
    float pos_static = (int)(rnd() * GRID_MAX_X * BUILDING_SIZE - X_CTR * BUILDING_SIZE)- 0.5 * BUILDING_SIZE;
    if (c->move_avenue) {
        c->y = pos_dir;
        c->x = pos_static;
    } else {
        c->x = pos_dir;
        c->y = pos_static;
    }
}

void move_car(CityVR_FlyingCar c) {
    corto_update_begin(c);
    if (c->freight) {
        if (!c->landing && rnd() > 0.9997) {
            c->landing = true;
        }
    }

    if (!c->landing && c->offset >= c->offset_orig) {
        if (c->move_avenue) {
            c->y += c->speed * 0.01;
            if (c->y > ((Y_CTR + 1.5) * HORIZON * BUILDING_SIZE) || c->y < ((-Y_CTR - 1.5) * HORIZON * BUILDING_SIZE)) reset_car(c);
        } else {
            c->x += c->speed * 0.01;
            if (c->x > ((X_CTR + 1.5) * HORIZON * BUILDING_SIZE) || c->x < ((-X_CTR - 1.5) * HORIZON * BUILDING_SIZE)) reset_car(c);
        }
    } else {
        if (c->landing) {
            if (c->offset > 0) {
                c->offset -= 0.01;
            } else {
                c->landing = false;
            }
        } else {
            c->offset += 0.01;
        }
    }
    corto_update_end(c);
}


void create_cars(
    corto_ll cars,
    float height,
    float speed,
    int count,
    bool freight)
{
    int i = 0;
    char *colors[] = {
        "cyan",
        "#2d4e58",
        "purple",
        "#ccc"
    };
    for (i = 0; i < count; i ++) {
        CityVR_FlyingCar c = corto_declare(data_o, NULL, CityVR_FlyingCar_o);
        c->offset = height;
        c->offset_orig = height;
        c->move_avenue = (int)(rnd() * 2) - 1;
        c->freight = freight;
        position_car(c);
        c->speed = speed;
        if (freight) {
            c->height = 0.05;
            if (c->move_avenue) {
                c->depth = 0.5 / fabs(c->speed * 2);
                c->width = 0.09;
            } else {
                c->depth = 0.09;
                c->width = 0.5 / fabs(c->speed * 2);
            }
            corto_set_str(&c->color, "orange");
        } else {
            corto_set_str(&c->color, colors[(int)(rnd() * 4)]);
        }
        if (c->move_avenue) {
            c->offset += 0.1;
        }
        if (corto_define(c)) {
            corto_error("failed to define car");
        }
        corto_ll_append(cars, c);
    }
}

int cortomain(int argc, char *argv[]) {
    int x, y;
    X_CTR = GRID_MAX_X / 2 - 1;
    Y_CTR = GRID_MAX_Y / 2 - 1;
    float MAX_D = sqrt((X_CTR * X_CTR) + (Y_CTR * Y_CTR));
    data_o = corto_lookup(root_o, "data");

    /* Create buildings */
    for (x = 0; x < GRID_MAX_X; x ++) {
        for (y = 0; y < GRID_MAX_Y; y ++) {
            float x_c = X_CTR - x, y_c = Y_CTR - y;
            float distance = sqrt((x_c * x_c) + (y_c * y_c)) / MAX_D;
            float height = 1.0 - distance + rnd() * 0.7 + MIN_HEIGHT;

            if (distance > 0.5) {
                height *= 0.5 * BUILDING_SIZE;
            } else if (rnd() > 0.9) {
                if (distance < 0.1) {
                    height *= 5 * BUILDING_SIZE;
                } else if (distance < 0.2) {
                    height *= 3 * BUILDING_SIZE;
                } else {
                    height *= 2 * BUILDING_SIZE;
                }
            }

            CityVR_Building b = corto_declare(data_o, NULL, CityVR_Building_o);
            b->x = (x - X_CTR) * BUILDING_SIZE;
            b->y = (y - Y_CTR) * BUILDING_SIZE;
            b->width = BUILDING_SIZE - (float)BUILDING_SIZE / 10.0;
            b->height = height;
            b->depth = BUILDING_SIZE - (float)BUILDING_SIZE / 10.0;
            corto_define(b);
        }
    }

    /* Create flying cars at height 3.0 */
    corto_ll cars = corto_ll_new();
    create_cars(cars, 4.0, 0.5, MAX_CARS / 7, true);
    create_cars(cars, 3.8, -0.5, MAX_CARS / 7, true);
    create_cars(cars, 2.2, 2, MAX_CARS / 4, false);
    create_cars(cars, 2.0, -2, MAX_CARS / 4, false);
    create_cars(cars, 1.2, 1, MAX_CARS / 3, false);
    create_cars(cars, 1.0, -1, MAX_CARS / 3, false);

    while (true) {
        corto_iter it = corto_ll_iter(cars);
        while (corto_iter_hasNext(&it)) {
            CityVR_FlyingCar c = corto_iter_next(&it);
            move_car(c);
        }
        corto_sleep(0, 50000000);
    }

    return 0;
}
