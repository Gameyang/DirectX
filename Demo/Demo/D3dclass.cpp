#include "d3dclass.h"

//对struct赋null值
D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

//初始化
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
	float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;

	// 储存 vsync 设置的变量，用来计算刷新率。为了避免DirectX将通过执行blit而不是缓冲区翻转来响应，这会降低性能并在调试输出中给我们带来烦人的错误。
	m_vsync_enabled = vsync;

	//创建 DirectX 图形工厂
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//利用工厂创建与显卡的连接器
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 枚举与显示器的连接器.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// 获取 DXGI_FORMAT_R8G8B8A8_UNORM 显示格式列表.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 创建一个列表来储存 显卡与显示器 的所有可能显示模式
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 填充显示模式列表
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 遍历所有显示模式，找到与屏幕匹配的尺寸
	// 找到匹配后，储存该显示模式刷新率的分子与分母
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 获取显卡描述,名称
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 以MB为单位，存储显卡内存
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 将显卡名称 转换为字符串 并 储存
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//清空显示模式列表
	delete[] displayModeList;
	displayModeList = 0;

	//清空输出连接器
	adapterOutput->Release();
	adapterOutput = 0;

	//情况连接器
	adapter->Release();
	adapter = 0;

	//情况工厂
	factory->Release();
	factory = 0;

	//初始化 swapChain
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//设置为 single back buffer.
	swapChainDesc.BufferCount = 1;

	// 设置 back buffer 的尺寸
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 设置 back buffer 的格式
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 设置 back buffer 的刷新率.刷新速率是每秒将后缓冲区拉到前缓冲区的次数。如果在我们的graphicsclass.h标头中将vsync设置为true，则这会将刷新率锁定到系统设置，但是，如果将vsync设置为false，则它将每秒尽可能多地绘制屏幕一次，但是这可能会导致一些视觉失真。
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 设置 back buffer 的方式.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 设置窗口的 handle
	swapChainDesc.OutputWindow = hwnd;

	// 关闭多重采样
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 设置为全屏 或 窗口模式
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 将 scan line 尺寸与顺序设置为 未指定
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 丢弃 back buffer 呈现之后的内容
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 不设置高级标签
	swapChainDesc.Flags = 0;

	// 将功能级别设置为 DirectX 11.此变量告诉DirectX我们计划使用哪个版本。如果计划支持多个版本或在较低端的硬件上运行，则可以将此值设置为10或9以使用较低级别的DirectX。
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 配置完毕，开始创建 swap chain, Direct3D device, 与 Direct3D device context.
	//请注意，如果用户没有DirectX 11视频卡，则此函数调用将无法创建device和device context。
	//可以将D3D_DRIVER_TYPE_HARDWARE替换为D3D_DRIVER_TYPE_REFERENCE，DirectX将使用您的CPU绘制而不是视频卡硬件
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// 获取指向back buffer的指针
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 使用back buffer指针 创建 render target。将back buffer附加到swapChain。
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 释放back buffer指针，因为我们不再需要它
	backBufferPtr->Release();
	backBufferPtr = 0;

	// 初始化depth buffer，我们将使用它来创建深度缓冲区，以便可以在3D空间中正确渲染多边形
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	//设置depth buffer的描述，
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 使用 filled out 为 depth buffer 创建纹理
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 初始化 stencil.可用于实现诸如运动模糊，体积阴影和其他事物的效果。
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 设置 stencil 状态.这使我们可以控制Direct3D对每个像素执行何种深度测试。
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 如果像素是正面的
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 如果像素是背面的
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 创建 depth stencil 状态.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 设置 depth stencil 状态.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 初始化 depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 设置 depth stencil view.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 创建 depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 绑定 render target view 与 depth stencil buffer 到渲染管道
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 设置 raster 该栅格描述将确定如何绘制多边形以及绘制哪些多边形
	//这将使我们能够控制多边形的渲染方式。例如使场景以线框模式渲染或让DirectX绘制多边形的正面和背面
	//默认情况下，DirectX已经设置了光栅化器状态，​​并且与以下状态完全相同，但是除非您自己进行设置，否则您无权对其进行更改。
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 创建 rasterizer 状态
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 设置 rasterizer 状态.
	m_deviceContext->RSSetState(m_rasterState);

	// 配置试图窗口以进行渲染，以便Direct3D可以将剪辑空间坐标映射到渲染目标空间。将其设置为窗口的整个大小。
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//创建视图窗口
	m_deviceContext->RSSetViewports(1, &viewport);

	//设置投影矩阵，投影矩阵用于将3D场景转换为我们先前创建的2D视口空间。我们将需要保留此矩阵的副本，以便可以将其传递到用于渲染场景的着色器。
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//创建用于3D渲染的投影矩阵。此矩阵用于将对象的顶点转换为3D场景中的顶点。该矩阵还将用于在3D空间中旋转，平移和缩放我们的对象。
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// 将 world matrix 初始化为 identity matrix.从一开始，我们就将矩阵初始化为单位矩阵，并将其副本保留在该对象中。
	//该副本将需要传递到着色器以进行渲染。
	//视图矩阵用于计算从中查看场景的位置。您可以将其视为摄像机，并且只能通过此摄像机查看场景。
	m_worldMatrix = XMMatrixIdentity();

	// 创建用于2D渲染的正交投影矩阵。此矩阵用于在屏幕上渲染2D元素（如用户界面），使我们可以跳过3D渲染
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

//Shutdown函数将释放并清理Initialize函数中使用的所有指针
void D3DClass::Shutdown()
{
	// 为了避免引发异常，在关闭之前将其设置为窗口模式。
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}


//每当我们要在每个帧的开头绘制一个新的3D场景时，就会调用BeginScene,它所做的只是初始化缓冲区，使它们为空白并准备绘制
void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// 设置颜色以清除缓冲区。
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// 清除 back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 清除 depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

//另一个功能是Endscene，它告诉交换链在每帧结束时完成所有绘图后显示3D场景。
void D3DClass::EndScene()
{
	//由于渲染完成，因此将back buffer示在屏幕上
	if (m_vsync_enabled)
	{
		// 锁定屏幕刷新率
		m_swapChain->Present(1, 0);
	}
	else
	{
		// 不限制刷新率
		m_swapChain->Present(0, 0);
	}

	return;
}

//获取 device  指针
ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}

//获取 device context.  指针
ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

//提供 projectionMatrix 给调用函数
void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}

//提供 worldMatrix 给调用函数
void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}

//提供 orthoMatrix 给调用函数
void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

//通过引用返回视频卡的名称和视频内存量。知道视频卡名称可以帮助调试不同的配置。
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}
