#include <iostream>
#include "Instance.h"
#include "Renderer.h"

int main(int argc, char **argv)
{
	Instance* instance = new Instance( "GammaEngine test", 42 );
// 	Window* window = new Window( instance, "Hello GammaEngine !", 1280, 720, 0 );

// 	Object* cube = new Object( "scene/cube.obj" );

	Renderer* renderer = new Renderer( instance );
	renderer->LoadVertexShader( "shaders/basic.bvs" );
	renderer->LoadFragmentShader( "shaders/basic.bfs" );
// 	renderer->AddObject( cube );
	renderer->Compute();

	while ( 1 ) {
// 		cube->matrix()->Rotate( 0.001f, 0.0014f, 0.0f );

// 		window->Clear();
		renderer->Draw();
// 		window->SwapBuffers();
	}

	return 0;
}
