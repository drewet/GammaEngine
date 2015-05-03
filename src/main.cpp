#include <unistd.h>
#include <string.h>
#include <iostream>
#include "Instance.h"
#include "Window.h"
#include "Scene.h"
#include "Renderer.h"
#include "Object.h"
#include "Image.h"
#include "Camera.h"
#include "Time.h"
#include "Input.h"

using namespace GE;

int main2();

int main( int argc, char** argv )
{
	chdir("..");

	Instance* instance = new Instance( "GammaEngine test", 42 );
	Window* window = new Window( instance, 0, "Hello GammaEngine !", 1280, 720 );
	Input* input = new Input( window );

	Object* cube = new Object( "scene/cube.obj" );

// 	Image* image = new Image( "scene/texture.png" );

	Renderer* renderer = new Renderer();
	renderer->LoadVertexShader( "shaders/basic.vert" );
	renderer->LoadFragmentShader( "shaders/basic.frag" );
	renderer->AddObject( cube );
	renderer->Compute();

	Scene* scene = new Scene();
	scene->AddRenderer( renderer );

	Camera* camera = new Camera();
	camera->LookAt( { -10.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );

	float fps = 0.0f;
	float time = Time::GetSeconds();
	uint32_t img = 0;

	while ( 1 ) {
		float dt = Time::Delta();

		cube->matrix()->RotateX( 1.0f * dt );
		cube->matrix()->RotateY( 1.4f * dt );

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
		}
// 		printf("FPS : %.2f\n", fps);
	}

	return 0;
}
