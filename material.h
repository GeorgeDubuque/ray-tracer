#ifndef MATERIAL_H
#define MATERIAL_H
#include "color.h"
#include "hittable.h"
#include "rt_constants.h"
#include "vec3.h"
#include <cmath>
#include <functional>
class material {
public:
  virtual ~material() = default;

  virtual bool scatter(const ray &r_in, const hit_record &recf,
                       color &attenuation, ray &scattered) const {
    return false;
  }
};

class lambertian : public material {
public:
  lambertian(const color &albedo) : albedo(albedo) {};

  bool scatter(const ray &r_in, const hit_record &recf, color &attenuation,
               ray &scattered) const override {

    auto scatter_direction = recf.normal + random_unit_vector();

    // Catch near zero scatter direction and just return normal
    if (scatter_direction.near_zero()) {
      scatter_direction = recf.normal;
    }

    scattered = ray(recf.p, scatter_direction);
    attenuation = albedo;
    return true;
  };

private:
  color albedo;
};

class metal : public material {
public:
  metal(const color &albedo, double fuzz)
      : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {};

  bool scatter(const ray &r_in, const hit_record &recf, color &attenuation,
               ray &scattered) const override {

    vec3 reflected = reflect(r_in.direction(), recf.normal);
    reflected = unit_vector(reflected) + (random_unit_vector() * fuzz);
    scattered = ray(recf.p, reflected);
    attenuation = albedo;
    return dot(scattered.direction(), recf.normal) > 0;
  };

private:
  color albedo;
  double fuzz;
};

class dielectric : public material {
public:
  dielectric(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const ray &r_in, const hit_record &recf, color &attenuation,
               ray &scattered) const override {

    attenuation = color(1.0, 1.0, 1.0);
    double ref_ind =
        recf.front_face ? (1.0 / refraction_index) : refraction_index;

    vec3 unit_dir = unit_vector(r_in.direction());

    double cos_theta = std::fmin(dot(-unit_dir, recf.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ref_ind * sin_theta > 1.0;
    vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ref_ind) > random_double()) {
      direction = reflect(unit_dir, recf.normal);
    } else {
      direction = refract(unit_dir, recf.normal, ref_ind);
    }

    scattered = ray(recf.p, direction);

    return true;
  };

private:
  double refraction_index;

  static double reflectance(double cos, double ref_ind) {

    auto r0 = (1 - ref_ind) / (1 + ref_ind);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cos), 5);
  }
};
#endif
