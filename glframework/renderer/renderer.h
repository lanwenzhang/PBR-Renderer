#pragma once

#include <vector>
#include "../core.h"
#include "../mesh/mesh.h"
#include "../../application/camera/camera.h"
#include "../light/directionalLight.h"
#include "../light/pointLight.h"
#include "../light/spotLight.h"
#include "../light/ambientLight.h"
#include "../shader.h"
#include "../scene.h"
#include "../framebuffer/framebuffer.h"

class Renderer {

public:

	Renderer();

	~Renderer();


	void render(Scene* scene,Camera* camera, std::vector<PointLight*> pointLights, unsigned int fbo = 0);

	void renderObject(Object* object, Camera* camera, std::vector<PointLight*> pointLights);

	//void renderShadowMap(Camera* camera, const std::vector<Mesh*>& meshes, PointLight* pointLight);

	void msaaResolve(Framebuffer* src, Framebuffer* dst);

	void setClearColor(glm::vec3 color);

	Material* mGlobalMaterial{ nullptr };

private:

	// Category objects into opacity and transparent 
	void projectObject(Object* obj);

	Shader* pickShader(MaterialType type);

	void setDepthState(Material* material);
	void setPolygonOffsetState(Material* material);
	void setStencilState(Material* material);
	void setBlendState(Material* material);
	void setFaceCullingState(Material* material);

private:

	// Shaders
	Shader* mPhongShader{ nullptr };
	Shader* mWhiteShader{ nullptr };
	Shader* mDepthShader{ nullptr };
	Shader* mOpacityMaskShader{ nullptr };
	Shader* mScreenShader{ nullptr };
	Shader* mCubeShader{ nullptr };
	Shader* mPhongEnvShader{ nullptr };
	Shader* mPhongInstanceShader{ nullptr };
	Shader* mGrassInstanceShader{ nullptr };

	Shader* mPhongNormalShader{ nullptr };
	Shader* mPhongParallaxShader{ nullptr };

	Shader* mShadowShader{ nullptr };
	Shader* mPhongShadowShader{ nullptr };
	Shader* mPhongCSMShadowShader{ nullptr };

	Shader* mShadowDistanceShader{ nullptr };
	Shader* mPhongPointShadowShader{ nullptr };

	Shader* mPbrShader{ nullptr };

	// Objects
	std::vector<Mesh*> mOpacityObjects{};
	std::vector<Mesh*> mTransparentObjects{};

};