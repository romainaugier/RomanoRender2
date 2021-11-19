#include "render.h"

void GenerateTiles(Tiles& tiles, 
                   const Settings& settings) noexcept
{   
    constexpr uint8_t tilesize = 16;

    const uint16_t tileCountX = ceil(settings.xres / tilesize);
    const uint16_t tileCountY = ceil(settings.yres / tilesize);

    const uint8_t lastTileSizeX = ceil(fmodf(settings.xres, tilesize)) == 0 ? 32 : ceil(fmodf(settings.xres, tilesize));
    const uint8_t lastTileSizeY = ceil(fmodf(settings.yres, tilesize)) == 0 ? 32 : ceil(fmodf(settings.yres, tilesize));

    tiles.count = tileCountX * tileCountY;

    tiles.tiles.reserve(tiles.count);

    uint16_t idx = 0;

    for (int y = 0; y < settings.yres; y += tilesize)
    {
        for (int x = 0; x < settings.xres; x += tilesize)
        {
            Tile tmpTile;
            tmpTile.id = idx;
            tmpTile.x_start = x; tmpTile.x_end = x + tilesize;
            tmpTile.y_start = y; tmpTile.y_end = y + tilesize;

            if (x + tilesize > settings.xres)
            {
                tmpTile.x_end = x + lastTileSizeX;
                tmpTile.size_x = lastTileSizeX;
            }
            if (y + tilesize > settings.yres)
            {
                tmpTile.y_end = y + lastTileSizeY;
                tmpTile.size_y = lastTileSizeY;
            }

            tmpTile.pixels = new color[tmpTile.size_x * tmpTile.size_y];
            tmpTile.randoms = new float[tmpTile.size_x * tmpTile.size_y * 2];

            tiles.tiles.push_back(tmpTile);

            idx++;
        }
    }
}

void ReleaseTiles(Tiles& tiles) noexcept
{
    for (auto& tile : tiles.tiles)
    {
        delete[] tile.pixels;
        tile.pixels = nullptr;

        delete[] tile.randoms;
        tile.randoms = nullptr;
    }
}

void RenderTiles(const RTCScene& embreeScene, 
                 OSL::ShadingSystem* oslShadingSys,
                 color* __restrict buffer,
                 const std::vector<Object>& sceneObjects,
                 const std::vector<OSL::ShaderGroupRef>& shaders,
                 const uint64_t& sample,
                 const uint64_t& sampleSeed,
                 const Tiles& tiles, 
                 const Camera& cam, 
                 const Settings& settings) noexcept
{
    static tbb::affinity_partitioner partitioner;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, tiles.count), [&](const tbb::blocked_range<size_t>& r)
        {
            for (size_t t = r.begin(), t_end = r.end(); t < t_end; t++)
            {
                RenderTile(embreeScene, oslShadingSys, sceneObjects, shaders, sample, sampleSeed, tiles.tiles[t], cam, settings);

                for (int y = 0; y < tiles.tiles[t].size_y; y++)
                    {
                        for (int x = 0; x < tiles.tiles[t].size_x; x++)
                        {
                            buffer[tiles.tiles[t].x_start + x + (tiles.tiles[t].y_start + y) * settings.xres].R = tiles.tiles[t].pixels[x + y * tiles.tiles[t].size_x].R;
                            buffer[tiles.tiles[t].x_start + x + (tiles.tiles[t].y_start + y) * settings.xres].G = tiles.tiles[t].pixels[x + y * tiles.tiles[t].size_x].G;
                            buffer[tiles.tiles[t].x_start + x + (tiles.tiles[t].y_start + y) * settings.xres].B = tiles.tiles[t].pixels[x + y * tiles.tiles[t].size_x].B;
                        }
                    }
            }

        }, partitioner);
}

void RenderTile(const RTCScene& embreeScene,
                OSL::ShadingSystem* oslShadingSys,
                const std::vector<Object>& sceneObjects,
                const std::vector<OSL::ShaderGroupRef>& shaders,
                const uint64_t& sample,
                const uint64_t& sampleSeed,
                const Tile& tile,
                const Camera& cam,
                const Settings& settings) noexcept
{   
    // Initialize OSL thread context and infos
    OSL::PerThreadInfo* info = oslShadingSys->create_thread_info();
    OSL::ShadingContext* ctx = oslShadingSys->get_context(info);

    // Initialize embree context
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    
    uint16_t idx = 0;

    // Render pixels
    for (int y = tile.y_start; y < tile.y_end; y++)
    {
        for (int x = tile.x_start; x < tile.x_end; x++)
        {
            RTCRayHit tmpRayHit;
            SetPrimaryRay(tmpRayHit, cam, x, y, settings.xres, settings.yres, sampleSeed);

            rtcIntersect1(embreeScene, &context, &tmpRayHit);

            if (tmpRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
            {
                uint32_t hitGeomID = tmpRayHit.hit.geomID;
                const uint32_t hitPrimID = tmpRayHit.hit.primID;
                const float hitU = tmpRayHit.hit.u;
                const float hitV = tmpRayHit.hit.v;

                embree::Vec3f hitPosition = embree::Vec3f(tmpRayHit.ray.org_x,
                                                          tmpRayHit.ray.org_y,
                                                          tmpRayHit.ray.org_z) +
                                                          tmpRayHit.ray.tfar *
                                                          embree::Vec3f(tmpRayHit.ray.dir_x,
                                                              tmpRayHit.ray.dir_y,
                                                              tmpRayHit.ray.dir_z);

                embree::Vec3f hitNormal;
                rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, (float*)&hitNormal, nullptr, nullptr, 3);

                embree::Vec2f hitUv;
                rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, (float*)&hitUv, nullptr, nullptr, 2);

                OSL::ShaderGlobals globals;
                GlobalsFromHit(globals, hitPosition, hitNormal, hitUv);

                const embree::Vec3f output = Pathtrace(embreeScene,
                                                       oslShadingSys, 
                                                       ctx,
                                                       sceneObjects, 
                                                       shaders, 
                                                       sampleSeed * x * y,
                                                       globals, 
                                                       hitGeomID, 
                                                       context);

                const float pixelR = tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].R;
                const float pixelG = tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].G;
                const float pixelB = tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].B;

                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].R = embree::lerp(pixelR, output.x, 1.0f / (sample + 1));
                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].G = embree::lerp(pixelG, output.y, 1.0f / (sample + 1));
                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].B = embree::lerp(pixelB, output.z, 1.0f / (sample + 1));
            }
            else
            {
                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].R = 0.0f;
                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].G = 0.0f;
                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].B = 0.0f;
            }

            idx++;
        }
    }

    // Release OSL context and info for this tile
    oslShadingSys->release_context(ctx);
    oslShadingSys->destroy_thread_info(info);
}

embree::Vec3f Pathtrace(const RTCScene& embreeScene,
                        OSL::ShadingSystem* oslShadingSys,
                        OSL::ShadingContext* oslCtx,
                        const std::vector<Object>& sceneObjects,
                        const std::vector<OSL::ShaderGroupRef>& shaders,
                        const uint64_t seed, 
                        OSL::ShaderGlobals& globals,
                        uint32_t& id,
                        RTCIntersectContext& context) noexcept
{
    embree::Vec3f output = embree::Vec3f(0.0f);
    embree::Vec3f tmp = embree::Vec3f(1.0f);

    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    RTCRayHit rayhit;
    float pdf;

    uint32_t hitGeomID = id;
    uint32_t hitPrimID;
    float hitU, hitV;

    uint8_t bounce = 0;

    while (true)
    {
        if (bounce > 6)
        {
            break;
        }

        int seeds[8];

        // Initialize seeds for random number generation
        for (int i = 0; i < 8; i++) seeds[i] = seed + bounce + (i + 1);

        float randoms[8];

        ispc::randomFloatWangHash(seeds, randoms, toFloat, 8);

        // Execute OSL Shader
        ShadingResult result;
        if(!oslShadingSys->execute(oslCtx, *shaders[0], globals)) break;

        // Process resulting closure
        ProcessClosure(result, globals.Ci);

        // Sample Light
        RTCRay shadow;
            
        embree::Vec3f rayOrig = globals.P + globals.N * 0.001f;
        embree::Vec3f rayDir;
        result.bsdf.Sample(globals, randoms[1], randoms[2], randoms[0], rayDir, pdf);
            
        shadow.org_x = rayOrig.x;
        shadow.org_y = rayOrig.y;
        shadow.org_z = rayOrig.z;
            
        shadow.dir_x = rayDir.x;
        shadow.dir_y = rayDir.y;
        shadow.dir_z = rayDir.z;

        shadow.tnear = 0.0001f;
        shadow.tfar = 10000.0f;
        shadow.mask = -1;
        shadow.flags = 0;

        rtcOccluded1(embreeScene, &context, &shadow);

        if (embree::finite(shadow.tfar))
        {
            output += tmp * result.bsdf.Eval(globals, rayDir, randoms[0]);
        }

        tmp = tmp * result.bsdf.Eval(globals, rayDir, randoms[0]);

        float rr = embree::min(0.95f, (0.2126f * tmp.x + 0.7152f * tmp.y + 0.0722f * tmp.z));
        if (rr < randoms[0]) break;
        else tmp /= rr;

        result.bsdf.Sample(globals, randoms[5], randoms[6], randoms[7], rayDir, pdf);

        SetRay(rayhit, rayOrig, rayDir, 10000.0f);

        rtcIntersect1(embreeScene, &context, &rayhit);

        if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID)
        {
            break;
        }

        hitGeomID = rayhit.hit.geomID;
        hitPrimID = rayhit.hit.primID;

        hitU = rayhit.hit.u;
        hitV = rayhit.hit.v;

        globals.P = embree::Vec3f(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z) + rayhit.ray.tfar * embree::Vec3f(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
        
        rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, (float*)&globals.N, nullptr, nullptr, 3);

        embree::Vec2f hitUv;
        rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, (float*)&hitUv, nullptr, nullptr, 2);

        globals.u = hitUv.x;
        globals.v = hitUv.y;

        bounce++;
    }

    return output;
}
