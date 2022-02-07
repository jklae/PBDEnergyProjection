#include "SimulationManager.h"
using namespace DirectX;
using namespace std;

SimulationManager::SimulationManager(int x, int y, float timeStep)
{
	_sim = new PBDSimulation(x, y, timeStep);
}

SimulationManager::~SimulationManager()
{
	delete _sim;
}


#pragma region Implementation
// ################################## Implementation ####################################
// Simulation methods
void SimulationManager::iUpdate()
{
	clock_t startTime = clock();
	_sim->iUpdate(_projFlag);
	clock_t endTime = clock();

	_simTime += endTime - startTime; // ms
	_simFrame++;
}

void SimulationManager::iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer)
{
	_sim->iResetSimulationState(constantBuffer);

	_simTime = 0;
	_simFrame = 0;
}


// Mesh methods
vector<Vertex>& SimulationManager::iGetVertice()
{
	return _sim->iGetVertice();
}

vector<unsigned int>& SimulationManager::iGetIndice()
{
	return _sim->iGetIndice();
}

UINT SimulationManager::iGetVertexBufferSize()
{
	return _sim->iGetVertexBufferSize();
}

UINT SimulationManager::iGetIndexBufferSize()
{
	return _sim->iGetIndexBufferSize();
}


// DirectX methods
void SimulationManager::iCreateObject(std::vector<ConstantBuffer>& constantBuffer)
{
	_sim->iCreateObject(constantBuffer);
}

void SimulationManager::iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i)
{
	_sim->iUpdateConstantBuffer(constantBuffer, i);
}

void SimulationManager::iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i)
{
	_sim->iDraw(mCommandList, size, indexCount, i);
}

void SimulationManager::iSetDXApp(DX12App* dxApp)
{
	_dxapp = dxApp;
}

UINT SimulationManager::iGetConstantBufferSize()
{
	return _sim->iGetConstantBufferSize();
}

DirectX::XMINT3 SimulationManager::iGetObjectCount()
{
	return _sim->iGetObjectCount();
}

DirectX::XMFLOAT3 SimulationManager::iGetObjectSize()
{
	return _sim->iGetObjectSize();
}

DirectX::XMFLOAT3 SimulationManager::iGetObjectPositionOffset()
{
	return _sim->iGetObjectPositionOffset();
}

bool SimulationManager::iIsUpdated()
{
	return _updateFlag;
}


void SimulationManager::iWMCreate(HWND hwnd, HINSTANCE hInstance)
{
	CreateWindow(L"button", _projFlag ? L"Proj : ON " : L"Proj : OFF ", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		90, 30, 100, 25, hwnd, reinterpret_cast<HMENU>(COM::PROJ_BTN), hInstance, NULL);

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
	case static_cast<int>(COM::PROJ_BTN):
	{
		_projFlag = !_projFlag;
		SetDlgItemText(hwnd, static_cast<int>(COM::PROJ_BTN), _projFlag ? L"Proj : ON " : L"Proj : OFF");
		_dxapp->resetSimulationState();
	}
	break;

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
	// #####################
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