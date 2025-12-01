#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

vec3 nice_pink =
    color(double(255 / 255.999), double(186 / 255.999), double(239 / 255.999));

bool hit_edge_of_sphere(const point3 &center, double radius, const ray &r) {
  vec3 oc = center - r.origin();
  auto a = dot(r.direction(), r.direction());
  auto b = dot(-2.0 * r.direction(), oc);
  auto c = dot(oc, oc) - radius * radius;
  auto discriminant = b * b - 4 * a * c;
  return discriminant == 0;
}

bool hit_sphere(const point3 &center, double radius, const ray &r) {
  // we get these numbers by essentially breaking down the function for a sphere
  // into a quadratic function and then solving for a, b and c
  vec3 oc = center - r.origin();
  auto a = dot(r.direction(), r.direction());
  auto b = dot(-2.0 * r.direction(), oc);
  auto c = dot(oc, oc) - radius * radius;
  auto discriminant = b * b - 4 * a * c;

  // if the discriminant is positive or equal to zero
  //
  // positive: there are two solutions to the quadractic formula
  //           meaning we hit two points
  //
  // zero:     there is one solution to the quadractic formula meaning there is
  //           one solution and we hit the edge
  //
  // negative: this is the case where there is no solution to the quadractic
  //           formula and therefore we didnt hit the sphere
  return discriminant > 0;
}

color ray_color(const ray &r) {
  // we are essentially placing a sphere here and then checking if we hit it
  //if (hit_edge_of_sphere(point3(0, 0, -1), 0.5, r)) {
  //  return color(1.0, 0.0, 0.0);
  //}

  if (hit_sphere(point3(0, 0, -1), 0.5, r)) {
    return nice_pink;
  }

  // linear interp
  // blendedVal = (1-a) * startValue + a * endValue
  vec3 unit_dir = unit_vector(r.direction());
  auto a = 0.5 * (unit_dir.y() + 1.0);
  return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
};

int main() {

  auto aspect_ratio = 16.0 / 9.0;

  int image_width = 400;

  int image_height = int(image_width / aspect_ratio);
  image_height = image_height < 1 ? 1 : image_height;

  // std::cout << "Image Height: " << image_height
  //           << " Image Width: " << image_width << "\n";

  // Camera
  auto focal_length = 1.0;
  auto viewport_height = 2.0;
  auto viewport_width = viewport_height * (double(image_width) / image_height);
  // std::cout << "Viewport Height: " << viewport_height
  //           << " Viewport Width: " << viewport_width << "\n";

  auto camera_center = point3(0, 0, 0);

  auto viewport_u = vec3(viewport_width, 0, 0);
  auto viewport_v = vec3(0, -viewport_height, 0);

  // std::cout << "Viewport U: " << viewport_u
  //           << " Viewport V: " << viewport_v << "\n";

  auto pixel_delta_u = viewport_u / image_width;
  auto pixel_delta_v = viewport_v / image_height;

  // std::cout << "Pixel Delta U: " << pixel_delta_u
  //           << " Pixel Delta V: " << pixel_delta_v << "\n";

  // need to move forward by focal length and then move
  // to the top left of the viewport
  auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) -
                             viewport_u / 2 - viewport_v / 2;

  // essentially getting the diaganol vector to the first pixel from the
  // viewport top left to the first pixel and dividing in half
  auto pixel00_loc =
      viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++) {
    std::clog << "\rScanlines remaining: " << (image_width - j);
    for (int i = 0; i < image_width; i++) {
      auto pixel_center =
          pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
      auto ray_direction = pixel_center - camera_center;
      ray r = ray(camera_center, ray_direction);

      color pixel_color = ray_color(r);

      write_color(std::cout, pixel_color);
    }
  }

  std::clog << "\rRender finished.                      \n";
};
