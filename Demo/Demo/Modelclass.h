#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

#include "textureclass.h"

//ModelClass负责封装3D模型的几何图形。
//我们还将为三角形创建一个顶点和索引缓冲区，以便可以对其进行渲染。
class Modelclass
{
private:

	//它将与该ModelClass中的顶点缓冲区一起使用,此typedef必须与ColorShaderClass中的布局匹配
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

	//这里的函数处理模型的顶点和索引缓冲区的初始化和关闭。
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	//请注意，所有DirectX 11缓冲区通常都使用通用ID3D11Buffer类型，并且在首次创建时通过缓冲区描述会更清楚地标识它们。
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	//顶点和索引缓冲区以及两个整数，用于跟踪每个缓冲区的大小。
	int m_vertexCount, m_indexCount;

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();
	TextureClass* m_Texture;

public:
	Modelclass();
	Modelclass(const Modelclass&);
	~Modelclass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();
};

#endif