#ifndef MATERIAL_H
#define MATERIAL_H
#include "color.h"
#include "hittable.h"
#include "vec3.h"
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
		if(scatter_direction.near_zero()){
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
  metal(const color &albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {};

  bool scatter(const ray &r_in, const hit_record &recf, color &attenuation,
               ray &scattered) const override {

    vec3 reflected = reflect(r_in.direction(), recf.normal);
		reflected =  unit_vector(reflected) + (random_unit_vector() * fuzz);
    scattered = ray(recf.p, reflected);
    attenuation = albedo;
    return dot(scattered.direction(), recf.normal) > 0;
  };

private:
  color albedo;
	double fuzz;
};
#endif
