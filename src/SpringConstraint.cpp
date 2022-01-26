#include "SpringConstraint.h"
using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

SpringConstraint::SpringConstraint(XMFLOAT2& p1, XMFLOAT2& p2, XMFLOAT2 d)
	:_p1(p1), _p2(p2), _d(d)
{
	_lamda = { 0.0f, 0.0f };
}

SpringConstraint::~SpringConstraint()
{
}

void SpringConstraint::solvePBD(float dt)
{
	float alpha = 0.001f;
	float alphaTilda = alpha / (dt * dt);

	XMFLOAT2 abs_p1_p2 = fabsxmf2(_p1 - _p2);

	XMFLOAT2 delta_p1 =
	{
		abs_p1_p2.x > FLT_EPSILON ? +_lamda.x * (_p1.x - _p2.x) / abs_p1_p2.x : 0.0f,
		abs_p1_p2.y > FLT_EPSILON ? +_lamda.y * (_p1.y - _p2.y) / abs_p1_p2.y : 0.0f
	};
	XMFLOAT2 delta_p2 =
	{
		abs_p1_p2.x > FLT_EPSILON ? -_lamda.x * (_p1.x - _p2.x) / abs_p1_p2.x : 0.0f,
		abs_p1_p2.y > FLT_EPSILON ? -_lamda.y * (_p1.y - _p2.y) / abs_p1_p2.y : 0.0f
	};
	XMFLOAT2 delta_lamda = (-0.5f * (abs_p1_p2 - _d) - alphaTilda * _lamda) / (1.0f + alphaTilda);

	_p1 += delta_p1;
	_p2 += delta_p2;
	_lamda += delta_lamda;
}
