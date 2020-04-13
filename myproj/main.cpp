
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/GLU.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_opengl.h>
#undef main


#pragma comment(lib, "Shcore.lib")
#include <ShellScalingAPI.h>
#include <windows.h>
#include <comdef.h>

#include "helperFunctions.h"
#include "default_constants.h"

#include "myShader.h"
#include "myCamera.h"
#include "myObject.h"
#include "mySubObject.h"

#include "myScene.h"
#include "myLights.h"
#include "myShaders.h"

#include "FBO.h"
#include "GeoFBO.h"
#include "CubeFBO.h"

#include "SSSS.h"

using namespace std;

// SDL variables
SDL_Window* window;
SDL_GLContext glContext;

int mouse_position[2];
bool mouse_button_pressed = false;
bool quit = false;
bool windowsize_changed = true;
bool crystalballorfirstperson_view = false;
float movement_stepsize = DEFAULT_KEY_MOVEMENT_STEPSIZE;

// Camera parameters.
myCamera *mainCam;

// All the meshes 
myScene scene;

//Triangle to draw to illustrate picking
size_t picked_triangle_index = 0;
myObject *picked_object = nullptr;

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
			crystalballorfirstperson_view = !crystalballorfirstperson_view;
		else if (current_event.key.keysym.sym == SDLK_o)
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
		mouse_button_pressed = true;

		const Uint8 *state = SDL_GetKeyboardState(nullptr);
		if (state[SDL_SCANCODE_LCTRL])
		{
			glm::vec3 ray = mainCam->constructRay(mouse_position[0], mouse_position[1]);
			scene.closestObject(ray, mainCam->camera_eye, picked_object, picked_triangle_index);
		}
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		mouse_button_pressed = false;
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

		if ((dx == 0 && dy == 0) || !mouse_button_pressed) return;

		if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) && crystalballorfirstperson_view)
			mainCam->crystalball_rotateView(dx, dy);
		else if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) && !crystalballorfirstperson_view)
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
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
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
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

int main(int argc, char *argv[])
{
	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

	// Using OpenGL 4.6 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetSwapInterval(1);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	auto flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
	// Create window
	window = SDL_CreateWindow("GLAB", 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED,
				DEFAULT_WINDOW_WIDTH, 
				DEFAULT_WINDOW_HEIGHT, flags);
	//SDL_SetWindowBordered(window, SDL_FALSE);
	auto render = SDL_GetRenderer(window);
	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(message_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

	float scaledDPI, defaultDPI;
	auto displayIndex = SDL_GetWindowDisplayIndex(window);
	SDL_GetDisplayDPIRatio(displayIndex, &scaledDPI, &defaultDPI);
	auto sacledRatio = scaledDPI / defaultDPI;

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();
		//ImGui::StyleColorsClassic();

		// Setup Platform/Renderer bindings
		ImGui_ImplSDL2_InitForOpenGL(window, glContext);
		ImGui_ImplOpenGL3_Init("#version 330");

		ImGui::GetIO().FontAllowUserScaling = true;
		ImGui::GetIO().FontGlobalScale = sacledRatio;
		ImGui::GetStyle().ScaleAllSizes(sacledRatio);
		ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.75;

		bool show_demo_window = false;
		bool show_another_window = false;
	
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	//glEnable(GL_BLEND);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	checkOpenGLInfo(true);

	mainCam = new myCamera(); mainCam->camera_eye = glm::vec3(0, 0, 32);
	SDL_GetWindowSize(window, &mainCam->window_width, &mainCam->window_height);

	
	/**************************INITIALIZING LIGHTS ***************************/
	scene.lights = new myLights();
	scene.lights->lights.push_back(new myLight(myLight::POINTLIGHT, glm::vec3(0.7, 0.1, 0.1), glm::vec3(-20, 20, 0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(1.0)));
	scene.lights->lights.push_back(new myLight(myLight::POINTLIGHT, glm::vec3(0.1, 0.7, 0.1), glm::vec3(20, -20, 0), glm::vec3(0.3, 0.5, 0.7), glm::vec3(1.0)));
	scene.lights->lights.push_back(new myLight(myLight::POINTLIGHT, glm::vec3(0.1, 0.1, 0.7), glm::vec3(0, 0, 40), glm::vec3(0.7), glm::vec3(1.0)));

	/**************************INITIALIZING FBO ***************************/
	//plane will draw the color_texture of the framebufferobject fbo.

	auto geometryFBO = new GeoFBO();
	geometryFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto lightingFBO = new FBO(true);
	lightingFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto environmentFBO = new FBO(true);
	environmentFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto blurFBO = new FBO();
	blurFBO->initFBO(mainCam->window_width, mainCam->window_height);
	auto rulbFBO = new FBO();
	rulbFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto bFBO = new FBO();
	bFBO->initFBO(512, 512);

	auto cFBO = new CubeFBO();
	cFBO->initFBO(1024, 1024);

	auto iFBO = new CubeFBO();
	iFBO->initFBO(64, 64);

	auto pFBO = new CubeFBO();
	pFBO->initFBO(1024, 1024);

	/**************************SETTING UP OPENGL SHADERS ***************************/
	myShaders shaders;

	shaders.addShader(new myShader("shaders/geo_buffer.vs.glsl", "shaders/geo_buffer.fs.glsl"), "geo_buffer");
	shaders.addShader(new myShader("shaders/pbr_buffer.vs.glsl", "shaders/pbr_buffer.fs.glsl"), "pbr_buffer");

	shaders.addShader(new myShader("shaders/basic.vs.glsl", "shaders/basic.fs.glsl"), "basicx");
	shaders.addShader(new myShader("shaders/postprocess.vs.glsl", "shaders/postprocess.fs.glsl"), "postprocess");
	shaders.addShader(new myShader("shaders/skycube.vs.glsl", "shaders/skycube.fs.glsl"), "shader_skycube");

	myShader *shaderCapture = new myShader("shaders/equirectangular.vs.glsl", "shaders/equirectangular.fs.glsl");
	myShader *shaderIrradiance = new myShader("shaders/irradiance.vs.glsl", "shaders/irradiance.fs.glsl");
	myShader *shaderPrefilter = new myShader("shaders/equirectangular.vs.glsl", "shaders/prefilter.fs.glsl");
	myShader *shaderBRDF = new myShader("shaders/brdf.vs.glsl", "shaders/brdf.fs.glsl");

	myShader *shaderPhong = new myShader("shaders/texture+phong.vs.glsl", "shaders/texture+phong.fs.glsl");
	shaders.addShader(shaderPhong, "shaderPhong");

	myShader *shaderBlur = new myShader("shaders/blur.vs.glsl", "shaders/blur.fs.glsl");
	shaders.addShader(shaderBlur, "blurShader");

	//myShader *shaderBlurSSSS = new myShader("shaders/blurSSSS.vs.glsl", "shaders/blurSSSS.fs.glsl");
	//shaders.addShader(shaderBlurSSSS, "shaderBlurSSSS");

	/**************************INITIALIZING OBJECTS THAT WILL BE DRAWN ***************************/

	auto obj = new myObject(); // for capture only
	obj->readObjects("models/skycube.obj", true, false);
	obj->createmyVAO();

	auto hdrTexture = new myTexture();
	hdrTexture->readTexture_HDR("textures/envirment/vulture_hide_4k.hdr");

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
		// ----------------------------------------------------------------------
		cFBO->bind();
		glViewport(0, 0, cFBO->getWidth(), cFBO->getHeight());
		obj->setTexture(hdrTexture, mySubObject::COLORMAP);
		shaderCapture->start();
		for (unsigned int i = 0; i < 6; ++i)
		{
			glNamedFramebufferTextureLayer(cFBO->fboID, GL_COLOR_ATTACHMENT0, cFBO->envTexture->texture_id, 0, i);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderCapture->setUniform("myview_matrix", captureViews[i]);
			//captureShader->setUniform("cam_position", mainCam->camera_eye);
			shaderCapture->setUniform("myprojection_matrix", captureProjection);
			
			obj->displayObjects(shaderCapture, captureViews[i]);
		}
		glGenerateTextureMipmap(cFBO->envTexture->texture_id);
		shaderCapture->stop();
		cFBO->unbind();

		iFBO->bind();
		glViewport(0, 0, iFBO->getWidth(), iFBO->getHeight());
		obj->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
		shaderIrradiance->start();
		for (unsigned int i = 0; i < 6; ++i)
		{
			glNamedFramebufferTextureLayer(iFBO->fboID, GL_COLOR_ATTACHMENT0, iFBO->envTexture->texture_id, 0, i);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderIrradiance->setUniform("myview_matrix", captureViews[i]);
			//captureShader->setUniform("cam_position", mainCam->camera_eye);
			shaderIrradiance->setUniform("myprojection_matrix", captureProjection);
			
			obj->displayObjects(shaderIrradiance, captureViews[i]);
		}
		glGenerateTextureMipmap(iFBO->envTexture->texture_id);
		shaderIrradiance->stop();
		iFBO->unbind();

		pFBO->bind();
		glViewport(0, 0, pFBO->getWidth(), pFBO->getHeight());
		obj->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
		shaderPrefilter->start();
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = pFBO->getWidth() * std::pow(0.5, mip);
			unsigned int mipHeight = pFBO->getHeight() * std::pow(0.5, mip);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			shaderPrefilter->setUniform("roughness", roughness);

			unsigned int i = 0;

			for (; i < 6; ++i)
			{
				glNamedFramebufferTextureLayer(pFBO->fboID, GL_COLOR_ATTACHMENT0, pFBO->envTexture->texture_id, mip, i);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				shaderPrefilter->setUniform("myview_matrix", captureViews[i]);
				//captureShader->setUniform("cam_position", mainCam->camera_eye);
				shaderPrefilter->setUniform("myprojection_matrix", captureProjection);
				
				obj->displayObjects(shaderPrefilter, captureViews[i]);
			}
			auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
		}
		shaderPrefilter->stop();
		pFBO->unbind();

		auto canvas = new myObject();
		canvas->readObjects("models/plane.obj", true, false);
		canvas->createmyVAO();

		bFBO->bind();
			shaderBRDF->start();
				glViewport(0, 0, bFBO->getWidth(), bFBO->getHeight());
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				canvas->displayObjects(shaderBRDF, captureViews[0]);
			shaderBRDF->stop();
		bFBO->unbind();

		canvas = new myObject();
		canvas->readObjects("models/plane.obj", true, false);
		canvas->createmyVAO();
	
	//enviornment mapped object
	obj = new myObject();
	obj->readObjects("models/skycube.obj", true, false);
	obj->createmyVAO();
	obj->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
	obj->scale(glm::vec3(2048));
	scene.addObjects(obj, "skycube");

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	obj->createmyVAO();
	scene.addObjects(obj, "env_canvas");

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	obj->createmyVAO();
	obj->setTexture(geometryFBO->gPosition, mySubObject::gPosition);
	obj->setTexture(geometryFBO->gAlbedo, mySubObject::gAlbedo);
	obj->setTexture(geometryFBO->gNormal, mySubObject::gNormal);

	obj->setTexture(iFBO->envTexture, mySubObject::gIrradiance);
	obj->setTexture(pFBO->envTexture, mySubObject::gPrefilter);
	obj->setTexture(bFBO->colortexture, mySubObject::BRDF_LUT);

	scene.addObjects(obj, "pbr_canvas");

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	obj->createmyVAO();
	obj->setTexture(lightingFBO->colortexture, mySubObject::COLORMAP);
	obj->setTexture(environmentFBO->colortexture, mySubObject::gEnv);
	scene.addObjects(obj, "ppe_canvas");

	obj = new myObject();
	obj->readObjects("models/shaderball.obj", true, false);
	obj->createmyVAO();
	scene.addObjects(obj, "shaderball");

	auto texAlbedo_A = new myTexture("textures/rustediron/albedo.png");
	auto texAO_A = new myTexture("textures/rustediron/ao.png");
	auto texMetallic_A = new myTexture("textures/rustediron/metalness.png");
	auto texNormal_A = new myTexture("textures/rustediron/normal.png");
	auto texRoughness_A = new myTexture("textures/rustediron/roughness.png");

	auto texAlbedo_B = new myTexture("textures/aluminum/basecolor.png");
	auto texAO_B = new myTexture("textures/aluminum/ao.png");
	auto texMetallic_B = new myTexture("textures/aluminum/metallic.png");
	auto texNormal_B = new myTexture("textures/aluminum/normal.png");
	auto texRoughness_B = new myTexture("textures/aluminum/roughness.png");

	auto texAlbedo_X = new myTexture("textures/blueburlap/worn-blue-burlap-albedo.png");
	auto texAO_X = new myTexture("textures/blueburlap/worn-blue-burlap-ao.png");
	auto texMetallic_X = new myTexture("textures/blueburlap/worn-blue-burlap-Metallic.png");
	auto texNormal_X = new myTexture("textures/blueburlap/worn-blue-burlap-Normal-dx.png");
	auto texRoughness_X = new myTexture("textures/blueburlap/worn-blue-burlap-Roughness.png");

	obj = new myObject();
	obj->readObjects("models/sphere.obj", true, false);
	obj->createmyVAO();
	scene.addObjects(obj, "lightball");

	// display loop
	myShader *curr_shader;
	float delta = M_PI/1000;

	glCheckError();
	while (!quit)
	{
		if (windowsize_changed)
		{
			SDL_GetWindowSize(window, &mainCam->window_width, &mainCam->window_height);
			windowsize_changed = false;

			if (geometryFBO) { delete geometryFBO; } geometryFBO = new GeoFBO();
			geometryFBO->initFBO(mainCam->window_width, mainCam->window_height);

			auto object = scene["pbr_canvas"];

			object->setTexture(geometryFBO->gPosition, mySubObject::gPosition);
			object->setTexture(geometryFBO->gAlbedo, mySubObject::gAlbedo);
			object->setTexture(geometryFBO->gNormal, mySubObject::gNormal);

			object->setTexture(iFBO->envTexture, mySubObject::gIrradiance);
			object->setTexture(pFBO->envTexture, mySubObject::gPrefilter);
			object->setTexture(bFBO->colortexture, mySubObject::BRDF_LUT);

			if (lightingFBO) { delete lightingFBO; } lightingFBO = new FBO();
			lightingFBO->initFBO(mainCam->window_width, mainCam->window_height);

			if (environmentFBO) { delete environmentFBO; } environmentFBO = new FBO(true);
			environmentFBO->initFBO(mainCam->window_width, mainCam->window_height);

			if (blurFBO) { delete blurFBO; } blurFBO = new FBO();
			blurFBO->initFBO(mainCam->window_width, mainCam->window_height);
			if (rulbFBO) { delete rulbFBO; } rulbFBO = new FBO();
			rulbFBO->initFBO(mainCam->window_width, mainCam->window_height);

			scene["ppe_canvas"]->setTexture(lightingFBO->colortexture, mySubObject::COLORMAP);
			scene["ppe_canvas"]->setTexture(environmentFBO->colortexture, mySubObject::gEnv);

			glCheckError();
		}

		glCheckError();

		//Computing transformation matrices.
		glViewport(0, 0, mainCam->window_width, mainCam->window_height);
		glm::mat4 projection_matrix = mainCam->projectionMatrix();
		glm::mat4 view_matrix = mainCam->viewMatrix();

		//Setting uniform variables for each shader
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			curr_shader = shaders[i]; curr_shader->start();
			curr_shader->setUniform("myprojection_matrix", projection_matrix);
			curr_shader->setUniform("myview_matrix", view_matrix);
			curr_shader->setUniform("cam_position", mainCam->camera_eye);

			curr_shader->setUniform("fovY", mainCam->fovy);
			curr_shader->setUniform("farZ", mainCam->zFar);
			curr_shader->setUniform("nearZ", mainCam->zNear);

			scene.lights->setUniform(curr_shader, "lights");
		}

		glCheckError();

		environmentFBO->clear();
		environmentFBO->bind(); {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			scene["skycube"]->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
			curr_shader = shaders["shader_skycube"];
			curr_shader->start();
			scene["skycube"]->displayObjects(curr_shader, view_matrix);
			curr_shader->stop();
			//glGenerateTextureMipmap(eFBO->colortexture->texture_id);

			curr_shader = shaders["basicx"]; 
			curr_shader->start();
			for (auto light : scene.lights->lights) {	
				curr_shader->setUniform("color", light->color);
				scene["lightball"]->translate(light->position);
				scene["lightball"]->displayObjects(curr_shader, view_matrix);
				scene["lightball"]->translate(-light->position);
				
				auto randX = (float)rand()/RAND_MAX;
				auto randY = (float)rand()/RAND_MAX;
				auto randZ = (float)rand()/RAND_MAX;
				//https://gamedev.stackexchange.com/questions/115032/how-should-i-rotate-vertices-around-the-origin-on-the-cpu
				glm::vec3 v3RotAxis(randX, randY, randZ); 
				
				glm::quat quatRot = glm::angleAxis(delta, v3RotAxis);
				glm::mat4x4 matRot = glm::mat4_cast(quatRot);
				glm::vec4 newpos = (matRot * glm::vec4(light->position, 1.0));
				light->position = glm::vec3(newpos.x, newpos.y, newpos.z);
			}
			curr_shader->stop();
		}; environmentFBO->unbind();

		glCheckError();

		canvas->setTexture(environmentFBO->colortexture, mySubObject::gAlbedo);
		canvas->setTexture(environmentFBO->extratexture, mySubObject::gPosition);

		static int bloomRange = 8;
		static int bloomStrength = 8;

		int horizontal = 0;
		int blurIndex = 0; int count = 0;
		FBO* blurList[2] = { blurFBO, rulbFBO };

		while (count < bloomStrength){
			auto currentFBO = blurList[blurIndex];
			currentFBO->clear();
			currentFBO->bind();
			shaderBlur->start();

			shaderBlur->setUniform("range", (float)bloomRange);
			shaderBlur->setUniform("horizontal", horizontal);
			canvas->displayObjects(shaderBlur, captureViews[0]);
			canvas->setTexture(currentFBO->colortexture, mySubObject::gAlbedo);
			horizontal = !horizontal;

			shaderBlur->stop();
			currentFBO->unbind();
			blurIndex = !blurIndex;
			++count;
		}; 

		glCheckError();
		
		geometryFBO->clear();
		geometryFBO->bind();
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			curr_shader = shaders["geo_buffer"]; curr_shader->start();

			obj = scene["shaderball"];
			obj->rotate(0.0f, 1.0f, 0.0f, delta);

			obj->setTexture(texAlbedo_A, mySubObject::pAlbedo);
			obj->setTexture(texAO_A, mySubObject::pAO);
			obj->setTexture(texMetallic_A, mySubObject::pMetal);
			obj->setTexture(texNormal_A, mySubObject::pNormal);
			obj->setTexture(texRoughness_A, mySubObject::pRough);
			
			obj->translate(-10, 0, 0);
			obj->displayObjects(curr_shader, view_matrix);
			obj->translate(10, 0, 0);

			obj->setTexture(texAlbedo_B, mySubObject::pAlbedo);
			obj->setTexture(texAO_B, mySubObject::pAO);
			obj->setTexture(texMetallic_B, mySubObject::pMetal);
			obj->setTexture(texNormal_B, mySubObject::pNormal);
			obj->setTexture(texRoughness_B, mySubObject::pRough);

			obj->translate(10, 0, 0);
			obj->displayObjects(curr_shader, view_matrix);
			obj->translate(-10, 0, 0);

			obj->setTexture(texAlbedo_X, mySubObject::pAlbedo);
			obj->setTexture(texAO_X, mySubObject::pAO);
			obj->setTexture(texMetallic_X, mySubObject::pMetal);
			obj->setTexture(texNormal_X, mySubObject::pNormal);
			obj->setTexture(texRoughness_X, mySubObject::pRough);

			obj->translate(0, 10, -20);
			obj->displayObjects(curr_shader, view_matrix);
			obj->translate(0, -10, 20);

			curr_shader->stop();
		};  geometryFBO->unbind();

		glCheckError();

		glGenerateTextureMipmap(geometryFBO->gAlbedo->texture_id);
		glGenerateTextureMipmap(geometryFBO->gNormal->texture_id);
		glGenerateTextureMipmap(geometryFBO->gPosition->texture_id);

		lightingFBO->bind(); {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			curr_shader = shaders["pbr_buffer"]; 
			curr_shader->start();
				scene["pbr_canvas"]->displayObjects(curr_shader, view_matrix);
			curr_shader->stop();
		}; lightingFBO->unbind();

		glCheckError();

		static float exposure = 1.0f;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/*-----------------------*/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		curr_shader = shaders["postprocess"]; curr_shader->start();
		curr_shader->setUniform("exposure", exposure);

		//scene["ppe_canvas"]->setTexture(preFBO->colortexture, mySubObject::COLORMAP);
		scene["ppe_canvas"]->setTexture(lightingFBO->colortexture, mySubObject::COLORMAP);
		scene["ppe_canvas"]->setTexture(geometryFBO->gPosition, mySubObject::gPosition);
		scene["ppe_canvas"]->setTexture(environmentFBO->extratexture, mySubObject::gExtra);
		scene["ppe_canvas"]->setTexture(environmentFBO->colortexture, mySubObject::gEnv);
		scene["ppe_canvas"]->setTexture(rulbFBO->colortexture, mySubObject::gBloom);
		
		scene["ppe_canvas"]->displayObjects(curr_shader, view_matrix); curr_shader->stop();

		/*-----------------------*/

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(window);
		ImGui::NewFrame(); //io.WantCaptureMouse = true;

		glCheckError();

		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static int counter = 0;

			ImGui::SetNextWindowCollapsed(true, ImGuiSetCond_Once);
			ImGui::Begin("Configuration", NULL, ImVec2(0, 0), -1.0, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Powered by ImGui");               // Display some text (you can use a format strings too)
			//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			//ImGui::Checkbox("Another Window", &show_another_window);
			//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			//ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color
			ImGui::SliderFloat("Exposure", &exposure, 0.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::SliderInt("Bloom Range", &bloomRange, 2, 16);
			ImGui::SliderInt("Bloom Strength", &bloomStrength, 2, 16);

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		glCheckError();

		// 3. Show another simple window.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		glCheckError();

		// Rendering
		ImGui::Render();
		SDL_GL_MakeCurrent(window, glContext);
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glCheckError();
		SDL_GL_SwapWindow(window); 
		SDL_Event current_event;
		glCheckError();

		while (SDL_PollEvent(&current_event) != 0) {
			
			if (ImGui::IsMouseHoveringAnyWindow()) {
				ImGui_ImplSDL2_ProcessEvent(&current_event);
			} else {
				processEvents(current_event);
			}
		}
		glCheckError();
	}

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// Freeing resources before exiting.
	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);
	// Quit SDL subsystems
	SDL_Quit();
	return 0;
}