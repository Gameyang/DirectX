#include "systemclass.h"

//程序入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	//创建系统类指针变量
	SystemClass* System;

	//初始化检测开关
	bool result;

	//创新systemclass类，并赋值给system指针
	System = new SystemClass;
	
	//如果创建失败则返回0
	if (!System)
	{
		return 0;
	}

	//调用Initialize函数，初始化systemclass，并返回初始化预后结果
	result = System->Initialize();

	//检测是否初始化成功，并运行
	if (result)
	{
		System->Run();
	}

	//关闭并释放内存
	System->Shutdown();
	delete System;
	System = 0;

	return 0;
}