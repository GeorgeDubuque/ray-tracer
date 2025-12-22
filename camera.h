#ifndef CAMERA_H
#define CAMERA_H

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "rt_constants.h"
#include "vec3.h"
#include <cmath>

class camera {
public:
  double aspect_ratio = 1.0;
  int image_width = 100;
  int samples_per_pixel = 10;
  int max_depth = 10;

  double vfov = 90; // vertical view angle (fov)
  point3 lookfrom = point3(0, 0, 0);
  point3 lookat = point3(0, 0, -1);
  vec3 vup = vec3(0, 1, 0); // cam relative up direction

  void render(const hittable &world) {
    initialize();

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
      std::clog << "\rScanlines remaining: " << (image_width - j);
      for (int i = 0; i < image_width; i++) {
        color pixel_color = color(0, 0, 0);
        for (int k = 0; k < samples_per_pixel; k++) {
          ray r = get_ray(i, j);
          pixel_color += ray_color(r, max_depth, world);
        }
        write_color(std::cout, pixel_color / samples_per_pixel);
      }
    }
    std::clog << "\rDone.                      \n";
  }

private:
  int image_height;
  point3 center;
  point3 pixel00_loc;
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;
  double pixel_sample_scale;
  vec3 u, v, w;

  void initialize() {

    image_height = int(image_width / aspect_ratio);
    image_height = image_height < 1 ? 1 : image_height;

    center = lookfrom;

    pixel_sample_scale = 1.0 / samples_per_pixel;
    // Camera
    auto focal_length = (lookfrom - lookat).length();
    auto theta = degrees_to_radians(vfov);
    auto h = std::tan(theta / 2);
    auto viewport_height = 2.0 * h * focal_length;
    auto viewport_width =
        viewport_height * (double(image_width) / image_height);

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);


    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    // need to move forward by focal length and then move
    // to the top left of the viewport
    auto viewport_upper_left =
        center - (focal_length * w) - viewport_u / 2 - viewport_v / 2;

    // essentially getting the diaganol vector to the first pixel from the
    // viewport top left to the first pixel and dividing in half
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  ray get_ray(double i, double j) const {
    vec3 offset = sample_square();
    vec3 sample_point = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);
    vec3 ray_origin = center;
    vec3 ray_direction = sample_point - ray_origin;
    return ray(ray_origin, ray_direction);
  }

  vec3 sample_square() const {
    // return a random point in the unit square we can use to offset our sample
    // ray we are subtracting by 0.5 to ensure the values are within -0.5 and
    // +0.5
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  color ray_color(const ray &r, int depth, const hittable &world) {
    if (depth <= 0) {
      return color(0, 0, 0);
    }

    hit_record hit_rec;
    if (world.hit(r, interval(0.001, infinity), hit_rec)) {

      // old diffusion model where choose a random reflection vector off
      // the surface of the sphere
      // vec3 direction = random_on_hemisphere(hit_rec.normal);

      // we are now moving to custom meterials which will have there own
      // reflection models
      // vec3 direction = hit_rec.normal + random_unit_vector();
      // return 0.1 * ray_color(ray(hit_rec.p, direction), depth - 1, world);

      // new slightly different diffusion model where we have two unit spheres
      // one on the inside of of the hit point on the sphere and one on the
      // outside then we only use the one on the outide of the unit vector
      ray scattered;
      color attenuation;
      if (hit_rec.mat->scatter(r, hit_rec, attenuation, scattered)) {
        return attenuation * ray_color(scattered, depth - 1, world);
      }
      return color(0, 0, 0);
    }

    // linear interp
    // blendedVal = (1-a) * startValue + a * endValue
    vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  };
};
#endif
