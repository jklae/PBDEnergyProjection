#pragma once

// Console window is displayed in debug mode.
#ifdef _DEBUG
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "PBDSimulation.h" // This includes Win32App.h


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    float dt = 0.001f;
    PBDSimulation* fluidsim = new PBDSimulation(dt);

    DX12App* dxapp = new DX12App();
    dxapp->setCameraProperties(
        PROJ::PERSPECTIVE, 
        0.0f,               // orthogonal distance
        10.0f, 0.0f, 0.0f);  // radius, theta, phi
    dxapp->setBackgroundColor(DirectX::Colors::Black);

    Win32App winApp(500, 500);
    winApp.setWinName(L"PBD Energy Projection");
    winApp.setWinOffset(400, 200);
    winApp.initialize(hInstance, dxapp, fluidsim);

    return winApp.run();
}