#pragma once

#include "stdafx.h"
#include "DDSTextureLoader.h"

#include "vertextypes.hpp"
#include <sstream>
#include <vector>

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
	ID3D11Buffer* m_ColorConstantBuffer;
	ID3D11VertexShader* m_VertexShader;
	ID3D11PixelShader* m_PixelShader;
	ID3D11InputLayout* m_InputLayout;
	int m_VertexCount;
	int m_IndexCount;
	double m_TotalTime;
	mvp_constantbuffer m_ConstantBufferData;
	ID3D11ShaderResourceView* m_TextureShaderResView;
	float m_AdjustableValues[64];
	bool m_ToggleWireframe;
public :
	XGame() {
		for(int i=0;i<64;i++)
			m_AdjustableValues[i] = 0.0f;
		m_AdjustableValues[3]= 5.0f;
		m_AdjustableValues[0] =10.0f;
		m_AdjustableValues[1] =9.0f;
		m_AdjustableValues[2] =3.0f;
		m_ToggleWireframe = false;
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
		rasterizerState.CullMode = D3D11_CULL_BACK;  // D3D11_CULL_NONE;D3D11_CULL_FRONT; D3D11_CULL_BACK
		rasterizerState.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_SOLID ;//  D3D11_FILL_WIREFRAME;
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

	void LoadModel_Obj(const std::string& filename,std::vector< vertex_type >& vecVertex, std::vector< unsigned short >& vecIndices ) {
		std::ifstream imeshfile( filename , std::ios::in );
		assert( imeshfile.is_open() == true );
		int p=0;
		int q=0;
		while( imeshfile.good() ) 
		{
			std::string linestr;
			getline( imeshfile, linestr );
			if( linestr[0] =='#' || linestr[0] ==' ')
				continue;
			switch( linestr[0] ) {
			case 'v' :
				if( linestr[1] =='t' ) {        // vertex texture coord
					//
				} else if( linestr[1] =='n' ) { // vertex normal
					//
				} else {                        // vertex
					float val[3];
					int idx=0;
					std::string temp="";
					for(int i=1;i<linestr.size();i++ ) {
						if( linestr[i]==' ') {
							if( temp!="" ) {
								val[idx] = atof( temp.c_str() );
								idx++;
								temp="";
							}
						} else {
							temp.push_back( linestr[i] );
						}
					}
					val[idx] =  atof( temp.c_str() ) *(-1.0f);
					int y=0;
					vertex_type ver;// = new vertex_type();
					ver.position = DirectX::XMFLOAT3(val[0], val[1], val[2]);
					//vecVertex.push_back( ver );
					vecVertex.push_back( ver );
				}
				break;
			case 'f' : // face/indices
					float val[3];
					int idx=0;
					std::string temp="";
					for(int i=1;i<linestr.size();i++ ) {
						if( linestr[i]==' ') {
							if( temp!="" ) {
								val[idx] = atof( temp.c_str() );
								idx++;
								temp="";
							}
						} else {
							temp.push_back( linestr[i] );
						}
					}
					val[idx] = atof( temp.c_str() );
					int y=0;
					vecIndices.push_back( val[0]-1 );
					vecIndices.push_back( val[1]-1 );
					vecIndices.push_back( val[2]-1 );
				break;
			}
		}//while
	}

	void LoadModel_VBO( unsigned char* meshData, ID3D11Buffer** vertexBuffer,ID3D11Buffer** indexBuffer,int& vertexCount,int& indexCount)  {
		// The first 4 bytes of the BasicMesh format define the number of vertices in the mesh.
		int numVertices = *reinterpret_cast<int*>(meshData);

		// The following 4 bytes define the number of indices in the mesh.
		int numIndices = *reinterpret_cast<int*>(meshData + sizeof(int));

		// The next segment of the BasicMesh format contains the vertices of the mesh.
		vertex_type* vertices = reinterpret_cast<vertex_type*>(meshData + sizeof(int) * 2);

		// The last segment of the BasicMesh format contains the indices of the mesh.
		unsigned short* indices = reinterpret_cast<unsigned short*>(meshData + sizeof(int) * 2 + sizeof(vertex_type) * numVertices);

		// Create the vertex and index buffers with the mesh data.

		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = vertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(numVertices * sizeof(vertex_type), D3D11_BIND_VERTEX_BUFFER);
		HRESULT hr = m_Device->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				vertexBuffer
				);
		assert( hr==S_OK );

		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = indices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(numIndices * sizeof(unsigned short), D3D11_BIND_INDEX_BUFFER);
			m_Device->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				indexBuffer
				);
		assert( hr==S_OK );
		vertexCount = numVertices;
		indexCount = numIndices;
	}
	void PreparingDraw() {
		// ----- #step05 ----- preparing vertex data & index to buffers
		HRESULT hr;
#if 0
		std::vector< vertex_type > cubeVertices;
		std::vector< unsigned short > cubeIndices;
		int f=sizeof(cubeVertices);
		//LoadModel_Obj("../data/models/cube.obj", cubeVertices, cubeIndices );
		LoadModel_Obj("../data/models/teapot_vf.obj", cubeVertices, cubeIndices );
		//
		m_IndexCount = cubeIndices.size();
		CD3D11_BUFFER_DESC verticesBufferDesc (sizeof(float)*3*cubeVertices.size(), D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA vertexSubResData;
		vertexSubResData.pSysMem     = (void*)cubeVertices.data();
		vertexSubResData.SysMemPitch = 0;
		vertexSubResData.SysMemSlicePitch = 0;

		hr = m_Device->CreateBuffer( &verticesBufferDesc, &vertexSubResData, &m_VertexBuffer );
		assert( hr == S_OK );

		// indices
		CD3D11_BUFFER_DESC indicesBufferDesc(sizeof( unsigned short ) *cubeIndices.size(), D3D11_BIND_INDEX_BUFFER);
		D3D11_SUBRESOURCE_DATA indexSubResData;
		indexSubResData.pSysMem = (void*) cubeIndices.data();
		indexSubResData.SysMemPitch = 0;
		indexSubResData.SysMemSlicePitch = 0;

		hr = m_Device->CreateBuffer( &indicesBufferDesc, &indexSubResData, &m_IndexBuffer);
		assert( hr == S_OK );
#else
		unsigned int vbo_ba_len=0;
		//unsigned char* vbo_ba = GetByteArrayFromFile("../data/models/cup.vbo",vbo_ba_len);
		//unsigned char* vbo_ba = GetByteArrayFromFile("../data/models/teapot_vf.vbo",vbo_ba_len);
		unsigned char* vbo_ba = GetByteArrayFromFile("../data/models/teapot_k.vbo",vbo_ba_len);

		LoadModel_VBO( vbo_ba ,&m_VertexBuffer,&m_IndexBuffer, m_VertexCount, m_IndexCount);
#endif
		// ----- #step06 ----- create shaders
		unsigned int vs_ba_len=0;
		unsigned char* vs_ba = GetByteArrayFromFile("Simple_VS_03.cso",vs_ba_len);
		assert( vs_ba != NULL );

		unsigned int ps_ba_len=0;
		unsigned char* ps_ba = GetByteArrayFromFile("Simple_PS_03.cso",ps_ba_len);
		assert( ps_ba != NULL );

		hr = m_Device->CreateVertexShader( vs_ba, vs_ba_len, NULL, &m_VertexShader );
		assert( hr == S_OK);

		hr = m_Device->CreatePixelShader( ps_ba, ps_ba_len, NULL, &m_PixelShader );
		assert( hr == S_OK);

		// ----- #step07 ----- create input layout
		D3D11_INPUT_ELEMENT_DESC inputVertexDesc[3]; // position, normal, texcoord

		inputVertexDesc[0].SemanticName = "POSITION";
		inputVertexDesc[0].SemanticIndex = 0;
		inputVertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputVertexDesc[0].InputSlot = 0;
		inputVertexDesc[0].AlignedByteOffset = 0;
		inputVertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputVertexDesc[0].InstanceDataStepRate = 0;

		inputVertexDesc[1].SemanticName = "NORMAL";
		inputVertexDesc[1].SemanticIndex = 0;
		inputVertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputVertexDesc[1].InputSlot = 0;
		inputVertexDesc[1].AlignedByteOffset =12; //D3D11_APPEND_ALIGNED_ELEMENT;
		inputVertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputVertexDesc[1].InstanceDataStepRate = 0;

		inputVertexDesc[2].SemanticName = "TEXCOORD";
		inputVertexDesc[2].SemanticIndex = 0;
		inputVertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputVertexDesc[2].InputSlot = 0;
		inputVertexDesc[2].AlignedByteOffset =24; //D3D11_APPEND_ALIGNED_ELEMENT;
		inputVertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputVertexDesc[2].InstanceDataStepRate = 0;

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

		D3D11_BUFFER_DESC ColorConstBufferDesc;
		ColorConstBufferDesc.Usage     = D3D11_USAGE_DYNAMIC;
		ColorConstBufferDesc.ByteWidth = sizeof(color_constantbuffer);
		ColorConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		ColorConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		ColorConstBufferDesc.MiscFlags      = 0;
		ColorConstBufferDesc.StructureByteStride = 0;

		hr = m_Device->CreateBuffer( &ColorConstBufferDesc , NULL , &m_ColorConstantBuffer );
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
		
		//load texture 
		CreateDDSTextureFromFile( m_Device , L"../data/textures/texture_03.DDS", nullptr, &m_TextureShaderResView, MAXSIZE_T);
		assert ( m_TextureShaderResView );
	}
	void Update(const double delta_time) {
		XMVECTOR eye = XMVectorSet(0.0, 20.0, m_AdjustableValues[3], 0.0f);
		XMVECTOR at  = XMVectorSet(0.0, 0.0, 0.0f, 0.0f);
		XMVECTOR up  = XMVectorSet(1.0f, 0.1f, 0.0f, 0.0f);

		m_TotalTime = m_TotalTime + delta_time;
		XMStoreFloat4x4(&m_ConstantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
		XMStoreFloat4x4(&m_ConstantBufferData.model, XMMatrixTranspose( XMMatrixRotationRollPitchYaw(m_AdjustableValues[0],m_AdjustableValues[1],m_AdjustableValues[2] ) ));
	}
	void Render() {
		// clear screen
		const float bg_color[] =  {0.1f, 0.1f, 0.1f, 1.0f};
		m_DevContext->ClearRenderTargetView(m_RenderTarget,bg_color);

		// set index buffer
		m_DevContext->UpdateSubresource( m_ConstantBuffer,0,NULL,&m_ConstantBufferData,0,0);

		//update ps constant buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		color_constantbuffer* colvalue;
		HRESULT hr = m_DevContext->Map(m_ColorConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		assert( hr == S_OK );
		// get ptr data
		colvalue = (color_constantbuffer*)mappedResource.pData;
		// set data
		colvalue->luminance  = DirectX::XMFLOAT4( m_AdjustableValues[4], m_AdjustableValues[5],m_AdjustableValues[6],m_AdjustableValues[7] );
		colvalue->saturation = DirectX::XMFLOAT4( m_AdjustableValues[8], m_AdjustableValues[9],m_AdjustableValues[10],m_AdjustableValues[11] );
		// Unlock the constant buffer.
		m_DevContext->Unmap(m_ColorConstantBuffer, 0);

		unsigned int stride, offset;
		stride = sizeof ( vertex_type );
		offset = 0;

		m_DevContext->IASetVertexBuffers( 0, 1 , &m_VertexBuffer , &stride, &offset );
		m_DevContext->IASetIndexBuffer( m_IndexBuffer , DXGI_FORMAT_R16_UINT, 0 );
		m_DevContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST   ); // D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST

		// draw issue
		m_DevContext->IASetInputLayout( m_InputLayout );
		m_DevContext->VSSetShader(m_VertexShader, NULL, 0);
		
		m_DevContext->VSSetConstantBuffers( 0,1,&m_ConstantBuffer);
		m_DevContext->PSSetConstantBuffers( 0,1,&m_ColorConstantBuffer);

		m_DevContext->PSSetShader(m_PixelShader, NULL, 0);
		m_DevContext->PSSetShaderResources(0, 1, &m_TextureShaderResView);
		m_DevContext->DrawIndexed(m_IndexCount, 0, 0);

		//swap
		m_SwapChain->Present(1, 0);
	}
	void ShutDown() {
	}
	void OnSuspending() {
	}
	void OnResuming() {
	}
	void OnHandleInput(int pid,int px,int py) {
		switch( pid )
		{
		case 85: // 'u'
			m_AdjustableValues[0] +=0.1f;   // roll +
			if( m_AdjustableValues[0] > 360 )
				m_AdjustableValues[0] = 0;
			break;
		case 73: // 'i'
			m_AdjustableValues[0] -=0.1f;  // roll -
			if( m_AdjustableValues[0] < 0 )
				m_AdjustableValues[0] = 360;
			break;
		case 79: // 'o'
			m_AdjustableValues[1] +=0.1f; // pitch +
			if( m_AdjustableValues[1] > 360 )
				m_AdjustableValues[1] = 0;
			break;
		case 80: // 'p'
			m_AdjustableValues[1] -=0.1f; // pitch -
			if( m_AdjustableValues[1] < 0 )
				m_AdjustableValues[1] = 360;
			break;
		case 75: // 'k'
			m_AdjustableValues[2] +=0.1f; // yaw +
			if( m_AdjustableValues[2] > 360 )
				m_AdjustableValues[2] = 0;
			break;
		case 76: // 'l'
			m_AdjustableValues[2] -=0.1f; // yaw -
			if( m_AdjustableValues[2] < 0 )
				m_AdjustableValues[2] = 360;
			break;
		case 78: // 'n'
			m_AdjustableValues[3] +=1.0f; // zoom in cam
			if( m_AdjustableValues[3] > 100 )
				m_AdjustableValues[3] = -100;
			break;
		case 77: // 'm'
			m_AdjustableValues[3] -=1.0f; // zoom out cam
			if( m_AdjustableValues[3] < -100 )
				m_AdjustableValues[3] = 100;
			break;
		case 86:
			m_ToggleWireframe =!m_ToggleWireframe;
			D3D11_RASTERIZER_DESC rasterizerState;
			rasterizerState.CullMode = D3D11_CULL_BACK;  // D3D11_CULL_NONE;D3D11_CULL_FRONT; D3D11_CULL_BACK
			if( !m_ToggleWireframe )
				rasterizerState.FillMode = D3D11_FILL_WIREFRAME; // D3D11_FILL_SOLID ;//  D3D11_FILL_WIREFRAME;
			else
				rasterizerState.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_SOLID ;//  D3D11_FILL_WIREFRAME;
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
			
			break;
		default:
			break;
		};

	if( pid > 48 && pid < 58 ) {
		m_AdjustableValues[3+(pid-48)] +=0.05;
		if( m_AdjustableValues[3+(pid-48)]  > 2.0f )
			m_AdjustableValues[3+(pid-48)] =0.0f;
	}
}

};// end class