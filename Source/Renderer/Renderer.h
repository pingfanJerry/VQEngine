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

#include "RenderCommands.h"
#include "RenderingEnums.h"
#include "Engine/Settings.h"
#include "Texture.h"
#include "Shader.h"
#include "Utilities/Color.h"

#include "Utilities/utils.h"
//#include "WorkerPool.h"

#include <string>
#include <vector>
#include <stack>
#include <queue>

class BufferObject;
class Camera;
class D3DManager;

namespace DirectX  { class ScratchImage; }


// todo struct?
using Viewport = D3D11_VIEWPORT;
using RasterizerState   = ID3D11RasterizerState;
using DepthStencilState = ID3D11DepthStencilState;
using RenderTargetIDs	= std::vector<RenderTargetID>;

struct BlendState
{
	BlendState() : ptr(nullptr) {}
	ID3D11BlendState* ptr; 
};

struct RenderTarget
{
	RenderTarget() : pRenderTargetView(nullptr) {}
	ID3D11Resource*	GetTextureResource() const { return texture._tex2D; }
	//--------------------------------------------
	Texture						texture;
	ID3D11RenderTargetView*		pRenderTargetView;
};

struct DepthTarget
{
	//--------------------------------------------
	Texture						texture;
	ID3D11DepthStencilView*		pDepthStencilView;
};

struct PipelineState
{
	ShaderID			_activeShader;
	InputBufferID		_activeInputBuffer;
	RasterizerStateID	_activeRSState;
	DepthStencilStateID	_activeDepthStencilState;
	RenderTargetIDs		_boundRenderTargets;
	DepthTargetID		_boundDepthTarget;
	RenderTargetID		_mainRenderTarget;		// this doesn't belong here
	BlendStateID		_activeBlendState;
	TextureID			_depthBufferTexture;	// ^
};

enum ETextureUsage 
{
	GPU_READ = D3D11_BIND_SHADER_RESOURCE,
	GPU_WRITE = D3D11_BIND_RENDER_TARGET,
	GPU_RW = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
	
	TEXTURE_USAGE_COUNT
};

struct TextureDesc
{
	int width;
	int height;
	EImageFormat format;
	ETextureUsage usage;
	std::string	 texFileName;
	void* data;
	int mipCount;

	TextureDesc() : width(1), height(1), format(RGBA32F), usage(GPU_READ), texFileName(""), data(nullptr), mipCount(1) {}

	D3D11_TEXTURE2D_DESC dxDesc;
};

class Renderer
{
	friend class Engine;		
	friend class SceneManager;
	friend class Shader;		// shader load/unload

	friend struct SetTextureCommand;
	friend struct SetSamplerCommand;	// todo: refactor commands - don't use friend for commands

public:
	// use > tabs w/ 4spaces
	Renderer();
	~Renderer();
	bool					Initialize(HWND hwnd, const Settings::Window& settings);
	void					Exit();
	inline void				ReloadShaders() { Shader::ReloadShaders(this); }

	// GETTERS
	//----------------------------------------------------------------------------------
	HWND					GetWindow()		const; 
	float					AspectRatio()	const;
	unsigned				WindowHeight()	const;
	unsigned				WindowWidth()	const;
	vec2					GetWindowDimensionsAsFloat2() const;
	inline RenderTargetID	GetDefaultRenderTarget() const	                { return m_state._mainRenderTarget; }
	inline DepthTargetID	GetBoundDepthTarget() const	                    { return m_state._boundDepthTarget; }
	inline TextureID		GetDefaultRenderTargetTexture() const           { return m_renderTargets[m_state._mainRenderTarget].texture._id; }
	inline TextureID		GetRenderTargetTexture(RenderTargetID RT) const { return m_renderTargets[RT].texture._id; }
	inline TextureID		GetDepthTargetTexture(DepthTargetID DT) const   { return m_depthTargets[DT].texture._id; }
	const PipelineState&	GetState() const;

	const Shader*			GetShader(ShaderID shader_id) const;
	const Texture&			GetTextureObject(TextureID) const;
	const TextureID			GetTexture(const std::string name) const;
	inline const ShaderID	GetActiveShader() const { return m_state._activeShader; }


	// RESOURCE INITIALIZATION (testing readability of function definitions)
	//----------------------------------------------------------------------------------
	ShaderID				AddShader(	const std::string&				shaderFileName, 
										const std::vector<InputLayout>& layouts			
							);

	ShaderID				AddShader(	const std::string&				shaderName, 
										const std::vector<std::string>&	shaderFileNames, 
										const std::vector<EShaderType>&	shaderTypes, 
										const std::vector<InputLayout>&	layouts
							);

	//						example params:			"bricks_d.png", "Data/Textures/"
	TextureID				CreateTextureFromFile(	const std::string&	texFileName, 
													const std::string&	fileRoot = sTextureRoot
							);

	TextureID				CreateTexture2D(const TextureDesc& texDesc);

	TextureID				CreateTexture2D(	D3D11_TEXTURE2D_DESC&	textureDesc, 
												bool					initializeSRV
							);	// used by AddRenderTarget()

	TextureID				CreateHDRTexture(	const std::string&	texFileName,
												const std::string&	fileRoot = sHDRTextureRoot
							);

	TextureID				CreateCubemapTexture(	const std::vector<std::string>& textureFiles
							);
	
	TextureID				CreateDepthTexture( unsigned width, 
												unsigned height, 
												bool bDepthOnly
							);

	SamplerID				CreateSamplerState(	D3D11_SAMPLER_DESC&	samplerDesc
							);


	RasterizerStateID		AddRasterizerState(	ERasterizerCullMode cullMode, 
												ERasterizerFillMode fillMode, 
												bool bEnableDepthClip, 
												bool bEnableScissors
							);

	DepthStencilStateID		AddDepthStencilState(	bool bEnableDepth, 
													bool bEnableStencil
							);	// todo params

	DepthStencilStateID		AddDepthStencilState(const D3D11_DEPTH_STENCIL_DESC&	dsDesc
							);

	BlendStateID			AddBlendState( // todo params
							);

	// initializes the texture data with the given desc
	RenderTargetID			AddRenderTarget(	D3D11_TEXTURE2D_DESC&			RTTextureDesc, 
												D3D11_RENDER_TARGET_VIEW_DESC&	RTVDesc
							);

	// uses the given texture object, doesn't create a new texture for the render target
	RenderTargetID			AddRenderTarget(	const Texture& textureObj, 
												D3D11_RENDER_TARGET_VIEW_DESC&	RTVDesc
							);

	DepthTargetID AddDepthTarget(	const D3D11_DEPTH_STENCIL_VIEW_DESC& dsvDesc, 
											Texture& surface
											);

	// PIPELINE STATE MANAGEMENT
	//----------------------------------------------------------------------------------
	void					SetViewport(const unsigned width, const unsigned height);
	void					SetViewport(const D3D11_VIEWPORT& viewport);
	void					SetShader(ShaderID);
	void					SetBufferObj(int BufferID);
	void					SetTexture(const char* texName, TextureID tex);
	void					SetSamplerState(const char* samplerName, SamplerID sampler);
	void					SetRasterizerState(RasterizerStateID rsStateID);
	void					SetBlendState(BlendStateID blendStateID);
	void					SetDepthStencilState(DepthStencilStateID depthStencilStateID);
	void					SetScissorsRect(int left, int right, int top, int bottom);

	template <typename... Args> 	
	inline void				BindRenderTargets(Args const&... renderTargetIDs) { m_state._boundRenderTargets = { renderTargetIDs... }; }
	void					BindRenderTarget(RenderTargetID rtvID);
	void					BindDepthTarget(DepthTargetID dsvID);

	void					UnbindRenderTargets();
	void					UnbindDepthTarget();

	void					SetConstant4x4f(const char* cName, const XMMATRIX& matrix);
	inline void				SetConstant3f(const char* cName, const vec3& float3)	{ SetConstant(cName, static_cast<const void*>(&float3.x())); }
	inline void				SetConstant2f(const char* cName, const vec2& float2)	{ SetConstant(cName, static_cast<const void*>(&float2.x())); }
	inline void				SetConstant1f(const char* cName, const float& data)		{ SetConstant(cName, static_cast<const void*>(&data)); }
	inline void				SetConstant1i(const char* cName, const int& data)		{ SetConstant(cName, static_cast<const void*>(&data)); }
	inline void				SetConstantStruct(const char * cName, const void* data) { SetConstant(cName, data); }

	void					Begin(const float clearColor[4], const float depthValue);
	void					Begin(const ClearCommand& clearCmd);
	void					End();
	void					Reset();

	void					Apply();

	void					BeginEvent(const std::string& marker);
	void					EndEvent();
	// DRAW FUNCTIONS
	//----------------------------------------------------------------------------------
	void					DrawIndexed(EPrimitiveTopology topology = EPrimitiveTopology::TRIANGLE_LIST);
	void					Draw(EPrimitiveTopology topology = EPrimitiveTopology::POINT_LIST);
	
	void					DrawQuadOnScreen(const DrawQuadOnScreenCommand& cmd); // BottomLeft<x,y> = (0,0)
	
	void					DrawLine();
	void					DrawLine(const vec3& pos1, const vec3& pos2, const vec3& color = LinearColor().Value());

private:
	void					SetConstant(const char* cName, const void* data);
//==================================================================================================================================================
public:
	static const char*				sShaderRoot;
	static const char*				sTextureRoot;
	static const char*				sHDRTextureRoot;

	ID3D11Device*					m_device;
	ID3D11DeviceContext*			m_deviceContext;
	D3DManager*						m_Direct3D;

	static bool						sEnableBlend; //temp
private:	
	Settings::Window				mWindowSettings;

	std::vector<BufferObject*>		m_bufferObjects;
	std::vector<Shader*>			m_shaders;
	std::vector<Texture>			m_textures;
	std::vector<Sampler>			m_samplers;

	std::queue<SetTextureCommand>	m_setTextureCmds;
	std::queue<SetSamplerCommand>	m_setSamplerCmds;

	std::vector<RasterizerState*>	m_rasterizerStates;
	std::vector<DepthStencilState*> m_depthStencilStates;
	std::vector<BlendState>			m_blendStates;

	std::vector<RenderTarget>		m_renderTargets;
	std::vector<DepthTarget>		m_depthTargets;

	Viewport						m_viewPort;

	PipelineState					m_state;
	
	// performance counters
	unsigned long long				m_frameCount;

	//std::vector<Point>				m_debugLines;
	
	//Worker		m_ShaderHotswapPollWatcher;

};
 