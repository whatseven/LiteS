#include "CPointCloudComponent.h"

CPointCloudComponent::CPointCloudComponent(const map<string, CPass*>& vPass,
                                           CScene* vScene,const std::string vResourceDir)
    : CComponent(vPass, vScene,vResourceDir),
      DisplayPass(nullptr),
      isRenderNormal(true),
      normalRenderPass(NULL) {}

void CPointCloudComponent::stepExtraAlgorithm() {
  std::unique_lock<std::mutex> lock(this->m_shouldStepMutex);
  this->m_shouldStep = true;
  this->m_shouldStepCV.notify_one();
}

void CPointCloudComponent::continueExtraAlgorithm() {
  m_shouldContinue = true;

  std::unique_lock<std::mutex> lock1(this->m_shouldStepMutex);
  this->m_shouldStep = true;
  this->m_shouldStepCV.notify_one();

  // std::unique_lock<std::mutex> lock(this->m_shouldContinueMutex);
  // this->m_shouldContinue = true;
  // this->m_shouldContinueCV.notify_one();
}

void CPointCloudComponent::waitForContinueSignal() {
  m_shouldContinue = false;

  // std::unique_lock<std::mutex> lock(this->m_shouldContinueMutex);
  // while (!this->m_shouldContinue)
  //	this->m_shouldContinueCV.wait(lock);
  // this->m_shouldContinue = false;
}

void CPointCloudComponent::waitForStepSignal() {
  if (!m_shouldContinue) {
    std::unique_lock<std::mutex> lock(this->m_shouldStepMutex);
    while (!this->m_shouldStep) this->m_shouldStepCV.wait(lock);
    this->m_shouldStep = false;
  }
}

void CPointCloudComponent::run() {
  DisplayPass->beginPass();

  const glm::mat4 projectionMatrix =
      glm::perspective(glm::radians(this->m_Scene->m_Camera->Zoom),
                       static_cast<float>(this->m_Scene->m_WindowWidth) /
                           static_cast<float>(this->m_Scene->m_WindowHeight),
                       0.1f, 1000000.0f);
  const glm::mat4 viewMatrix = this->m_Scene->m_Camera->GetViewMatrix();
  const glm::mat4 modelMatrix =
      glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 0.1));

  DisplayPass->getShader()->setMat4("projection", projectionMatrix);
  DisplayPass->getShader()->setMat4("model", modelMatrix);
  DisplayPass->getShader()->setMat4("view", viewMatrix);

  for (auto const& item : this->m_Scene->m_SystemModel)
    item->Draw(DisplayPass->getShader(), false);
  DisplayPass->endPass(this->m_Scene);

  if (this->isRenderNormal) {
    if (normalRenderPass == NULL)
      if (__generateRenderNormalPass()) return;

    // This will clear viewport
    // normalRenderPass->beginPass();
    normalRenderPass->getShader()->use();

    normalRenderPass->getShader()->setMat4("projection", projectionMatrix);
    normalRenderPass->getShader()->setMat4("model", modelMatrix);
    normalRenderPass->getShader()->setMat4("view", viewMatrix);
    normalRenderPass->endPass(this->m_Scene);
  }
}

bool CPointCloudComponent::__generateRenderNormalPass() {
  normalRenderPass = new CPass(true);
  normalRenderPass->m_Width = this->m_Scene->m_WindowWidth;
  normalRenderPass->m_Height = this->m_Scene->m_WindowHeight;
  std::string VertFile =
      this->m_ResourceDir + "shaders/NormalVert.glsl";
  std::string GeomFile =
      this->m_ResourceDir + "shaders/NormalGeom.glsl";
  std::string FragFile =
      this->m_ResourceDir + "shaders/NormalFrag.glsl";
  if (!normalRenderPass->setShader(VertFile.c_str(), FragFile.c_str(), GeomFile.c_str())) {
    std::cout << "Shader for "
              << "Normal"
              << " init failed" << std::endl;
    return false;
  }
  return true;
}