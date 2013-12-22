#include "stdafx.h"

struct mvp_constantbuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

struct vertex_type
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
};