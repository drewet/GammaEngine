#include <unistd.h>
#include <iostream>
#include "Instance.h"
#include "Renderer.h"
#include "Object.h"
#include "Image.h"

using namespace GE;

int main(int argc, char **argv)
{
	chdir("..");

	/*Instance* instance = */new Instance( "GammaEngine test", 42 );
// 	Window* window = new Window( [instance], "Hello GammaEngine !", 1280, 720, 0 );

	Object* cube = new Object( "scene/cube.obj" );

	Image* image = new Image( "scene/texture.png" );

	Renderer* renderer = new Renderer();
	renderer->LoadVertexShader( "shaders/basic.bvs" );
	renderer->LoadFragmentShader( "shaders/basic.bfs" );
	renderer->AddObject( cube );
	renderer->Compute();

	while ( 1 ) {
		cube->matrix()->RotateX( 0.001f );
		cube->matrix()->RotateY( 0.0014f );

// 		window->Clear();
		renderer->Draw();
// 		window->SwapBuffers();
	}

	return 0;
}
