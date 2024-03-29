#include "scene.h"

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

    transformationMatrix = translate_matrix * rotate_matrix * scale_matrix;

    rVertex* vertices = (rVertex*)rtcGetGeometryBufferData(geometry, RTC_BUFFER_TYPE_VERTEX, 0);

    for (int i = 0; i < vtxCount; i++)
    {
        embree::Vec3f temp(origPositions[i].x, origPositions[i].y, origPositions[i].z);

        embree::Vec3f temp2 = transform(temp, transformationMatrix);

        vertices[i].x = temp2.x;
        vertices[i].y = temp2.y;
        vertices[i].z = temp2.z;
    }

    rtcUpdateGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0);
}


void Object::Release() noexcept
{
    // make sure we free the original position pointer to avoid memory leaks
    free(origPositions);
    origPositions = nullptr;

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

    Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(geo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), object.Indices.size() / 3);

    int tri = 0;

    for (int i = 0; i < object.Indices.size(); i += 3)
    {
        triangles[tri].v0 = object.Indices[i];
        triangles[tri].v1 = object.Indices[i + 1];
        triangles[tri].v2 = object.Indices[i + 2];


        tri++;
    }

    // Vertices attributes
    rtcSetGeometryVertexAttributeCount(geo, 2);

    // Normals
    rVertex* normals = new rVertex[object.Vertices.size()];

    for (int i = 0; i < object.Vertices.size(); i++)
    {
        normals[i].x = object.Vertices[i].Normal.X;
        normals[i].y = object.Vertices[i].Normal.Y;
        normals[i].z = object.Vertices[i].Normal.Z;
    }

    rtcSetSharedGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, normals, 0, sizeof(rVertex), object.Vertices.size() + 1);

    // UV Coordinates
    rTexCoord* texCoords = new rTexCoord[object.Vertices.size()];

    for (int i = 0; i < object.Vertices.size(); i++)
    {
        texCoords[i].u = object.Vertices[i].TextureCoordinate.X;
        texCoords[i].v = object.Vertices[i].TextureCoordinate.Y;
    }

    rtcSetSharedGeometryBuffer(geo, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2, texCoords, 0, sizeof(rTexCoord), object.Vertices.size());

    name = object.MeshName;
    size = object.Vertices.size();

    return geo;
}

namespace utils
{
    void LoadObject(RTCDevice& g_device, std::string path, std::unordered_map<uint32_t, Object>& objects) noexcept
    {
        objl::Loader loader;

        bool ret = loader.LoadFile(path);

        if (ret)
        {
            std::string name;

            std::string baseFilename = path.substr(path.find_last_of("/\\") + 1);
            std::string::size_type const p(baseFilename.find_last_of('.'));
            std::string objectPath = baseFilename.substr(0, p);

            // To have a correct hash, we need to increment with the total amount of objects in the scene
            int i = objects.size();

            //#pragma omp parallel for
            for (auto& object : loader.LoadedMeshes)
            {
                int vtx_number;

                // here we create a custom buffer to store the original vertices positions to be able to apply transform on it 
                // this buffer gets freed with the Object::release() method
                rVertex* orig = (rVertex*)malloc(sizeof(rVertex) * object.Vertices.size());

                RTCGeometry current_geometry = LoadGeometry(object, g_device, name, orig, vtx_number);
                
                //#pragma omp critical
                {
                    objects.emplace(i, Object(i, name, objectPath, current_geometry, orig, vtx_number));
                }

                i++;
            }
        }

    }


    void BuildScene(RTCDevice& g_device, RTCScene& g_scene, std::unordered_map<uint32_t, Object>& objects) noexcept
    {
        for (auto& [key, object] : objects)
        {
            // not sure of this one
            //auto nodeHandle = objects.extract(key);
            //rtcCommitGeometry(nodeHandle.mapped().geometry);
            //nodeHandle.key() = rtcAttachGeometry(g_scene, nodeHandle.mapped().geometry);
            //objects.insert(std::move(nodeHandle));
        
            rtcCommitGeometry(object.geometry);
            const unsigned int geomID = rtcAttachGeometry(g_scene, object.geometry);
            objects[geomID] = object;
        }

        rtcCommitScene(g_scene);
    }


    void RebuildScene(RTCDevice& g_device, RTCScene& g_scene, std::unordered_map<uint32_t, Object>& objects) noexcept
    {
        rtcReleaseScene(g_scene);

        g_scene = rtcNewScene(g_device);
        rtcSetSceneBuildQuality(g_scene, RTC_BUILD_QUALITY_HIGH);
        rtcSetSceneFlags(g_scene, RTC_SCENE_FLAG_DYNAMIC | RTC_SCENE_FLAG_ROBUST);

        for (const auto& [key, object] : objects)
        {
            rtcCommitGeometry(object.geometry);
            const unsigned int geomID = rtcAttachGeometry(g_scene, object.geometry);
            objects[geomID] = object;
        }

        rtcCommitScene(g_scene);
    }
}