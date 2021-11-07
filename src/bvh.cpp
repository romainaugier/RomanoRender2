//#include "bvh.h"
//
//Node* BuildNode(const std::vector<Sphere>& spheres, 
//				std::vector<PrimitiveInfo>& primInfos, 
//				std::vector<Sphere, tbb::cache_aligned_allocator<Sphere>>& orderedSpheres, 
//				uint32_t start, 
//				uint32_t end, 
//				uint32_t& nodeCount) noexcept
//{
//	nodeCount++;
//
//	// Compute the bounding box of all the primitives in the bvh node
//	BoundingBox bvhBbox;
//
//	for (int i = start; i < end; i++) bvhBbox = Union(bvhBbox, primInfos[i].bbox);
//
//	const uint32_t primCount = end - start;
//
//	// Create leaf node if the prim count is 8 or less
//	if (primCount < 9)
//	{
//		const uint32_t firstPrimOffset = orderedSpheres.size();
//
//		for (int i = start; i < end; i++)
//		{
//			orderedSpheres.push_back(spheres[primInfos[i].index]);
//		}
//
//		const uint8_t maxDimension = MaximumDimension(bvhBbox);
//
//		return new Node(bvhBbox, nullptr, nullptr, firstPrimOffset, maxDimension, primCount);
//	}
//	else
//	{
//		// Compute the bounding box of all the primitive centroids
//		BoundingBox centroidBbox;
//
//		for (int i = start; i < end; i++) centroidBbox = Union(centroidBbox, primInfos[i].bbox);
//
//		const uint8_t maxDimension = MaximumDimension(centroidBbox);
//		uint32_t mid = (start + end) / 2;
//
//		if (centroidBbox.p1[maxDimension] == centroidBbox.p0[maxDimension])
//		{
//			const uint32_t offset = orderedSpheres.size();
//
//			orderedSpheres.reserve(offset + (end - start));
//
//			for (int i = start; i < end; i++)
//			{
//				orderedSpheres.push_back(spheres[primInfos[i].index]);
//			}
//
//			return new Node(bvhBbox, nullptr, nullptr, offset, maxDimension, (end - start));
//		}
//		else
//		{
//			// Mid split
//			/*
//			const float positionMid = (centroidBbox.p0[maxDimension] + centroidBbox.p1[maxDimension]) / 2;
//			
//			PrimitiveInfo* midPtr = std::partition(&primInfos[start], &primInfos[end - 1] + 1,
//				[maxDimension, positionMid](const PrimitiveInfo& primInfo)
//				{ return primInfo.centroid[maxDimension] < positionMid; });
//
//			const uint32_t tmpMid = midPtr - &primInfos[0];
//
//			if (tmpMid != start && tmpMid != end)
//			{
//				mid = tmpMid;
//			}
//			*/
//
//			// SAH
//			
//			if (primCount < 9)
//			{
//				mid = (start + end) / 2;
//				std::nth_element(&primInfos[start], &primInfos[mid], &primInfos[end - 1] + 1,
//					[maxDimension](const PrimitiveInfo& a, const PrimitiveInfo& b)
//					{ return a.centroid[maxDimension] < b.centroid[maxDimension]; });
//			}
//			else
//			{
//				// Initialize buckets and infos
//				constexpr int bucketCount = 12;
//				Bucket buckets[bucketCount];
//
//				for (int i = start; i < end; i++)
//				{
//					uint32_t b = bucketCount * Offset(centroidBbox, primInfos[i].centroid)[maxDimension];
//					if (b == bucketCount) b = bucketCount - 1;
//					buckets[b].count++;
//					buckets[b].bbox = Union(buckets[b].bbox, primInfos[i].bbox);
//				}
//
//				// Compute cost
//				float cost[bucketCount - 1];
//
//				for (int i = 0; i < bucketCount - 1; i++)
//				{
//					BoundingBox b0, b1;
//					int count0 = 0, count1 = 0;
//
//					for (int j = 0; j <= i; j++)
//					{
//						b0 = Union(b0, buckets[j].bbox);
//						count0 += buckets[j].count;
//					}
//
//					for (int j = i + 1; j < bucketCount; j++)
//					{
//						b1 = Union(b1, buckets[j].bbox);
//						count1 += buckets[j].count;
//					}
//
//					cost[i] = 0.125f + ((count0 * SurfaceArea(b0) + count1 * SurfaceArea(b1)) / SurfaceArea(bvhBbox));
//
//					if (std::isnan(cost[i])) cost[i] = 100.0f;
//				}
//
//				// Get minimum cost
//				float minCost = cost[0];
//				int minCostSplitBucket = 0;
//
//				for (int i = 1; i < bucketCount - 1; i++)
//				{
//					if (cost[i] < minCost)
//					{
//						minCost = cost[i];
//						minCostSplitBucket = i;
//					}
//				}
//
//				// Split at selected bucket or create leaf
//				float leafCost = primCount;
//
//				if (primCount > 8 || minCost < leafCost)
//				{
//					PrimitiveInfo* pMid = std::partition(&primInfos[start], &primInfos[end - 1] + 1,
//						[=](const PrimitiveInfo& pi)
//						{
//							int b = bucketCount * Offset(centroidBbox, pi.centroid)[maxDimension];
//							if (b == bucketCount) b = bucketCount - 1;
//							return b <= minCostSplitBucket;
//						});
//
//					mid = pMid - &primInfos[0];
//				}
//				else
//				{
//					const uint32_t firstPrimOffset = orderedSpheres.size();
//
//					for (int i = start; i < end; i++)
//						orderedSpheres.push_back(spheres[primInfos[i].index]);
//
//					return new Node(bvhBbox, nullptr, nullptr, firstPrimOffset, maxDimension, primCount);
//				}
//			}
//			
//			
//		}
//		
//		return new Node(bvhBbox, BuildNode(spheres, primInfos, orderedSpheres, start, mid, nodeCount), 
//							     BuildNode(spheres, primInfos, orderedSpheres, mid, end, nodeCount), -1, maxDimension, 0);
//	}
//}
//
//void DeleteNode(Node* node) noexcept
//{
//	if (node == nullptr) return;
//	else
//	{
//		DeleteNode(static_cast<Node*>(node->left));
//		DeleteNode(static_cast<Node*>(node->right));
//	}
//
//	delete node;
//}
//
//uint32_t FlattenAccelerator(LinearNode* linearBvh, 
//							Node* node, 
//							uint32_t* offset) noexcept
//{
//	LinearNode* linearNode = &linearBvh[*offset];
//	linearNode->bbox = node->bbox;
//	uint32_t tmpOffset = (*offset)++;
//
//	if (node->primCount > 0)
//	{
//		linearNode->primCount = node->primCount;
//		linearNode->primitivesOffset = node->primOffset;
//		linearNode->axis = node->axis;
//	}
//	else
//	{
//		linearNode->primCount = 0;
//		linearNode->axis = node->axis;
//		FlattenAccelerator(linearBvh, node->left, offset);
//		linearNode->secondChildOffset = FlattenAccelerator(linearBvh, node->right, offset);
//	} 
//
//	return tmpOffset;
//}
//
//Accelerator BuildAccelerator(const std::vector<Sphere>& spheres) noexcept
//{
//	// Build the primitive info array
//	std::vector<PrimitiveInfo> primInfos;
//	primInfos.reserve(spheres.size());
//
//	unsigned int index = 0;
//
//	for (auto& sphere : spheres)
//	{
//		primInfos.emplace_back(PrimitiveInfo(SphereBBox(sphere), sphere.center, index));
//		index++;
//	}
//	
//	// Get the scene bounding box
//	BoundingBox sceneBbox;
//
//	for (auto& info : primInfos)
//	{
//		sceneBbox = Union(sceneBbox, info.bbox);
//	}
//
//	Accelerator bvh;
//
//	std::vector<Sphere, tbb::cache_aligned_allocator<Sphere>> orderedSpheres;
//	orderedSpheres.reserve(spheres.size());
//
//	bvh.bvhHead = BuildNode(spheres, primInfos, orderedSpheres, 0, primInfos.size(), bvh.nodeCount);
//	orderedSpheres.shrink_to_fit();
//	bvh.orderedSpheres = std::move(orderedSpheres);
//
//	bvh.linearBvhHead = bvh.tbbAllocator.allocate(bvh.nodeCount);
//
//	uint32_t offset = 0;
//
//	FlattenAccelerator(bvh.linearBvhHead, bvh.bvhHead, &offset);
//
//	ReleaseAcceleratorTemp(bvh);
//
//	return bvh;
//}
//
//bool Intersect(const Accelerator& accelerator, 
//			   RayHit& rayhit) noexcept
//{
//	bool hit = false;
//
//	const uint8_t dirIsNegative[3] = { rayhit.ray.inverseDirection.x < 0, rayhit.ray.inverseDirection.y < 0, rayhit.ray.inverseDirection.z < 0 };
//
//	constexpr uint8_t stackSize = 64;
//
//	uint32_t nodesToVisit[stackSize];
//	uint32_t toVisit = 0, currentNodeIndex = 0;
//
//	while (true)
//	{
//		const LinearNode* node = &accelerator.linearBvhHead[currentNodeIndex];
//
//		if (Slabs(node->bbox, rayhit))
//		{	
//			if (node->primCount > 0)
//			{
//				if (SphereHitN(&accelerator.orderedSpheres[node->primitivesOffset], rayhit, node->primCount))
//				{
//					hit = true;
//				}
//
//				if (toVisit == 0) break;
//				currentNodeIndex = nodesToVisit[--toVisit];
//			}
//			else
//			{
//				if (dirIsNegative[node->axis])
//				{
//					nodesToVisit[toVisit++] = currentNodeIndex + 1;
//					currentNodeIndex = node->secondChildOffset;
//				}
//				else
//				{
//					nodesToVisit[toVisit++] = node->secondChildOffset;
//					currentNodeIndex = currentNodeIndex + 1;
//				}
//			}
//		}
//		else
//		{
//			if (toVisit == 0) break;
//			currentNodeIndex = nodesToVisit[--toVisit];
//		}
//	}
//	
//	return hit;
//}
//
//bool Occlude(const Accelerator& accelerator, 
//			 ShadowRay& shadow) noexcept
//{
//	bool hit = false;
//
//	const uint8_t dirIsNegative[3] = {shadow.inverseDirection.x < 0, shadow.inverseDirection.y < 0, shadow.inverseDirection.z < 0};
//
//	constexpr unsigned char stackSize = 64;
//
//	uint32_t nodesToVisit[stackSize];
//	uint32_t toVisit = 0, currentNodeIndex = 0;
//
//	while (true)
//	{
//		const LinearNode* node = &accelerator.linearBvhHead[currentNodeIndex];
//
//		if (SlabsOcclude(node->bbox, shadow))
//		{	
//			if (node->primCount > 0)
//			{
//				if (SphereOccludeN(&accelerator.orderedSpheres[node->primitivesOffset], shadow, node->primCount))
//				{
//					hit = true;
//					break;
//				}
//
//				if (toVisit == 0) break;
//				currentNodeIndex = nodesToVisit[--toVisit];
//			}
//			else
//			{
//				if (dirIsNegative[node->axis])
//				{
//					nodesToVisit[toVisit++] = currentNodeIndex + 1;
//					currentNodeIndex = node->secondChildOffset;
//				}
//				else
//				{
//					nodesToVisit[toVisit++] = node->secondChildOffset;
//					currentNodeIndex = currentNodeIndex + 1;
//				}
//			}
//		}
//		else
//		{
//			if (toVisit == 0) break;
//			currentNodeIndex = nodesToVisit[--toVisit];
//		}
//	}
//	
//	return hit;
//}
//
//void ReleaseAcceleratorTemp(Accelerator& accelerator) noexcept
//{
//	DeleteNode(accelerator.bvhHead);
//}
//
//void ReleaseAccelerator(Accelerator& accelerator) noexcept
//{
//	accelerator.tbbAllocator.destroy(accelerator.linearBvhHead);
//}
//
