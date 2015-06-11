#include <unistd.h>
#include <string.h>
#include <iostream>
#include "Debug.h"
#include "Instance.h"
#include "Window.h"
#include "Scene.h"
#include "Renderer.h"
#include "Object.h"
#include "Image.h"
#include "Camera.h"
#include "Time.h"
#include "Input.h"
#include "PhysicalBody.h"
#include "PhysicalGraph.h"
#include "Light.h"

using namespace GE;

int main( int argc, char** argv )
{
	chdir("..");

	Instance* instance = Instance::Create( "GammaEngine test", 42 );
	Window* window = instance->CreateWindow( "Hello GammaEngine !", 1280, 720, Window::Resizable );
	Input* input = new Input( window );

	Object* cube = instance->LoadObject( "scene/cube.obj" );
	Object* cube2 = instance->LoadObject( "scene/street.obj" );

	Image* texture = new Image( "scene/texture.png" );
	cube->setTexture( 0, texture );
	cube2->setTexture( 0, texture );

	Light* light0 = new Light( Light::Spot, { 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, -1.0f }, 70.0f );

	Renderer* renderer = instance->CreateRenderer();
	renderer->LoadVertexShader( "shaders/basic.vert" );
	renderer->LoadFragmentShader( "shaders/basic.frag" );
	renderer->AddObject( cube );
	renderer->AddObject( cube2 );
	renderer->AddLight( light0 );
	renderer->Compute();

	Scene* scene = new Scene();
	scene->AddRenderer( renderer );

	Camera* camera = new Camera();
	camera->setInertia( 0.999f );
	camera->LookAt( { -6.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );

	float fps = 0.0f;
	float time = Time::GetSeconds();
	uint32_t ticks = Time::GetTick();
	uint32_t img = 0;

	while ( 1 ) {
		float dt = Time::Delta();

		input->Update();
		if ( input->pressed( 'Z' ) ) {
			camera->WalkForward( 5.0 );
		}
		if ( input->pressed( 'S' ) ) {
			camera->WalkBackward( 5.0 );
		}
		if ( input->pressed( 'Q' ) ) {
			camera->WalkLeft( 5.0 );
		}
		if ( input->pressed( 'D' ) ) {
			camera->WalkRight( 5.0 );
		}
		if ( input->pressed( ' ' ) ) {
			camera->Translate( { 0.0f, 0.0f, (float)( 5.0 * Time::Delta() ) } );
		}
		if ( input->pressed( 'C' ) ) {
			camera->Translate( { 0.0f, 0.0f, (float)( -5.0 * Time::Delta() ) } );
		}
		if ( input->pressed( Input::LBUTTON ) ) {
			camera->RotateH( input->cursorWarp().x, -2.0f );
			camera->RotateV( input->cursorWarp().y, -2.0f );
		}

		cube->matrix()->Identity();
// 		cube2->matrix()->Identity();
// 		cube2->matrix()->Translate( { 0.0f, 4.0f, 0.0f } );

		window->Clear( 0xFF202020 );
		window->BindTarget();

		scene->Draw( camera );

		window->SwapBuffers();
		Time::GlobalSync();

		img++;
		if ( Time::GetSeconds() - time > 0.2f ) {
			fps = img * 1.0f / ( Time::GetSeconds() - time );
			time = Time::GetSeconds();
			img = 0;
// 			printf("FPS : %.2f\n", fps);
// 			printf(" RAM : %lu kB\n", instance->cpuRamCounter() / 1024);
// 			printf("VRAM : %lu kB\n", instance->gpuRamCounter() / 1024);
		}
	}

	return 0;
}
