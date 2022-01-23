#include "PBDSimulation.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::util;

PBDSimulation::PBDSimulation(float timeStep)
	:_timeStep(timeStep)
{
	_nodeCount = { 0, 6 };

}

PBDSimulation::~PBDSimulation()
{
}

void PBDSimulation::_update()
{
	_force();
}

void PBDSimulation::_force()
{
	for (int j = 0; j < _nodeCount.y; j++)
	{
		_nodePosition[j].y -= 0.98f * _timeStep;
	}
}

void PBDSimulation::_project()
{
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
		XMFLOAT2 pos = { 0.0f, static_cast<float>(j) * 2.0f };
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