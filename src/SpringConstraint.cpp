#include "SpringConstraint.h"
using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

SpringConstraint::SpringConstraint(XMFLOAT2 p1, XMFLOAT2 p2, XMFLOAT2 d)
	:_p1(p1), _p2(p2), _d(d)
{

}

SpringConstraint::~SpringConstraint()
{
}
