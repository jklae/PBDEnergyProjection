#include "PBDSimulation.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;

PBDSimulation::PBDSimulation(float timeStep)
	:_timeStep(timeStep)
{
	// Float initialization
	_nodeCount = { 10, 10 };
	_floorPosition = -2.0f * _nodeCount.y;
	_stride = 3.0f;

	// Vector initialization
	size_t vSize = static_cast<size_t>(_nodeCount.x) * static_cast<size_t>(_nodeCount.y);
	_newPosition.assign(vSize, { 0.0f, 0.0f });

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
	_newPosition = _nodePosition;
	/*for (SpringConstraint& sp : _constraint)
	{
		sp.setLamda(XMFLOAT2(0.0f, 0.0f));
	}*/

	float dt = _timeStep;
	int nSteps = 10;
	float subdt = dt / static_cast<float>(nSteps);

	for (int t = 0; t < nSteps; t++)
	{
		// External force
		for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
		{
			_newPosition[j].y += (subdt * _nodeVelocity[j].y) + (subdt * subdt * -9.8f);
		}

		// Constraint projection
		for (int iter = 0; iter < 1; iter++)
		{
			for (SpringConstraint& sp : _constraint)
			{
				sp.springConstraint(subdt);
			}

			// Floor boundary condition
			for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
			{
				if (_newPosition[j].y < _floorPosition)
					_newPosition[j].y = _floorPosition;
			}
		}

		// Update the velocity
		for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
		{
			_nodeVelocity[j] = (_newPosition[j] - _nodePosition[j]) / subdt;
			_nodePosition[j] = _newPosition[j];
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
		for (int i = 0; i < _nodeCount.x; i++)
		{
			XMFLOAT2 pos = { static_cast<float>(i) * _stride, 10.0f + static_cast<float>(j) * _stride };
			_nodePosition.push_back(pos);
			_nodeVelocity.push_back(XMFLOAT2(0.0f, 0.0f));

			ConstantBuffer objectCB;
			objectCB.world = DXViewer::util::transformMatrix(pos.x, pos.y, 0.0f, 1.0f);
			objectCB.worldViewProj = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f);
			objectCB.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			constantBuffer.push_back(objectCB);
		}
	}


	// Constraint initialization
	float alpha = 0.001f;
	for (int j = 0; j < _nodeCount.x * _nodeCount.y - 1; j++)
	{
		for (int i = j + 1; i < _nodeCount.x * _nodeCount.y; i++)
		{
			XMFLOAT2& p1 = _newPosition[i];
			XMFLOAT2& p2 = _newPosition[j];
			XMFLOAT2 d = fabsxmf2(_nodePosition[j] - _nodePosition[i]);
			float dist = sqrtf(d.x * d.x + d.y * d.y);

			if (dist < 1.5f * _stride)
			{
				SpringConstraint sp(p1, p2, d, alpha);
				_constraint.push_back(sp);
			}
		}
		
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
	return _nodeCount.x * _nodeCount.y;
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