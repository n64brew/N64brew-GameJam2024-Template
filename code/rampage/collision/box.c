#include "./cylinder.h"

#include "./dynamic_object.h"
#include <math.h>

#define TUCK_SIZE   16.0f

void box_minkowski_sum(void* data, struct Vector3* direction, struct Vector3* output) {
    union dynamic_object_type_data* shape_data = (union dynamic_object_type_data*)data;

    struct Vector3 modified_half = shape_data->box.half_size;

    if (direction->x > direction->z) {
        modified_half.z = shape_data->box.half_size.z - TUCK_SIZE;
    } else {
        modified_half.x = shape_data->box.half_size.x - TUCK_SIZE;
    }

    if (direction->x > direction->y) {
        modified_half.y = shape_data->box.half_size.y - TUCK_SIZE;
    } else {
        modified_half.x = shape_data->box.half_size.x - TUCK_SIZE;
    }

    if (direction->y > direction->z) {
        modified_half.z = shape_data->box.half_size.z - TUCK_SIZE;
    } else {
        modified_half.y = shape_data->box.half_size.y - TUCK_SIZE;
    }

    output->x = direction->x > 0.0f ? modified_half.x : -modified_half.x;
    output->y = direction->y > 0.0f ? modified_half.y : -modified_half.y;
    output->z = direction->z > 0.0f ? modified_half.z : -modified_half.z;


}

void box_bounding_box(void* data, struct Vector2* rotation, struct Box3D* box) {
    union dynamic_object_type_data* shape_data = (union dynamic_object_type_data*)data;

    struct Vector3* half_size = &shape_data->box.half_size;

    if (!rotation) {
        vector3Negate(half_size, &box->min);
        box->max = *half_size;
        return;
    }

    box->max.x = half_size->x * fabsf(rotation->x) + half_size->z * fabsf(rotation->y);
    box->max.y = half_size->y;
    box->max.z = half_size->x * fabsf(rotation->y) + half_size->z * fabsf(rotation->x);

    vector3Negate(&box->max, &box->min);
}