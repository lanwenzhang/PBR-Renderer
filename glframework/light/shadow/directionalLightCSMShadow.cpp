#include "directionalLightCSMShadow.h"
#include "../../../application/camera/orthographicCamera.h"
#include "../../application/camera/perspectiveCamera.h"
#include "../../tools/tools.h"

DirectionalLightCSMShadow::DirectionalLightCSMShadow(){

	mRenderTarget = Framebuffer::createCSMShadowFbo(1024, 1024, mLayerCount);


}

DirectionalLightCSMShadow::~DirectionalLightCSMShadow() {
	
	delete mRenderTarget;
}

void DirectionalLightCSMShadow::generateCascadeLayers(std::vector<float>& layers, float near, float far) {

	layers.clear();
	
	for (int i = 0; i <= mLayerCount; i++) {

		float layer = near * glm::pow((far / near), (float)i / (float)mLayerCount);
		layers.push_back(layer);
	}

}

glm::mat4 DirectionalLightCSMShadow::getLightMatrix(Camera* camera, glm::vec3 lightDir, float near, float far) {

	auto perpCamera = (PerspectiveCamera*)camera;
	auto perpViewMatrix = perpCamera->getViewMatrix();
	auto perpProjectiotnMatrix = glm::perspective(glm::radians(perpCamera->mFovy), perpCamera->mAspect, near, far);
	auto corners = Tools::getFrustumCornersWorldSpace(perpProjectiotnMatrix * perpViewMatrix);


	// 2 Eight corners
	glm::vec3 center = glm::vec3(0.0f);
	for (int i = 0; i < corners.size(); i++) {

		center += glm::vec3(corners[i]);
	}

	center /= corners.size();

	auto lightViewMatrix = glm::lookAt(center, center + lightDir, glm::vec3(0.0, 1.0, 0.0));

	// 3 ViewMatrix
	float minX = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::min();
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::min();
	float minZ = std::numeric_limits<float>::max();
	float maxZ = std::numeric_limits<float>::min();

	for (const auto& v : corners) {

		const auto pt = lightViewMatrix * v;
		minX = std::min(minX, pt.x);
		maxX = std::max(maxX, pt.x);
		minY = std::min(minY, pt.y);
		maxY = std::max(maxY, pt.y);
		minZ = std::min(minZ, pt.z);
		maxZ = std::max(maxZ, pt.z);

	}

	// 4 Adjust the length of AABB box
	maxZ *= 10;
	minZ *= 10;

	// 5 Calculate light projection matrix
	auto lightProjectionMatrix = glm::ortho(minX, maxX, minY, maxY, -maxZ, -minZ);

	return lightProjectionMatrix * lightViewMatrix;
}

std::vector<glm::mat4> DirectionalLightCSMShadow::getLightMatrices(Camera* camera, glm::vec3 lightDir, const std::vector<float>& clips) {

	std::vector<glm::mat4> matrices;
	for (int i = 0; i < clips.size() - 1; i++) {

		auto lightMatrix = getLightMatrix(camera, lightDir, clips[i], clips[i + 1]);
		matrices.push_back(lightMatrix);
	}

	return matrices;
}

void DirectionalLightCSMShadow::setRenderTargetSize(int width, int height) {

	if (mRenderTarget != nullptr) {
		
		delete mRenderTarget;
	}

	mRenderTarget = Framebuffer::createCSMShadowFbo(1024, 1024, mLayerCount);
}
