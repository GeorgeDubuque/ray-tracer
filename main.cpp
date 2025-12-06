
#include "camera.h"
#include "rt_constants.h"

#include "hittable_list.h"
#include "sphere.h"

vec3 nice_pink =
    color(double(255 / 255.999), double(186 / 255.999), double(239 / 255.999));

int main() {
  // World
  hittable_list world;

  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
  world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

  // Camera
  camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;

  cam.render(world);
};
