#pragma once
#include "CComponent.h"
#include <mutex>
#include "CPointCloudComponent.h"

class CPathComponent : public CPointCloudComponent {
public:
	CPathComponent(CScene * vScene);
	~CPathComponent();
	void sample_mesh(string vPath);
	void fixDiscontinuecy(string vPath);
	void addSafeSpace(string vPath);
	void reconstructMesh(string vPath);
	void simplexPoint();
	void extraAlgorithm() override;
	void extraInit() override;

private:
	;
};

