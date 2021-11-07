#include "embree.h"

void errorFunction(void* userPtr, enum RTCError error, const char* str) noexcept
{
    printf("error %d: %s\n", error, str);
}


RTCDevice initializeDevice() noexcept
{
    RTCDevice device = rtcNewDevice("isa=avx2");

    if (!device)
        printf("error %d: cannot create device\n", rtcGetDeviceError(nullptr));

    rtcSetDeviceErrorFunction(device, errorFunction, nullptr);
    return device;
}

#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_TRIANGULATE


void Object::SetTransform() noexcept
{
    mat44 translate_matrix = mat44();
    mat44 rotate_matrix = mat44();
    mat44 scale_matrix = mat44();

    set_translation(translate_matrix, translate);
    set_rotation(rotate_matrix, rotate);
    set_scale(scale_matrix, scale);

    transformation_matrix = translate_matrix * rotate_matrix * scale_matrix;

    rVertex* vertices = (rVertex*)rtcGetGeometryBufferData(geometry, RTC_BUFFER_TYPE_VERTEX, 0);

    for (int i = 0; i < vtx_count; i++)
    {
        vec3 temp(orig_positions[i].x, orig_positions[i].y, orig_positions[i].z);

        vec3 temp2 = transform(temp, transformation_matrix);

        vertices[i].x = temp2.x;
        vertices[i].y = temp2.y;
        vertices[i].z = temp2.z;
    }

    rtcUpdateGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0);
}


void Object::Release() noexcept
{
    // make sure we free the original position pointer to avoid memory leaks
    free(orig_positions);
    orig_positions = nullptr;

    // release the embree geometry 
    rtcReleaseGeometry(geometry);
}


RTCGeometry LoadGeometry(objl::Mesh& object, RTCDevice& g_device, std::string& name, rVertex* orig, int& size) noexcept
{
    RTCGeometry geo = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);

    rVertex* vertices = (rVertex*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(rVertex), object.Vertices.size());

    for (int i = 0; i < object.Vertices.size(); i++)
    {
        vertices[i].x = object.Vertices[i].Position.X;
        vertices[i].y = object.Vertices[i].Position.Y;
        vertices[i].z = object.Vertices[i].Position.Z;

        orig[i].x = object.Vertices[i].Position.X;
        orig[i].y = object.Vertices[i].Position.Y;
        orig[i].z = object.Vertices[i].Position.Z;
    }

    rVertex* normals = new rVertex[object.Vertices.size()];

    for (int i = 0; i < object.Vertices.size(); i++)
    {
        normals[i].x = object.Vertices[i].Normal.X;
        normals[i].y = object.Vertices[i].Normal.Y;
        normals[i].z = object.Vertices[i].Normal.Z;
    }


    rtcSetGeometryVertexAttributeCount(geo, 1);
    rtcSetSharedGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, normals, 0, sizeof(rVertex), object.Vertices.size());

    Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), object.Indices.size() / 3);

    int tri = 0;

    for (int i = 0; i < object.Indices.size(); i += 3)
    {
        triangles[tri].v0 = object.Indices[i];
        triangles[tri].v1 = object.Indices[i + 1];
        triangles[tri].v2 = object.Indices[i + 2];


        tri++;
    }


    name = object.MeshName;
    size = object.Vertices.size();


    return geo;
}


void LoadObject(RTCDevice& g_device, std::string path, std::vector<Object>& objects) noexcept
{
    objl::Loader loader;

    bool ret = loader.LoadFile(path);



    if (ret)
    {
        std::string name;

        int i = 0;

        //#pragma omp parallel for
        for (auto& object : loader.LoadedMeshes)
        {
            int vtx_number;

            // here we create a custom buffer to store the original vertices positions to be able to apply transform on it 
            // this buffer gets freed with the Object::release() method
            rVertex* orig = (rVertex*)malloc(sizeof(rVertex) * object.Vertices.size());

            RTCGeometry current_geometry = LoadGeometry(object, g_device, name, orig, vtx_number);

            unsigned int m_id = i;
            i++;

            //#pragma omp critical
            {
                objects.push_back(Object(m_id, name, current_geometry, orig, vtx_number));
            }
        }
    }

}


void BuildScene(RTCDevice& g_device, RTCScene& g_scene, std::vector<Object>& objects) noexcept
{
    for (auto object : objects)
    {
        rtcCommitGeometry(object.geometry);
        unsigned int geoID = rtcAttachGeometry(g_scene, object.geometry);
    }

    rtcCommitScene(g_scene);
}


void RebuildScene(RTCDevice& g_device, RTCScene& g_scene, std::vector<Object>& objects) noexcept
{
    rtcReleaseScene(g_scene);

    g_scene = rtcNewScene(g_device);
    rtcSetSceneBuildQuality(g_scene, RTC_BUILD_QUALITY_HIGH);
    rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);


    for (auto object : objects)
    {
        rtcCommitGeometry(object.geometry);
        unsigned int geoID = rtcAttachGeometry(g_scene, object.geometry);
    }

    rtcCommitScene(g_scene);
}