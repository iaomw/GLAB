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

#include "gFBO.h"
#include "myFBO.h"

#include <ctime>


using namespace std;

// SDL variables
SDL_Window* window;
SDL_GLContext glContext;
static Uint32 next_time;
#define TICK_INTERVAL 16.6

Uint32 time_left(void)
{
	Uint32 now;

	now = SDL_GetTicks();
	if (next_time <= now)
		return 0;
	else
		return next_time - now;
}

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

int main(int argc, char *argv[])
{
	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

	// Using OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetSwapInterval(1);

	//glPixelStori(GL_UNPACK_ALIGNMENT, 2);
	//glPixelStori(GL_UNPACK_SWAP, TRUE);

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	// Create window
	window = SDL_CreateWindow("IN4I24", 
				SDL_WINDOWPOS_CENTERED, 
				SDL_WINDOWPOS_CENTERED,
				DEFAULT_WINDOW_WIDTH, 
				DEFAULT_WINDOW_HEIGHT, 
				SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	mainCam = new myCamera(); mainCam->camera_eye = glm::vec3(0, 0, 32);
	SDL_GetWindowSize(window, &mainCam->window_width, &mainCam->window_height);
	
	checkOpenGLInfo(true);

	/**************************INITIALIZING LIGHTS ***************************/
	scene.lights = new myLights();
	scene.lights->lights.push_back(new myLight(glm::vec3(1, 0, 0.4), glm::vec3(0.5, 0.5, 0.5), myLight::POINTLIGHT));
	scene.lights->lights.push_back(new myLight(glm::vec3(0, 1, 0.7), glm::vec3(0.6, 0.6, 0.6), myLight::POINTLIGHT));

	/**************************INITIALIZING FBO ***************************/
	//plane will draw the color_texture of the framebufferobject fbo.

	gFBO *gfbo = new gFBO();
	gfbo->initFBO(mainCam->window_width, mainCam->window_height);

	myFBO *lfbo = new myFBO();
	lfbo->initFBO(mainCam->window_width, mainCam->window_height);

	/**************************INITIALIZING OBJECTS THAT WILL BE DRAWN ***************************/
	myObject *obj;

	//enviornment mapped object
	obj = new myObject();
	obj->readObjects("models/skycube.obj", true, false);
	//obj->computeTexturecoordinates_sphere();
	obj->createmyVAO();
	vector <string> cubemaps = { 
		"textures/GalaxyTex/GalaxyTex_PositiveX.png",
		"textures/GalaxyTex/GalaxyTex_NegativeX.png",
		"textures/GalaxyTex/GalaxyTex_NegativeY.png",
		"textures/GalaxyTex/GalaxyTex_PositiveY.png",
		"textures/GalaxyTex/GalaxyTex_PositiveZ.png",
		"textures/GalaxyTex/GalaxyTex_NegativeZ.png" };
	
	cubemaps = { 
		"textures/LatePark/right.png",
		"textures/LatePark/left.png",
		"textures/LatePark/up.png",
		"textures/LatePark/down.png",
		"textures/LatePark/front.png",
		"textures/LatePark/back.png" };

	auto cubeTex = new myTexture(cubemaps);
	obj->setTexture(cubeTex, mySubObject::CUBEMAP);
	obj->translate(0, 0, 0);
	obj->scale(1000.0f, 1000.0f, 1000.0f);
	scene.addObjects(obj, "skycube");

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	//obj->computeTexturecoordinates_plane();
	obj->createmyVAO();
	obj->setTexture(gfbo->gAlbedo, mySubObject::COLORMAP);
	//obj->translate(0, 0, -10);
	scene.addObjects(obj, "lighting_plane");

	scene["lighting_plane"]->setTexture(gfbo->gPosition, mySubObject::gPosition);
	scene["lighting_plane"]->setTexture(gfbo->gAlbedo, mySubObject::gAlbedo);
	scene["lighting_plane"]->setTexture(gfbo->gNormal, mySubObject::gNormal);
	scene["lighting_plane"]->setTexture(gfbo->gEnv, mySubObject::gEnv);

	obj = new myObject();
	obj->readObjects("models/sphere.obj", false, false);
	obj->computeTexturecoordinates_plane();
	obj->createmyVAO();
	obj->scale(1, 1, 1);
	obj->translate(0, 0, 0);
	obj->setTexture(cubeTex, mySubObject::CUBEMAP);
	scene.addObjects(obj, "light_ball");

	obj = new myObject();
	obj->readObjects("models/plane.obj", true, false);
	//obj->computeTexturecoordinates_plane();
	obj->createmyVAO();
	obj->setTexture(gfbo->gAlbedo, mySubObject::COLORMAP);
	scene.addObjects(obj, "postproc_plane");

	obj->setTexture(lfbo->colortexture, mySubObject::COLORMAP);

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

	obj->setTexture(texAlbedo, mySubObject::pAlbedo);
	obj->setTexture(texAO, mySubObject::pAO);
	obj->setTexture(texMetallic, mySubObject::pMetal);
	obj->setTexture(texNormal, mySubObject::pNormal);
	obj->setTexture(texRoughness, mySubObject::pRough);
	obj->setTexture(cubeTex, mySubObject::CUBEMAP);

	// aluminum
	obj = new myObject();
	obj->readObjects("models/shaderball.obj", true, false);
	obj->createmyVAO();
	obj->translate(10, 0, 0);
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
	obj->setTexture(cubeTex, mySubObject::CUBEMAP);

	/**************************SETTING UP OPENGL SHADERS ***************************/
	myShaders shaders;

	shaders.addShader(new myShader("shaders/skycube.vs", "shaders/skycube.ps"), "shader_skycube");

	shaders.addShader(new myShader("shaders/plane.vs", "shaders/plane.ps"), "lighting_plane");
	shaders.addShader(new myShader("shaders/geo_buffer.vs", "shaders/geo_buffer.ps"), "geo_buffer");
	shaders.addShader(new myShader("shaders/pbr_buffer.vs", "shaders/pbr_buffer.ps"), "pbr_buffer");

	shaders.addShader(new myShader("shaders/environmentmap-vertexshader.glsl", "shaders/environmentmap-fragmentshader.glsl"), "basicx");
	
	// display loop
	myShader *curr_shader;
	next_time = SDL_GetTicks() + TICK_INTERVAL;
	while (!quit)
	{
		if (windowsize_changed)
		{
			SDL_GetWindowSize(window, &mainCam->window_width, &mainCam->window_height);
			windowsize_changed = false;

			if (gfbo) { delete gfbo; } gfbo = new gFBO();
			gfbo->initFBO(mainCam->window_width, mainCam->window_height);

			scene["lighting_plane"]->setTexture(gfbo->gPosition, mySubObject::gPosition);
			scene["lighting_plane"]->setTexture(gfbo->gAlbedo, mySubObject::gAlbedo);
			scene["lighting_plane"]->setTexture(gfbo->gNormal, mySubObject::gNormal);
			scene["lighting_plane"]->setTexture(gfbo->gEnv, mySubObject::gEnv);

			if (lfbo) { delete lfbo; } lfbo = new myFBO();
			lfbo->initFBO(mainCam->window_width, mainCam->window_height);

			scene["postproc_plane"]->setTexture(lfbo->colortexture, mySubObject::COLORMAP);
		}

		//Computing transformation matrices. Note that model_matrix will be computed and set in the displayScene function for each object separately
		glViewport(0, 0, mainCam->window_width, mainCam->window_height);
		glm::mat4 projection_matrix = mainCam->projectionMatrix();
		glm::mat4 view_matrix = mainCam->viewMatrix();
		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix)));

		time_t t = time(0); struct tm * now = localtime(&t);

		//for (unsigned int i = 0; i < scene.lights->lights.size(); i++)
		//{
			//scene.lights->lights[i]->position = glm::vec3(sin(t/200.0f));
		//}

		//Setting uniform variables for each shader
		for (unsigned int i=0; i < shaders.size(); i++)
		{
			curr_shader = shaders[i]; curr_shader->start();
			curr_shader->setUniform("myprojection_matrix", projection_matrix);
			curr_shader->setUniform("myview_matrix", view_matrix);
			curr_shader->setUniform("cam_position", mainCam->camera_eye);

			scene.lights->setUniform(curr_shader, "lights");
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*-----------------------*/
		curr_shader = shaders["shader_skycube"];
		curr_shader->start();
		scene["skycube"]->displayObjects(curr_shader, view_matrix);

		curr_shader = shaders["basicx"]; curr_shader->start();
		scene["light_ball"]->setTexture(texAlbedo, mySubObject::COLORMAP);

		for (auto pLight : scene.lights->lights) {

			auto pos = pLight->position;
			auto xxx = sin(t);
			pos.x = (float)xxx;

			pLight->setUniform(curr_shader, "lights");

			scene["light_ball"]->translate(0, 0, 0);
			scene["light_ball"]->scale(3, 3, 3);
			scene["light_ball"]->translate(pos.x, pos.y, pos.z);
			scene["light_ball"]->displayObjects(curr_shader, view_matrix);
		}

		gfbo->clear();
		gfbo->bind();
		{
			glDisable(GL_BLEND);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			curr_shader = shaders["geo_buffer"]; curr_shader->start();

			scene["rustediron"]->translate(10, 0, 0);
			scene["rustediron"]->rotate(0.0f, 1.0f, 0.0f, sin(t) / 100);
			scene["rustediron"]->translate(-10, 0, 0);

			scene["rustediron"]->displayObjects(curr_shader, view_matrix);

			scene["aluminum"]->translate(-10, 0, 0);
			scene["aluminum"]->rotate(0.0f, 1.0f, 0.0f, -sin(t) / 100);
			scene["aluminum"]->translate(10, 0, 0);

			scene["aluminum"]->displayObjects(curr_shader, view_matrix);

			glEnable(GL_BLEND);
		}
		gfbo->unbind();

		lfbo->bind(); {
			glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			curr_shader = shaders["pbr_buffer"]; curr_shader->start();
			//scene["lighting_plane"]->setTexture(gfbo->gEnv, mySubObject::COLORMAP);
			scene["lighting_plane"]->displayObjects(curr_shader, view_matrix);
		}lfbo->unbind();

		glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
		curr_shader = shaders["lighting_plane"]; curr_shader->start();
		//scene["lighting_plane"]->setTexture(gfbo->gEnv, mySubObject::COLORMAP);
		scene["postproc_plane"]->displayObjects(curr_shader, view_matrix);

		//curr_shader = shaders["lighting_plane"]; curr_shader->start();
		
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, gfbo->gBuffer);
		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
												   // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
												   // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
												   // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		//glBlitFramebuffer(0, 0, mainCam->window_width, mainCam->window_height, 
							//0, 0, mainCam->window_width, mainCam->window_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*-----------------------*/
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//curr_shader = shaders["pbr_buffer"];
		//curr_shader->start();

		/*-----------------------*/

		SDL_GL_SwapWindow(window); SDL_Event current_event;
		while (SDL_PollEvent(&current_event) != 0) {
			processEvents(current_event);
		}
		SDL_Delay(time_left());
		next_time += TICK_INTERVAL;
	}

	// Freeing resources before exiting.
	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);
	// Quit SDL subsystems
	SDL_Quit();
	return 0;
}