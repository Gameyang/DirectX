﻿#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

//加速build速度
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "inputclass.h"
#include "graphicsclass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	//检测os消息
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	//定义两个class私有指针变量
	InputClass* m_Input;
	GraphicsClass* m_Graphics;
};

//处理消息
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;

#endif