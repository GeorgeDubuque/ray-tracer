#ifndef HITTABLE_H
#define HITTABLE_H
#include "interval.h"
#include "ray.h"
#include "vec3.h"

class material;

class hit_record {

public:
	// the point in space where we hit
  point3 p;
	// the normal at the hit point of the hittable
  vec3 normal;
	shared_ptr<material> mat;
	// the point along the ray in which we hit the hittable
  double t;
	// whether or not we hit the front or back face of the hittable
  bool front_face;

	// determines if we hit the front or back face using dot product
	// and sets the normal accordingly
	// NOTE: outward_normal is assumed to be unit length
  void set_front_face(const ray &r, const vec3 &outward_normal) {
		front_face = dot(r.direction(), outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {

public:
  virtual ~hittable() = default;
  virtual bool hit(const ray &r, interval ray_t,
                   hit_record &rec) const = 0;
};

#endif
