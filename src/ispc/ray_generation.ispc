#define INVPI 1.0f / PI

struct vec3
{
	float x, y, z;
};

uniform struct vec3 operator*(uniform struct vec3 a, uniform struct vec3 b) { uniform struct vec3 result; result.x = a.x * b.x; result.y = a.y * b.y; result.z = a.z * b.z; return result; }
varying struct vec3 operator*(varying struct vec3 a, varying struct vec3 b) { varying struct vec3 result; result.x = a.x * b.x; result.y = a.y * b.y; result.z = a.z * b.z; return result; }
uniform struct vec3 operator*(uniform struct vec3 a, uniform float b) { uniform struct vec3 result; result.x = a.x * b; result.y = a.y * b; result.z = a.z * b; return result; }
varying struct vec3 operator*(varying struct vec3 a, varying float b) { varying struct vec3 result; result.x = a.x * b; result.y = a.y * b; result.z = a.z * b; return result; }

uniform struct vec3 operator+(uniform struct vec3 a, uniform struct vec3 b) { uniform struct vec3 result; result.x = a.x + b.x; result.y = a.y + b.y; result.z = a.z + b.z; return result; }
varying struct vec3 operator+(varying struct vec3 a, varying struct vec3 b) { varying struct vec3 result; result.x = a.x + b.x; result.y = a.y + b.y; result.z = a.z + b.z; return result; }
uniform struct vec3 operator+(uniform struct vec3 a, uniform float b) { uniform struct vec3 result; result.x = a.x + b; result.y = a.y + b; result.z = a.z + b; return result; }
varying struct vec3 operator+(varying struct vec3 a, varying float b) { varying struct vec3 result; result.x = a.x + b; result.y = a.y + b; result.z = a.z + b; return result; }

uniform struct vec3 operator-(uniform struct vec3 a, uniform struct vec3 b) { uniform struct vec3 result; result.x = a.x - b.x; result.y = a.y - b.y; result.z = a.z - b.z; return result; }
varying struct vec3 operator-(varying struct vec3 a, varying struct vec3 b) { varying struct vec3 result; result.x = a.x - b.x; result.y = a.y - b.y; result.z = a.z - b.z; return result; }
uniform struct vec3 operator-(uniform struct vec3 a, uniform float b) { uniform struct vec3 result; result.x = a.x - b; result.y = a.y - b; result.z = a.z - b; return result; }
varying struct vec3 operator-(varying struct vec3 a, varying float b) { varying struct vec3 result; result.x = a.x - b; result.y = a.y - b; result.z = a.z - b; return result; }


static inline uniform vec3 normalize(uniform vec3 v)
{
	uniform float t = rsqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= t;
	v.y *= t;
	v.z *= t;
	return v;
}

static inline varying vec3 normalize(varying vec3 v)
{
	varying float t = rsqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x *= t;
	v.y *= t;
	v.z *= t;
	return v;
}

struct mat44
{
	float m[4][4];
};

struct RTCRay
{
	float* org_x;        
	float* org_y;        
	float* org_z;        
	float* tnear;       

	float* dir_x;        
	float* dir_y;        
	float* dir_z;        
	float* time;    

	float* tfar;         
	unsigned int* mask;  
	unsigned int* id;    
	unsigned int* flags; 
};

struct RTCHit
{
	float* Ng_x;         
	float* Ng_y;       
	float* Ng_z;       

	float* u;         
	float* v;            

	unsigned int* primID;
	unsigned int* geomID;
	unsigned int* instID[1]; 
};

struct RTCRayHit
{
	struct RTCRay ray;
	struct RTCHit hit;
};

// very simple and naive matrix multiplication function

static void transform(uniform float x[], 
				      uniform float y[], 
					  uniform float z[], 
					  const uniform mat44& matrix, 
					  uniform float outX[], 
					  uniform float outY[], 
					  uniform float outZ[],
					  uniform int count)
{
	foreach(i = 0 ... count)
	{
		outX[i] = x[i] * matrix.m[0][0] + y[i] * matrix.m[0][1] + z[i] * matrix.m[0][2] + matrix.m[0][3];
		outY[i] = x[i] * matrix.m[1][0] + y[i] * matrix.m[1][1] + z[i] * matrix.m[1][2] + matrix.m[1][3];
		outZ[i] = x[i] * matrix.m[2][0] + y[i] * matrix.m[2][1] + z[i] * matrix.m[2][2] + matrix.m[2][3];
	}
}

static float lerp(float a, float b, float t) 
{
	return (1.0 - t) * a + t * b;
}

// very simple function to sample a random direction in an hemisphere given a normal

static inline void sampleLambert(uniform float hitNormalX[], 
								 uniform float hitNormalY[], 
								 uniform float hitNormalZ[], 
								 uniform float outDirX[],
								 uniform float outDirY[],
								 uniform float outDirZ[], 
								 float* randoms, 
								 uniform int count)
{
	foreach(i = 0 ... count)
	{
		float signZ = (hitNormalZ[i] >= 0.0f) ? 1.0f : -1.0f;

		float a = -1.0f / (signZ + hitNormalZ[i]);
		float b = hitNormalX[i] * hitNormalY[i] * a;

		// vec3 b1; // = vec3(1.0f + signZ * hit_normal.x * hit_normal.x * a, signZ * b, -signZ * hit_normal.x);
		
		float b1X = 1.0f + signZ * hitNormalX[i] * hitNormalX[i] * a;
		float b1Y = signZ * b;
		float b1Z = -signZ * hitNormalX[i];

		// vec3 b2; // = vec3(b, signZ + hit_normal.y * hit_normal.y * a, -hit_normal.y);
		float b2X = b;
		float b2Y = signZ + hitNormalY[i] * hitNormalY[i] * a;
		float b2Z = -hitNormalY[i];

		float phi = 2.0f * PI * randoms[0];
		float cosTheta = sqrt(randoms[1]);
		float sinTheta = sqrt(1.0f - randoms[1]);

		float rX = (b1X * cos(phi) + b2X * sin(phi)) * cosTheta + hitNormalX[i] * sinTheta;
		float rY = (b1Y * cos(phi) + b2Y * sin(phi)) * cosTheta + hitNormalY[i] * sinTheta;
		float rZ = (b1Z * cos(phi) + b2Z * sin(phi)) * cosTheta + hitNormalZ[i] * sinTheta;

		float t = sqrt(rX * rX + rY * rY + rZ * rZ);

		outDirX[i] = rX * t;
		outDirY[i] = rY * t;
		outDirZ[i] = rZ * t;

		// return normalize(((b1 * cos(phi) + b2 * sin(phi)) * cosTheta + hit_normal * sinTheta));
	}
}

export void generate2dXYTiles(uniform float randoms[],
	const uniform float x, const uniform float y,
	uniform float xres, uniform float yres,
	uniform float camAspect, uniform float camScale,
	uniform float xOut[], uniform float yOut[], uniform float zOut[],
	uniform int count)
{
	foreach(index = 0 ... count)
	{
		const float rand1 = randoms[index];
		const float rand2 = randoms[index + 1];

		// const float dx = sqrt(-0.5 * log(rand1)) * cos(2.0 * PI * rand2);
		// const float dy = sqrt(-0.5 * log(rand1)) * sin(2.0 * PI * rand2);

		const float dx = lerp(-0.5, 0.5, rand1);
		const float dy = lerp(-0.5, 0.5, rand2);

		float x_ = x + index;

		xOut[index] = (2.0 * (x_ + dx) / xres - 1.0) * camAspect * camScale;
		yOut[index] = (1.0 - 2.0 * (y + dy) / yres) * camScale;
	}
}

export void generate2dXY(uniform float randoms[], 
						 const uniform int x,
						 const uniform int xres, const uniform int yres,
						 const uniform float camAspect, const uniform float camScale,
						 uniform float xOut[], uniform float yOut[], uniform float zOut[],
						 const uniform int count)
{
	foreach(index = 0 ... count)
	{
		const float rand1 = randoms[index];
		const float rand2 = randoms[(index + 1)];

		// const float dx = sqrt(-0.5 * log(rand1)) * cos(2.0 * PI * rand2);
		// const float dy = sqrt(-0.5 * log(rand1)) * sin(2.0 * PI * rand2);

		const float dx = lerp(-0.5, 0.5, rand1);
		const float dy = lerp(-0.5, 0.5, rand2);

		int x_ = (x + index) % xres;
		int y = (x + index) / xres;

		xOut[index] = (2.0 * (x_ + dx) / xres - 1.0) * camAspect * camScale;
		yOut[index] = (1.0 - 2.0 * (y + dy) / yres) * camScale;
	}
}

export void rayTransform(uniform float x[], 
				         uniform float y[], 
				   	     uniform float z[], 
				   	     const uniform mat44& matrix, 
				   	     uniform float outX[], 
				   	     uniform float outY[], 
				   	     uniform float outZ[],
				   	     uniform int count)
{
	transform(x, y, z, matrix, outX, outY, outZ, count);	
}

export void rayNormalize(uniform float origX[], 
						 uniform float origY[], 
						 uniform float origZ[],
						 uniform float worldX[],
						 uniform float worldY[],
						 uniform float worldZ[],
						 uniform float outX[],
						 uniform float outY[],
						 uniform float outZ[],
						 uniform int count)
{
	foreach(i = 0 ... count)
	{
		float tX = worldX[i] - origX[i];
		float tY = worldY[i] - origY[i];
		float tZ = worldZ[i] - origZ[i];

		float t = rsqrt(tX * tX + tY * tY + tZ * tZ);

		outX[i] = tX * t;
		outY[i] = tY * t;
		outZ[i] = tZ * t;
	}
}

export void raySet(uniform RTCRayHit &rays, uniform float posX, uniform float posY, uniform float posZ,
				   uniform float dirX[], uniform float dirY[], uniform float dirZ[], 
				   uniform float t, uniform int idx, uniform int count)
{

	foreach(i = 0 ... count)
	{
		rays.ray.org_x[idx + i] = posX;
		rays.ray.org_y[idx + i] = posY;
		rays.ray.org_z[idx + i] = posZ;

		rays.ray.dir_x[idx + i] = dirX[i];
		rays.ray.dir_y[idx + i] = dirY[i];
		rays.ray.dir_z[idx + i] = dirZ[i];

		rays.ray.tnear[idx + i] = 0.0001f;
		rays.ray.time[idx + i] = 0.0f;
		rays.ray.tfar[idx + i] = t;
		rays.ray.id[idx + i] = i;
		rays.ray.mask[idx + i] = -1;
		rays.ray.flags[idx + i] = 0;

		rays.hit.geomID[idx + i] = -1;
		// rays.hit.instID[0][idx + i] = -1;
	}
}				   

// export void generateDiffuseRays(uniform RTCRayHit rayhits[],
// 								uniform float randoms[],
// 								uniform int count)
// {
// 	foreach(i = 0 ... count)
// 	{
// 		cif(rayhits[i].hit.geomID == -1)
// 		{
// 			rayhits[i].ray.tnear = rayhits[i].ray.tfar + 10.0f;
// 			continue;
// 		}

// 		float* rand = &randoms[i];

// 		rayhits[i].ray.org_x = rayhits[i].ray.dir_x * rayhits[i].ray.tfar;
// 		rayhits[i].ray.org_y = rayhits[i].ray.dir_y * rayhits[i].ray.tfar;
// 		rayhits[i].ray.org_z = rayhits[i].ray.dir_z * rayhits[i].ray.tfar;

// 		vec3 normal;

// 		normal.x = rayhits[i].hit.Ng_x;
// 		normal.y = rayhits[i].hit.Ng_y;
// 		normal.z = rayhits[i].hit.Ng_z;

// 		vec3 direction = sampleLambert(normalize(normal), rand);

// 		rayhits[i].ray.dir_x = direction.x;
// 		rayhits[i].ray.dir_y = direction.y;
// 		rayhits[i].ray.dir_z = direction.z;
// 	}
// }