#include "SpringConstraint.h"
using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

SpringConstraint::SpringConstraint(
	XMFLOAT2& currentP1, XMFLOAT2& currentP2, XMFLOAT2& newP1, XMFLOAT2& newP2,
	int i, int j, XMFLOAT2 d, float alpha)
	:_currentP1(currentP1), _currentP2(currentP2), _newP1(newP1), _newP2(newP2),
	_p1Index(i), _p2Index(j), _d(d), _alpha(alpha)
{
	_lambda = { 0.0f, 0.0f };
	_k = 1.0f / _alpha;
}

SpringConstraint::~SpringConstraint()
{
}

void SpringConstraint::setLambda(DirectX::XMFLOAT2 lamda)
{
	_lambda = lamda;
}

int SpringConstraint::getP1Index()
{
	return _p1Index;
}

int SpringConstraint::getP2Index()
{
	return _p2Index;
}

void SpringConstraint::projectConstraint(float subdt)
{
	XMFLOAT2& p1 = _newP1;
	XMFLOAT2& p2 = _newP2;
	float alphaTilda = _alpha / (subdt * subdt);

	XMFLOAT2 abs_p1_p2 = fabsxmf2(p1 - p2);

	XMFLOAT2 delta_p1 =
	{
		abs_p1_p2.x > FLT_EPSILON ? +_lambda.x * (p1.x - p2.x) / abs_p1_p2.x : 0.0f,
		abs_p1_p2.y > FLT_EPSILON ? +_lambda.y * (p1.y - p2.y) / abs_p1_p2.y : 0.0f
	};
	XMFLOAT2 delta_p2 =
	{
		abs_p1_p2.x > FLT_EPSILON ? -_lambda.x * (p1.x - p2.x) / abs_p1_p2.x : 0.0f,
		abs_p1_p2.y > FLT_EPSILON ? -_lambda.y * (p1.y - p2.y) / abs_p1_p2.y : 0.0f
	};
	XMFLOAT2 delta_lamda = (-0.5f * (abs_p1_p2 - _d) - alphaTilda * _lambda) / (1.0f + alphaTilda);

	p1 += delta_p1;
	p2 += delta_p2;
	_lambda += delta_lamda;
}

float SpringConstraint::computeElasticEnergy()
{
	float dx = _computeDx();
	return 0.5f * _k * (dx * dx);
}

float SpringConstraint::computeElasticEnergyGradient(int j)
{
	float dx = _computeDx();

	return _k * dx;
}

float SpringConstraint::_computeDx()
{
	XMFLOAT2& p1 = _currentP1;
	XMFLOAT2& p2 = _currentP2;

	XMFLOAT2 p1_p2 = (p1 - p2);
	float dist1 = sqrtf(_d.x * _d.x + _d.y * _d.y);
	float dist2 = sqrtf(p1_p2.x * p1_p2.x + p1_p2.y * p1_p2.y);
	float dx = fabsf(dist2 - dist1);

	return dx;
}
