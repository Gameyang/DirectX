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


	//ôøã·ûùá¶êó??£¬áìãæá¶êóäÎ?? false??
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
{
	//??äÎù»îÜ?? ??
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	//áæ????£¬???ñé?Û¯???
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	//Ú÷üŞäÎ???£¬äÎù»ûäÚ±äÎù»
	return m_keys[key];
}