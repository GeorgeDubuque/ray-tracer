#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "ray.h"
#include "vec3.h"
#include <cmath>
class sphere : public hittable {
public:
  sphere(const point3 &center, double radius, shared_ptr<material> mat)
      : center(center), radius(std::fmax(0, radius)), mat(mat) {
		// TODO: initialize the material pointer mat
	};

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {

    // we get these numbers by essentially breaking down the function for a
    // sphere into a quadratic function and then solving for t (the point along
    // ray r that intersects)
    //
    // now this code has been simplified since we know
    vec3 oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius * radius;
    // if the discriminant is positive or equal to zero
    //
    // positive: there are two solutions to the quadractic formula
    //           meaning we hit two points
    //
    // zero:     there is one solution to the quadractic formula meaning there
    // is
    //           one solution and we hit the edge
    //
    // negative: this is the case where there is no solution to the quadractic
    //           formula and therefore we didnt hit the sphere
    //
    auto discriminant = h * h - a * c;

    if (discriminant < 0)
      return false;

    auto sqrtd = std::sqrt(discriminant);

    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root)) {
      root = (h + sqrtd) / a;
      if (!ray_t.surrounds(root)) {
        return false;
      }
    }

    rec.t = root;
    rec.p = r.at(root);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_front_face(r, outward_normal);
		rec.mat = mat;

    return true;
  }

private:
  point3 center;
  double radius;
	shared_ptr<material> mat;
};

#endif
