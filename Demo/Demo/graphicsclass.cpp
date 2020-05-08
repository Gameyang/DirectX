#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	//初始化指针为0
	m_Direct3D = 0;

	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//创建 D3D对象
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	//初始化 D3D对象
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D", "Error", MB_OK);
		return false;
	}

	// 创建相机对象。
	m_Camera = new Cameraclass;
	if (!m_Camera)
	{  
		return false;
	}

	// 设置摄像机的初始位置。
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	// 创建模型对象。
	m_Model = new Modelclass;
	if (!m_Model)
	{
		return false;
	}

	//初始化模型对象。
	const char *texPath = "tex.jpg";
	if (!texPath) {
		MessageBox(hwnd, "Could not find texture.", "Error", MB_OK);
		return false;
	}
	char* texFile = const_cast<char *>(texPath);
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), texFile);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}
	/*
	// 创建颜色着色器对象。
	m_ColorShader = new Colorshaderclass;
	if (!m_ColorShader)
	{
		return false;
	}

	// 初始化颜色着色器对象。
	//result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);

	char* texFile = const_cast<char*>("uv-mapping.jpg");
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), texFile);
	if (!result)
	{
		MessageBox(hwnd, "Could not initialize the color shader object.", "Error", MB_OK);
		return false;
	}*/

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{

		MessageBox(hwnd, "Could not initialize the color shader object.", "Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	/*
	//清除缓冲
	m_Direct3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);


	//渲染在屏幕上
	m_Direct3D->EndScene();
	*/

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// 释放颜色着色器对象。
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// 释放模型对象
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// 释放相机对象。
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// 释放D3D对象。
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;


	// 渲染图形场景。
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// 清除缓冲区以开始场景。
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 根据相机的位置生成视图矩阵。
	m_Camera->Render();

	// 从相机和d3d对象获取世界，视图和投影矩阵。
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 将模型顶点和索引缓冲区放在图形管线上，以准备进行绘制。
	m_Model->Render(m_Direct3D->GetDeviceContext());

	/*
	// 使用颜色着色器渲染模型。
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}
	*/
	// Render the model using the texture shader.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	// 将渲染的场景呈现到屏幕上。
	m_Direct3D->EndScene();

	return true;
}