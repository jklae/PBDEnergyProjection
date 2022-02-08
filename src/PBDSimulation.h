#pragma once
#include <fstream>
#include "SpringConstraint.h"
#include "Win32App.h" // This includes ISimulation.h

class PBDSimulation
{
public:
	PBDSimulation(int x, int y, float timeStep, bool projFlag);
	~PBDSimulation();

#pragma region Implementation
	// ################################## Implementation ####################################
	// Simulation methods
	void iUpdate();
	void iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer);

	// Mesh methods
	std::vector<Vertex>& iGetVertice();
	std::vector<unsigned int>& iGetIndice();
	UINT iGetVertexBufferSize();
	UINT iGetIndexBufferSize();

	// DirectX methods
	void iCreateObject(std::vector<ConstantBuffer>& constantBuffer);
	void iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i);
	void iDraw(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& mCommandList, int size, UINT indexCount, int i);
	UINT iGetConstantBufferSize();
	DirectX::XMINT3 iGetObjectCount();
	DirectX::XMFLOAT3 iGetObjectSize();
	DirectX::XMFLOAT3 iGetObjectPositionOffset();
	// #######################################################################################

private:
	//
	DirectX::XMINT2 _nodeCount;
	std::vector<Vertex> _vertices;
	std::vector<unsigned int> _indices;

	//
	enum class GradType { X, V };
	std::vector<DirectX::XMFLOAT2> _nodePosition;
	std::vector<DirectX::XMFLOAT2> _nodeVelocity;
	std::vector<DirectX::XMFLOAT2> _newPosition;
	std::vector<SpringConstraint> _constraint;
	float _timeStep;
	float _floorPosition;
	float _stride;
	float _gravity;
	float _hamiltonian = 0.0f;
	int _lineCount;
	float _alpha;
	bool _projFlag;
	DirectX::XMFLOAT2 _posOffset;

	std::ofstream _filePBD;

	void _initializeNode(std::vector<ConstantBuffer>& constantBuffer);
	void _solvePBD();
	float _computeHamiltonian();
	float _computeHamiltonianGradient(GradType gradType, int j);
	void _projectHamiltonian();
};

