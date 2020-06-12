#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <GL/glew.h>
#include <GL/GLU.h>
#include <glm/glm.hpp>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_opengl.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include "helperFunctions.h"
#include "default_constants.h"

#include "Camera.h"
#include "MeshPack.h"
#include "ShaderPack.h"

#include "SSSS.h"

#include "Scene.h"
//#include "LightList.h"

#include "FBO.h"
#include "GeoFBO.h"
#include "CubeFBO.h"

// Meshes 
Scene scene;

// SDL variables
SDL_Window* _window;
SDL_GLContext context;

// Camera parameters.
std::unique_ptr<Camera> mainCam;

int mouse_position[2];
bool mouse_pressed = false;

bool quit = false;
bool renderloop_paused = false;
bool windowsize_changed = true;
bool crystalball_viewing = false;
float movement_stepsize = DEFAULT_KEY_MOVEMENT_STEPSIZE;

//Triangle to draw to illustrate picking
size_t picked_triangle_index = 0;
MeshPack* picked_object = nullptr;

// Process the event.  
void processEvents(SDL_Event current_event)
{
	switch (current_event.type)
	{
		// window close button is pressed
	case SDL_QUIT:
	{
		quit = true;
		break;
	}
	case SDL_KEYDOWN:
	{
		if (current_event.key.keysym.sym == SDLK_ESCAPE)
			quit = true;
		if (current_event.key.keysym.sym == SDLK_r)
			mainCam->reset();
		if (current_event.key.keysym.sym == SDLK_UP || current_event.key.keysym.sym == SDLK_w)
			mainCam->moveForward(movement_stepsize);
		if (current_event.key.keysym.sym == SDLK_DOWN || current_event.key.keysym.sym == SDLK_s)
			mainCam->moveBack(movement_stepsize);
		if (current_event.key.keysym.sym == SDLK_LEFT || current_event.key.keysym.sym == SDLK_a)
			mainCam->turnLeft(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
		if (current_event.key.keysym.sym == SDLK_RIGHT || current_event.key.keysym.sym == SDLK_d)
			mainCam->turnRight(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
		if (current_event.key.keysym.sym == SDLK_v)
			crystalball_viewing = !crystalball_viewing;
		if (current_event.key.keysym.sym == SDLK_SPACE)
			renderloop_paused = !renderloop_paused;
		if (current_event.key.keysym.sym == SDLK_o)
		{
			//nfdchar_t *outPath = NULL;
			//nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);
			//if (result != NFD_OKAY) return;

			//myObject *obj_tmp = new myObject();
			//if (!obj_tmp->readObjects(outPath))
			//{
			//	delete obj_tmp;
			//	return;
			//}
			//delete obj1;
			//obj1 = obj_tmp;
		}
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		mouse_position[0] = current_event.button.x;
		mouse_position[1] = current_event.button.y;
		mouse_pressed = true;

		const Uint8* state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_LCTRL])
		{
			glm::vec3 ray = mainCam->constructRay(mouse_position[0], mouse_position[1]);
			scene.closestObject(ray, mainCam->camera_eye, picked_object, picked_triangle_index);
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		mouse_pressed = false;
		break;
	}
	case SDL_MOUSEMOTION:
	{
		int x = current_event.motion.x;
		int y = current_event.motion.y;

		int dx = x - mouse_position[0];
		int dy = y - mouse_position[1];

		mouse_position[0] = x;
		mouse_position[1] = y;

		if ((dx == 0 && dy == 0) || !mouse_pressed) return;

		if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) && crystalball_viewing)
			mainCam->crystalball_rotateView(dx, dy);
		else if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) && !crystalball_viewing)
			mainCam->firstperson_rotateView(dx, dy);
		else if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT))
			mainCam->panView(dx, dy);

		break;
	}
	case SDL_WINDOWEVENT:
	{
		if (current_event.window.event == SDL_WINDOWEVENT_RESIZED)
			windowsize_changed = true;
		break;
	}
	case SDL_MOUSEWHEEL:
	{
		if (current_event.wheel.y < 0)
			mainCam->moveBack(DEFAULT_MOUSEWHEEL_MOVEMENT_STEPSIZE);
		else if (current_event.wheel.y > 0)
			mainCam->moveForward(DEFAULT_MOUSEWHEEL_MOVEMENT_STEPSIZE);
		break;
	}
	default:
		break;
	}
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		default: return "UNKNOW";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		default: return "UNKNOW";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		default: return "UNKNOW";
		}
	}();

	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << errorCode << ": " << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

#ifdef _DEBUG
#define glCheckError() glCheckError_(__FILE__, __LINE__) 
#endif // _DEBUG

enum class RenderPipeline {
	PBR, SSSS, Debug
};

int main(int argc, char* argv[])
{
	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

	// Using OpenGL 4.6 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	//SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	auto flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	// Create window
	_window = SDL_CreateWindow("GLAB",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		DEFAULT_WINDOW_WIDTH,
		DEFAULT_WINDOW_HEIGHT, flags);
	//SDL_SetWindowBordered(window, SDL_FALSE);
	auto render = SDL_GetRenderer(_window);
	context = SDL_GL_CreateContext(_window);
	glewInit(); SDL_GL_MakeCurrent(_window, context);

	auto vsync = SDL_GL_SetSwapInterval(0);
	auto errot = SDL_GetError();

#ifdef _DEBUG
	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(message_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#endif // _DEBUG

	float scaledDPI, defaultDPI;
	auto displayIndex = SDL_GetWindowDisplayIndex(_window);
	SDL_GetDisplayDPIRatio(displayIndex, &scaledDPI, &defaultDPI);
	auto sacledRatio = scaledDPI / defaultDPI;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); 
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	// https://github.com/ocornut/imgui/issues/2956
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\Arial.ttf", sacledRatio * 15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(_window, context);
	ImGui_ImplOpenGL3_Init("#version 330");


	ImGui::GetIO().FontDefault->FontSize;
	ImGui::GetIO().FontAllowUserScaling = true;
	ImGui::GetIO().FontGlobalScale = 1.0;// sacledRatio;
	ImGui::GetStyle().ScaleAllSizes(sacledRatio);
	ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.75;

	bool show_demo_window = false;
	bool show_another_window = false;

	glClearColor(0, 0, 0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_MULTISAMPLE);   
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	checkOpenGLInfo(true);

	mainCam = std::make_unique<Camera>(); mainCam->camera_eye = glm::vec3(0, 0, 32);
	SDL_GetWindowSize(_window, &mainCam->window_width, &mainCam->window_height);

	mainCam->zFar = 5000; mainCam->zNear = 0.5; mainCam->fovy = 45;
	 
	/**************************INITIALIZING LIGHTS ***************************/
	scene.lightList = std::make_unique<LightList>();
	scene.lightList->addLight(std::move(std::make_unique<Light>(LightType::POINTLIGHT, glm::vec3(-20, 20, 0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.7, 0.1, 0.1) )));
	scene.lightList->addLight(std::move(std::make_unique<Light>(LightType::POINTLIGHT, glm::vec3(20, -20, 0), glm::vec3(0.3, 0.5, 0.7), glm::vec3(0.1, 0.7, 0.1) )));
	scene.lightList->addLight(std::move(std::make_unique<Light>(LightType::POINTLIGHT, glm::vec3(0, 0, 40), glm::vec3(0.7), glm::vec3(0.1, 0.1, 0.7) )));
	glCheckError();

	std::vector<std::unique_ptr<CubeFBO>> shadowList;
	
	for (size_t i = 0; i < scene.lightList->list.size(); ++i) {
		auto shadowFBO = std::make_unique<CubeFBO>(true);
		shadowFBO->initFBO(1024, 1024);
		shadowList.push_back(std::move(shadowFBO));
	}

	/**************************INITIALIZING FBO ***************************/
	//plane will draw the color_texture of the framebufferobject fbo.

	auto geometryFBO = std::make_shared<GeoFBO>();
	geometryFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto lightingFBO = std::make_shared<FBO>(false, false);
	lightingFBO->initFBO(mainCam->window_width, mainCam->window_height);
	auto environmentFBO = std::make_shared<FBO>(false, true);
	environmentFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto blurFBO = std::make_shared<FBO>();
	blurFBO->initFBO(mainCam->window_width, mainCam->window_height);
	auto rulbFBO = std::make_shared<FBO>();
	rulbFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto bFBO = std::make_unique<FBO>();
	bFBO->initFBO(512, 512);

	auto cFBO = std::make_unique<CubeFBO>();
	cFBO->initFBO(1024, 1024);

	auto pFBO = std::make_unique<CubeFBO>();
	pFBO->initFBO(1024, 1024);

	auto iFBO = std::make_unique<CubeFBO>();
	iFBO->initFBO(256, 256);

	/**************************SETTING UP OPENGL SHADERS ***************************/
	ShaderPack shaderPack;

	shaderPack.add(std::make_unique<Shader>("geo_buffer"), ShaderName::geo_buffer);
	shaderPack.add(std::make_unique<Shader>("pbr_buffer"), ShaderName::pbr_buffer);

	shaderPack.add(std::make_unique<Shader>("skycube"), ShaderName::skycube);
	shaderPack.add(std::make_unique<Shader>("basic"), ShaderName::basic);

	auto shaderCapture = std::make_unique<Shader>(literial(ShaderName::equirectangular));
	auto shaderIrradiance = std::make_unique<Shader>(literial(ShaderName::irradiance));
	auto shaderPrefilter = std::make_unique<Shader>(literial(ShaderName::prefilter));
	auto shaderBRDF = std::make_unique<Shader>( literial(ShaderName::brdf) );
	
	shaderPack.add(std::make_unique<Shader>("postprocess"), ShaderName::postprocess);
	shaderPack.add(std::make_unique<Shader>("blur"), ShaderName::blur);

	auto shadowShader = std::make_unique<Shader>("point_shadow");

	/**************************INITIALIZING OBJECTS THAT WILL BE DRAWN ***************************/

	auto cube = std::make_unique<MeshPack>(); // for capture only
	cube->readObjects("models/skycube.obj", true, false);
	cube->createVAO();

	auto hdrTexture = std::make_unique<Texture>();
	hdrTexture->readTextureHDR("textures/envirment/vulture_hide_4k.hdr");

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// -------------------------------------------w-------------------------
	cube->setTexture(hdrTexture.get(), Texture_Type::colortex);
	cFBO->render(shaderCapture, cube, glm::vec3(0.0f), captureProjection);
	glCheckError();

	cube->setTexture(cFBO->envTexture.get(), Texture_Type::cubetex);
	iFBO->render(shaderIrradiance, cube, glm::vec3(0.0f), captureProjection);
	glCheckError();

	cube->setTexture(cFBO->envTexture.get(), Texture_Type::cubetex);
	pFBO->render(shaderPrefilter, cube, glm::vec3(0.0f), captureProjection, 5);
	glCheckError();

	auto the_canvas = std::make_unique<MeshPack>();
	the_canvas->readObjects("models/plane.obj", true, false);
	the_canvas->createVAO();

	bFBO->render(shaderBRDF, the_canvas, captureViews[0]);
	glCheckError();

	//enviornment mapped object
	auto skycube = std::make_unique<MeshPack>();
	skycube->readObjects("models/skycube.obj", true, false);
	skycube->createVAO();
	skycube->setTexture(cFBO->envTexture.get(), Texture_Type::cubetex);
	skycube->scale(glm::vec3(2048));
	glCheckError();

	auto pbr_canvas = std::make_unique<MeshPack>();
	pbr_canvas->readObjects("models/plane.obj", true, false);
	pbr_canvas->createVAO();
	glCheckError();

	auto final_canvas = std::make_unique<MeshPack>();
	final_canvas->readObjects("models/plane.obj", true, false);
	final_canvas->createVAO();
	glCheckError();

	auto shaderball = std::make_unique<MeshPack>();
	shaderball->readObjects("models/shaderball.obj", true, false);
	shaderball->createVAO();
	glCheckError();

	auto texAlbedo_A = std::make_unique<Texture>("textures/rustediron/albedo.png");
	auto texAO_A = std::make_unique<Texture>("textures/rustediron/ao.png");
	auto texMetallic_A = std::make_unique<Texture>("textures/rustediron/metalness.png");
	auto texNormal_A = std::make_unique<Texture>("textures/rustediron/normal.png");
	auto texRoughness_A = std::make_unique<Texture>("textures/rustediron/roughness.png");
	glCheckError();

	auto texAlbedo_B = std::make_unique<Texture>("textures/aluminum/basecolor.png");
	auto texAO_B = std::make_unique<Texture>("textures/aluminum/ao.png");
	auto texMetallic_B = std::make_unique<Texture>("textures/aluminum/metallic.png");
	auto texNormal_B = std::make_unique<Texture>("textures/aluminum/normal.png");
	auto texRoughness_B = std::make_unique<Texture>("textures/aluminum/roughness.png");
	glCheckError();

	auto texAlbedo_X = std::make_unique<Texture>("textures/coatedball/albedo.png");
	auto texAO_X = std::make_unique<Texture>("textures/coatedball/ao.png");
	auto texMetallic_X = std::make_unique<Texture>("textures/coatedball/metalness.png");
	auto texNormal_X = std::make_unique<Texture>("textures/coatedball/normal.png");
	auto texRoughness_X = std::make_unique<Texture>("textures/coatedball/roughness.png");
	glCheckError();

	auto posTextureMap = PosTextureMap {

			{ glm::vec3(-15, 0, -10), std::map<Texture_Type, Texture*> {
				{ Texture_Type::texAO, texAO_A.get() },
				{ Texture_Type::texAlbedo, texAlbedo_A.get() },
				{ Texture_Type::texMetal, texMetallic_A.get() },
				{ Texture_Type::texNormal, texNormal_A.get() },
				{ Texture_Type::texRough, texRoughness_A.get() }
			} },
			{ glm::vec3(15, 0, -10), std::map<Texture_Type, Texture*> {
				{ Texture_Type::texAO, texAO_B.get() },
				{ Texture_Type::texAlbedo, texAlbedo_B.get()},
				{ Texture_Type::texMetal, texMetallic_B.get() },
				{ Texture_Type::texNormal, texNormal_B.get() },
				{ Texture_Type::texRough, texRoughness_B.get() }
			} },
			{ glm::vec3(0, 0, 0), std::map<Texture_Type, Texture*> {
				{ Texture_Type::texAO, texAO_X.get() },
				{ Texture_Type::texAlbedo, texAlbedo_X.get()},
				{ Texture_Type::texMetal, texMetallic_X.get() },
				{ Texture_Type::texNormal, texNormal_X.get() },
				{ Texture_Type::texRough, texRoughness_X.get() }
			} }
	};

	auto lightSize = 2.0f;
	auto lightball = std::make_unique<MeshPack>(); 
	lightball->readObjects("models/sphere.obj", true, true);
	lightball->scale(glm::vec3(lightSize));
	lightball->createVAO();
	glCheckError();

	// SSSS: set up SSSS
	auto headObject = std::make_unique<MeshPack>();
	headObject->readObjects("lpshead/head.obj", true, false);
	glCheckError();
	headObject->scale(64, 64, 64);
	headObject->translate(0, 5, 0);
	headObject->createVAO();
	glCheckError();
	
	auto skinTexture = std::make_unique<Texture>("lpshead/albedo.png");
	headObject->setTexture(skinTexture.get(), Texture_Type::texAlbedo);
	glCheckError();

	auto skinNormal = std::make_unique<Texture>("lpshead/normal.png");
	headObject->setTexture(skinNormal.get(), Texture_Type::texNormal);
	glCheckError();

	shaderPack.add(std::make_unique<Shader>("ssss_phong"), ShaderName::ssss_phong);
	shaderPack.add(std::make_unique<Shader>("ssss_blur"), ShaderName::ssss_blur);
	glCheckError();

	auto ssssLightFBO = std::make_shared<FBO>(true);
	ssssLightFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto ssssBlurFBO = std::make_shared<FBO>(false);
	ssssBlurFBO->initFBO(mainCam->window_width, mainCam->window_height);
	auto ssssRulbFBO = std::make_shared<FBO>(false);
	ssssRulbFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto ssss_canvas = std::make_unique<MeshPack>();
	ssss_canvas->readObjects("models/plane.obj", true, false);
	ssss_canvas->createVAO();
	glCheckError();

	auto emptyTexture = std::make_shared<Texture>();
	emptyTexture->empty();
	glCheckError();

	// display loop
	float delta = (float)M_PI / 1000.0f;

	float gamma = 2.2f;
	float exposure = 1.0f;
	float background = 1.0f;

	bool tonemapping_object = false;
	bool tonemapping_background = true;

	static float bloomRange = 0.01f;
	static int bloomStrength = 8;

	float ambient = 0.5f;
	float ssss_kD = 0.1f;
	float ssss_kS = 0.1f;

	float shadow_bias = 0.75;
	float distance_scale = 1.0;

	float translucency = 0.02f;
	float luminance_threshold = 0.1f;
	
	float depthtest_coefficient = 1.0f;

	float ssss_width = 0.012f;
	static glm::vec3 ssss_falloff = glm::vec3(1.0f, 0.37f, 0.30f);
	static glm::vec3 ssss_strength = glm::vec3(0.48f, 0.41f, 0.28f);

	std::vector<float> frametime_cache(1000, 0);

	std::function<void(int, int)> pbr_pipeline_init = [&](int WIDTH, int HEIGHT) {

		geometryFBO->initFBO(WIDTH, HEIGHT);
		lightingFBO->initFBO(WIDTH, HEIGHT);
	};

	std::function<void(int, int)> ssss_pipeline_init = [&](int WIDTH, int HEIGHT) {

		ssssLightFBO->initFBO(WIDTH, HEIGHT);
		ssssBlurFBO->initFBO(WIDTH, HEIGHT);
		ssssRulbFBO->initFBO(WIDTH, HEIGHT);
	};

	auto current_pipeline = RenderPipeline::SSSS;
	current_pipeline = RenderPipeline::PBR;

	std::function<void(int, int)>* pipeline_init;
	pipeline_init = &ssss_pipeline_init;
	pipeline_init = &pbr_pipeline_init;
	bool pipeline_changed = true;

	std::vector<std::shared_ptr<FBO>> FBOs = { environmentFBO, blurFBO, rulbFBO, geometryFBO, lightingFBO, ssssLightFBO, ssssBlurFBO, ssssRulbFBO };

	auto a = std::make_shared<FBO>(); auto b = std::make_shared<CubeFBO>(); auto c = std::make_shared<GeoFBO>();

	std::function<void(bool)> ui_pipeline_work = [&](bool paused) {

		auto framerate = ImGui::GetIO().Framerate;
		auto frametime = 1000.0f / framerate;
		frametime_cache.erase(frametime_cache.begin());
		frametime_cache.emplace_back(frametime);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(_window);
		ImGui::NewFrame(); io.WantCaptureMouse = true;

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
		static int counter = 0;
		ImGui::SetNextWindowCollapsed(true, ImGuiSetCond_Once);
		ImGui::Begin("Configuration", NULL, ImVec2(0, 0), -1.0, ImGuiWindowFlags_AlwaysAutoResize);
		// Display some text (you can use a format strings too)
		//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//ImGui::Checkbox("Another Window", &show_another_window);
		//static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		auto& width = mainCam->window_width;
		auto& height = mainCam->window_height;

		ImGui::BeginGroup();
		ImGui::Text("%d * %d @ %.1f fps average %.1f ms", width, height, framerate, frametime);
		ImGui::PlotLines("Frame Time", frametime_cache.data(), (int)frametime_cache.size());
		ImGui::EndGroup();

		ImGui::Text("General");
		ImGui::SliderFloat("Gamma", &gamma, 1.0f, 3.0f);
		ImGui::SliderFloat("Exposure", &exposure, 0.2f, 2.0f);
		ImGui::SliderFloat("Background", &background, 0.0f, 1.0f);

		ImGui::Checkbox("Tone mapping object", &tonemapping_object);
		ImGui::Checkbox("Tone mapping background", &tonemapping_background);
		
		ImGui::Text("Bloom light source");
		ImGui::SliderFloat("Bloom Range", &bloomRange, 0.01f, 0.11f);
		ImGui::SliderInt("Bloom Strength", &bloomStrength, 2, 16);
		ImGui::SliderFloat("Lightball Size", &lightSize, 0.1f, 10.0f);

		ImGui::Text("Render pipeline");
		static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("TabBar", tab_bar_flags))
		{
			if (ImGui::BeginTabItem("PBR"))
			{
				if (current_pipeline != RenderPipeline::PBR) {
					current_pipeline = RenderPipeline::PBR;
					pipeline_init = &pbr_pipeline_init;
					pipeline_changed = true;
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("SSSS"))
			{
				if (current_pipeline != RenderPipeline::SSSS) {
					current_pipeline = RenderPipeline::SSSS;
					pipeline_init = &ssss_pipeline_init;
					pipeline_changed = true;
				}

				ImGui::Text("Light pass for SSSS");
				ImGui::SliderFloat("kD", &ssss_kD, 0.0f, 1.0f);
				ImGui::SliderFloat("kS", &ssss_kS, 0.0f, 1.0f);

				ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
				ImGui::SliderFloat("Translucency", &translucency, 0.0f, 0.1f);

				ImGui::SliderFloat("Shadow Bias", &shadow_bias, 0.0f, 1.0f);
				ImGui::SliderFloat("Distance Scale", &distance_scale, 0.0f, 1.0f);
				
				ImGui::Text("Gaussian pass for SSSS");
				ImGui::SliderFloat("SSSS Width", &ssss_width, 0.0f, 2.0f);

				ImGui::ColorEdit3("SSSS Falloff", (float*)&ssss_falloff);
				ImGui::ColorEdit3("SSSS Strength", (float*)&ssss_strength);

				ImGui::SliderFloat("Luminance Threshold", &luminance_threshold, 0.0f, 1.0f);
				ImGui::SliderFloat("Depthtest Coefficient", &depthtest_coefficient, 0.1f, 2.0f);     

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Other"))
			{
				if (ImGui::Button("Test Button"))
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		} glCheckError();
		
		ImGui::End();
		glCheckError();

		// Rendering
		ImGui::Render();
		SDL_GL_MakeCurrent(_window, context);
		auto d_width = (GLsizei)io.DisplaySize.x;
		auto d_height = (GLsizei)io.DisplaySize.y;
		glViewport(0, 0, d_width, d_height);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glCheckError();

		SDL_Event current_event; glCheckError();
		if (!paused) {SDL_GL_SwapWindow(_window);}
		glCheckError(); auto error = SDL_GetError();

		while (SDL_PollEvent(&current_event) != 0) {

			if (ImGui::IsMouseHoveringAnyWindow()) {
				ImGui_ImplSDL2_ProcessEvent(&current_event);
			}
			else {
				processEvents(current_event);
			}
		} glCheckError();
	};

	while (!quit)
	{
		if (renderloop_paused) {   
			ui_pipeline_work(true);
			continue; 
		}

		if (windowsize_changed || pipeline_changed)
		{   
			SDL_GetWindowSize(_window, &mainCam->window_width, &mainCam->window_height);
			windowsize_changed = false; pipeline_changed = false;

			for (auto& fbo : FBOs) { fbo->reset(); }

			environmentFBO->initFBO(mainCam->window_width, mainCam->window_height);
			blurFBO->initFBO(mainCam->window_width, mainCam->window_height);
			rulbFBO->initFBO(mainCam->window_width, mainCam->window_height);

			(*pipeline_init)(mainCam->window_width, mainCam->window_height);

			glCheckError();
		}

		//Computing transformation matrices.
		glViewport(0, 0, mainCam->window_width, mainCam->window_height);
		glm::mat4 projection_matrix = mainCam->projectionMatrix();
		glm::mat4 view_matrix = mainCam->viewMatrix();
		glm::mat4 weiv_matrix = glm::inverse(view_matrix);
		ui_pipeline_work(false);

		//Setting uniform variables for each shader
		for (unsigned int i = 0; i < shaderPack.size(); i++)
		{
			auto& current_shader = shaderPack[i]; current_shader->start();

			current_shader->setUniform("projection_matrix", projection_matrix);
			current_shader->setUniform("view_matrix", view_matrix);
			current_shader->setUniform("weiv_matrix", weiv_matrix);
			
			current_shader->setUniform("nearZ", mainCam->zNear);
			current_shader->setUniform("farZ", mainCam->zFar);
			current_shader->setUniform("fovY", mainCam->fovy);

			current_shader->setUniform("exposure", exposure);
			current_shader->setUniform("gamma", gamma);

			current_shader->setLightList(scene.lightList, "lights");
			current_shader->stop();
		}

		skycube->setTexture(cFBO->envTexture.get(), Texture_Type::cubetex);

		environmentFBO->multi_render(
			&std::function<void()>([&] {

					auto& the_shader = shaderPack[ShaderName::skycube];
					the_shader->start();
						skycube->displayObjects(the_shader, view_matrix);
					the_shader->stop();

				auto& current_shader = shaderPack[ShaderName::basic];
				current_shader->start();
				for (auto& light : scene.lightList->list) {
					current_shader->setUniform("color", light->color);
					lightball->translate(light->position);
						lightball->displayObjects(current_shader, view_matrix);
					lightball->translate(-light->position);

					auto randX = (float)rand() / RAND_MAX;
					auto randY = (float)rand() / RAND_MAX;
					auto randZ = (float)rand() / RAND_MAX;
					//https://gamedev.stackexchange.com/questions/115032/how-should-i-rotate-vertices-around-the-origin-on-the-cpu
					glm::vec3 v3RotAxis(randX, randY, randZ);

					glm::quat quatRot = glm::angleAxis(delta, v3RotAxis);
					glm::mat4x4 matRot = glm::mat4_cast(quatRot);
					glm::vec4 newpos = (matRot * glm::vec4(light->position, 1.0));
					light->position = glm::vec3(newpos.x, newpos.y, newpos.z);
				}
				current_shader->stop();
			})
		); glCheckError();

		the_canvas->setTexture(environmentFBO->colorTexture.get(), Texture_Type::colortex);
		//the_canvas->setTexture(environmentFBO->depthTexture, Texture_Type::depthtex);
		glCheckError();

		int blurIndex = 0; int count = 0;
		static std::vector<std::shared_ptr<FBO>> blurList = { blurFBO, rulbFBO };

		while (count < bloomStrength) {
			auto currentFBO = blurList[blurIndex];

			glm::vec2 direction = (blurIndex == 0) ? glm::vec2(1, 0) : glm::vec2(0, 1);

			auto& shaderBlur = shaderPack[ShaderName::blur];
			auto lambda = std::function<void()>([&] {
				shaderBlur->setUniform("range", bloomRange);
				shaderBlur->setUniform("direction", direction);
				shaderBlur->setUniform("lightSize", lightSize);
			});

			currentFBO->render(shaderBlur, the_canvas, captureViews[0], &lambda);	
			the_canvas->setTexture(currentFBO->colorTexture.get(), Texture_Type::colortex);
			++count; blurIndex = !blurIndex;

		}; glCheckError();

		if (RenderPipeline::PBR == current_pipeline) {

			auto& current_shader = shaderPack[ShaderName::geo_buffer];
			shaderball->rotate(0.0f, 1.0f, 0.0f, delta);
			geometryFBO->loopRender(current_shader, shaderball, view_matrix, posTextureMap);
			glCheckError();
			
			pbr_canvas->setTexture(geometryFBO->gPosition.get(), Texture_Type::gPosition);
			pbr_canvas->setTexture(geometryFBO->gAlbedo.get(), Texture_Type::gAlbedo);
			pbr_canvas->setTexture(geometryFBO->gNormal.get(), Texture_Type::gNormal);

			pbr_canvas->setTexture(iFBO->envTexture.get(), Texture_Type::gIrradiance);
			pbr_canvas->setTexture(pFBO->envTexture.get(), Texture_Type::gPrefilter);
			pbr_canvas->setTexture(bFBO->colorTexture.get(), Texture_Type::BRDF_LUT);

			lightingFBO->render(shaderPack[ShaderName::pbr_buffer], pbr_canvas, view_matrix);
			glCheckError();

		} else if (RenderPipeline::SSSS == current_pipeline) {

			//for (auto& light : scene.lightList->list) {

				//shadowList[i]->shadowMapping(shadowShader, headObject, light->position, captureProjection);

			//} glCheckError();

			shadowList[0]->shadowMapping(shadowShader, headObject, scene.lightList->list[0]->position, captureProjection);
			headObject->setTexture(shadowList[0]->envTexture.get(), Texture_Type::shadowCube);

				auto& ssssPhongShader = shaderPack[ShaderName::ssss_phong];

				auto lambda = std::function<void()>([&] {
					
					ssssPhongShader->setUniform("kD", ssss_kD);
					ssssPhongShader->setUniform("kS", ssss_kS);

					ssssPhongShader->setUniform("ambient", ambient);
					ssssPhongShader->setUniform("translucency", translucency);

					ssssPhongShader->setUniform("shadow_bias", shadow_bias);
					ssssPhongShader->setUniform("distance_scale", distance_scale);
				});

				ssssLightFBO->render(ssssPhongShader, headObject, view_matrix, &lambda);
				glCheckError();

				ssss_canvas->setTexture(ssssLightFBO->colorTexture.get(), Texture_Type::gAlbedo);
				ssss_canvas->setTexture(ssssLightFBO->extraTexture.get(), Texture_Type::gExtra);

				auto& ssssBlurShader = shaderPack[ShaderName::ssss_blur];

				lambda = std::function<void()>([&] {

					ssssBlurShader->setUniform("direction", glm::vec2(1, 0));

					ssssBlurShader->setUniform("ssss_width", ssss_width);
					ssssBlurShader->setUniform("ssss_falloff", ssss_falloff);
					ssssBlurShader->setUniform("ssss_strength", ssss_strength);
					
					ssssBlurShader->setUniform("luminance_threshold", luminance_threshold);
					ssssBlurShader->setUniform("depthtest_coefficient", depthtest_coefficient);

					ssssBlurShader->setUniform("kernel", kernelSSSS);
					ssssBlurShader->setUniform("kernelSize", (int)kernelSSSS.size());
				});

				ssssBlurFBO->render(ssssBlurShader, ssss_canvas, view_matrix, &lambda);
				glCheckError();

				ssss_canvas->setTexture(ssssBlurFBO->colorTexture.get(), Texture_Type::gAlbedo);
				ssss_canvas->setTexture(ssssLightFBO->extraTexture.get(), Texture_Type::gExtra);

				lambda = std::function<void()>([&] {
					ssssBlurShader->setUniform("direction", glm::vec2(0, 1));
				});

				ssssRulbFBO->render(ssssBlurShader, ssss_canvas, view_matrix, &lambda);
				glCheckError();
		}

		if (RenderPipeline::PBR == current_pipeline) {

			final_canvas->setTexture(lightingFBO->colorTexture.get(), Texture_Type::colortex);
			final_canvas->setTexture(emptyTexture.get(), Texture_Type::gExtra);

		}
		else if (RenderPipeline::SSSS == current_pipeline) {

			final_canvas->setTexture(ssssRulbFBO->colorTexture.get(), Texture_Type::colortex);
			final_canvas->setTexture(ssssLightFBO->extraTexture.get(), Texture_Type::gExtra);
		}

		final_canvas->setTexture(environmentFBO->colorTexture.get(), Texture_Type::gEnv);
		final_canvas->setTexture(rulbFBO->colorTexture.get(), Texture_Type::gBloom);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			auto& current_shader = shaderPack[ShaderName::postprocess];
			current_shader->start();
			current_shader->setUniform("background", background);
			current_shader->setUniform("tonemapping_object", tonemapping_object);
			current_shader->setUniform("tonemapping_background", tonemapping_background);
				final_canvas->displayObjects(current_shader, view_matrix);
			current_shader->stop(); 
		glCheckError();
	}

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	glCheckError();

	// Freeing resources before exiting.
	// Destroy window
	if (context) SDL_GL_DeleteContext(context);
	if (_window) SDL_DestroyWindow(_window);
	// Quit SDL subsystems
	SDL_Quit();
	return 0;
}