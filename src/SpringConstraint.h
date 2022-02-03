#pragma once
#include "dx12header.h"

class SpringConstraint
{
public:
	SpringConstraint(
		DirectX::XMFLOAT2& currentP1, DirectX::XMFLOAT2& currentP2,
		DirectX::XMFLOAT2& newP1, DirectX::XMFLOAT2& newP2,
		int i, int j,
		DirectX::XMFLOAT2 d, float alpha);
	~SpringConstraint();

	void setLambda(DirectX::XMFLOAT2 lamda);
	int getP1Index();
	int getP2Index();
	void projectConstraint(float subdt);
	float computeElasticEnergy();
	float computeElasticEnergyGradient(int j);

private:
	DirectX::XMFLOAT2& _currentP1;
	DirectX::XMFLOAT2& _currentP2;
	DirectX::XMFLOAT2& _newP1; // Position after the external force
	DirectX::XMFLOAT2& _newP2; // Position after the external force
	int _p1Index, _p2Index;
	DirectX::XMFLOAT2 _d;
	DirectX::XMFLOAT2 _lambda;

	float _alpha;
	float _k;

	float _computeDx();
};

