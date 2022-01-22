#pragma once
#include <SFML/Graphics.hpp>
int viewX, viewY;
sf::View view;

void getplayercoordinateforview(float x, float y) { 
	viewX = int(x); viewY = int(y);
	view.setCenter(viewX, viewY); 
}
void setposview(float x, float y)
{
	viewX = int(x); viewY = int(y);
	view.setCenter(viewX, viewY);
}
int getviewx() { return(viewX); }
int getviewy() { return(viewY); }
