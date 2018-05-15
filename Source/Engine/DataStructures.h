//	DX11Renderer - VDemo | DirectX11 Renderer
//	Copyright(C) 2016  - Volkan Ilbeyli
//
//	This program is free software : you can redistribute it and / or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see <http://www.gnu.org/licenses/>.
//
//	Contact: volkanilbeyli@gmail.com
#pragma once

#include "Utilities/vectormath.h"

#include <array>
#include <sstream>
#include <iomanip>

struct PointLightGPU	// 48 Bytes | 3 registers
{	
	vec3 position;
	float  range;
	vec3 color;
	float  brightness;
	vec2 attenuation;
	vec2 padding;
};

struct SpotLightGPU		// 48 bytes | 3 registers
{
	vec3 position;
	float  halfAngle;
	vec3 color;
	float  brightness;
	vec3 spotDir;
	float padding;
};

struct DirectionalLightGPU // 28(+4) Bytes | 2 registers
{
	vec3 lightDirection;
	float  brightness;
	vec3 color;
};

//struct ShadowView
//{
//	TextureID shadowMap = -1;
//	SamplerID shadowSampler = -1;
//	XMMATRIX  lightSpaceMatrix;
//};

#define NUM_POINT_LIGHT 100
#define NUM_SPOT_LIGHT 20
#define NUM_DIRECTIONAL_LIGHT 4
using PointLightDataArray		= std::array<PointLightGPU, NUM_POINT_LIGHT>;
using SpotLightDataArray		= std::array<SpotLightGPU, NUM_SPOT_LIGHT>;
using DirectionalLightDataArray = std::array<DirectionalLightGPU, NUM_DIRECTIONAL_LIGHT>;

#define NUM_POINT_LIGHT_SHADOW 5
#define NUM_SPOT_LIGHT_SHADOW 5
#define NUM_DIRECTIONAL_LIGHT_SHADOW 4
using ShadowingPointLightDataArray			= std::array<PointLightGPU, NUM_POINT_LIGHT_SHADOW>;
using ShadowingSpotLightDataArray			= std::array<SpotLightGPU, NUM_SPOT_LIGHT_SHADOW>;
using ShadowingDirectionalLightDataArray	= std::array<DirectionalLightGPU, NUM_DIRECTIONAL_LIGHT_SHADOW>;

using SpotShadowViewArray = std::array<XMMATRIX, NUM_SPOT_LIGHT_SHADOW>;

//#pragma pack(push, 1)
struct SceneLightingData
{
	struct cb{	// shader constant buffer
		int       pointLightCount;
		int        spotLightCount;
		int directionalLightCount;

		int       pointLightCount_shadow;
		int        spotLightCount_shadow;
		int directionalLightCount_shadow;
		int _pad0, _pad1;

		PointLightDataArray pointLights;
		ShadowingPointLightDataArray pointLightsShadowing;

		SpotLightDataArray spotLights;
		ShadowingSpotLightDataArray spotLightsShadowing;

		// todo directional
		// todo directional shadowing

		SpotShadowViewArray shadowViews;
	} _cb;


	inline void ResetCounts() {
		_cb.pointLightCount = _cb.spotLightCount = _cb.directionalLightCount =
		_cb.pointLightCount_shadow = _cb.spotLightCount_shadow = _cb.directionalLightCount_shadow = 0;
	}
};
//#pragma pack(pop)

class TextRenderer;
struct TextDrawDescription;

struct RendererStats
{
	union
	{
		struct  
		{
			int numVertices;
			int numIndices;
			int numDrawCalls;
		};
		int arr[3];
	};
};
struct FrameStats
{
	static const size_t numStat = 5;
	union 
	{
		struct
		{
			int numSceneObjects;
			int numCulledObjects;
			int numDrawCalls;
			int numVertices;
			int numIndices;
		};
		struct
		{
			int numSceneObjects;
			int numCulledObjects;
			RendererStats rstats;
		};
		int stats[numStat];
	};

	bool bShow;
	void Render(TextRenderer* pTextRenderer, const vec2& screenPosition, const TextDrawDescription& drawDesc);	// implementation in Engine.cpp
	static const char* statNames[numStat];
};

#include "PerfTree.h"
