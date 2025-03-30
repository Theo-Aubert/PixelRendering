#include "../Public/RayTracer.h"

#include <strstream>


const Interval Interval::empty = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);
 
const AABB AABB::empty = AABB(Interval::empty, Interval::empty, Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);


bool RayTracer::OnUserCreate()
{
    
    camera.dAspectRatio = 16.0 / 9.0;
    camera.iImageWidth = 960;
    camera.iSampleCount = 32;
    camera.iMaxDepth = 16;
    camera.dFoV = 25;
    camera.vLookFrom = glm::vec3(13, 2, 3);
    camera.vLookAt = glm::vec3(0, 0, 0);
    camera.vUp = glm::vec3(0, 1, 0);

    camera.dDefocusAngle = 0.2;
    camera.dFocusDistance = 3.5;

    camera.Initialize();

    //Clear camera render target
    SetDrawTarget(camera.GetRenderTarget());
    Clear(olc::BLACK);
    SetDrawTarget(nullptr);

    //Add objects to scene

    /*auto material_ground    = std::make_shared<Lambertian>(glm::vec3(0.8, 0.8, 0.0));
    auto material_center    = std::make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5));
    auto material_left      = std::make_shared<Dielectric>(1.5);
    auto material_bubble    = std::make_shared<Dielectric>(1.00 / 1.50);
    auto material_right     = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 0.);

    world.Add(std::make_shared<Sphere>(glm::vec3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.Add(std::make_shared<Sphere>(glm::vec3(0.0, 0.0, -1.2), 0.5, material_center));
    world.Add(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.Add(std::make_shared<Sphere>(glm::vec3(-1.0, 0.0, -1.0), 0.4, material_bubble));
    world.Add(std::make_shared<Sphere>(glm::vec3(1.0, 0.0, -1.0), 0.5, material_right));*/

    //auto ground_material = std::make_shared<Lambertian>(glm::vec3(0.5, 0.5, 0.5));
    //world.Add(std::make_shared<Sphere>(glm::vec3(0, -1000, 0), 1000, ground_material));

    auto checker = std::make_shared<CheckerTexture>(0.32, glm::vec3(.2, .3, .1), glm::vec3(.9, .9, .9));
    world.Add(std::make_shared<Sphere>(glm::vec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = RandomDouble();
            glm::vec3 center(a + 0.9 * RandomDouble(), 0.2, b + 0.9 * RandomDouble());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Ray::RandomRay() * Ray::RandomRay();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    auto center2 = center + glm::vec3(0, RandomDouble(0, .5), 0);
                    world.Add(make_shared<Sphere>(center, center2, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = Ray::RandomRay(0.5, 1);
                    auto fuzz = RandomDouble(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    world.Add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.Add(make_shared<Sphere>(glm::vec3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(glm::vec3(0.4, 0.2, 0.1));
    world.Add(make_shared<Sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
    world.Add(make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0, material3));

    light = RTDirectionalLight(glm::vec3(0.5, -1, -.7));

    world = HittableList(std::make_shared<BVHNode>(world));

    return true;
}

bool RayTracer::OnUserUpdate(float fElapsedTime)
{
    //Quit app
    if (GetKey(olc::ESCAPE).bReleased) return false;

    Clear(olc::BLACK);

    camera.Render(world, light);

    //DrawSprite(0, 0, camera.GetRenderTarget());

    if (camera.iCurrentRow < camera.iRowNum)
    {
        std::ostringstream stream;
        stream << "Row : " << camera.iCurrentRow -1 << " / " << camera.iRowNum - 1;
        DrawString(olc::vi2d(35, 35), stream.str(), olc::WHITE, 4);
    }
    else
    {
        DrawSprite(0, 0, camera.GetRenderTarget());
        stream << "Row : " << iCurrentRow <<" / "<<iRowNum;
        std::cout << stream.str() <<'\n';
        DrawString(olc::vi2d(35,25), stream.str(),olc::WHITE, 4);
        DrawString(olc::vi2d(35,35), stream.str(),olc::BLACK, 3);
        

        iCurrentRow ++;
    }

    return true;
}

bool Sphere::Hit(const Ray& ray, Interval rayT, HitRecord& record) const
{
    glm::vec3 vCurrentCenter = m_vCenter.at(ray.Time());
    glm::vec3 oc = vCurrentCenter - ray.origin();
    double a = dot(ray.direction(), ray.direction());
    double h = dot(ray.direction(), oc);
    double c = dot(oc, oc) - m_dRadius * m_dRadius;
    double discriminant = h * h - a * c;

    if (discriminant < 0)
    {
        return false;
    }

    double sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    double root = (h - sqrtd) / a;
    if (!rayT.surrounds(root)) {
        root = (h + sqrtd) / a;
        if (!rayT.surrounds(root)) {
            return false;
        }
    }

    record.t = root;
    record.vHitPoint = ray.at(record.t);
    glm::vec3 vOutwardNormal = (record.vHitPoint - vCurrentCenter) / float(m_dRadius);
    record.SetFaceNormal(ray, vOutwardNormal);
    record.pMat = mat;

    return true;
}

bool HittableList::Hit(const Ray& ray, Interval rayT, HitRecord& record) const
{

    HitRecord tempRecord;
    bool bHasHitAnything = false;

    double dClosest = rayT.max;

    for (const auto& object : arrObjects)
    {
        if (object->Hit(ray, Interval(rayT.min, dClosest), tempRecord))
        {
            bHasHitAnything = true;
            dClosest = tempRecord.t;
            record = tempRecord;
        }
    }

    return bHasHitAnything;
}

void Camera::Render(const Hittable& world, const RTDirectionalLight& light)
{
    if (iCurrentRow < iRowNum)
    {
        for (int i = 0; i < pDrawTarget->width; i++) {

            //Start a new ray 
            glm::vec3 vPixelCenter = vPixel00Loc + float(i) * vPixeldU + float(iCurrentRow) * vPixeldV;
            glm::vec3 vRayDir = vPixelCenter - vCameraCenter;

            Ray ray(vCameraCenter, vRayDir);

            glm::vec3 color(0.);
            
            for (int sample = 0; sample < iSampleCount; sample++)
            {
                Ray samplingRay = GetSamplingRay(i, iCurrentRow);
                color += GetRayColor(samplingRay, iMaxDepth, world, light);
            }

            color *= fPixelSamplesScale;
            WriteColor(pDrawTarget, i, iCurrentRow, color);
        }

        iCurrentRow++;
    }
}

void Camera::Initialize()
{
    //Compute image height & viewport width

    iImageHeight = int(iImageWidth / dAspectRatio);
    iImageHeight = (iImageHeight < 1) ? 1 : iImageHeight;


    vCameraCenter = vLookFrom;

    //double focalLength = length((vLookFrom - vLookAt));
    double theta = Deg2Rad(dFoV);
    double h = tan(theta / 2);
    dViewportHeight = 2 * h * dFocusDistance;
    dViewportWidth = dViewportHeight * (double(iImageWidth) / iImageHeight);

    w = normalize(vLookFrom - vLookAt);
    u = normalize(cross(vUp, w));
    v = cross(w, u);

    fPixelSamplesScale = 1.f / iSampleCount;


    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    viewport_u = float(dViewportWidth) * u;
    viewport_v = float(dViewportHeight) * -v;

    //Horizontal & Vertical delta vectors
    vPixeldU = viewport_u / float(iImageWidth);
    vPixeldV = viewport_v / float(iImageHeight);

    // Calculate the location of the upper left pixel.
    vViewportUpperLeft = vCameraCenter - float(dFocusDistance) * w - viewport_u / 2.f - viewport_v / 2.f;
    vPixel00Loc = vViewportUpperLeft + 0.5f * (vPixeldU + vPixeldV);

    // Calculate the camera defocus disk basis vectors.
    float fDefocusRadius = dFocusDistance * tan(Deg2Rad(dDefocusAngle / 2.));
    vDefocusDiskU = u * fDefocusRadius;
    vDefocusDiskV = v * fDefocusRadius;


    //Init render target
    pDrawTarget = new olc::Sprite(iImageWidth, iImageHeight);

    iRowNum = pDrawTarget->height;
}

glm::vec3 Camera::GetRayColor(const Ray& ray, int depth, const Hittable& world, const RTDirectionalLight& light) const
{
    //return when exceeding bounce limit
    if (depth <= 0)
        return glm::vec3(0, 0, 0);

    HitRecord record;

    //intersect
    if (world.Hit(ray, Interval(0.001, infinity), record))
    {
        Ray scattered;
        glm::vec3 attenuation;

        HitRecord shadowRecord;
        Ray shadowRay(record.vHitPoint, light.GetDirection(record.vHitPoint));
        glm::vec3 lightColor = light.GetLightColor();

        if (world.Hit(shadowRay, Interval(0.001, infinity), shadowRecord))
            lightColor = glm::vec3(0.1);

        if (record.pMat->Scatter(ray, record, attenuation, scattered))
            return lightColor * attenuation * GetRayColor(scattered, depth - 1, world, light);

        return glm::vec3(0.);

        /*
        //glm::vec3 direction = Ray::RandomOnHemisphere(record.vNormal); //UNFIORM SCATTERING

        glm::vec3 direction = record.vNormal + Ray::RandomUnitVector(); //LABERTIAN SCATTERING DISTRIBUTION
        //return .5f * (record.vNormal + glm::vec3(1, 1 , 1));  //NORMAL VISUALISATION

        int iSliceSize = iRowNum / iGamutSlices;
        
        return (iCurrentRow / iSliceSize) * .1f * GetRayColor(Ray(record.vHitPoint, direction), depth-1, world);*/
    }

    //sky
    glm::vec3 vUnitDirection = normalize(ray.direction());
    float a = 0.5 * (vUnitDirection.y + 1.0);
    return (1.0f - a) * glm::vec3(1., 1., 1.) + a * glm::vec3(0.5, 0.7, 1.);
}

Ray Camera::GetSamplingRay(int i, int j)
{
    auto offset = SampleSquare();
    auto pixelSample = vPixel00Loc
        + ((i + offset.x) * vPixeldU)
        + ((j + offset.y) * vPixeldV);

    auto rayOrigin = (dDefocusAngle <= 0) ? vCameraCenter : DefocusDiskSample();
    auto rayDirection = pixelSample - rayOrigin;
    auto rayTime = RandomDouble(); //motion blur

    return Ray(rayOrigin, rayDirection, rayTime); 
}

glm::vec3 Camera::SampleSquare() const
{
    return glm::vec3(RandomDouble() - 0.5, RandomDouble() - 0.5, 0);
}

glm::vec3 Camera::DefocusDiskSample() const
{
    glm::vec3 p = Ray::RandomInUnitDisk();
    return vCameraCenter + (p.x * vDefocusDiskU) + (p.y * vDefocusDiskV);
}

void Camera::WriteColor(olc::Sprite* pTarget, int i, int j, glm::vec3& color)
{
    if (!pTarget)
    {
        return;
    }

    auto r = color.x;
    auto g = color.y;
    auto b = color.z;

    //Linear to Gamma correction iusing gamma 2
    r = LinearToGamma(r);
    g = LinearToGamma(g);
    b = LinearToGamma(b);

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    pTarget->SetPixel(i, j, olc::Pixel(rbyte, gbyte, bbyte));
}

bool Lambertian::Scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
    auto vScatterDirection = rec.vNormal + Ray::RandomUnitVector();

    // Catch degenerate scatter direction
    if (Ray::NearZero(vScatterDirection))
    {
        vScatterDirection = rec.vNormal;
    }

    scattered = Ray(rec.vHitPoint, vScatterDirection, r_in.Time());
    attenuation = pTexture->Value(rec.u, rec.v, rec.vHitPoint);
    return true;
}

bool Metal::Scatter(const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
    glm::vec3 vReflected = Ray::Reflect(r_in.direction(), rec.vNormal);
    vReflected = normalize(vReflected) + fuzz * Ray::RandomUnitVector();
    scattered = Ray(rec.vHitPoint, vReflected, r_in.Time());
    attenuation = albedo;
    return dot(scattered.direction(), rec.vNormal) > 0;
}

bool Dielectric::Scatter(const Ray& inRay, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const
{
    {
        attenuation = glm::vec3(1.0, 1.0, 1.0);
        double ri = rec.bFrontFace ? (1.0 / dRefractionIndex) : dRefractionIndex;

        glm::vec3 vUnitDirection = normalize(inRay.direction());

        float fCosTheta = std::min(dot(-vUnitDirection, rec.vNormal), 1.f);
        float fSinTheta = std::sqrt(1.f - fCosTheta * fCosTheta);

        bool bCannotRefract = ri * fSinTheta > 1.f;

        glm::vec3 vDirection;
        if (bCannotRefract || Reflectance(fCosTheta, ri) > RandomDouble())
        {
            vDirection = Ray::Reflect(vUnitDirection, rec.vNormal);
        }
        else
        {
            vDirection = Ray::Refract(vUnitDirection, rec.vNormal, ri);
        }
        scattered = Ray(rec.vHitPoint, vDirection, inRay.Time());
        return true;
    }
}

glm::vec3 RTDirectionalLight::GetDirection(const glm::vec3& p) const
{
    return -vLightDirection;
}

glm::vec3 RTPointLight::GetDirection(const glm::vec3& p) const
{
    return normalize(vPos - p);
}

glm::vec3 LightsList::GetDirection(const glm::vec3& p) const
{
    return glm::vec3();
}

AABB::AABB(const glm::vec3& a, const glm::vec3& b)
{
    x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
    y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
    z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
}

bool AABB::Hit(const Ray& r, Interval ray_t) const
{
    const glm::vec3& ray_orig = r.origin();
    const glm::vec3& ray_dir = r.direction();

    for (int axis = 0; axis < 3; axis++) {
        const Interval& ax = AxisInterval(axis);
        const double adinv = 1.0 / ray_dir[axis];

        auto t0 = (ax.min - ray_orig[axis]) * adinv;
        auto t1 = (ax.max - ray_orig[axis]) * adinv;

        if (t0 < t1) {
            if (t0 > ray_t.min) ray_t.min = t0;
            if (t1 < ray_t.max) ray_t.max = t1;
        }
        else {
            if (t1 > ray_t.min) ray_t.min = t1;
            if (t0 < ray_t.max) ray_t.max = t0;
        }

        if (ray_t.max <= ray_t.min)
            return false;
    }
    return true;
}

int AABB::LongestAxis() const
{
    // Returns the index of the longest axis of the bounding box.
    if (x.size() > y.size())
        return x.size() > z.size() ? 0 : 2;
    else
        return y.size() > z.size() ? 1 : 2;
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& arrObjects, size_t start, size_t end)
{
    bbox = AABB::empty;

    for (size_t object_index = start; object_index < end; object_index++)
        bbox = AABB(bbox, arrObjects[object_index]->BoundingBox());

    int axis = bbox.LongestAxis();

    auto comparator = (axis == 0)   ? BoxXCompare
                    : (axis == 1)   ? BoxYCompare
                                    : BoxZCompare;

    size_t object_span = end - start;

    if (object_span == 1) {
        pLeft = pRight = arrObjects[start];
    }
    else if (object_span == 2) {
        pLeft = arrObjects[start];
        pRight = arrObjects[start + 1];
    }
    else {
        std::sort(std::begin(arrObjects) + start, std::begin(arrObjects) + end, comparator);

        auto mid = start + object_span / 2;
        pLeft = std::make_shared<BVHNode>(arrObjects, start, mid);
        pRight = std::make_shared<BVHNode>(arrObjects, mid, end);
    }
}

bool BVHNode::Hit(const Ray& ray, Interval rayT, HitRecord& record) const
{
    if (!bbox.Hit(ray, rayT))
        return false;

    bool bHitLeft = pLeft->Hit(ray, rayT, record);
    bool bHitRight = pRight->Hit(ray, Interval(rayT.min, bHitLeft ? record.t : rayT.max), record);

    return bHitLeft || bHitRight;
}

bool BVHNode::BoxCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axisIndex)
{
    auto a_axis_interval = a->BoundingBox().AxisInterval(axisIndex);
    auto b_axis_interval = b->BoundingBox().AxisInterval(axisIndex);
    return a_axis_interval.min < b_axis_interval.min;
}

bool BVHNode::BoxXCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return BoxCompare(a, b, 0);
}

bool BVHNode::BoxYCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return BoxCompare(a, b, 1);
}

bool BVHNode::BoxZCompare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b)
{
    return BoxCompare(a, b, 2);
}
