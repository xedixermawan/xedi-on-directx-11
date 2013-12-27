#include "stdafx.h"

struct mvp_constantbuffer
{
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};
struct color_constantbuffer
{
	DirectX::XMFLOAT4 saturation;
	DirectX::XMFLOAT4 luminance;
};
struct vertex_type
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT2 texcoord;
};