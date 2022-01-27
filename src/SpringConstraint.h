#pragma once
#include "dx12header.h"

class SpringConstraint
{
public:
	SpringConstraint(DirectX::XMFLOAT2& p1, DirectX::XMFLOAT2& p2, DirectX::XMFLOAT2 d, float alpha);
	~SpringConstraint();

	void setLamda(DirectX::XMFLOAT2 lamda);
	void projectConstraint(float subdt);
	float computeElasticEnergy();

private:
	DirectX::XMFLOAT2& _p1;
	DirectX::XMFLOAT2& _p2;
	DirectX::XMFLOAT2 _d;
	DirectX::XMFLOAT2 _lamda;

	float _alpha;
};

