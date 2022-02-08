#include "PBDSimulation.h"

using namespace DirectX;
using namespace std;
using namespace DXViewer::xmfloat2;
using namespace DXViewer::xmint2;

PBDSimulation::PBDSimulation(int x, int y, float timeStep, 
	bool projFlag, DirectX::XMFLOAT2 posOffset, float floorPosition)
	:_timeStep(timeStep), _projFlag(projFlag), _posOffset(posOffset), _floorPosition(floorPosition)
{
	// Int, Float initialization
	_nodeCount = { x, y };
	_stride = 2.0f;
	_gravity = 9.8f;
	_alpha = 0.001f; // Inverse k

	// Vector initialization
	size_t vSize = static_cast<size_t>(_nodeCount.x) * static_cast<size_t>(_nodeCount.y);
	_newPosition.assign(vSize, { 0.0f, 0.0f });

	//_filePBD.open("filePBD2.txt");
}

PBDSimulation::~PBDSimulation()
{
	//_filePBD.close();
}

void PBDSimulation::_initializeNode(std::vector<ConstantBuffer>& constantBuffer)
{
	// Body initialization
	for (int j = 0; j < _nodeCount.y; j++)
	{
		for (int i = 0; i < _nodeCount.x; i++)
		{
			float f_i = static_cast<float>(i);
			float f_j = static_cast<float>(j);

			XMFLOAT2 pos = 
			{ 
				_posOffset.x + f_i * _stride,
				_posOffset.y + f_j * _stride
			};
			_nodePosition.push_back(pos);
			_nodeVelocity.push_back(XMFLOAT2(0.0f, 0.0f));

			ConstantBuffer objectCB;
			objectCB.world = DXViewer::util::transformMatrix(pos.x, pos.y, 0.0f, 1.0f);
			objectCB.worldViewProj = DXViewer::util::transformMatrix(0.0f, 0.0f, 0.0f);
			objectCB.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			constantBuffer.push_back(objectCB);
		}
	}
}

void PBDSimulation::_solvePBD()
{
	_newPosition = _nodePosition;
	for (SpringConstraint& sp : _constraint)
	{
		sp.setLambda(XMFLOAT2(0.0f, 0.0f));
	}

	float dt = _timeStep;
	int nSteps = 20;
	float subdt = dt / static_cast<float>(nSteps);

	for (int t = 0; t < nSteps; t++)
	{
		// External force
		for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
		{
			_newPosition[j].y += (subdt * _nodeVelocity[j].y) + (subdt * subdt * -_gravity);
		}

		// Constraint projection
		for (int iter = 0; iter < 10; iter++)
		{
			for (SpringConstraint& sp : _constraint)
			{
				sp.projectConstraint(subdt);
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

float PBDSimulation::_computeHamiltonian()
{
	float H = 0.0f;

	for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
	{
		H += 0.5 * (_nodeVelocity[j] * _nodeVelocity[j]);					// Kinetic energy
		H += _gravity * (_nodePosition[j].y - _floorPosition);				// Potential energy
	}

	for (SpringConstraint& sp : _constraint)
	{
		H += sp.computeElasticEnergy();										// Elastic energy
	}

	return H;
}

float PBDSimulation::_computeHamiltonianGradient(GradType gradType, int j)
{
	float gradH = 0.0f;
	switch (gradType)
	{
	case GradType::V:
		gradH += _nodeVelocity[j].x + _nodeVelocity[j].y;
		break;

	case GradType::X:
		gradH += _gravity;

		for (SpringConstraint& sp : _constraint)
		{
			if (sp.getP1Index() == j)
				gradH += sp.computeElasticEnergyGradient(j);
			else if (sp.getP2Index() == j)
				gradH -= sp.computeElasticEnergyGradient(j);
		}
		break;
	}

	return gradH;
}

void PBDSimulation::_projectHamiltonian()
{
	float H = _hamiltonian;
	float delta_x = 0.0f, delta_v = 0.0f;
	float new_H;
	float grad_Hx, grad_Hv;
	float pow_grad_Hx = 0.0f, pow_grad_Hv = 0.0f;
	float h = _timeStep;

	for (int iter = 0; iter < 1; iter++)
	{
		new_H = _computeHamiltonian();

		for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
		{
			grad_Hx = _computeHamiltonianGradient(GradType::X, j);
			grad_Hv = _computeHamiltonianGradient(GradType::V, j);

			pow_grad_Hx += pow(grad_Hx, 2.0f);
			pow_grad_Hv += (1.0f / pow(h, 2.0f)) * pow(grad_Hv, 2.0f);

		}

		for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
		{
			grad_Hx = _computeHamiltonianGradient(GradType::X, j);
			grad_Hv = _computeHamiltonianGradient(GradType::V, j);

			delta_x = grad_Hx * (-new_H + H) / (pow_grad_Hx + pow_grad_Hv);
			delta_v = (1.0f / pow(h, 2.0f)) * grad_Hv * (-new_H + H) / (pow_grad_Hx + pow_grad_Hv);

			_nodePosition[j] = _nodePosition[j] + delta_x;
			_nodeVelocity[j] = _nodeVelocity[j] + delta_v;

			// Floor boundary condition
			for (int j = 0; j < _nodeCount.x * _nodeCount.y; j++)
			{
				if (_nodePosition[j].y < _floorPosition)
					_nodePosition[j].y = _floorPosition;
			}

		}
	}
}


#pragma region Implementation
// ################################## Implementation ####################################
// Simulation methods
void PBDSimulation::iUpdate()
{
	_solvePBD();
	if (_projFlag) _projectHamiltonian();
	//_filePBD << _computeHamiltonian() << endl;
}

void PBDSimulation::iResetSimulationState(std::vector<ConstantBuffer>& constantBuffer)
{
	_nodePosition.clear();
	_nodeVelocity.clear();
	constantBuffer.clear();

	_initializeNode(constantBuffer);
}

// DirectX methods
void PBDSimulation::iCreateObject(std::vector<ConstantBuffer>& constantBuffer)
{
	// Node initialization
	_initializeNode(constantBuffer);

	// Constraint initialization
	for (int j = 0; j < _nodeCount.x * _nodeCount.y - 1; j++)
	{
		for (int i = j + 1; i < _nodeCount.x * _nodeCount.y; i++)
		{
			XMFLOAT2& curr_p1 = _nodePosition[i];
			XMFLOAT2& curr_p2 = _nodePosition[j];
			XMFLOAT2& new_p1 = _newPosition[i];
			XMFLOAT2& new_p2 = _newPosition[j];
			XMFLOAT2 d = fabsxmf2(_nodePosition[j] - _nodePosition[i]);
			float dist = sqrtf(d.x * d.x + d.y * d.y);

			if (dist < 1.5f * _stride)
			{
				SpringConstraint sp(curr_p1, curr_p2, new_p1, new_p2, i, j, d, _alpha);
				_constraint.push_back(sp);
			}
		}
		
	}

	// Hamiltonian initialization
	_hamiltonian = _computeHamiltonian();

}

void PBDSimulation::iUpdateConstantBuffer(std::vector<ConstantBuffer>& constantBuffer, int i, int simIndex)
{
	int nodeCount = _nodeCount.x * _nodeCount.y;

	// if      simIndex == 0 then      0    <= i < nodeCount
	// else if simIndex == 1 then nodeCount <= i < nodeCount * 2
	if (i >= simIndex * nodeCount && i < (simIndex + 1) * nodeCount)
	{
		int idx = i % nodeCount; // Prevent the out of index
		constantBuffer[i].world._41 = _nodePosition[idx].x;
		constantBuffer[i].world._42 = _nodePosition[idx].y;
	}
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
	return { _stride, _stride, 0.0f };
}

DirectX::XMFLOAT3 PBDSimulation::iGetObjectPositionOffset()
{
	return { 0.0f, 0.0f, 0.0f };
}
// #######################################################################################
#pragma endregion