#include <iostream>
#include "Instance.h"
#include "Renderer.h"

int main(int argc, char **argv)
{
	Instance* instance = new Instance( "GammaEngine test", 42 );
	Renderer* renderer = new Renderer( instance );

	return 0;
}
