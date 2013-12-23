#pragma once

#include "stdafx.h"
#include "vertextypes.hpp"

using namespace DirectX;

extern unsigned char* GetByteArrayFromFile(std::string filename,unsigned int& length);


class XGame {
private:
	unsigned int m_ScreenWidth;
	unsigned int m_ScreenHeight;
	HWND* m_HWindow;
	// d3d variable
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DevContext;
	IDXGISwapChain* m_SwapChain;
	ID3D11RenderTargetView* m_RenderTarget;
	// draw variable
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;
	ID3D11Buffer* m_ConstantBuffer;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_InputLayout;
	unsigned long m_VertexCount;
	unsigned long m_IndexCount;
	double m_TotalTime;
	mvp_constantbuffer m_ConstantBufferData;
public :
	XGame() {
	}
	~XGame() {
	}

	void Initialize(HWND& hwindow, unsigned int sheight, unsigned int swidth) {
		// ----- #step01 ----- create device & swap-chain
		m_HWindow = &hwindow;
		m_ScreenWidth  = swidth;
		m_ScreenHeight = sheight;

		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_0 };
		// create swapchain & device
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc.Width  = (UINT) m_ScreenWidth;
		swapChainDesc.BufferDesc.Height = (UINT) m_ScreenHeight;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		// swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = (*m_HWindow);
		swapChainDesc.Windowed = TRUE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = 0;

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
		                 NULL,
		                 D3D_DRIVER_TYPE_HARDWARE, // original : D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP
		                 NULL,
		                 D3D11_CREATE_DEVICE_SINGLETHREADED,
		                 featureLevels,
		                 _countof(featureLevels),
		                 D3D11_SDK_VERSION,
		                 &swapChainDesc,
		                 &m_SwapChain,
		                 &m_Device,
		                 NULL,
		                 &m_DevContext);
		assert(SUCCEEDED(hr));

		// ----- #step02 ----- setting render target
		// Get swap chain's back buffer, create its render target view and set that view as render target
		ID3D11Texture2D* backbuffer;
		hr = m_SwapChain->GetBuffer(0, __uuidof(*backbuffer), (void**)&backbuffer);
		assert(SUCCEEDED(hr));

		hr = m_Device->CreateRenderTargetView(backbuffer, NULL, &m_RenderTarget);
		assert(SUCCEEDED(hr));

		m_DevContext->OMSetRenderTargets(1, &m_RenderTarget, NULL);

		// ----- #step03 ----- setting view port
		// Set viewport
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width  = (FLOAT) m_ScreenWidth;
		viewport.Height = (FLOAT) m_ScreenHeight;
		viewport.MinDepth = 0.f;
		viewport.MaxDepth = 1.f;
		m_DevContext->RSSetViewports(1, &viewport);

		// ----- #step04 ----- set raster state
		D3D11_RASTERIZER_DESC rasterizerState;
		rasterizerState.CullMode = D3D11_CULL_FRONT;// D3D11_CULL_NONE;D3D11_CULL_FRONT;D3D11_CULL_BACK
		rasterizerState.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_SOLID ;//D3D11_FILL_WIREFRAME;
		rasterizerState.FrontCounterClockwise = FALSE;
		rasterizerState.DepthBias = false;
		rasterizerState.DepthBiasClamp = 0;
		rasterizerState.SlopeScaledDepthBias = 0;
		rasterizerState.DepthClipEnable = true;
		rasterizerState.ScissorEnable = false;
		rasterizerState.MultisampleEnable = false;
		rasterizerState.AntialiasedLineEnable = true;
		ID3D11RasterizerState* pRS;
		m_Device->CreateRasterizerState( &rasterizerState, &pRS );
		m_DevContext->RSSetState(pRS);

		PreparingDraw();
	}

	void PreparingDraw() {
		// ----- #step05 ----- preparing vertex data & index to buffers
		vertex_type cubeVertices[] = {
		    // Front Face
			{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT3(0.0f, 0.0f, 0.0f) },
			{XMFLOAT3(-1.0f,  1.0f, -1.0f),XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{XMFLOAT3( 1.0f,  1.0f, -1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3( 1.0f, -1.0f, -1.0f),XMFLOAT3(0.0f, 0.0f, 1.0f) },

			// Back Face
			{XMFLOAT3(-1.0f, -1.0f, 1.0f),XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{XMFLOAT3( 1.0f, -1.0f, 1.0f),XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{XMFLOAT3( 1.0f,  1.0f, 1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3(-1.0f,  1.0f, 1.0f),XMFLOAT3(1.0f, 1.0f, 1.0f) },

			// Top Face
			{XMFLOAT3(-1.0f, 1.0f, -1.0f),XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{XMFLOAT3(-1.0f, 1.0f,  1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3( 1.0f, 1.0f,  1.0f),XMFLOAT3(1.0f, 1.0f, 0.0f) },
			{XMFLOAT3( 1.0f, 1.0f, -1.0f),XMFLOAT3(1.0f, 0.0f, 1.0f) },

			// Bottom Face
			{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3( 1.0f, -1.0f, -1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3( 1.0f, -1.0f,  1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3(-1.0f, -1.0f,  1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },

			// Left Face
			{XMFLOAT3(-1.0f, -1.0f,  1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3(-1.0f,  1.0f,  1.0f),XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{XMFLOAT3(-1.0f,  1.0f, -1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{XMFLOAT3(-1.0f, -1.0f, -1.0f),XMFLOAT3(1.0f, 0.0f, 0.0f) },

			// Right Face
			{XMFLOAT3( 1.0f, -1.0f, -1.0f),XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{XMFLOAT3( 1.0f,  1.0f, -1.0f),XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{XMFLOAT3( 1.0f,  1.0f,  1.0f),XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{XMFLOAT3( 1.0f, -1.0f,  1.0f),XMFLOAT3(1.0f, 0.0f, 1.0f) },
		};

		unsigned short cubeIndices[] = {
			// Front Face
			0,  1,  2,
			0,  2,  3,

			// Back Face
			4,  5,  6,
			4,  6,  7,

			// Top Face
			8,  9, 10,
			8, 10, 11,

			// Bottom Face
			12, 13, 14,
			12, 14, 15,

			// Left Face
			16, 17, 18,
			16, 18, 19,

			// Right Face
			20, 21, 22,
			20, 22, 23
		};
		m_IndexCount = ARRAYSIZE(cubeIndices);

		CD3D11_BUFFER_DESC verticesBufferDesc (sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA vertexSubResData;
		vertexSubResData.pSysMem     = cubeVertices;
		vertexSubResData.SysMemPitch = 0;
		vertexSubResData.SysMemSlicePitch = 0;

		HRESULT hr = m_Device->CreateBuffer( &verticesBufferDesc, &vertexSubResData, &m_VertexBuffer );
		assert( hr == S_OK );

		// indices
		CD3D11_BUFFER_DESC indicesBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		D3D11_SUBRESOURCE_DATA indexSubResData;
		indexSubResData.pSysMem = cubeIndices;
		indexSubResData.SysMemPitch = 0;
		indexSubResData.SysMemSlicePitch = 0;

		hr = m_Device->CreateBuffer( &indicesBufferDesc, &indexSubResData, &m_IndexBuffer);
		assert( hr == S_OK );

		// ----- #step06 ----- create shaders
		unsigned int vs_ba_len=0;
		unsigned char* vs_ba = GetByteArrayFromFile("simple_vs.cso",vs_ba_len);
		assert( vs_ba != NULL );

		unsigned int ps_ba_len=0;
		unsigned char* ps_ba = GetByteArrayFromFile("simple_ps.cso",ps_ba_len);
		assert( ps_ba != NULL );

		hr = m_Device->CreateVertexShader( vs_ba, vs_ba_len, NULL, &m_VertexShader );
		assert( hr == S_OK);

		hr = m_Device->CreatePixelShader( ps_ba, ps_ba_len, NULL, &m_PixelShader );
		assert( hr == S_OK);

		// ----- #step07 ----- create input layout
		D3D11_INPUT_ELEMENT_DESC inputVertexDesc[2]; // position , color

		inputVertexDesc[0].SemanticName = "POSITION";
		inputVertexDesc[0].SemanticIndex = 0;
		inputVertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputVertexDesc[0].InputSlot = 0;
		inputVertexDesc[0].AlignedByteOffset = 0;
		inputVertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputVertexDesc[0].InstanceDataStepRate = 0;

		inputVertexDesc[1].SemanticName = "COLOR";
		inputVertexDesc[1].SemanticIndex = 0;
		inputVertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputVertexDesc[1].InputSlot = 0;
		inputVertexDesc[1].AlignedByteOffset =12;//D3D11_APPEND_ALIGNED_ELEMENT;
		inputVertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputVertexDesc[1].InstanceDataStepRate = 0;



		int numElement = sizeof( inputVertexDesc ) / sizeof ( inputVertexDesc[0] );

		hr = m_Device->CreateInputLayout( inputVertexDesc, numElement , vs_ba , vs_ba_len , &m_InputLayout );
		assert( hr == S_OK );

		delete[] vs_ba;
		delete[] ps_ba;

		// ----- #step08 ----- create constant buffer
		D3D11_BUFFER_DESC constBufferDesc;

		constBufferDesc.Usage     = D3D11_USAGE_DEFAULT;
		constBufferDesc.ByteWidth = sizeof(mvp_constantbuffer);
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.CPUAccessFlags = 0;
		constBufferDesc.MiscFlags      = 0;
		constBufferDesc.StructureByteStride = 0;

		hr = m_Device->CreateBuffer( &constBufferDesc , NULL , &m_ConstantBuffer );
		assert( hr == S_OK );

		// ----- #step09 ----- projection matrix
		float aspectRatio = 800.0f / 480.0f;
		float fovAngleY = 100.0f * XM_PI / 180.0f;
		if (aspectRatio < 1.0f) {
			fovAngleY /= aspectRatio;
		}

		XMStoreFloat4x4(&m_ConstantBufferData.projection,
		                XMMatrixTranspose(
		                    XMMatrixPerspectiveFovRH(
		                        fovAngleY,
		                        aspectRatio,
		                        0.01f,
		                        100.0f
		                    )
		                )
		               );
	}
	void Update(const double delta_time) {
		// ----- #step10 : game update here
		XMVECTOR eye = XMVectorSet(0.0f, 0.0f, 3.5f, 0.0f);
		XMVECTOR at  = XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
		XMVECTOR up  = XMVectorSet(0.0f, 0.1f, 0.0f, 0.0f);

		m_TotalTime = m_TotalTime + delta_time;
		XMStoreFloat4x4(&m_ConstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
		XMStoreFloat4x4(&m_ConstantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(m_TotalTime)));
		if(m_TotalTime > 360)
			m_TotalTime =0;
	}
	void Render() {
		// ----- #step11 :  clear screen
		const float bg_color[] =  {0.1f, 0.1f, 0.1f, 1.0f};
		m_DevContext->ClearRenderTargetView(m_RenderTarget,bg_color);

		// ----- #step12 : set constant buffer
		m_DevContext->UpdateSubresource( m_ConstantBuffer,0,NULL,&m_ConstantBufferData,0,0);

		// ----- #step13 : set index & vertex buffer
		unsigned int stride, offset;
		stride = sizeof ( vertex_type );
		offset = 0;

		m_DevContext->IASetVertexBuffers( 0, 1 , &m_VertexBuffer , &stride, &offset );
		m_DevContext->IASetIndexBuffer( m_IndexBuffer , DXGI_FORMAT_R16_UINT, 0 );
		// ----- #step14 : set Primitive Topology
		m_DevContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST   ); // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

		// ----- #step15 : set shader, input layout, send draw
		m_DevContext->IASetInputLayout( m_InputLayout );
		m_DevContext->VSSetShader(m_VertexShader, NULL, 0);
		m_DevContext->VSSetConstantBuffers( 0,1,&m_ConstantBuffer);
		m_DevContext->PSSetShader(m_PixelShader, NULL, 0);
		m_DevContext->DrawIndexed(36, 0, 0);

		// ----- #step16 : swap buffer
		m_SwapChain->Present(1, 0);
	}
	void ShutDown() {
	}
	void OnSuspending() {
	}
	void OnResuming() {
	}
	void OnHandleInput() {
	}
};