#pragma once
#include "dx12header.h"

class SpringConstraint
{
public:
	SpringConstraint(DirectX::XMFLOAT2& p1, DirectX::XMFLOAT2& p2, DirectX::XMFLOAT2 d);
	~SpringConstraint();

	void solvePBD(float dt);

private:
	DirectX::XMFLOAT2& _p1;
	DirectX::XMFLOAT2& _p2;
	DirectX::XMFLOAT2 _d;
	DirectX::XMFLOAT2 _lamda;
};

