#include <iostream>
#include "glframework/core.h"
#include "glframework/shader.h"
#include <string>
#include <assert.h>
#include "wrapper/checkError.h"
#include "application/Application.h"
#include "glframework/texture.h"

#include "application/camera/perspectiveCamera.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/trackBallCameraControl.h"
#include "application/camera/gameCameraControl.h"


#include "glframework/geometry.h"
#include "glframework/material/phongMaterial.h"
#include "glframework/material/screenMaterial.h"
#include "glframework/material/cubeMaterial.h"
#include "glframework/material/phongEnvMaterial.h"

#include "glframework/material/advanced/pbrMaterial.h"

#include "glframework/mesh/mesh.h"
#include "glframework/mesh/instancedMesh.h"
#include "glframework/renderer/renderer.h"
#include "glframework/light/pointLight.h"
#include "glframework/light/spotLight.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "glframework/scene.h"
#include "application/assimpLoader.h"

#include "glframework/framebuffer/framebuffer.h"
#include "glframework/renderer/bloom.h"

// 1 Variables
// 1.1 Window
int WIDTH = 2560;
int HEIGHT = 1080;

glm::vec3 clearColor{};


// 1.2 Objects
std::vector<PointLight*> pointLights{};
AmbientLight* ambLight = nullptr;
Camera* camera = nullptr;
GameCameraControl* cameraControl = nullptr;


// 1.3 Render
Renderer* renderer = nullptr;
Scene* scene = nullptr;
Scene* sceneOff = nullptr;
Bloom* bloom = nullptr;

Framebuffer* fboMulti = nullptr;
Framebuffer* fboResolve = nullptr;
ScreenMaterial* screenMat = nullptr;
PbrMaterial* material = nullptr;

// --------------------------------------------

// 2 Mouse and keyboard event
void OnResize(int width, int height) {

    GL_CALL(glViewport(0, 0, width, height));
}

void OnKey(int key, int action, int mods) {

    cameraControl->onKey(key, action, mods);
}

void OnMouse(int button, int action, int mods) {

    double x, y;
    glApp->getCursorPosition(&x, &y);
    cameraControl->onMouse(button, action, x, y);

}

void OnCursor(double xpos, double ypos) {

    cameraControl->onCursor(xpos, ypos);
}

void OnScroll(double offset) {

    cameraControl->onScroll(offset);
}


// --------------------------------------------

// 3 Prepare render objects and camera

void prepare() {

    fboMulti = Framebuffer::createMultiSampleHDRFbo(WIDTH, HEIGHT);
    fboResolve = Framebuffer::createHDRFbo(WIDTH, HEIGHT);

    renderer = new Renderer();
    bloom = new Bloom(WIDTH, HEIGHT);
    sceneOff = new Scene();
    scene = new Scene();

    // Pass 01
    auto geometry = Geometry::createSphere(1.0f);
    material = new PbrMaterial();
    material->mAlbedo = new Texture("assets/textures/pbr/slab_tiles_diff_2k.jpg", 0, GL_SRGB_ALPHA);
    material->mNormal = Texture::createNearestTexture("assets/textures/pbr/slab_tiles_nor_gl_2k.jpg");
    material->mRoughness = Texture::createNearestTexture("assets/textures/pbr/slab_tiles_rough_2k.jpg");
    material->mMetallic = Texture::createNearestTexture("assets/textures/pbr/slab_tiles_arm_2k.jpg");
    material->mIrradianceIndirect = Texture::createExrCubeMap(
        { 
            "assets/textures/pbr/IBL/env_0.exr",
            "assets/textures/pbr/IBL/env_1.exr",
            "assets/textures/pbr/IBL/env_2.exr",
            "assets/textures/pbr/IBL/env_3.exr",
            "assets/textures/pbr/IBL/env_4.exr",
            "assets/textures/pbr/IBL/env_5.exr",
        });


    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            auto mesh = new Mesh(geometry, material);
            mesh->setPosition(glm::vec3(i * 2.5, j * 2.5, 0.0f));
            sceneOff->addChild(mesh);
        }
    }

    auto boxGeo = Geometry::createBox(1.0f, true);
    auto boxMat = new CubeMaterial();
    boxMat->mDiffuse = Texture::createExrTexture("assets/textures/pbr/qwantani_dusk_1_4k.exr");
    auto boxMesh = new Mesh(boxGeo, boxMat);
    sceneOff->addChild(boxMesh);


    // Pass 02: Post-process 
    auto sgeo = Geometry::createScreenPlane();
    screenMat = new ScreenMaterial();
    screenMat->mScreenTexture = fboResolve->mColorAttachment;
    auto smesh = new Mesh(sgeo, screenMat);
    scene->addChild(smesh);


    // 4 Create light
    glm::vec3 lightPositions[] = {
            glm::vec3(-3.0f,  3.0f, 10.0f),
            glm::vec3(3.0f,  3.0f, 10.0f),
            glm::vec3(-3.0f, -3.0f, 10.0f),
            glm::vec3(3.0f, -3.0f, 10.0f),
    };
    glm::vec3 lightColors[] = {
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f),
        glm::vec3(300.0f, 300.0f, 300.0f)
    };
    for (int i = 0; i < 4; i++) {
        auto pointLight = new PointLight();
        pointLight->setPosition(lightPositions[i]);
        pointLight->mColor = lightColors[i];
        pointLights.push_back(pointLight);
    }
}



void prepareCamera() {

    camera = new PerspectiveCamera(
        60.0f,
        (float)glApp->getWidth() / glApp->getHeight(),
        0.1f,
        1000.0f);

    cameraControl = new GameCameraControl();
    cameraControl->setCamera(camera);
    cameraControl->setSensitivity(0.4f);
    cameraControl->setSpeed(0.1f);
}

// --------------------------------------------

// 4 Prepare UI

void initIMGUI() {

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(glApp->getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void renderIMGUI() {

    // 1 Initial
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2 GUI widget
    ImGui::Begin("MaterialEditor");
    ImGui::SliderFloat("exposure:", &screenMat->mExposure, 0.0f, 10.0f);
    ImGui::SliderFloat("Threshold:", &bloom->mThreshold, 0.0f, 100.0f);
    ImGui::SliderFloat("BloomAttenuation:", &bloom->mBloomAttenuation, 0.0f, 1.0f);
    ImGui::SliderFloat("BloomIntensity:", &bloom->mBloomIntensity, 0.0f, 1.0f);
    ImGui::SliderFloat("BloomRadius:", &bloom->mBloomRadius, 0.0f, 1.0f);


    ImGui::End();

    // 3 Render
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(glApp->getWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


// 5 Render
int main() {

    // 5.1 Initial the window
    if (!glApp->init(WIDTH, HEIGHT)) {
        return -1;
    }

    // 5.2 Size and keyboard callback
    glApp->setResizeCallback(OnResize);
    glApp->setKeyBoardCallback(OnKey);
    glApp->setMouseCallback(OnMouse);
    glApp->setCursorCallback(OnCursor);
    glApp->setScrollCallback(OnScroll);

    // 5.3 Set openGl rendering viewport and clear canvas color
    GL_CALL(glViewport(0, 0, WIDTH, HEIGHT));
    GL_CALL(glClearColor(0.0f, 0.18f, 0.65f, 1.0f));

    // 4 Set up camera, objects, UI
    prepareCamera();
    prepare();
    initIMGUI();

    // 4 Set window loop
    while (glApp->update()) {

        cameraControl->update();

        renderer->setClearColor(clearColor);

        // 4.1 Pass 1
        renderer->render(sceneOff, camera, pointLights, fboMulti->mFBO);
       
        // 4.2 Pass 2 MSAA + Bloom
        renderer->msaaResolve(fboMulti, fboResolve);
        bloom->doBloom(fboResolve);
        renderer->render(scene, camera, pointLights);

        renderIMGUI();
    }

    glApp->destroy();

    return 0;
}