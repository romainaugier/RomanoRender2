#pragma once

#ifndef RENDER
#define RENDER

#include "mat44.h"
#include "camera.h"
#include "material.h"
#include "settings.h"
#include "scene/scene.h"
#include "GL/glew.h"
#include "ispc/rand.h"
#include "ispc/ray_generation.h"
#include "embree3/rtcore.h"
#include "tbb/tbb.h"
#include "common/sys/sysinfo.h"

#include <vector>
#include <iostream>

typedef struct { GLfloat R, G, B; } color;

// Tile rendering structs and functions

struct alignas(32) Tile
{
	color* pixels = nullptr;
	RayPacket rayPacket;
	float* randoms;

	uint16_t x_start, x_end;
	uint16_t y_start, y_end;
	uint16_t id;

	uint8_t size_x = 16; // 16x16 size
	uint8_t size_y = 16;
};

struct Tiles
{
	std::vector<Tile> tiles;
	
	uint16_t count;
};


void GenerateTiles(Tiles& tiles, 
				   const Settings& settings) noexcept;

void ReleaseTiles(Tiles& tiles) noexcept;

void SetTilePixel(Tile& tile, 
				  const embree::Vec3f& color, 
			      uint32_t x, 
				  uint32_t y) noexcept;

void RenderTiles(const RTCScene& embreeScene,
				 color* __restrict buffer,
				 const std::vector<Object>& sceneObjects,
				 const uint64_t& sample, 
				 const Tiles& tiles, 
				 const Camera& cam, 
				 const Settings& settings) noexcept;

void RenderTile(const RTCScene& embreeScene,
				const std::vector<Object>& sceneObjects,
				const uint64_t& sample,
				const Tile& tile,
				const Camera& cam,
				const Settings& settings) noexcept;

void RenderTilePixel(const RTCScene& embreeScene,
					 const std::vector<Object>& sceneObjects,
					 const uint64_t& seed,
					 const uint16_t x,
					 const uint16_t y,
				     const Tile& tile,
					 RTCRayHit& rayhit,
					 const Camera& cam,
					 const Settings& settings,
					 RTCIntersectContext& context) noexcept;

// Progressive rendering structs and functions

#define RAYBATCHSIZE 2048

struct PixelBatch
{
	color* pixels = nullptr;
	RayPacket rayPacket;
	float* cache;
	int* seeds;

	uint32_t size;
	uint32_t xStart;
	
	uint16_t id;
	uint32_t yStart;
};

struct PixelBatches
{
	std::vector<PixelBatch, tbb::cache_aligned_allocator<PixelBatch>> batches;

	tbb::cache_aligned_allocator<color> pixelsAllocator;

	uint16_t count;
};

void GeneratePixelBatches(PixelBatches& batches, 
						  const Settings& settings) noexcept;

void ReleasePixelBatches(PixelBatches& batches) noexcept;

void RenderBatch(PixelBatch& batch,
				 const std::vector<Object>& sceneObjects,
				 const RTCScene& embreeScene,
				 const uint64_t& sample,
				 const Camera& cam,
                 const Settings& settings) noexcept;

void RenderProgressive(const RTCScene& embreeScene,
					   const std::vector<Object>& sceneObjects,
					   PixelBatches& batches,
					   const uint64_t& sample,
					   color* __restrict buffer,
					   const Camera& cam,
					   const Settings& settings) noexcept;


embree::Vec3f Pathtrace(const RTCScene& embreeScene,
					    const std::vector<Object>& sceneObjects,
						const uint64_t seed,
						embree::Vec3f& hitPosition,
						embree::Vec3f& hitNormal,
						uint32_t& id,
						RTCIntersectContext& context) noexcept;

#endif // !RENDER

