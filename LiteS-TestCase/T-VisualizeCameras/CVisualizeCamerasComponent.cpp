
#include "CVisualizeCamerasComponent.h"
#include <CEngine.h>
#include <tbb/blocked_range.h>
#include <tbb/blocked_range2d.h>
#include <tbb/concurrent_vector.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/spin_mutex.h>
#include <tbb/task_scheduler_init.h>
#include <algorithm>
#include <array>
#include <fstream>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>

#include "CBVHACCEL.h"
#include "CPointCloudMesh.h"
#include "trajectory_util.h"
#include "util.h"

const char CAMERALOG[] = "../../../my_test/camera.log";
const char CAMERALOGUNREAL[] = "../../../my_test/cameraUnreal.log";

CVisualizeCamerasComponent::CVisualizeCamerasComponent(const map<string, CPass*>& vPass,
                                               CScene* vScene,const std::string vResourceDir)
    : CPointCloudComponent(vPass, vScene,vResourceDir) {
  DisplayPass = this->m_Pass.at("display");
}

CVisualizeCamerasComponent::~CVisualizeCamerasComponent() = default;

void CVisualizeCamerasComponent::visualizeMyAsiaCamera(string vPath) {
  CMesh* cameraMesh;
  vector<Vertex> cameraVertexVector;

  LiteS_Trajectory::loadTrajectoryMVESpline(vPath, cameraVertexVector);

  LiteS_Trajectory::saveTrajectory(CAMERALOG, cameraVertexVector);
  LiteS_Trajectory::saveTrajectoryUnreal(CAMERALOGUNREAL, cameraVertexVector,false);
  cameraMesh =
      new CPointCloudMesh(cameraVertexVector, glm::vec3(0.3f, 0.7f, 1.f), 15);
  cameraMesh->isRender = true;
  cameraMesh->isRenderNormal = true;
  // Lock the target arrays
  std::lock_guard<std::mutex> lg(CEngine::m_addMeshMutex);
  CEngine::toAddModels.push_back(std::make_pair("camera", cameraMesh));

  cameraMesh->changeColor(glm::vec3(1.f, 0.f, 0.f), 0);
  cameraMesh->changeColor(glm::vec3(1.f, 0.f, 0.f), 1);
}

void CVisualizeCamerasComponent::extraAlgorithm() {
  visualizeMyAsiaCamera(CEngine::m_Arguments.at("spline_file"));

  // staticsForPath(15.f);

  std::cout << "Extra Algorithm done" << endl;

  this->waitForStepSignal();
  return;
}

void CVisualizeCamerasComponent::extraInit() {}
