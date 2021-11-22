#include "util.h"
#include "data_structures.h"
#include "geometry.h"


//getter and setter for hit_record
hit_record::hit_record(){};

void hit_record::register_hit(vec3f normal, vec3f sect_coords, QRgb surface_color, float t){
    this->normal = normal;
    this->sect_coords = sect_coords;
    this->surface_color = surface_color;
    this->t = t;
}

void hit_record::reset(){
    t = std::numeric_limits<float>::max();
}

vec3f*  hit_record::get_normal(){return &normal;};

vec3f*  hit_record::get_sect_coords(){return &sect_coords;};

QRgb*  hit_record::get_surface_color(){return &surface_color;};

float hit_record::get_t(){return t;};


box::box() : min(vec3f(1e30f,1e30f,1e30f)), max(vec3f(-1e30f,-1e30f,-1e30f)){};

box::box(vec3f min, vec3f max) : min(min), max(max){};

box box_union(const box &first, const box &second){
    vec3f min = vec3f(std::min(first.min.x, second.min.x),std::min(first.min.y, second.min.y),std::min(first.min.z, second.min.z));
    vec3f max = vec3f(std::max(first.max.x, second.max.x),std::max(first.max.y, second.max.y),std::max(first.max.z, second.max.z));
    return box(min, max);
}

bool contains(const box &b, const vec3f &pt){
    return pt.x < b.max.x && pt.y < b.max.y && pt.z < b.max.z && pt.x > b.min.x && pt.y > b.min.y && pt.z > b.min.z;
}

bool intersects(const box &b, const ray &r){
    float t_min_x = (b.min.x - r.origin.x) / r.dir.x;
    float t_max_x = (b.max.x - r.origin.x) / r.dir.x;
    if(t_min_x > t_max_x) std::swap(t_min_x, t_max_x);

    float t_min_y = (b.min.y - r.origin.y) / r.dir.y;
    float t_max_y = (b.max.y - r.origin.y) / r.dir.y;
    if(t_min_y > t_max_y) std::swap(t_min_y, t_max_y);

    if((t_min_x > t_max_y) || (t_min_y > t_max_x))
        return false;

    float t_min_z = (b.min.z - r.origin.z) / r.dir.z;
    float t_max_z = (b.max.z - r.origin.z) / r.dir.z;
    if(t_min_z > t_max_z) std::swap(t_min_z, t_max_z);

    if (t_min_z > t_min_x)
         t_min_x = t_min_z;

     if (t_max_z < t_max_x)
         t_max_x = t_max_z;

    if((t_min_x > t_max_z) || (t_min_z > t_max_x))
        return false;

    return true;
}


inline void intersectXPlane(const ray &ray, const float xPlane, float &rayParam, float &yIntersect, float &zIntersect) {
   rayParam = (xPlane - ray.origin.x) / ray.dir.x;
   yIntersect = ray.dir.y*rayParam + ray.origin.y;
   zIntersect = ray.dir.z*rayParam + ray.origin.z;
}
inline void intersectYPlane(const ray &ray, const float yPlane, float &rayParam, float &xIntersect, float &zIntersect) {
   rayParam = (yPlane - ray.origin.y) / ray.dir.y;
   xIntersect = ray.dir.x*rayParam + ray.origin.x;
   zIntersect = ray.dir.z*rayParam + ray.origin.z;
}
inline void intersectZPlane(const ray &ray, const float zPlane, float &rayParam, float &xIntersect, float &yIntersect) {
   rayParam = (zPlane - ray.origin.z) / ray.dir.z;
   xIntersect = ray.dir.x*rayParam + ray.origin.x;
   yIntersect = ray.dir.y*rayParam + ray.origin.y;
}

bool intersectBox(const ray &ray, const box &bb, float &firstHitParam, float &secondHitParam) {
   bool exitFound = false;
   float hitParam;



   if (ray.dir.x != 0) {
      float yIntersect;
      float zIntersect;
      if (ray.dir.x > 0) {
         intersectXPlane(ray, bb.max.x, hitParam, yIntersect, zIntersect);
      } else {
         intersectXPlane(ray, bb.min.x, hitParam, yIntersect, zIntersect);
      }
      if (yIntersect >= bb.min.y && yIntersect <= bb.max.y
       && zIntersect >= bb.min.z && zIntersect <= bb.max.z) {
         secondHitParam = hitParam;
         exitFound = true;
      }
   }



   if (!exitFound) {
      float xIntersect;
      float zIntersect;
      if (ray.dir.y != 0) {
         if (ray.dir.y > 0) {
            intersectYPlane(ray, bb.max.y, hitParam, xIntersect, zIntersect);
         } else {
            intersectYPlane(ray, bb.min.y, hitParam, xIntersect, zIntersect);
         }
         if (xIntersect >= bb.min.x && xIntersect <= bb.max.x
          && zIntersect >= bb.min.z && zIntersect <= bb.max.z) {
            secondHitParam = hitParam;
            exitFound = true;
         }
      }
   }



   if (!exitFound) {
      float xIntersect;
      float yIntersect;
      if (ray.dir.z != 0) {
         if (ray.dir.z > 0) {
            intersectZPlane(ray, bb.max.z, hitParam, xIntersect, yIntersect);
         } else {
            intersectZPlane(ray, bb.min.z, hitParam, xIntersect, yIntersect);
         }
         if (xIntersect >= bb.min.x && xIntersect <= bb.max.x
          && yIntersect >= bb.min.y && yIntersect <= bb.max.y) {
            secondHitParam = hitParam;
            exitFound = true;
         }
      }
   }



   if (!exitFound) {
      return false;
   }



   bool entryFound = false;



   if (ray.dir.x != 0) {
      float yIntersect;
      float zIntersect;
      if (ray.dir.x > 0) {
         intersectXPlane(ray, bb.min.x, hitParam, yIntersect, zIntersect);
      } else {
         intersectXPlane(ray, bb.max.x, hitParam, yIntersect, zIntersect);
      }
      if (yIntersect >= bb.min.y && yIntersect <= bb.max.y
       && zIntersect >= bb.min.z && zIntersect <= bb.max.z) {
         firstHitParam = hitParam;
         entryFound = true;
      }
   }



   if (!entryFound) {
      float xIntersect;
      float zIntersect;
      if (ray.dir.y != 0) {
         if (ray.dir.y > 0) {
            intersectYPlane(ray, bb.min.y, hitParam, xIntersect, zIntersect);
         } else {
            intersectYPlane(ray, bb.max.y, hitParam, xIntersect, zIntersect);
         }
         if (xIntersect >= bb.min.x && xIntersect <= bb.max.x
          && zIntersect >= bb.min.z && zIntersect <= bb.max.z) {
            firstHitParam = hitParam;
            entryFound = true;
         }
      }
   }



   if (!entryFound) {
      float xIntersect;
      float yIntersect;
      if (ray.dir.z != 0) {
         if (ray.dir.z > 0) {
            intersectZPlane(ray, bb.min.z, hitParam, xIntersect, yIntersect);
         } else {
            intersectZPlane(ray, bb.max.z, hitParam, xIntersect, yIntersect);
         }
         if (xIntersect >= bb.min.x && xIntersect <= bb.max.x
          && yIntersect >= bb.min.y && yIntersect <= bb.max.y) {
            firstHitParam = hitParam;
            entryFound = true;
         }
      }
   }



   if (!entryFound) {
      firstHitParam = 0.0f;
   }



   return true;
}


