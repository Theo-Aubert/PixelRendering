#pragma once

#include "PixelApp.h"
#include "AppManager.h"
#include "Constants.h"

#include <memory>
#include <limits>
#include <algorithm>

#include <glm.hpp>
#include <vec3.hpp>


class Material;
class Ray;
class Texture;
class SolidColor;
class CheckerTexture;

class Interval {
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    Interval(double min, double max) : min(min), max(max) {}

    Interval(const Interval& a, const Interval& b) {
        // Create the interval tightly enclosing the two input intervals.
        min = a.min <= b.min ? a.min : b.min;
        max = a.max >= b.max ? a.max : b.max;
    }

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    double clamp(double x) const
    {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    Interval expand(double delta) const
    {
        double padding = delta / 2;
        return Interval(min - padding, max + padding);
    }

    static const Interval empty, universe;
};

class AABB
{
public:
    Interval x, y, z;

    AABB() {}; // The default AABB is empty, since intervals are empty by default.

    AABB(const Interval& x, const Interval& y, const Interval& z) : x(x), y(y), z(z) {};

    // Treat the two points a and b as extrema for the bounding box, so we don't require a
    // particular minimum/maximum coordinate order.
    AABB(const glm::vec3& a, const glm::vec3& b); 

    AABB(const AABB& box0, const AABB& box1)
    {
        x = Interval(box0.x, box1.x);
        y = Interval(box0.y, box1.y);
        z = Interval(box0.z, box1.z);
    }

    const Interval& AxisInterval(int  n) const
    {
        if (n == 1) return y;
        if (n == 2) return z;
        return x;
    }

    bool Hit(const Ray& r, Interval ray_t) const;

    int LongestAxis() const;

    static const AABB empty, universe;
};

class Ray
{

public :
    Ray() {};

    Ray(const glm::vec3& origin, const glm::vec3& direction) : m_origin(origin), m_dir(direction) {};

    Ray(const glm::vec3& origin, const glm::vec3& direction, double time) : m_origin(origin), m_dir(direction),  m_dTime(time) {};

    const glm::vec3& origin()   const { return m_origin;}
    const glm::vec3 direction() const { return m_dir; };

    double Time() const { return m_dTime; }

    glm::vec3 at(float t) const { return m_origin + t * m_dir; }

    //utility
    static glm::vec3 RandomRay()
    {
        return glm::vec3(RandomDouble(), RandomDouble(), RandomDouble());
    }

    static glm::vec3 RandomRay(double min, double max)
    {
        return glm::vec3(RandomDouble(min, max), RandomDouble(min, max), RandomDouble(min, max));
    }

    static glm::vec3 RandomUnitVector()
    {
        while (true)
        {
            auto p = RandomRay(-1, 1);
            auto length = dot(p, p);
            if (1e-16 < length && length <= 1)
            {
                return p / sqrt(length);
            }
        }
    }

    static glm::vec3 RandomOnHemisphere(const glm::vec3& normal)
    {
        glm::vec3 vOnUnitSphere = RandomUnitVector();
        if (dot(vOnUnitSphere, normal) > 0.0) // In the same hemisphere as the normal
            return vOnUnitSphere;
        else
            return -vOnUnitSphere;
    }

    static glm::vec3 RandomInUnitDisk()
    {
        while (true)
        {
            glm::vec3 p = glm::vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
            if (dot(p,p) < 1)
                return p;
        }
    }

    static glm::vec3 Reflect(const glm::vec3& vec, const glm::vec3& normal)
    {
        return vec - 2 * dot(vec, normal) * normal;
    }

    static glm::vec3 Refract(const glm::vec3& uv,const glm::vec3& normal, float etaRatio)
    {
        float cos_theta = std::fmin(dot(-uv, normal), 1.0);
        glm::vec3 r_out_perp = etaRatio * (uv + cos_theta * normal);
        glm::vec3 r_out_parallel = float(- std::sqrt(std::fabs(1.0 - dot(r_out_perp, r_out_perp)))) * normal;
        return r_out_perp + r_out_parallel;
    }

    static bool NearZero(const glm::vec3& vec)
    {
        // Return true if the vector is close to zero in all dimensions.
        auto s = 1e-8;
        return (std::fabs(vec.x) < s) && (std::fabs(vec.y) < s) && (std::fabs(vec.z) < s);
    }

private:

    glm::vec3 m_origin;
    glm::vec3 m_dir;
    double m_dTime;
};

class HitRecord
{
public:
    glm::vec3 vHitPoint;
    glm::vec3 vNormal;
    std::shared_ptr<Material> pMat;
    double t;
    double u;
    double v;
    bool bFrontFace;

    void SetFaceNormal(const Ray& r, const glm::vec3& vOutwardNormal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        bFrontFace = dot(r.direction(), vOutwardNormal) < 0;
        vNormal = bFrontFace ? vOutwardNormal : -vOutwardNormal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool Hit(const Ray& ray, Interval rayT, HitRecord& record) const = 0;

    virtual AABB BoundingBox() const = 0;

};

class Sphere : public Hittable
{
public:

    //Stationary sphere
    Sphere(const glm::vec3& vStaticCenter, double dRadius, std::shared_ptr<Material> pMat) : m_vCenter(vStaticCenter, glm::vec3(0)), m_dRadius(std::max(0., dRadius)), mat(pMat) 
    {
        auto rvec = glm::vec3(dRadius, dRadius, dRadius);
        bbox = AABB(vStaticCenter - rvec, vStaticCenter + rvec);
    };

    //moving sphere
    Sphere(const glm::vec3& vCenter1, glm::vec3 vCenter2, double dRadius, std::shared_ptr<Material> pMat) : m_vCenter(vCenter1, vCenter2 - vCenter1), m_dRadius(std::max(0., dRadius)), mat(pMat) {};

    bool Hit(const Ray& ray, Interval rayT, HitRecord& record) const override;

    AABB BoundingBox() const override { return bbox; }

private:
    Ray m_vCenter;
    double m_dRadius;
    std::shared_ptr<Material> mat;
    AABB bbox;
};

class HittableList : public Hittable
{
public:

    std::vector<std::shared_ptr<Hittable>> arrObjects;

    HittableList() {};
    HittableList(std::shared_ptr<Hittable> pObject) { Add(pObject); }

    void Clear() {
        arrObjects.clear();
    };

    void Add(std::shared_ptr<Hittable> pObject) {
        arrObjects.push_back(pObject);
    };

    bool Hit(const Ray& ray, Interval rayT, HitRecord& record) const override;

    AABB BoundingBox() const override {
        return AABB();
    };
};

class BVHNode : public Hittable
{
public:

    BVHNode(HittableList List) : BVHNode(List.arrObjects, 0, List.arrObjects.size()) {}

    BVHNode(std::vector <std::shared_ptr<Hittable>>& arrObjects, size_t start, size_t end);

    bool Hit(const Ray& ray, Interval rayT, HitRecord& record) const override;

    AABB BoundingBox() const override { return bbox; }

private:
    std::shared_ptr<Hittable> pLeft;
    std::shared_ptr<Hittable> pRight;
    AABB bbox;

    static bool BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axisIndex);
    static bool BoxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
    static bool BoxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
    static bool BoxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
};

class Texture
{
public:
    virtual ~Texture() = default;

    virtual glm::vec3 Value(double u, double v, const glm::vec3& point) const = 0;
};

class SolidColor : public Texture
{
public:
    SolidColor(const glm::vec3& albedo) : albedo(albedo) {}

    SolidColor(double red, double green, double blue) : SolidColor(glm::vec3(red, green, blue)) {}

    glm::vec3 Value(double u, double v, const glm::vec3& p) const override {
        return albedo;
    }

private:
    glm::vec3 albedo;
};

class CheckerTexture : public Texture {
public:
    CheckerTexture(double scale, std::shared_ptr<Texture> even, std::shared_ptr<Texture> odd)
        : dInvScale(1.0 / scale), even(even), odd(odd) {}

    CheckerTexture(double scale, const glm::vec3& c1, const glm::vec3& c2)
        : CheckerTexture(scale, std::make_shared<SolidColor>(c1), std::make_shared<SolidColor>(c2)) {}

    glm::vec3 Value(double u, double v, const glm::vec3& p) const override {
        auto xInteger = int(std::floor(dInvScale * p.x));
        auto yInteger = int(std::floor(dInvScale * p.y));
        auto zInteger = int(std::floor(dInvScale * p.z));

        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        return isEven ? even->Value(u, v, p) : odd->Value(u, v, p);
    }

private:
    double dInvScale;
    std::shared_ptr<Texture> even;
    std::shared_ptr<Texture> odd;
};

class Material
{
public:
    virtual ~Material() = default;

    virtual bool Scatter(
        const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
    ) const {
        return false;
    }
};

class Lambertian : public Material {
public:
    Lambertian(const glm::vec3& albedo) : pTexture(std::make_shared<SolidColor>(albedo)) {}
    Lambertian(std::shared_ptr<Texture> pTex) : pTexture(pTex) {}

    bool Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered)
        const override;

private:
    std::shared_ptr<Texture> pTexture;
};

class Metal : public Material {
public:
    Metal(const glm::vec3& albedo, float fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered)
        const override;

private:
    glm::vec3 albedo;
    float fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(double refractIndex) : dRefractionIndex(refractIndex) {}

    bool Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered)
        const override;

private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double dRefractionIndex;

    static double Reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0 * r0;
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
};



class RTLight
{
public:

    virtual ~RTLight() = default;

    //return the unit vector from the point P to the ligt source
    virtual glm::vec3 GetDirection(const glm::vec3& p) const { return glm::vec3(0.); }

    glm::vec3 GetLightColor() const { return vLightColor; }

protected:

    glm::vec3 vLightColor = glm::vec3(1,1,1);
};

class RTDirectionalLight : public RTLight
{
public:

    RTDirectionalLight() {};
    RTDirectionalLight(const glm::vec3& vLightdir, const glm::vec3& vLightCol = glm::vec3(1.)) : vLightDirection(normalize(vLightdir)){ vLightColor = vLightCol; };

    glm::vec3 GetDirection(const glm::vec3& p) const override;

    glm::vec3 GetLightDirection() const { return vLightDirection; };

private:
    
    glm::vec3 vLightDirection;
    
};

class RTPointLight : public RTLight
{
public:

    RTPointLight() {};
    RTPointLight(const glm::vec3& vPos, const glm::vec3& vLightCol = glm::vec3(1.)) : vPos(vPos) { vLightColor = vLightCol; };

    glm::vec3 GetDirection(const glm::vec3& p) const override;

private:

    glm::vec3 vPos;
};

class LightsList : public RTLight
{

public:
    std::vector<std::shared_ptr<RTLight>> arrLights;

    LightsList() {};
    LightsList(std::shared_ptr<RTLight> pLight) { Add(pLight); }

    void Clear() {
        arrLights.clear();
    };

    void Add(std::shared_ptr<RTLight> pLight) {
        arrLights.push_back(pLight);
    };

    glm::vec3 GetDirection(const glm::vec3& p) const override;


};

class Camera
{
public:
    void Initialize();
    void Render(const Hittable& world, const RTDirectionalLight& light);

    olc::Sprite* GetRenderTarget() { return pDrawTarget; }

private:

    glm::vec3 GetRayColor(const Ray& ray, int depth, const Hittable& world, const RTDirectionalLight& light) const;

    // Construct a camera ray originating from the origin and directed at randomly sampled
    // point around the pixel location i, j.
    Ray GetSamplingRay(int i, int j);

    // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
    glm::vec3 SampleSquare() const;

    // Returns a random point in the camera defocus disk.
    glm::vec3 DefocusDiskSample() const;

    void WriteColor(olc::Sprite* pTarget, int i, int j, glm::vec3& color);

    inline double LinearToGamma(double linearComponent)
    {
        if (linearComponent > 0)
        {
            return sqrt(linearComponent);
        }

        return 0;
    }
   
public:

    int iCurrentRow = 0;
    int iRowNum = 0;

    //Viewport
    double dAspectRatio = 16.0 / 9.0;
    double dFoV = 20.; // Vertical view angle (field of view)

    int iImageWidth = 860;

    //Multi sampling
    int iSampleCount = 32;

    //Bouncing limt
    int iMaxDepth = 50;

    int iGamutSlices = 10;
    

    //Camera
    double dFocalLength = 1.0;
    glm::vec3 vLookFrom = glm::vec3(-2, 2, 1);
    glm::vec3 vLookAt = glm::vec3(0, 0, -1);
    glm::vec3 vUp = glm::vec3(0, 1, 0);

    double dDefocusAngle = 2.3;   // Variation angle of rays through each pixel
    double dFocusDistance = 3.4;  // Distance from camera lookfrom point to plane of perfect focus

private:

    int iImageHeight;
    double dViewportWidth;
    double dViewportHeight;

    float fPixelSamplesScale;

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    glm::vec3 viewport_u;
    glm::vec3 viewport_v;

    //Horizontal & Vertical delta vectors
    glm::vec3 vPixeldU;
    glm::vec3 vPixeldV;

    glm::vec3 vCameraCenter = glm::vec3(0.);
    glm::vec3 u, v, w; // Camera frame basis vectors
    glm::vec3   vDefocusDiskU;       // Defocus disk horizontal radius
    glm::vec3   vDefocusDiskV;       // Defocus disk vertical radius

    // Calculate the location of the upper left pixel.
    glm::vec3 vViewportUpperLeft;
    glm::vec3 vPixel00Loc;

    //Render target
    olc::Sprite* pDrawTarget = nullptr;
};


class RayTracer : public olc::PixelGameEngine
{
public:
    RayTracer()
    {
        // Name your application
        sAppName = "RayTracer";
    }

public:
    bool OnUserCreate() override;

    bool OnUserUpdate(float fElapsedTime) override;

private:

    //Scene
    HittableList world;
    RTDirectionalLight light = RTDirectionalLight(glm::vec3(-1,-1,-1));
    Camera camera;
};
REGISTER_CLASS(RayTracer)
