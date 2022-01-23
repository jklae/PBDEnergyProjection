#pragma once
#include <iostream>
#include "Win32App.h" // This includes ISimulation.h

class PBDSimulation : public ISimulation
{
public:
	PBDSimulation(float timeStep);
	~PBDSimulation();

#pragma region Implementation
	// ################################## Implementation ####################################
	// Simulation methods
	void iUpdate() override;
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer) override;

	// Mesh methods
	std::vector<Vertex>& iGetVertice() override;
	std::vector<unsigned int>& iGetIndice() override;
	UINT iGetVertexBufferSize() override;
	UINT iGetIndexBufferSize() override;

	// DirectX methods
	void iCreateObject(std::vector<ConstantBuffer>& constantBuffer) override;
	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i) override;
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i) override;
	void iSetDXApp(DX12App* dxApp) override;
	UINT iGetConstantBufferSize() override;
	DirectX::XMINT3 iGetObjectCount() override;
	DirectX::XMFLOAT3 iGetObjectSize() override;
	DirectX::XMFLOAT3 iGetObjectPositionOffset() override;
	bool iIsUpdated() override;

	// WndProc methods
	void iWMCreate(HWND hwnd, HINSTANCE hInstance) override;
	void iWMCommand(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) override;
	void iWMHScroll(HWND hwnd, WPARAM wParam, LPARAM lParam, HINSTANCE hInstance) override;
	void iWMTimer(HWND hwnd) override;
	void iWMDestory(HWND hwnd) override;
	// #######################################################################################

private:
	DirectX::XMINT2 _nodeCount;
	DX12App* _dxApp = nullptr;

	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;

	std::vector<DirectX::XMFLOAT2> _nodePosition;
	std::vector<DirectX::XMFLOAT2> _nodeVelocity;

	//
	float _timeStep;
	float _floorPosition;

	//void _springConstraint(DirectX::XMFLOAT2 p1, DirectX::XMFLOAT2 p2, float d);

	void _update();

	void _force();
	void _project();
	void _updateVelocity();
};

