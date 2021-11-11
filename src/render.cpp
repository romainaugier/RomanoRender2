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
            
            AllocateRayhitNp(tmpTile.rays, 256);

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

        FreeRayhitNp(tile.rays);

        delete[] tile.randoms;
        tile.randoms = nullptr;
    }
}

void TilesToBuffer(color* __restrict buffer,
                   const Tiles& tiles,
                   const Settings& settings) noexcept
{
    for (int globY = 0; globY < settings.yres; globY++)
    {
        for (auto& tile : tiles.tiles)
        {
            if (globY < tile.y_start || globY >= tile.y_end) continue;

            for (int x = 0; x < tile.size_x; x++)
            {
                buffer[tile.x_start + x + (globY) * settings.xres].R += tile.pixels[x + (globY - tile.y_start) * tile.size_x].R;
                buffer[tile.x_start + x + (globY) * settings.xres].G += tile.pixels[x + (globY - tile.y_start) * tile.size_x].G;
                buffer[tile.x_start + x + (globY) * settings.xres].B += tile.pixels[x + (globY - tile.y_start) * tile.size_x].B;
            }
        }
    }

    //for (auto& tile : tiles.tiles)
    //{
    //    for (int y = 0; y < tile.size_y; y++)
    //    {
    //        for (int x = 0; x < tile.size_x; x++)
    //        {
    //            buffer[tile.x_start + x + (tile.y_start + y) * settings.xres].R += tile.pixels[x + y * tile.size_x].R;
    //            buffer[tile.x_start + x + (tile.y_start + y) * settings.xres].G += tile.pixels[x + y * tile.size_x].G;
    //            buffer[tile.x_start + x + (tile.y_start + y) * settings.xres].B += tile.pixels[x + y * tile.size_x].B;
    //        }
    //    }
    //}
}

void SetTilePixel(Tile& tile, const embree::Vec3f& color, uint32_t x, uint32_t y) noexcept
{
    tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].R = color.x;
    tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].G = color.y;
    tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].B = color.z;
}

void RenderTiles(const RTCScene& embreeScene, 
                 const std::vector<Object>& sceneObjects,
                 const uint64_t& sample,
                 const Tiles& tiles, 
                 const Camera& cam, 
                 const Settings& settings) noexcept
{
    static tbb::affinity_partitioner partitioner;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, tiles.count), [&](const tbb::blocked_range<size_t>& r)
        {
            for (size_t t = r.begin(), t_end = r.end(); t < t_end; t++)
            {
                RenderTile(embreeScene, sceneObjects, sample, tiles.tiles[t], cam, settings);
            }

        }, partitioner);
}

void RenderTile(const RTCScene& embreeScene,
                const std::vector<Object>& sceneObjects,
                const uint64_t& sample,
                const Tile& tile,
                const Camera& cam,
                const Settings& settings) noexcept
{
    // Generate rays
    int seeds[512];

    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    for (int i = 0; i < 512; i++)
    {
        seeds[i] = sample * i + (tile.x_start + i) * (tile.y_start * i) * 29;
    }

    ispc::randomFloatWangHash(seeds, tile.randoms, toFloat, 512);

    ispc::mat44 mat;
    mat.m[0][0] = cam.transformation_matrix[0][0];
    mat.m[0][1] = cam.transformation_matrix[0][1];
    mat.m[0][2] = cam.transformation_matrix[0][2];
    mat.m[0][3] = cam.transformation_matrix[0][3];
    mat.m[1][0] = cam.transformation_matrix[1][0];
    mat.m[1][1] = cam.transformation_matrix[1][1];
    mat.m[1][2] = cam.transformation_matrix[1][2];
    mat.m[1][3] = cam.transformation_matrix[1][3];
    mat.m[2][0] = cam.transformation_matrix[2][0];
    mat.m[2][1] = cam.transformation_matrix[2][1];
    mat.m[2][2] = cam.transformation_matrix[2][2];
    mat.m[2][3] = cam.transformation_matrix[2][3];
    mat.m[3][0] = cam.transformation_matrix[3][0];
    mat.m[3][1] = cam.transformation_matrix[3][1];
    mat.m[3][2] = cam.transformation_matrix[3][2];
    mat.m[3][3] = cam.transformation_matrix[3][3];

    uint16_t idx = 0;

    for (int y = tile.y_start; y < tile.y_end; y++)
    {
        // Random numbers generation

        float xOut[16];
        float yOut[16];
        float zOut[16] = { -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f };

        uint8_t idx2 = 0;

        ispc::generate2dXYTiles(&tile.randoms[idx * 2], tile.x_start, y, settings.xres, settings.yres, cam.aspect, cam.scale, xOut, yOut, zOut, 16);

        float rayOriginWorldX[16] = { 0.0f };
        float rayOriginWorldY[16] = { 0.0f };
        float rayOriginWorldZ[16] = { 0.0f };
        
        float rayPosWorldX[16] = { 0.0f };
        float rayPosWorldY[16] = { 0.0f };
        float rayPosWorldZ[16] = { 0.0f };

        float zeros[16] = { 0.0f };

        ispc::rayTransform(xOut, yOut, zOut, mat, rayPosWorldX, rayPosWorldY, rayPosWorldZ, 16);
        ispc::rayTransform(zeros, zeros, zeros, mat, rayOriginWorldX, rayOriginWorldY, rayOriginWorldZ, 16);

        ispc::rayNormalize(rayOriginWorldX, rayOriginWorldY, rayOriginWorldZ, rayPosWorldX, rayPosWorldY, rayPosWorldZ, xOut, yOut, zOut, 16);

        ispc::raySet((ispc::RTCRayHit&)tile.rays, cam.pos.x, cam.pos.y, cam.pos.z, xOut, yOut, zOut, 10000.0f, idx, 16);

        idx += 16;
    }


    // Coherent context for primary rays
    RTCIntersectContext context;
    context.flags = RTC_INTERSECT_CONTEXT_FLAG_COHERENT;
    rtcInitIntersectContext(&context);

    rtcIntersectNp(embreeScene, &context, &tile.rays, 256);

    /*ispc::generateDiffuseRays(tile.rays, tile.randoms, 256);

    rtcOccludedNp(embreeScene, &context, &tile.rays->ray, 256);*/
    
    idx = 0;

    // Render pixel
    for (int y = tile.y_start; y < tile.y_end; y++)
    {
        for (int x = tile.x_start; x < tile.x_end; x++)
        {
            if (tile.rays.hit.geomID[idx] != RTC_INVALID_GEOMETRY_ID)
            {
                const uint32_t hitGeomID = tile.rays.hit.geomID[idx];
                const uint32_t hitPrimID = tile.rays.hit.primID[idx];
                const float hitU = tile.rays.hit.u[idx];
                const float hitV = tile.rays.hit.v[idx];

                embree::Vec3f hitNormal;
                
                rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, (float*)&hitNormal, nullptr, nullptr, 3);

                embree::Vec3f output = (hitNormal + embree::Vec3f(0.5f)) / 2.0f;

                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].R = output.x;
                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].G = output.y;
                tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].B = output.z;
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

    //RTCRayHit tmpRayHit;

    //for (int y = tile.y_start; y < tile.y_end; y++)
    //{
    //    for (int x = tile.x_start; x < tile.x_end; x++)
    //    {
    //        SetRay(&tmpRayHit, embree::Vec3f(tile.rays.ray.org_x[idx], tile.rays.ray.org_y[idx], tile.rays.ray.org_z[idx]), embree::Vec3f(tile.rays.ray.dir_x[idx], tile.rays.ray.dir_y[idx], tile.rays.ray.dir_z[idx]), 10000.0f);
    //        
    //        RenderTilePixel(embreeScene, sample, x, y, tile, tmpRayHit, cam, settings, context);

    //        idx++;
    //    }
    //}
}

void RenderTilePixel(const RTCScene& embreeScene,
                     const std::vector<Object>& sceneObjects,
                     const uint64_t& seed,
                     const uint16_t x,
                     const uint16_t y,
                     const Tile& tile,
                     RTCRayHit& rayhit,
                     const Camera& cam,
                     const Settings& settings,
                     RTCIntersectContext& context) noexcept
{
    // const embree::Vec3f output = Pathtrace(embreeScene, seed * 9483 * x * y, rayhit, context);

    constexpr float gamma = 1.0f / 2.2f;

    /*tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].R = output.x;
    tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].G = output.y;
    tile.pixels[(x - tile.x_start) + (y - tile.y_start) * tile.size_y].B = output.z;*/
}

void GeneratePixelBatches(PixelBatches& batches,
                          const Settings& settings) noexcept
{
    const uint16_t batchCount = embree::getNumberOfLogicalThreads();
    const uint32_t renderSize = settings.xres * settings.yres;

    const uint16_t batchSize = renderSize / batchCount;

    for (uint16_t i = 0; i < batchCount; i++)
    {
        PixelBatch tmpPixelBatch;

        tmpPixelBatch.size = batchSize;
        tmpPixelBatch.id = i;

        tmpPixelBatch.xStart = batchSize * i;

        tmpPixelBatch.yStart = batchSize / settings.xres;

        AllocateRayhitNp(tmpPixelBatch.rays, 256);

        tmpPixelBatch.pixels = batches.pixelsAllocator.allocate(batchSize);

        tmpPixelBatch.randoms = new float[256 * 2];

        batches.batches.push_back(tmpPixelBatch);
    }

    batches.count = batchCount;
}

void ReleasePixelBatches(PixelBatches& batches) noexcept
{
    for (auto& batch : batches.batches)
    {
        batches.pixelsAllocator.deallocate(batch.pixels, batch.size);
        delete[] batch.randoms;
    }
}

void RenderBatch(PixelBatch& batch,
                 const std::vector<Object>& sceneObjects,
                 const RTCScene& embreeScene,
                 const uint64_t& sample,
                 const Camera& cam,
                 const Settings& settings) noexcept
{
    RTCIntersectContext context;
    context.flags = RTC_INTERSECT_CONTEXT_FLAG_COHERENT;
    rtcInitIntersectContext(&context);

    for (uint32_t i = batch.xStart; i < (batch.xStart + batch.size); i += 256)
    {
        // Generate rays
        int seeds[512];

        constexpr unsigned int floatAddr = 0x2f800004u;
        auto toFloat = float();
        memcpy(&toFloat, &floatAddr, 4);

        for (int j = 0; j < 512; j++)
        {
            seeds[j] = sample * (batch.id + 1) + (batch.id + j) * 29;
        }

        ispc::randomFloatWangHash(seeds, batch.randoms, toFloat, 512);

        float xOut[256];
        float yOut[256];
        float zOut[256];

        for (int k = 0; k < 256; k++) zOut[k] = -1.0f;

        ispc::generate2dXY(&batch.randoms[0], i, settings.xres, settings.yres, cam.aspect, cam.scale, xOut, yOut, zOut, 256);

        float rayOriginWorldX[256] = { 0.0f };
        float rayOriginWorldY[256] = { 0.0f };
        float rayOriginWorldZ[256] = { 0.0f };

        float rayPosWorldX[256] = { 0.0f };
        float rayPosWorldY[256] = { 0.0f };
        float rayPosWorldZ[256] = { 0.0f };

        float zeros[256] = { 0.0f };

        ispc::rayTransform(xOut, yOut, zOut, cam.ispcTransformMatrix, rayPosWorldX, rayPosWorldY, rayPosWorldZ, 256);
        ispc::rayTransform(zeros, zeros, zeros, cam.ispcTransformMatrix, rayOriginWorldX, rayOriginWorldY, rayOriginWorldZ, 256);

        ispc::rayNormalize(rayOriginWorldX, rayOriginWorldY, rayOriginWorldZ, rayPosWorldX, rayPosWorldY, rayPosWorldZ, xOut, yOut, zOut, 256);

        ispc::raySet((ispc::RTCRayHit&)batch.rays, cam.pos.x, cam.pos.y, cam.pos.z, xOut, yOut, zOut, 10000.0f, 0, 256);

        rtcIntersectNp(embreeScene, &context, &batch.rays, 256);

        context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
        rtcInitIntersectContext(&context);

        for (uint16_t k = 0; k < 256; k++)
        {
            if (batch.rays.hit.geomID[k] != RTC_INVALID_GEOMETRY_ID)
            {
                const uint32_t hitGeomID = batch.rays.hit.geomID[k];
                const uint32_t hitPrimID = batch.rays.hit.primID[k];
                const float hitU = batch.rays.hit.u[k];
                const float hitV = batch.rays.hit.v[k];

                embree::Vec3f hitPosition = embree::Vec3f(batch.rays.ray.org_x[k], batch.rays.ray.org_y[k], batch.rays.ray.org_z[k]) + batch.rays.ray.tfar[k] * embree::Vec3f(batch.rays.ray.dir_x[k], batch.rays.ray.dir_y[k], batch.rays.ray.dir_z[k]);
                // const embree::Vec3f hitNormal = embree::normalize(embree::Vec3f(batch.rays.hit.Ng_x[k], batch.rays.hit.Ng_y[k], batch.rays.hit.Ng_z[k]));

                embree::Vec3f hitNormal;
                rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, (float*)&hitNormal, nullptr, nullptr, 3);

                embree::Vec2f hitUv;
                rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, (float*)&hitUv, nullptr, nullptr, 2);

                // const embree::Vec3f output = Pathtrace(embreeScene, sceneObjects, sample * i * (k + 1), hitPosition, hitNormal, batch.rays.hit.geomID[k], context);
                const embree::Vec3f output = (embree::sin(hitUv.x * 100.0f) * embree::sin(hitUv.y * 100.0f)) < 0.0f ? embree::Vec3f(0.25f) : embree::Vec3f(0.75f);

                constexpr float gamma = 1.0f / 2.2f;

                batch.pixels[i + k - batch.xStart].R = embree::pow(output.x, gamma);
                batch.pixels[i + k - batch.xStart].G = embree::pow(output.y, gamma);
                batch.pixels[i + k - batch.xStart].B = embree::pow(output.z, gamma);
            }
            else
            {
                batch.pixels[i + k - batch.xStart].R = 0.0f;
                batch.pixels[i + k - batch.xStart].G = 0.0f;
                batch.pixels[i + k - batch.xStart].B = 0.0f;
            }
        }
    }
}

void RenderProgressive(const RTCScene& embreeScene,
                       const std::vector<Object>& sceneObjects,
                       PixelBatches& batches,
                       const uint64_t& sample,
                       color* __restrict buffer,
                       const Camera& cam,
                       const Settings& settings) noexcept
{
    static tbb::affinity_partitioner partitioner;

    tbb::parallel_for(tbb::blocked_range<size_t>(0, batches.count), [&](const tbb::blocked_range<size_t>& r)
        {
            for (size_t t = r.begin(), t_end = r.end(); t < t_end; t++)
            {
                RenderBatch(batches.batches[t], sceneObjects, embreeScene, sample, cam, settings);

                uint32_t batchPixelIndex = 0;

                for (int i = batches.batches[t].xStart; i < (batches.batches[t].xStart + batches.batches[t].size); i++)
                {
                    buffer[i].R += batches.batches[t].pixels[batchPixelIndex].R;
                    buffer[i].G += batches.batches[t].pixels[batchPixelIndex].G;
                    buffer[i].B += batches.batches[t].pixels[batchPixelIndex].B;

                    batchPixelIndex++;
                }
            }

        }, partitioner);
}

embree::Vec3f Pathtrace(const RTCScene& embreeScene,
                        const std::vector<Object>& sceneObjects,
                        const uint64_t seed, 
                        embree::Vec3f& hitPosition,
                        embree::Vec3f& hitNormal,
                        uint32_t& id,
                        RTCIntersectContext& context) noexcept
{
    embree::Vec3f output = embree::Vec3f(0.0f);
    embree::Vec3f tmp = embree::Vec3f(1.0f);

    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    RTCRayHit rayhit;

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

        int seeds[4] = { seed + bounce + 422 * id, seed + bounce + 4332 * id, seed + bounce + 938 * id, seed + bounce + 2345 * id };
        float randoms[4];

        ispc::randomFloatWangHash(seeds, randoms, toFloat, 4);

        // Sample Light
        RTCRay shadow;
            
        embree::Vec3f rayOrig = hitPosition + hitNormal * 0.001f;
        embree::Vec3f rayDir = sample_ray_in_hemisphere(hitNormal, randoms);
            
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
            output += tmp * 1.5f * embree::max(0.0f, embree::dot(rayDir, hitNormal)) * INVPI;
        }

        tmp *= 0.75f;

        float rr = embree::min(0.95f, (0.2126f * tmp.x + 0.7152f * tmp.y + 0.0722f * tmp.z));
        if (rr < randoms[3]) break;
        else tmp /= rr;

        SetRay(&rayhit, hitPosition, sample_ray_in_hemisphere(hitNormal, &randoms[2]), 10000.0f);

        rtcIntersect1(embreeScene, &context, &rayhit);

        if (rayhit.hit.geomID == RTC_INVALID_GEOMETRY_ID)
        {
            break;
        }

        hitGeomID = rayhit.hit.geomID;
        hitPrimID = rayhit.hit.primID;
        hitU = rayhit.hit.u;
        hitV = rayhit.hit.v;
        hitPosition = embree::Vec3f(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z) + rayhit.ray.tfar * embree::Vec3f(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
        rtcInterpolate1(sceneObjects[hitGeomID].geometry, hitPrimID, hitU, hitV, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, (float*)&hitNormal, nullptr, nullptr, 3);
        // hitNormal = embree::normalize(embree::Vec3f(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z));

        bounce++;
    }

    return output;
}
