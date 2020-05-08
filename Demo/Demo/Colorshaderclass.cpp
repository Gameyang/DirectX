#include "Colorshaderclass.h"
#include <atlconv.h>

Colorshaderclass::Colorshaderclass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}


Colorshaderclass::Colorshaderclass(const Colorshaderclass& other)
{
}


Colorshaderclass::~Colorshaderclass()
{
}


bool Colorshaderclass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	WCHAR vs[256] = L"color.vs";
	WCHAR ps[256] = L"color.ps";

	// 初始化 vertex 与 pixel shaders.
	result = InitializeShader(device, hwnd, vs,ps);
	if (!result)
	{
		return false;
	}

	return true;
}

void Colorshaderclass::Shutdown()
{
	// 关闭 vertex 与 pixel shaders 以及相关对象。
	ShutdownShader();

	return;
}

bool Colorshaderclass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	bool result;


	//设置将用于渲染的着色器参数。
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	//现在，使用着色器渲染准备好的buffers 。
	RenderShader(deviceContext, indexCount);

	return true;
}


//加载着色器文件并使其可用于DirectX和GPU的功能。
bool Colorshaderclass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	USES_CONVERSION;

	//初始化此函数将使用的指针为null。
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//在这里我们将着色器程序编译到缓冲区中。
	//编译顶点着色器代码。
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 如果着色器无法编译，则应该在错误消息中写入了一些内容。
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 如果错误消息中没有任何内容，则它根本找不到着色器文件本身。
		else
		{
			LPCSTR vsfile = W2A(vsFilename);
			MessageBox(hwnd, vsfile, "Missing Shader File", MB_OK);
		}

		return false;
	}

	// 编译像素着色器代码。
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 如果着色器无法编译，则应该在错误消息中写入了一些内容。
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 如果错误消息中没有任何内容，则它根本找不到文件本身。
		else
		{
			LPCSTR psfile = W2A(psFilename);
			MessageBox(hwnd, psfile, "Missing Shader File", MB_OK);
		}

		return false;
	}

	// 将顶点着色器和像素着色器代码成功编译到缓冲区后，我们便可以使用这些缓冲区来创建着色器对象本身。
	// 从缓冲区创建顶点着色器。
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 从缓冲区创建像素着色器。
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 下一步是创建将由着色器处理的顶点数据的layout。
	// 由于此着色器使用position和color vector，因此我们需要在layout中创建两者并指定两者的大小。
	// 对于位置，我们使用DXGI_FORMAT_R32G32B32_FLOAT，对于颜色，我们使用DXGI_FORMAT_R32G32B32A32_FLOAT。
	// AlignedByteOffset，它指示缓冲区中数据的间隔方式,我们告诉它前12个字节为位置，接下来的16个字节为彩色，
	// 创建vertex input layout 描述。
	// 此设置需要匹配ModelClass和着色器中的VertexType结构体。
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 获取layout中元素的数量。
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 创建顶点InputLayout。
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// 释放顶点着色器缓冲区和像素着色器缓冲区，因为不再需要它们。
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 需要设置以使用着色器的最后一件事是常量缓冲区。
	// 我们目前只有一个常量缓冲区，因此我们只需要在此处设置一个常量即可与着色器进行交互。
	// 需要将缓冲区使用率设置为动态，因为我们将在每个帧中对其进行更新。
	//填写完描述后，我们可以创建常量缓冲区接口，然后使用SetShaderParameters函数使用该接口访问着色器中的内部变量。
	// 设置顶点着色器中动态矩阵常量缓冲区的描述。
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 创建常量缓冲区指针，以便我们可以从此类内访问顶点着色器常量缓冲区。
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

//释放在InitializeShader函数中设置的四个接口。
void Colorshaderclass::ShutdownShader()
{
	// 释放 matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 释放 layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 释放 pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 释放 vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

//OutputShaderErrorMessage会写出编译顶点着色器或像素着色器时生成的错误消息。
void Colorshaderclass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// 获取指向错误消息文本缓冲区的指针。
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 获取消息的长度。
	bufferSize = errorMessage->GetBufferSize();

	// 打开一个文件，将错误消息写入其中。
	fout.open("shader-error.txt");

	// 写入错误消息。
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 关闭文件。
	fout.close();

	// 释放 error message.
	errorMessage->Release();
	errorMessage = 0;

	// 在屏幕上弹出一条消息，通知用户检查文本文件是否存在编译错误。
	USES_CONVERSION;
	LPCSTR shaderfile = W2A(shaderFilename);
	MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderfile, MB_OK);

	return;
}

//简化着色器中的全局变量设置。此函数中使用的矩阵是在GraphicsClass内部创建的
//之后，将在调用Render函数期间调用此函数以将其从那里发送到顶点着色器。
bool Colorshaderclass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//在将矩阵发送到着色器之前，请确保对其进行转置，这是DirectX 11的要求。
	// 转置矩阵以为着色器做准备。
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 锁定常量缓冲区，以便可以对其进行写入。
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 获取指向常量缓冲区中数据的指针。
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 将矩阵复制到常量缓冲区中。
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 解锁常量缓冲区。
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 设置常量着色器在顶点着色器中的位置。
	bufferNumber = 0;

	// 最后在顶点着色器中使用更新后的值设置常量缓冲区。
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
}

//在此之前，将调用SetShaderParameters以确保正确设置着色器参数。
//此功能的第一步是将输入布局设置为在输入汇编器中处于活动状态。这使GPU知道顶点缓冲区中的数据格式。
void Colorshaderclass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 设置 vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// 设置将用于渲染此三角形的顶点和像素着色器
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 渲染三角形。
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
