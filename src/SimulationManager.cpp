#include "SimulationManager.h"
using namespace DirectX;
using namespace std;

SimulationManager::SimulationManager(int x, int y, float timeStep)
{
	float maxCount = static_cast<float>(max(x, y));
	_lineCount = max(x, y) * 5;
	_floorPosition = -2.0f * y;

	_sim.push_back(new PBDSimulation(x, y, timeStep, 
		false, XMFLOAT2(-maxCount * 1.5f, maxCount), _floorPosition));
	_sim.push_back(new PBDSimulation(x, y, timeStep, 
		true, XMFLOAT2(+maxCount * 1.5f, maxCount), _floorPosition));
}

SimulationManager::~SimulationManager()
{
}

void SimulationManager::_createLine(std::vector<ConstantBuffer>& constantBuffer)
{
	// Floor, Line initialization
	for (int i = 0; i < _lineCount * 2; i++)
	{
		float x = static_cast<float>(-_lineCount + i);
		ConstantBuffer floorCB, lineCB;

		floorCB.world = DXViewer::util::transformMatrix(
			x, _floorPosition - 1.0f, 0.0f, 1.0f);
		floorCB.worldViewProj = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f);
		floorCB.color = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);

		lineCB.world = DXViewer::util::transformMatrix(
			x, -_floorPosition, 0.0f, 1.0f);
		lineCB.worldViewProj = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f);
		lineCB.color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

		constantBuffer.push_back(floorCB);
		constantBuffer.push_back(lineCB);
	}
}

#pragma region Implementation
// ################################## Implementation ####################################
// Simulation methods
void SimulationManager::iUpdate()
{
	clock_t startTime = clock();
	_sim[0]->iUpdate();
	_sim[1]->iUpdate();
	clock_t endTime = clock();

	_simTime += endTime - startTime; // ms
	_simFrame++;
}

void SimulationManager::iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer)
{
	constantBuffer.clear();
	_sim[0]->iResetSimulationState(constantBuffer);
	_sim[1]->iResetSimulationState(constantBuffer);

	_createLine(constantBuffer);

	_simTime = 0;
	_simFrame = 0;
}


// Mesh methods
vector<Vertex>& SimulationManager::iGetVertice()
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

vector<unsigned int>& SimulationManager::iGetIndice()
{
	_indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,
	};

	return _indices;
}

UINT SimulationManager::iGetVertexBufferSize()
{
	return 4;
}

UINT SimulationManager::iGetIndexBufferSize()
{
	return 6;
}


// DirectX methods
void SimulationManager::iCreateObject(std::vector<ConstantBuffer>& constantBuffer)
{
	_sim[0]->iCreateObject(constantBuffer);
	_sim[1]->iCreateObject(constantBuffer);

	_createLine(constantBuffer);
}

void SimulationManager::iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i)
{
	_sim[0]->iUpdateConstantBuffer(constantBuffer, i, 0);
	_sim[1]->iUpdateConstantBuffer(constantBuffer, i, 1);
}

void SimulationManager::iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i)
{
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void SimulationManager::iSetDXApp(DX12App* dxApp)
{
	_dxapp = dxApp;
}

UINT SimulationManager::iGetConstantBufferSize()
{
	return 
		_sim[0]->iGetConstantBufferSize() + _sim[1]->iGetConstantBufferSize()   // Node
		+ (_lineCount * 2)														// Floor
		+ (_lineCount * 2);														// Line
}

DirectX::XMINT3 SimulationManager::iGetObjectCount()
{
	return _sim[0]->iGetObjectCount();
}

DirectX::XMFLOAT3 SimulationManager::iGetObjectSize()
{
	return _sim[0]->iGetObjectSize();
}

DirectX::XMFLOAT3 SimulationManager::iGetObjectPositionOffset()
{
	return _sim[0]->iGetObjectPositionOffset();
}

bool SimulationManager::iIsUpdated()
{
	return _updateFlag;
}


void SimulationManager::iWMCreate(HWND hwnd, HINSTANCE hInstance)
{
	CreateWindow(L"button", _updateFlag ? L"¡«" : L"¢º", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		65, 305, 50, 25, hwnd, reinterpret_cast<HMENU>(COM::PLAY), hInstance, NULL);
	CreateWindow(L"button", L"¡á", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		115, 305, 50, 25, hwnd, reinterpret_cast<HMENU>(COM::STOP), hInstance, NULL);
	CreateWindow(L"button", L"¢ºl", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		165, 305, 50, 25, hwnd, reinterpret_cast<HMENU>(COM::NEXTSTEP), hInstance, NULL);

	CreateWindow(L"static", L"time :", WS_CHILD | WS_VISIBLE,
		95, 350, 40, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", to_wstring(_simTime).c_str(), WS_CHILD | WS_VISIBLE,
		140, 350, 40, 20, hwnd, reinterpret_cast<HMENU>(COM::TIME_TEXT), hInstance, NULL);
	CreateWindow(L"static", L"frame :", WS_CHILD | WS_VISIBLE,
		86, 370, 45, 20, hwnd, reinterpret_cast<HMENU>(-1), hInstance, NULL);
	CreateWindow(L"static", to_wstring(_simFrame).c_str(), WS_CHILD | WS_VISIBLE,
		140, 370, 40, 20, hwnd, reinterpret_cast<HMENU>(COM::FRAME_TEXT), hInstance, NULL);

	if (_updateFlag)
	{
		EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::NEXTSTEP)), false);
	}

	SetTimer(hwnd, 1, 10, NULL);
}

void SimulationManager::iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
	switch (LOWORD(wParam))
	{
		// ### Execution buttons ###
		case static_cast<int>(COM::PLAY):
		{
			_updateFlag = !_updateFlag;
			SetDlgItemText(hwnd, static_cast<int>(COM::PLAY), _updateFlag ? L"¡«" : L"¢º");

			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::STOP)), true);
			EnableWindow(GetDlgItem(hwnd, static_cast<int>(COM::NEXTSTEP)), !_updateFlag);
		}
		break;
		case static_cast<int>(COM::STOP):
		{
			_dxapp->resetSimulationState();
		}
		break;
		case static_cast<int>(COM::NEXTSTEP):
		{
			iUpdate();
			_dxapp->update();
			_dxapp->draw();
		}
		break;
		
	}
}

void SimulationManager::iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance)
{
}

void SimulationManager::iWMTimer(HWND hwnd)
{
	SetDlgItemText(hwnd, static_cast<int>(COM::TIME_TEXT), to_wstring(_simTime).c_str());
	SetDlgItemText(hwnd, static_cast<int>(COM::FRAME_TEXT), to_wstring(_simFrame).c_str());
}

void SimulationManager::iWMDestory(HWND hwnd)
{
	KillTimer(hwnd, 1);
}
// #######################################################################################
#pragma endregion