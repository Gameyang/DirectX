#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//移除window.h头文件，改用d3dclass.h
//#include <windows.h>
#include "d3dclass.h"

#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include "Textureshaderclass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	//添加 D3Dclass 的 私有指针
	D3DClass* m_Direct3D;

	Cameraclass* m_Camera;
	Modelclass* m_Model;
	Colorshaderclass* m_ColorShader;

	TextureShaderClass* m_TextureShader;
};

#endif