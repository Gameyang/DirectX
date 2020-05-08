#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

//链接 DirextX11 设置和绘制3D图形的所有功能
#pragma comment(lib, "d3d11.lib")
//链接 硬件接口工具，获得显示器，显卡，刷新率等信息
#pragma comment(lib, "dxgi.lib")
//链接 用于编译Shader的功能
#pragma comment(lib, "d3dcompiler.lib")

//引用 Dirext 接口定义
#include <d3d11.h>
//引用 DirectX 数学库，是用户游戏图形的 c++ 线性代数库
#include <directxmath.h>

using namespace DirectX;

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
};

#endif