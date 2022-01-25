#include "PBDSimulation.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::util;
using namespace DXViewer::xmfloat2;

PBDSimulation::PBDSimulation(float timeStep)
	:_timeStep(timeStep)
{
	_nodeCount = { 0, 20 };
	_floorPosition = -2.0f * _nodeCount.y;
	_stride = { 2.0f, 3.0f };
}

PBDSimulation::~PBDSimulation()
{
}

void PBDSimulation::_update()
{
	_project();
}


void PBDSimulation::_project()
{
	vector<XMFLOAT2> newPosition(_nodePosition);
	vector<XMFLOAT2> lamda(_nodeCount.y, { 0.0f, 0.0f });

	float dt = _timeStep;
	float alpha = 0.001f;
	float alphaTilda = alpha / (dt * dt);

	int nSteps = 10;
	float subdt = dt / static_cast<float>(nSteps);

	for (int t = 0; t < nSteps; t++)
	{
		// External force
		for (int j = 0; j < _nodeCount.y; j++)
		{
			newPosition[j].y += _nodeVelocity[j].y * dt - 9.8f * dt * dt;
		}

		// Constraint projection
		for (int iter = 0; iter < 1; iter++)
		{
			
			for (int j = 0; j < _nodeCount.y - 1; j++)
			{
				XMFLOAT2 p1 = newPosition[j];
				XMFLOAT2 p2 = newPosition[j + 1];
				XMFLOAT2 d = _stride;
				XMFLOAT2 abs_p1_p2 = fabsxmf2(p1 - p2);

				XMFLOAT2 delta_p1 = 
				{
					abs_p1_p2.x > FLT_EPSILON ? +lamda[j].x * (p1.x - p2.x) / abs_p1_p2.x : 0.0f,
					abs_p1_p2.y > FLT_EPSILON ? +lamda[j].y * (p1.y - p2.y) / abs_p1_p2.y : 0.0f
				};
				XMFLOAT2 delta_p2 =
				{
					abs_p1_p2.x > FLT_EPSILON ? -lamda[j].x * (p1.x - p2.x) / abs_p1_p2.x : 0.0f,
					abs_p1_p2.y > FLT_EPSILON ? -lamda[j].y * (p1.y - p2.y) / abs_p1_p2.y : 0.0f
				};
				XMFLOAT2 delta_lamda = (-0.5f * (abs_p1_p2 - d) - alphaTilda * lamda[j]) / (1.0f + alphaTilda);

				newPosition[j] += delta_p1;
				newPosition[j + 1] += delta_p2;
				lamda[j] += delta_lamda;
			}

			// Floor boundary condition
			for (int j = 0; j < _nodeCount.y; j++)
			{
				if (newPosition[j].y < _floorPosition)
					newPosition[j].y = _floorPosition;
			}
		}

		// Update the velocity
		for (int j = 0; j < _nodeCount.y; j++)
		{
			_nodeVelocity[j] = (newPosition[j] - _nodePosition[j]) / dt;
			_nodePosition[j] = newPosition[j];
		}

	}
	
}


#pragma region Implementation
// ################################## Implementation ####################################
// Simulation methods
void PBDSimulation::iUpdate()
{
	for (int i = 0; i < 1; i++)
	{
		_update();
	}
}

void PBDSimulation::iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer)
{
}


// Mesh methods
vector<Vertex>& PBDSimulation::iGetVertice()
{
	_vertices =
	{
		Vertex({ XMFLOAT3(-0.5f, -0.5f, 0.0f) }),
		Vertex({ XMFLOAT3(-0.5f, +0.5f, 0.0f) }),
		Vertex({ XMFLOAT3(+0.5f, +0.5f, 0.0f) }),
		Vertex({ XMFLOAT3(+0.5f, -0.5f, 0.0f) })
	};

	return _vertices;
}

vector<unsigned int>& PBDSimulation::iGetIndice()
{
	_indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,
	};

	return _indices;
}

UINT PBDSimulation::iGetVertexBufferSize()
{
	return 4;
}

UINT PBDSimulation::iGetIndexBufferSize()
{
	return 6;
}


// DirectX methods
void PBDSimulation::iCreateObject(std::vector<ConstantBuffer>& constantBuffer)
{
	/*int floorCount = 80;
	for (int i = 0; i < floorCount * 2; i++)
	{
		ConstantBuffer objectCB;

		objectCB.world = transformMatrix(static_cast<float>(-floorCount + i), 0.0f, 0.0f, 1.0f);
		objectCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
		objectCB.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		constantBuffer.push_back(objectCB);
	}*/

	for (int j = 0; j < _nodeCount.y; j++)
	{
		XMFLOAT2 pos = { static_cast<float>(j) * _stride.x, 10.0f + static_cast<float>(j) * _stride.y };
		_nodePosition.push_back(pos);
		_nodeVelocity.push_back(XMFLOAT2(0.0f, 0.0f));

		ConstantBuffer objectCB;
		objectCB.world = transformMatrix(pos.x, pos.y, 0.0f, 1.0f);
		objectCB.worldViewProj = transformMatrix(0.0f, 0.0f, 0.0f);
		objectCB.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		constantBuffer.push_back(objectCB);
	}
}

void PBDSimulation::iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i)
{
	constantBuffer[i].world._41 = _nodePosition[i].x;
	constantBuffer[i].world._42 = _nodePosition[i].y;
}

void PBDSimulation::iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i)
{
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void PBDSimulation::iSetDXApp(DX12App* dxApp)
{
	_dxApp = dxApp;
}

UINT PBDSimulation::iGetConstantBufferSize()
{
	return 160;
}

DirectX::XMINT3 PBDSimulation::iGetObjectCount()
{
	return { _nodeCount.x, _nodeCount.y, 0 };
}

DirectX::XMFLOAT3 PBDSimulation::iGetObjectSize()
{
	return { 1.0f, 1.0f, 0.0f };
}

DirectX::XMFLOAT3 PBDSimulation::iGetObjectPositionOffset()
{
	return { 0.0f, 0.0f, 0.0f };
}

bool PBDSimulation::iIsUpdated()
{
	return true;
}


void PBDSimulation::iWMCreate(HWND hwnd, HINSTANCE hInstance)
{
}

void PBDSimulation::iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
}

void PBDSimulation::iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
}

void PBDSimulation::iWMTimer(HWND hwnd)
{
}

void PBDSimulation::iWMDestory(HWND hwnd)
{
}
// #######################################################################################
#pragma endregion