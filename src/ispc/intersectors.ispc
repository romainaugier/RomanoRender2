struct vec3 { float x, y, z; };

struct vec3 operator-(struct vec3 a, struct vec3 b)
{
    struct vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

inline static float dot(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

export void SphereHitN(uniform float spheresCentersX[],
    uniform float spheresCentersY[],
    uniform float spheresCentersZ[],
    uniform float spheresRadius[],
    uniform vec3& rayOrigin, uniform vec3& rayDirection,
    uniform float t[], uniform int count)
{
    float a = dot(rayDirection, rayDirection);

    foreach(i = 0 ... count)
    {
        vec3 center;
        center.x = spheresCentersX[i];
        center.y = spheresCentersY[i];
        center.z = spheresCentersZ[i];

        vec3 oc = rayOrigin - center;
        float b = dot(oc, rayDirection);
        float c = dot(oc, oc) - spheresRadius[i] * spheresRadius[i];
        float discriminant = b * b - a * c;

        if (discriminant > 0)
        {
            float tTmp = (-b - sqrt(discriminant)) / a;

            if (tTmp > 0.0f && tTmp < t[i])
            {
                t[i] = tTmp;
            }
        }
        else
        {
            t[i] = 10000000.0f;
        }
    }  
}