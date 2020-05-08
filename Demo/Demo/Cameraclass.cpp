#include "Cameraclass.h"



Cameraclass::Cameraclass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

Cameraclass::Cameraclass(const Cameraclass& other)
{
}


Cameraclass::~Cameraclass()
{
}

void Cameraclass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void Cameraclass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 Cameraclass::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


XMFLOAT3 Cameraclass::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Cameraclass::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// 设置指向上方的向量。
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// 将其加载到XMVECTOR结构中
	upVector = XMLoadFloat3(&up);

	//设置默认情况下相机所处的位置。
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	//将其加载到XMVECTOR结构中。
	positionVector = XMLoadFloat3(&position);

	//设置默认情况下相机所处的位置。
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	// 将其加载到XMVECTOR结构中。
	lookAtVector = XMLoadFloat3(&lookAt);

	// 以弧度为单位设置偏航（Y轴），俯仰（X轴）和滚动（Z轴）旋转。
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// 根据偏航，俯仰和横滚值创建旋转矩阵
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// 通过旋转矩阵转换lookAt和上向量，以便在原点正确旋转视图
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// 将旋转的相机位置平移到查看器的位置。
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// 最后，根据三个更新的向量创建视图矩阵。
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Cameraclass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}