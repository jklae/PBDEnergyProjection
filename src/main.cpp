#pragma once

// Console window is displayed in debug mode.
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "SimulationManager.h" // This includes Win32App.h

using namespace DXViewer::xmint3;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    int x = 8;
    int y = 16;
    float dt = 0.01f;
    SimulationManager* sim = new SimulationManager(x, y, dt);

    DX12App* dxapp = new DX12App();
    dxapp->setCameraProperties(
        PROJ::ORTHOGRAPHIC,
        static_cast<float>(max_element(sim->iGetObjectCount())) * 0.015f,
        2.0f, 0.0f, 0.0f);
    dxapp->setBackgroundColor(DirectX::Colors::Black);

    Win32App winApp(500, 500);
    winApp.setWinName(L"PBD Energy Projection");
    winApp.setWinOffset(400, 200);
    winApp.initialize(hInstance, dxapp, sim);

    return winApp.run();
}