#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}

//初始化函数
bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	//初始化窗口
	InitializeWindows(screenWidth, screenHeight);

	//创建输入对象，用于读取键盘输入
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	//调用键盘初始化，即刷新所有按键为fasle
	m_Input->Initialize();

	//创建一个 图形对象，用于渲染
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	//初始化图形对象
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

//关闭程序时调用
void SystemClass::Shutdown()
{
	//如果有图形对象，则清除
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//如果有输入对象，则清除
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	//关闭窗口
	ShutdownWindows();

	return;
}

//创建完窗口后运行函数
void SystemClass::Run()
{
	//os消息变量
	MSG msg;
	bool done, result;


	//初始化消息结构体
	ZeroMemory(&msg, sizeof(MSG));

	
	done = false;

	//循环直到检测出退出消息
	while (!done)
	{
		//检测消息列队是否有消息，并返回
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//转换消息为字符串
			TranslateMessage(&msg);
			//将消息传递给处理接口 WndProc()
			DispatchMessage(&msg);
		}

		//如果系统发出结束消息则退出循环
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{		
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

	}

	return;
}

//帧检查
bool SystemClass::Frame()
{
	bool result;


	//检测是否有按下 ESC键
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//调用图形对象 Frame()
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

//关闭窗口
void SystemClass::ShutdownWindows()
{
	//显示鼠标指针
	ShowCursor(true);

	//如果是全屏窗口，则修复显示设置
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	//利用handle，移除窗口
	DestroyWindow(m_hwnd);
	//指针变量为空
	m_hwnd = NULL;

	//清除 程序 Instance handle
	UnregisterClass(m_applicationName, m_hinstance);
	//实力变量为空
	m_hinstance = NULL;

	//释放指针类
	ApplicationHandle = NULL;

	return;
}

//检测os消息
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//检测键盘上是否有键按下
		case WM_KEYDOWN:
		{
			//如果按下某关键，则返回该键值
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		//检测键盘是否有键被释放
		case WM_KEYUP:
		{
			//返回释放键值
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		//处理其他默认消息
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}



//消息处理
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//窗口销毁
	case WM_DESTROY:
	{
		//向系统指示线程已请求终止
		PostQuitMessage(0);
		return 0;
	}

	//窗口关闭
	case WM_CLOSE:
	{
		//向系统指示线程已请求终止
		PostQuitMessage(0);
		return 0;
	}

	//转递其他消息
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}


//初始化窗口
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	//窗口信息结构体，包含窗口大小，标题，图标等信息。
	WNDCLASSEX wc;
	//显示设备相关设置结构体。
	DEVMODE dmScreenSettings;
	//窗口坐标变量
	int posX, posY;

	//获得应用Handle
	ApplicationHandle = this;

	//获取应用的Instance Handle
	m_hinstance = GetModuleHandle(NULL);

	//程序命名
	m_applicationName = "Engine";

	//使用默认设置配置初始化
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	//注册窗口类.
	RegisterClassEx(&wc);

	//获取客户端桌面分辨率
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//设置全屏与窗口模式
	if (FULL_SCREEN)
	{
		//将屏幕设置为最大尺寸
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//修改显示设置为全屏
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//将窗口位置设置为左上角
		posX = posY = 0;
	}
	else
	{
		//窗口模式时的大小
		screenWidth = 800;
		screenHeight = 600;

		//将窗口位置放在屏幕中间
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	//创建窗口，并返回handle
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	//把窗口显示在屏幕上，并带入handle设为焦点
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//隐藏鼠标指针
	ShowCursor(false);

	return;
}

