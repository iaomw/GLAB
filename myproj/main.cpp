#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/GLU.h>

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

#include <iostream>
#include <glm/glm.hpp>

#include "FBO.h"
#include "GeoFBO.h"
#include "CubeFBO.h"

#include <ctime>


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

void PrintError() {
	GLenum err;
	for (;;) {
		err = glGetError();
		if (err == GL_NO_ERROR) break;
		printf("Error: %d %s\n", err, glewGetErrorString(err));
	}
}

int main(int argc, char *argv[])
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
	
	SDL_GL_SetSwapInterval(1);

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	// Create window
	window = SDL_CreateWindow("GLAB", 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED,
				DEFAULT_WINDOW_WIDTH, 
				DEFAULT_WINDOW_HEIGHT, 
				SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(message_callback, nullptr);

	//Gamma correct
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	checkOpenGLInfo(true);

	mainCam = new myCamera(); mainCam->camera_eye = glm::vec3(0, 0, 32);
	SDL_GetWindowSize(window, &mainCam->window_width, &mainCam->window_height);
	
	/**************************INITIALIZING LIGHTS ***************************/
	scene.lights = new myLights();
	scene.lights->lights.push_back(new myLight(myLight::POINTLIGHT, glm::vec3(-20, 20, 0), glm::vec3(0.5, 0.5, 0.5)));
	scene.lights->lights.push_back(new myLight(myLight::POINTLIGHT, glm::vec3(20, -20, 0), glm::vec3(0.4, 0.6, 0.4)));

	/**************************INITIALIZING FBO ***************************/
	//plane will draw the color_texture of the framebufferobject fbo.

	auto gFBO = new GeoFBO();
	gFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto lFBO = new FBO();
	lFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto eFBO = new FBO();
	eFBO->initFBO(mainCam->window_width, mainCam->window_height);

	auto bFBO = new FBO();
	bFBO->initFBO(512, 512);

	auto cFBO = new CubeFBO();
	cFBO->initFBO(2048, 2048);

	auto iFBO = new CubeFBO();
	iFBO->initFBO(64, 64);

	auto pFBO = new CubeFBO();
	pFBO->initFBO(512, 512);

	/**************************SETTING UP OPENGL SHADERS ***************************/
	myShaders shaders;

	shaders.addShader(new myShader("shaders/geo_buffer.vs.glsl", "shaders/geo_buffer.fs.glsl"), "geo_buffer");
	shaders.addShader(new myShader("shaders/pbr_buffer.vs.glsl", "shaders/pbr_buffer.fs.glsl"), "pbr_buffer");

	shaders.addShader(new myShader("shaders/basic.vs.glsl", "shaders/basic.fs.glsl"), "basicx");
	//shaders.addShader(new myShader("shaders/texture.vs.glsl", "shaders/texture.fs.glsl"), "basicx");
	shaders.addShader(new myShader("shaders/postprocess.vs.glsl", "shaders/postprocess.fs.glsl"), "postprocess");
	shaders.addShader(new myShader("shaders/skycube.vs.glsl", "shaders/skycube.fs.glsl"), "shader_skycube");

	myShader *captureShader = new myShader("shaders/equirectangular.vs.glsl", "shaders/equirectangular.fs.glsl");
	myShader *irradianceShader = new myShader("shaders/irradiance.vs.glsl", "shaders/irradiance.fs.glsl");
	myShader *prefilterShader = new myShader("shaders/equirectangular.vs.glsl", "shaders/prefilter.fs.glsl");
	myShader *brdfShader = new myShader("shaders/brdf.vs.glsl", "shaders/brdf.fs.glsl");

	/**************************INITIALIZING OBJECTS THAT WILL BE DRAWN ***************************/

	//enviornment mapped object
	auto obj = new myObject();
	obj->readObjects("models/skycube.obj", true, false);
	obj->createmyVAO();
	obj->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
	obj->translate(0, 0, 0);
	obj->scale(1000.0f, 1000.0f, 1000.0f);
	scene.addObjects(obj, "skycube");

	obj = new myObject();
	obj->readObjects("models/skycube.obj", true, false);
	obj->createmyVAO();

	auto hdrTexture = new myTexture("textures/envirment/vulture_hide_4k.hdr");

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
		captureShader->start();
		for (unsigned int i = 0; i < 6; ++i)
		{
			glNamedFramebufferTextureLayer(cFBO->fboID, GL_COLOR_ATTACHMENT0, cFBO->envTexture->texture_id, 0, i);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			captureShader->setUniform("myview_matrix", captureViews[i]);
			//captureShader->setUniform("cam_position", mainCam->camera_eye);
			captureShader->setUniform("myprojection_matrix", captureProjection);
			
			obj->displayObjects(captureShader, captureViews[i]);
		}
		glGenerateTextureMipmap(cFBO->envTexture->texture_id);
		captureShader->stop();
		cFBO->unbind();

		//scene["skycube"]->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);

		iFBO->bind();
		glViewport(0, 0, iFBO->getWidth(), iFBO->getHeight());
		obj->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
		//glGenerateTextureMipmap(iFBO->envTexture->texture_id);
		irradianceShader->start();
		for (unsigned int i = 0; i < 6; ++i)
		{
			glNamedFramebufferTextureLayer(iFBO->fboID, GL_COLOR_ATTACHMENT0, iFBO->envTexture->texture_id, 0, i);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			irradianceShader->setUniform("myview_matrix", captureViews[i]);
			//captureShader->setUniform("cam_position", mainCam->camera_eye);
			irradianceShader->setUniform("myprojection_matrix", captureProjection);
			
			obj->displayObjects(irradianceShader, captureViews[i]);
		}
		glGenerateTextureMipmap(iFBO->envTexture->texture_id);
		irradianceShader->stop();
		iFBO->unbind();

		pFBO->bind();
		glViewport(0, 0, pFBO->getWidth(), pFBO->getHeight());
		obj->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
		glGenerateTextureMipmap(pFBO->envTexture->texture_id);
		prefilterShader->start();
		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = pFBO->getWidth() * std::pow(0.5, mip);
			unsigned int mipHeight = pFBO->getHeight() * std::pow(0.5, mip);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			prefilterShader->setUniform("roughness", roughness);

			for (unsigned int i = 0; i < 6; ++i)
			{
				glNamedFramebufferTextureLayer(pFBO->fboID, GL_COLOR_ATTACHMENT0, pFBO->envTexture->texture_id, mip, i);
				//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pFBO->envTexture->texture_id, mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				prefilterShader->setUniform("myview_matrix", captureViews[i]);
				//captureShader->setUniform("cam_position", mainCam->camera_eye);
				prefilterShader->setUniform("myprojection_matrix", captureProjection);
				
				obj->displayObjects(prefilterShader, captureViews[i]);
			}
			auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete: " << fboStatus << std::endl;
		}
		prefilterShader->stop();
		pFBO->unbind();

		auto xx = new myObject();
		xx->readObjects("models/plane.obj", true, false);
		xx->createmyVAO();

		bFBO->bind();
			brdfShader->start();
				glViewport(0, 0, bFBO->getWidth(), bFBO->getHeight());
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				xx->displayObjects(brdfShader, captureViews[0]);
			brdfShader->stop();
			bFBO->bind();
		bFBO->unbind();

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	obj->createmyVAO();
	scene.addObjects(obj, "env_canvas");

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	obj->createmyVAO();
	obj->setTexture(gFBO->gPosition, mySubObject::gPosition);
	obj->setTexture(gFBO->gAlbedo, mySubObject::gAlbedo);
	obj->setTexture(gFBO->gNormal, mySubObject::gNormal);

	obj->setTexture(iFBO->envTexture, mySubObject::gIrradiance);
	obj->setTexture(pFBO->envTexture, mySubObject::gPrefilter);
	obj->setTexture(bFBO->colortexture, mySubObject::BRDF_LUT);

	scene.addObjects(obj, "pbr_canvas");

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	obj->createmyVAO();
	obj->setTexture(lFBO->colortexture, mySubObject::COLORMAP);
	obj->setTexture(eFBO->colortexture, mySubObject::gEnv);
	scene.addObjects(obj, "ppe_canvas");

	// rustediron
	obj = new myObject();
	obj->readObjects("models/shaderball.obj", true, false);
	obj->createmyVAO();
	obj->translate(-10, 0, 0);
	scene.addObjects(obj, "rustediron");

	myTexture* texAlbedo = new myTexture("textures/rustediron/albedo.png");
	myTexture* texAO = new myTexture("textures/rustediron/ao.png");
	myTexture* texMetallic = new myTexture("textures/rustediron/metalness.png");
	myTexture* texNormal = new myTexture("textures/rustediron/normal.png");
	myTexture* texRoughness = new myTexture("textures/rustediron/roughness.png");
	/*
	texAlbedo = new myTexture("textures/metal/Metal23_col.jpg.");
	texAO = new myTexture("textures/metal/Metal23_disp.jpg");
	texMetallic = new myTexture("textures/metal/Metal23_met.jpg");
	texNormal = new myTexture("textures/metal/Metal23_nrm.jpg");
	texRoughness = new myTexture("textures/metal/Metal23_rgh.jpg");
	*/
	obj->setTexture(texAlbedo, mySubObject::pAlbedo);
	obj->setTexture(texAO, mySubObject::pAO);
	obj->setTexture(texMetallic, mySubObject::pMetal);
	obj->setTexture(texNormal, mySubObject::pNormal);
	obj->setTexture(texRoughness, mySubObject::pRough);

	// aluminum
	obj = new myObject();
	obj->readObjects("models/shaderball.obj", true, false);
	obj->createmyVAO();
	obj->translate(+10, 0, 0);
	scene.addObjects(obj, "aluminum");

	texAlbedo = new myTexture("textures/aluminum/basecolor.png");
	texAO = new myTexture("textures/aluminum/ao.png");
	texMetallic = new myTexture("textures/aluminum/metallic.png");
	texNormal = new myTexture("textures/aluminum/normal.png");
	texRoughness = new myTexture("textures/aluminum/roughness.png");

	obj->setTexture(texAlbedo, mySubObject::pAlbedo);
	obj->setTexture(texAO, mySubObject::pAO);
	obj->setTexture(texMetallic, mySubObject::pMetal);
	obj->setTexture(texNormal, mySubObject::pNormal);
	obj->setTexture(texRoughness, mySubObject::pRough);
	
	obj = new myObject();
	obj->readObjects("models/sphere.obj", true, false);
	obj->computeTexturecoordinates_sphere();
	obj->createmyVAO();
	obj->scale(1, 1, 1);
	obj->translate(0, 30, 0);
	obj->setTexture(texAlbedo, mySubObject::COLORMAP);
	scene.addObjects(obj, "light_ball");

	// display loop
	myShader *curr_shader;

	while (!quit)
	{
		if (windowsize_changed)
		{
			SDL_GetWindowSize(window, &mainCam->window_width, &mainCam->window_height);
			windowsize_changed = false;

			if (gFBO) { delete gFBO; } gFBO = new GeoFBO();
			gFBO->initFBO(mainCam->window_width, mainCam->window_height);

			auto object = scene["pbr_canvas"];

			object->setTexture(gFBO->gPosition, mySubObject::gPosition);
			object->setTexture(gFBO->gAlbedo, mySubObject::gAlbedo);
			object->setTexture(gFBO->gNormal, mySubObject::gNormal);

			object->setTexture(iFBO->envTexture, mySubObject::gIrradiance);
			object->setTexture(pFBO->envTexture, mySubObject::gPrefilter);
			object->setTexture(bFBO->colortexture, mySubObject::BRDF_LUT);

			if (lFBO) { delete lFBO; } lFBO = new FBO();
			lFBO->initFBO(mainCam->window_width, mainCam->window_height);

			if (eFBO) { delete eFBO; } eFBO = new FBO();
			eFBO->initFBO(mainCam->window_width, mainCam->window_height);

			scene["ppe_canvas"]->setTexture(lFBO->colortexture, mySubObject::COLORMAP);
			scene["ppe_canvas"]->setTexture(eFBO->colortexture, mySubObject::gEnv);
		}

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

			scene.lights->setUniform(curr_shader, "lights");
		}

		eFBO->clear();
		eFBO->bind(); {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			scene["skycube"]->setTexture(cFBO->envTexture, mySubObject::CUBEMAP);
			curr_shader = shaders["shader_skycube"];
			curr_shader->start();
			scene["skycube"]->displayObjects(curr_shader, view_matrix);
			curr_shader->stop();
			glGenerateTextureMipmap(eFBO->colortexture->texture_id);
		}; eFBO->unbind();
		
		glDisable(GL_BLEND);
		gFBO->clear();
		gFBO->bind();
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			auto currentTime = SDL_GetTicks();
			auto ratio = currentTime / 10000000.0;

			curr_shader = shaders["geo_buffer"]; curr_shader->start();

			scene["rustediron"]->translate(10, 0, 0);
			scene["rustediron"]->rotate(0.0f, 1.0f, 0.0f, ratio);
			scene["rustediron"]->translate(-10, 0, 0);
			scene["rustediron"]->displayObjects(curr_shader, view_matrix);

			scene["aluminum"]->translate(-10, 0, 0);
			scene["aluminum"]->rotate(0.0f, 1.0f, 0.0f, -ratio);
			scene["aluminum"]->translate(10, 0, 0);
			scene["aluminum"]->displayObjects(curr_shader, view_matrix);

			curr_shader->stop();
			
		};  gFBO->unbind();
		glEnable(GL_BLEND);

		lFBO->bind(); {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			curr_shader = shaders["pbr_buffer"]; 
			curr_shader->start();
				scene["pbr_canvas"]->displayObjects(curr_shader, view_matrix);
			curr_shader->stop();
		}; lFBO->unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		/*-----------------------*/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		curr_shader = shaders["postprocess"]; curr_shader->start();
		scene["ppe_canvas"]->setTexture(lFBO->colortexture, mySubObject::COLORMAP);
		scene["ppe_canvas"]->setTexture(eFBO->colortexture, mySubObject::gEnv);
		scene["ppe_canvas"]->displayObjects(curr_shader, view_matrix); curr_shader->stop();

		PrintError();
		
		//glDisable(GL_TEXTURE_2D);
		//curr_shader = shaders["basicx"]; curr_shader->start();
		//auto light = scene.lights->lights[0];
		//scene["light_ball"]->displayObjects(curr_shader, view_matrix);
		//scene["light_ball"]->translate(0, 5, 0);
		//scene["light_ball"]->displayObjects(curr_shader, view_matrix);
		//glEnable(GL_TEXTURE_2D);

		PrintError();
		/*-----------------------*/
		SDL_GL_SwapWindow(window); SDL_Event current_event;
		while (SDL_PollEvent(&current_event) != 0) {
			processEvents(current_event);
		}
	}

	// Freeing resources before exiting.
	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);
	// Quit SDL subsystems
	SDL_Quit();
	return 0;
}