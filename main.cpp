
#include "camera.h"
#include "rt_constants.h"

#include "hittable_list.h"
#include "sphere.h"

color nice_pink =
    color(double(255 / 255.999), double(186 / 255.999), double(239 / 255.999));

int main() {
  // World
  hittable_list world;

  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
  world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

  // Camera
  camera cam;

	cam.samples_per_pixel = 100;
  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
	cam.max_depth = 50;

  cam.render(world);
};
