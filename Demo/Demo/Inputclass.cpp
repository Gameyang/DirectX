#include "inputclass.h"


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	int i;


	//��������??�����������?? false??
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
{
	//??������?? ??
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	//��????��???��?ۯ???
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	//������???��������ڱ����
	return m_keys[key];
}