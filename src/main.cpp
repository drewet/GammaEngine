#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#include "Instance.h"
#include "Window.h"
#include "Scene.h"
#include "Renderer.h"
#include "Renderer2D.h"
#include "Object.h"
#include "Image.h"
#include "Font.h"
#include "Camera.h"
#include "Time.h"
#include "Input.h"
#include "PhysicalBody.h"
#include "PhysicalGraph.h"
#include "Thread.h"
#include "Socket.h"
#include "Light.h"
#include "DeferredRenderer.h"
#include "SkyRenderer.h"
#include "Sound.h"
#include "Music.h"
#include "Debug.h"

using namespace GE;

class LightsThread : public Thread {
public:
	LightsThread( Window* win ) : Thread( win ) { srand( time(nullptr) ); }
	DeferredRenderer* deferredRenderer;
	Light** lights;
	float n = 0.0;
protected:
	virtual bool run() {
		for ( uint32_t i = 0; lights[i]; i++ ) {
			Vector3f pos = lights[i]->position();
			float dist = ( pos - Vector3f( -50.0f, -50.0f, 0.0f ) ).length();
			pos.x += cos(n) * 0.01 * dist;
			pos.y += sin(n) * 0.01 * dist;
			lights[i]->setPosition( pos );
		}
		n += 0.1;
//		deferredRenderer->Update();
		usleep( 1000000 / 60 );
		return true;
	}
};

int main( int argc, char** argv )
{
	srand( time(nullptr) );

	Instance* instance = Instance::Create( "GammaEngine test", 42, true, "opengl43" );
	Window* window = instance->CreateWindow( "Hello GammaEngine !", 1280, 720, Window::Resizable );
	Input* input = new Input( window );
	LightsThread* thread = new LightsThread( window );

	auto sound_devs = AudioOutput::DevicesList();
	for ( auto dev : sound_devs ) {
		gDebug() << "Sound card " << dev.first << ": " << dev.second << "\n";
	}

	Music* music = new Music( "scene/sounds/LRMonoPhase4.mp3" );
	gDebug() << "Duration : " << music->duration() << "s\n";
	music->Play();
	gDebug() << "Playing...\n";
	while ( true or music->isPlaying() or true ) {
		Time::Sleep( 1 );
	}
	delete music;
	return 0;

// 	Sound* sound = new Sound( "scene/sounds/LRMonoPhase4.wav" ); // http://www.kozco.com/tech/soundtests.html
	Sound* sound = new Sound( "scene/sounds/LRMonoPhase4.mp3" );
	gDebug() << "Duration : " << sound->duration() << "s\n";
	sound->Play();
	gDebug() << "Playing...\n";
	while ( sound->isPlaying() or true ) {
		Time::Sleep( 1 );
	}
	delete sound;
	return 0;

	Font* font = new Font( "scene/Arial Unicode MS.ttf" );
	gDebug() << "font->texture() : " << font->texture() << "\n";
// 	return 0;

 	std::list< Object* > scene_objects = Object::LoadObjects( "scene/street.obj", instance );
//	std::list< Object* > scene_objects = Object::LoadObjects( "scene/Paris/paris.obj", instance );
//	std::list< Object* > scene_objects = Object::LoadObjects( "scene/city.obj", instance );

	Object* cube = instance->LoadObject( "scene/cube.obj" );
// 	Object* cube2 = instance->LoadObject( "scene/street.obj" );

	Image* texture = new Image( "scene/texture.png" );
	cube->setTexture( instance, 0, texture );
// 	cube2->setTexture( instance, 0, texture );

	const int nLights = 32;
	Light* lights[nLights + 1];
	for ( int i = 0; i < nLights; i++ ) {
		Vector4f pos;
		pos.x = -(float)( ( rand() % 128 ) - 25 );
		pos.y = -(float)( ( rand() % 128 ) - 25 );
		pos.z = 5.8f;
		lights[i] = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), pos );
	}
	lights[nLights] = nullptr;

	Light* sun_light = new Light( Vector4f( 1.0, 1.0, 1.0, 1.0 ), Vector3f( 10000000.0f, 5000000.0f, 10000000.0f ), 0.0f );
// 	Light* light0 = new Light( { 0.0f, 0.0f, 10.0f }, { 0.0f, 0.0f, -1.0f }, 70.0f );
	Light* lightm1 = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), Vector3f( 2.8f, 25.0f, 5.9f ) );
// 	Light* light0 = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), Vector3f( 2.8f, 0.0f, 5.8f ) );
	Light* light0 = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), Vector3f( 2.8f, 0.0f, 5.9f ), Vector3f( -0.4f, 0.0f, -1.0f ), 30.0f, 50.0f );
	Light* light1 = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), Vector3f( 2.8f, -25.0f, 5.9f ), Vector3f( -0.4f, 0.0f, -1.0f ), 30.0f, 50.0f );
	Light* light2 = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), Vector3f( 2.8f, -50.0f, 5.9f ), Vector3f( -0.4f, 0.0f, -1.0f ), 30.0f, 50.0f );
	Light* light3 = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), Vector3f( 2.8f, -75.0f, 5.9f ), Vector3f( -0.4f, 0.0f, -1.0f ), 30.0f, 50.0f );
	Light* light4 = new Light( Vector4f( 1.0, 1.0, 1.0, 4.0 ), Vector3f( 2.8f, -100.0f, 5.9f ), Vector3f( -0.4f, 0.0f, -1.0f ), 30.0f, 50.0f );


	gDebug() << "A\n";
	Renderer* renderer = instance->CreateRenderer();
	gDebug() << "B\n";
	renderer->LoadVertexShader( "shaders/basic.vert" );
	renderer->LoadFragmentShader( "shaders/basic.frag" );
	gDebug() << "C\n";
	renderer->AddObject( cube );
	gDebug() << "D\n";
// 	renderer->AddObject( cube2 );
	for ( decltype(scene_objects)::iterator it = scene_objects.begin(); it != scene_objects.end(); ++it ) {
		renderer->AddObject( *it );
	}
	gDebug() << "E\n";
	renderer->Compute();
	gDebug() << "F\n";

	Renderer2D* renderer2d = instance->CreateRenderer2D();
	renderer2d->AssociateSize( window );

	DeferredRenderer* deferredRenderer = instance->CreateDeferredRenderer( window->width(), window->height() );
	deferredRenderer->AssociateSize( window );
// 	deferredRenderer->setAmbientColor( Vector4f( 0.15f, 0.15f, 0.15f, 1.0f ) );
	deferredRenderer->setAmbientColor( Vector4f( 0.5f, 0.5f, 0.5f, 1.0f ) );
	deferredRenderer->AddSunLight( sun_light );

	for ( int i = 0; i < nLights; i++ ) {
// 		deferredRenderer->AddLight( lights[i] );
	}

	deferredRenderer->AddLight( sun_light );
	deferredRenderer->AddLight( lightm1 );
	deferredRenderer->AddLight( light0 );
	deferredRenderer->AddLight( light1 );
	deferredRenderer->AddLight( light2 );
	deferredRenderer->AddLight( light3 );
	deferredRenderer->AddLight( light4 );

// 	SkyRenderer* sky = new SkyRenderer( instance, 100000.0f );
	SkyRenderer* sky = new SkyRenderer( instance, 1378114.0f );
	sky->AssociateSize( window );
	sky->AddLight( sun_light );

	thread->deferredRenderer = deferredRenderer;
	thread->lights = lights;
	thread->Start();

	Scene* scene = new Scene();
	scene->AddRenderer( renderer );

	Camera* camera = new Camera();
	camera->setInertia( 0.999f );
	camera->LookAt( { -6.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } );

	float fps = 0.0f;
	float fps_min = 1.0e34f;
	float fps_max = 0.0f;
	float time = Time::GetSeconds();
	float sun_angle = 0.0f;
	uint32_t img = 0;

// 	exit(0);

	while ( 1 ) {
		input->Update();
		if ( input->pressed( 'Z' ) ) {
			camera->WalkForward( 10.0 );
		}
		if ( input->pressed( 'S' ) ) {
			camera->WalkBackward( 10.0 );
		}
		if ( input->pressed( 'Q' ) ) {
			camera->WalkLeft( 10.0 );
		}
		if ( input->pressed( 'D' ) ) {
			camera->WalkRight( 10.0 );
		}
		if ( input->pressed( ' ' ) ) {
			camera->Translate( { 0.0f, 0.0f, (float)( 10.0 * Time::Delta() ) } );
		}
		if ( input->pressed( 'C' ) ) {
			camera->Translate( { 0.0f, 0.0f, (float)( -10.0 * Time::Delta() ) } );
		}
		if ( input->pressed( 'R' ) ) {
			sun_angle += 0.01f;
			Vector3f pos = sun_light->position();
			pos.x = 10000000.0f * std::cos( sun_angle );
			pos.z = 10000000.0f * std::sin( sun_angle );
			sun_light->setPosition( pos );
		}
		if ( input->pressed( 'T' ) ) {
			sun_angle -= 0.01f;
			Vector3f pos = sun_light->position();
			pos.x = 10000000.0f * std::cos( sun_angle );
			pos.z = 10000000.0f * std::sin( sun_angle );
			sun_light->setPosition( pos );
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

		renderer->projectionMatrix()->Perspective( 60.0f, (float)window->width() / window->height(), 0.01f, 1000.0f );

		deferredRenderer->Bind();
		scene->Draw( camera );
		deferredRenderer->Look( camera );
		deferredRenderer->Render();
		sky->Render( camera );

// 		renderer2d->Draw( 0, 0, font->texture() );
		renderer2d->DrawText( 0, 0, font, 0xFFFFFFFF, "FPS : " + std::to_string( (int)fps ) );
// 		renderer2d->DrawText( 10, 256, font, 0xFFFFFFFF,  L"\x20ac \x21ac \x22ac \x23ac \x24ac 漢 字 汉 字" );

		window->SwapBuffers();
		Time::GlobalSync();

		img++;
		if ( Time::GetSeconds() - time > 0.5f ) {
			fps = img * 1.0f / ( Time::GetSeconds() - time );
			fps_min = std::min( fps_min, fps );
			fps_max = std::max( fps_max, fps );
			time = Time::GetSeconds();
			img = 0;
// 			printf("FPS : %.2f  ( %.2f - %.2f )\n", fps, fps_min, fps_max); fflush(stdout);
// 			printf(" RAM : %lu kB\n", instance->cpuRamCounter() / 1024);
// 			printf("VRAM : %lu kB\n", instance->gpuRamCounter() / 1024);
		}
	}

	return 0;
}
