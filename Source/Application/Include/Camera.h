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

#ifndef _CAMERA_H
#define _CAMERA_H

#include <directxmath.h>
#include "Components/Transform.h"	// dependency. rethink this

using namespace DirectX;

class Input;

//__declspec(align(16)) class Camera
class Camera
{
public:
	Camera(Input const*);
	~Camera(void);

	void SetOthoMatrix(int screenWidth, int screenHeight, float screenNear, float screenFar);
	void SetProjectionMatrix(float fov, float screenAspect, float screenNear, float screenFar);

	void Update(float dt);

	XMFLOAT3 GetPositionF() const;
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;
	XMMATRIX GetOrthoMatrix() const;
	XMMATRIX RotMatrix() const;

	void SetPosition(float x, float y, float z);
	void Rotate(float yaw, float pitch, const float dt);

public:
	float Drag;				// 15.0f
	float AngularSpeedDeg;	// 40.0f
	float MoveSpeed;		// 1000.0f

private:
	void Move(const float dt);
	void Rotate(const float dt);

private:
	XMFLOAT3	m_pos;
	XMFLOAT3	m_velocity;
	float		m_yaw, m_pitch;

	XMFLOAT4X4	m_viewMatrix;
	XMFLOAT4X4	m_projectionMatrix;
	XMFLOAT4X4	m_orthoMatrix;

	Input const* m_input;
};

#endif