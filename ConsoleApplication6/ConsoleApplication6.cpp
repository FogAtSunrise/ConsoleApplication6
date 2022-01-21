#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <Windows.h>
#include <time.h>
#include <string>
#include "view.h"
#include <math.h>

using namespace sf;
using namespace std;

//расстояние между точками (дляна вектора)
float getLengthVector(float x[2], float y[2])
{
	return sqrt(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2));
}

//получить угол по его синусу и косинусу
int getAngle(float sx, float sy)
{
	//вытащили синус из основного тригонометрического тождества
	float result = sqrt(sx * sx + sy * sy);
	float cosin = abs(sx / result);
	float sinus = abs(sy / result);

	int angle = 0;
	int fourth = 0;
//определяю в какой четверти прямая
	if (sx >= 0 && sy >= 0) fourth = 0; else
	if (sx <= 0 && sy >= 0) fourth = 1; else
	if (sx <= 0 && sy <= 0) fourth = 2; else
	if (sx >= 0 && sy <= 0) fourth = 3; else

		//преобразовать радианы в градусы
	angle = asin(sinus) * 57.29577951308232;

	angle = angle + 90 * fourth;
	//если граничное
	if (sx == 0 && sy > 0) angle = 90;
	if (sx == 0 && sy < 0) angle = 270;
	if (sy == 0 && sx > 0) angle = 0;
	if (sy == 0 && sx < 0) angle = 180;

	if (sx == 0 && sy == 0) return 0; 
	else return angle;
}

int main()
{
	setlocale(LC_ALL, "ru");
	srand(time(NULL));

	//длина/ширина экрана
	int W = GetSystemMetrics(SM_CXSCREEN);
	int H = GetSystemMetrics(SM_CYSCREEN);

	float stuffy = -126;// ?
	float culldown = 5;// ?
	const int countLenses = 1; //количество линз

	int refractiveIndexLenses[countLenses]; //толщина линз, от которой зависит преломляющая способность 
	int lensData[countLenses][4] = { int(W / 2), int(H / 2), 0, 20 }; //данные о линзах (координаты, угол, )
	int lightershit[3] = { 100, int(H / 2), 0 };//параметры источника света, координаты и угол относительно x

	int folowing = 0;
	int mousechange[2];

	//расстявляю имеющиеся линзы на экране
	for (int i = 0; i < countLenses; i++)
	{
		lensData[i][0] = int(W / 2) + i * 30;
		lensData[i][1] = int(H / 2);
		lensData[i][2] = 0;
		refractiveIndexLenses[i] = 15;
	}

	Font font;
	font.loadFromFile("font.ttf");
	Text text("", font, 24);
	text.setFillColor(Color(120, 120, 120));
	text.setStyle(sf::Text::Bold);

	Image exit_image;
	exit_image.loadFromFile("images/exitimage.png");
	exit_image.createMaskFromColor(Color(255, 255, 255));
	Texture exit_t;
	exit_t.loadFromImage(exit_image);
	Sprite s_exit;
	s_exit.setTexture(exit_t);

	Image lighter_image;
	lighter_image.loadFromFile("images/stuff.png");
	lighter_image.createMaskFromColor(Color(255, 255, 255));
	Texture lighter_t;
	lighter_t.loadFromImage(lighter_image);
	Sprite s_lighter;
	s_lighter.setTexture(lighter_t);
	s_lighter.setTextureRect(IntRect(0, 64, 128, 56));
	s_lighter.setPosition(lightershit[0], lightershit[1]);
	s_lighter.setRotation(lightershit[2]);


	Image bg_image;
	//bg_image.loadFromFile("images/bg.png");
	bg_image.createMaskFromColor(Color(255, 255, 255));
	Texture bg_t;
	bg_t.loadFromImage(bg_image);
	Sprite s_bg;
	s_bg.setTexture(bg_t);
	s_bg.setScale((float(W) / float(1366)), (float(H) / float(768)));
	s_bg.setPosition(0, 0);

	Image smallbg_image;
	smallbg_image.loadFromFile("images/stuff.png");
	smallbg_image.createMaskFromColor(Color(255, 255, 255));
	Texture smallbg_t;
	smallbg_t.loadFromImage(smallbg_image);
	Sprite smalls_bg;
	smalls_bg.setTexture(smallbg_t);
	smalls_bg.setTextureRect(IntRect(8, 8, 42, 14));
	smalls_bg.setScale(3, 3);

	Image point_image;
	point_image.loadFromFile("images/stuff.png");
	point_image.createMaskFromColor(Color(255, 255, 255));
	Texture point_t;
	point_t.loadFromImage(point_image);
	Sprite point_s[2];
	point_s[0].setTexture(point_t);
	point_s[1].setTexture(point_t);
	point_s[0].setTextureRect(IntRect(128, 0, 1, 1));
	point_s[1].setTextureRect(IntRect(134, 0, 2, 2));
	point_s[0].setPosition(0, 0);
	point_s[1].setPosition(0, 0);
	point_s[0].setColor(Color(255, 255, 255, 140));
	point_s[1].setColor(Color(255, 255, 255, 180));

	Image stuff_image;
	stuff_image.loadFromFile("images/stuff.png");
	stuff_image.createMaskFromColor(Color(255, 255, 255));
	Texture stuff_t;
	stuff_t.loadFromImage(stuff_image);
	const int qofstuff = 4;
	Sprite s_stuff[4];
	for (int i = 0; i < qofstuff; i++)
	{
		s_stuff[i].setTexture(stuff_t);
		if (i % 2 == 0) s_stuff[i].setTextureRect(IntRect(0, 0, 64, 64));
		else s_stuff[i].setTextureRect(IntRect(64, 0, 64, 64));
	}

	//Координаты кнопок
	if (true)
	{
		s_stuff[0].setPosition(W - 128, H - 64);
		s_stuff[1].setPosition(W - 64, H - 64);
		s_stuff[2].setPosition(W - 256, H - 64);
		s_stuff[3].setPosition(W - 192, H - 64);
	}

	RenderWindow window(sf::VideoMode(W, H), "Optics", Style::Fullscreen);
	view.reset(sf::FloatRect(0, 0, W, H));
	window.setPosition(Vector2i(0, 0));
	Clock clock;
	getplayercoordinateforview(int(W / 2), int(H / 2));

	float mouseshit[2] = { 0, 0 };
	float optimiz = 8;

	int choosenlens = -1;

	int choosenlight = 0;
	bool shiiiiiiiiiit = false;

	Image len_image;
	len_image.create(500, 200, Color(255, 255, 255));
	len_image.createMaskFromColor(Color(255, 255, 255));
	Texture len_t[countLenses];
	Sprite s_len[countLenses];
	for (int i = 0; i < countLenses; i++)
	{
		len_t[i].loadFromImage(len_image);
		s_len[i].setTexture(len_t[i]);
		s_len[i].setRotation(lensData[i][2]);
		s_len[i].setPosition(lensData[i][0], lensData[i][1]);
		s_len[i].setOrigin(250, 100);
		lensData[i][3] = 1;
	}

	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asMilliseconds();
		clock.restart();
		if (culldown < 5) culldown += time * 0.05;
		if (culldown >= 5)
		{
			for (int i = 0; i < qofstuff; i++)
				s_stuff[i].setColor(Color(255, 255, 255));
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			s_exit.setColor(Color(200, 200, 200));
			window.draw(s_exit);
			window.display();
			Sleep(300);
			window.close();
		}
		if (Keyboard::isKeyPressed(Keyboard::Q) && shiiiiiiiiiit)
		{
			choosenlight--; Sleep(100);
		}
		if (Keyboard::isKeyPressed(Keyboard::W) && shiiiiiiiiiit)
		{
			choosenlight++; Sleep(100);
		}

		Vector2i pixelPos = Mouse::getPosition(window);
		Vector2f pos = window.mapPixelToCoords(pixelPos);
		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == Event::MouseButtonPressed)//если нажата клавиша мыши
				if (event.key.code == Mouse::Left) {//а именно левая
					if (s_exit.getGlobalBounds().contains(pos.x, pos.y))//и при этом координата курсора попадает в спрайт
					{
						s_exit.setColor(Color::Green);
						window.draw(s_exit);
						window.display();
						Sleep(300);
						window.close();
					}
					if (s_lighter.getGlobalBounds().contains(pos.x, pos.y))//и при этом координата курсора попадает в спрайт
					{
						folowing = 1; choosenlens = -1;
						s_lighter.setColor(Color(220, 220, 220));
					}
					int shitfuck = 30;
					for (int i = 0; i < countLenses; i++)
						if (pos.x > lensData[i][0] - shitfuck && pos.x < lensData[i][0] + shitfuck && pos.y > lensData[i][1] - shitfuck && pos.y < lensData[i][1] + shitfuck)//и при этом координата курсора попадает в спрайт
						{
							folowing = 2 + i;
							choosenlens = i;
						}
					if (true)
					{
						int pressedbut = -1;
						for (int i = 0; i < qofstuff; i++)
							if (s_stuff[i].getGlobalBounds().contains(pos.x, pos.y))
								pressedbut = i;

						if (pressedbut == 0 && choosenlens != -1)
						{
							int secondrefr = refractiveIndexLenses[choosenlens];
							if (Keyboard::isKeyPressed(Keyboard::LControl)) refractiveIndexLenses[choosenlens] -= 6;
							else if (Keyboard::isKeyPressed(Keyboard::LShift)) refractiveIndexLenses[choosenlens] -= 24;
							else refractiveIndexLenses[choosenlens] -= 12;

							float refr = ((refractiveIndexLenses[choosenlens] / 8) + 9 * refractiveIndexLenses[choosenlens] / abs(refractiveIndexLenses[choosenlens]));
							int shit = int(sqrt(abs(pow(refr, 2))) / 10000);

							if (abs(int(((shit * (1 - 0.05)) - refr * 0.05))) > 250)
								refractiveIndexLenses[choosenlens] = secondrefr;
							else
								lensData[choosenlens][3] = 1;
						}
						if (pressedbut == 1 && choosenlens != -1)
						{
							int secondrefr = refractiveIndexLenses[choosenlens];
							if (Keyboard::isKeyPressed(Keyboard::LControl)) refractiveIndexLenses[choosenlens] += 6;
							else if (Keyboard::isKeyPressed(Keyboard::LShift)) refractiveIndexLenses[choosenlens] += 24;
							else refractiveIndexLenses[choosenlens] += 12;
							float refr = ((refractiveIndexLenses[choosenlens] / 8) + 9 * refractiveIndexLenses[choosenlens] / abs(refractiveIndexLenses[choosenlens]));
							int shit = int(sqrt(abs(pow(refr, 2))) / 10000);
							if (abs(int(((shit * (1 - 0.05)) - refr * 0.05))) > 250)
								refractiveIndexLenses[choosenlens] = secondrefr;
							else
								lensData[choosenlens][3] = 1;
						}
						if (pressedbut == 2) optimiz = optimiz * 2;
						if (pressedbut == 3 && optimiz > 0) optimiz = optimiz / 2;

						if (pressedbut != -1)
						{
							culldown = 0;
							s_stuff[pressedbut].setColor(Color(200, 200, 200));
						}
					}
				}
			if (event.type == Event::MouseButtonReleased)
				if (event.key.code == Mouse::Left)
					folowing = 0;

			if (event.type == Event::MouseWheelMoved)
			{
				if (folowing == 1)
				{
					lightershit[2] += event.key.code * 5;
					while (lightershit[2] > 360) lightershit[2] -= 360;
					while (lightershit[2] < 0) lightershit[2] += 360;
					s_lighter.setRotation(lightershit[2]);
				}
				if (folowing > 1)
				{
					lensData[folowing - 2][2] += event.key.code * 5;
					while (lensData[folowing - 2][2] > 360) lensData[folowing - 2][2] -= 360;
					while (lensData[folowing - 2][2] < 0) lensData[folowing - 2][2] += 360;
					s_len[folowing - 2].setRotation(lensData[folowing - 2][2]);
				}
			}
		}

		if (folowing != 0)
		{
			if (folowing == 1)
			{
				lightershit[0] += pos.x - mouseshit[0];
				lightershit[1] += pos.y - mouseshit[1];
				s_lighter.setPosition(lightershit[0], lightershit[1]);
			}
			else
			{
				lensData[choosenlens][0] += pos.x - mouseshit[0];
				lensData[choosenlens][1] += pos.y - mouseshit[1];
				s_len[choosenlens].setPosition(lensData[choosenlens][0], lensData[choosenlens][1]);
			}
		}
		else
		{
			s_lighter.setColor(Color(255, 255, 255));
			point_s[0].setColor(Color(255, 255, 255, 150));
		}

		mouseshit[0] = pos.x;
		mouseshit[1] = pos.y;

		if (pos.y > 128)
		{
			if (stuffy > -126) stuffy -= 0.5 * time;
		}
		else
			if (stuffy < 0) stuffy += 0.5 * time;
		if (stuffy > 0) stuffy = 0;
		if (stuffy < -126) stuffy = -126;

		window.setView(view);
		window.clear();
		window.getSystemHandle();

		window.draw(s_bg);

		if (true)
		{
			int light = -1;
			//128 / 56
			for (int i = 1; i < (54 / optimiz); i++)
			{
				light++;
				bool show = false;
				float cosin = cos(lightershit[2] * 0.0175);
				float sinus = sin(lightershit[2] * 0.0175);
				float lightcords[2] = { int(lightershit[0] + int(((124 * cosin) - ((((i)*optimiz)) * sinus)))), int(lightershit[1] + int((128 * sinus) + ((((i)*optimiz)) * cosin))) };
				point_s[1].setPosition(lightcords[0], lightcords[1]);
				float move[2];
				move[0] = cos(lightershit[2] * 0.0175);
				move[1] = sin(lightershit[2] * 0.0175);
				if (light == choosenlight && shiiiiiiiiiit)
				{
					show = true;
					point_s[1].setColor(Color(255, 255, 255, 180));
				}
				else
					if (shiiiiiiiiiit) point_s[1].setColor(Color(255, 0, 0, 180));

				while (lightcords[0] > 0 && lightcords[0] < W && lightcords[1] > 0 && lightcords[1] < H)
				{
					bool lensishere = false;
					int whichlens = 0;
					for (int g = 0; g < countLenses; g++)
					{
						float shit[2];
						shit[0] = lensData[g][0] - lightcords[0];
						shit[1] = lensData[g][1] - lightcords[1];
						cosin = cos((360 - lensData[g][2]) * 0.0175);
						sinus = sin((360 - lensData[g][2]) * 0.0175);
						float shhhit[2];
						shhhit[0] = shit[0] * cosin - shit[1] * sinus;
						shhhit[1] = shit[0] * sinus + shit[1] * cosin;
						if (abs(shhhit[0]) <= 1 && abs(shhhit[1]) <= 100)
						{
							if (abs(shhhit[0]) <= 1 && abs(shhhit[1]) > 90)
							{
								lightcords[0] = -100;
								lightcords[1] = -100;
								g = countLenses + 1;
							}
							else
							{
								bool vpered = true;
								if (true)
								{
									float shiiiit[2];
									shiiiit[0] = lightcords[0];
									shiiiit[1] = lightcords[1];
									shiiiit[0] += move[0] * 200;
									shiiiit[1] += move[1] * 200;
									float fosucshit[2];
									float fosucshit1[2];
									cosin = cos((lensData[g][2]) * 0.0175);
									sinus = sin((lensData[g][2]) * 0.0175);
									fosucshit[0] = lensData[g][0] + refractiveIndexLenses[g] * cosin;
									fosucshit[1] = lensData[g][1] + refractiveIndexLenses[g] * sinus;
									fosucshit1[0] = lensData[g][0] - refractiveIndexLenses[g] * cosin;
									fosucshit1[1] = lensData[g][1] - refractiveIndexLenses[g] * sinus;
									if (getLengthVector(shiiiit, fosucshit1) < getLengthVector(shiiiit, fosucshit))
										vpered = false;
								}


								float newmove[2];
								float heremove[2];
								float fuckingshit[2];
								float herelensData[3];
								herelensData[0] = 100;
								herelensData[1] = 200;
								herelensData[2] = 0;
								cosin = cos(-lensData[g][2] * 0.0175);
								sinus = sin(-lensData[g][2] * 0.0175);
								heremove[0] = move[0] * cosin - move[1] * sinus;
								heremove[1] = move[0] * sinus + move[1] * cosin;
								if (heremove[0] < 0)
								{
									heremove[0] = -heremove[0];
									heremove[1] = -heremove[1];
									shhhit[0] = -shhhit[0];
									shhhit[1] = -shhhit[1];
								}
								fuckingshit[0] = refractiveIndexLenses[g];
								fuckingshit[1] = (refractiveIndexLenses[g] / heremove[0]) * heremove[1];

								if (true)
								{
									float fuckit[2];
									float fuckit2[2];
									fuckit[0] = herelensData[0] - shhhit[0];
									fuckit[1] = herelensData[1] - shhhit[1];
									fuckit2[0] = herelensData[0] + fuckingshit[0];
									fuckit2[1] = herelensData[1] + fuckingshit[1];
									float dist = getLengthVector(fuckit, fuckit2);
									newmove[0] = -(fuckit[0] - fuckit2[0]) / dist;
									newmove[1] = -(fuckit[1] - fuckit2[1]) / dist;
								}

								if (refractiveIndexLenses[g] < 0)
								{
									newmove[0] = -newmove[0];
									newmove[1] = -newmove[1];
								}


								cosin = cos(lensData[g][2] * 0.0175);
								sinus = sin(lensData[g][2] * 0.0175);
								move[0] = newmove[0] * cosin - newmove[1] * sinus;
								move[1] = newmove[0] * sinus + newmove[1] * cosin;

								if (!vpered)
								{
									move[0] = -move[0];
									move[1] = -move[1];
								}

								if (refractiveIndexLenses[g] < 0)
								{
									move[0] = -move[0];
									move[1] = -move[1];
								}

								if (true)
								{
									bool shit = true;
									while (shit)
									{
										if (true)
										{
											float cosin1;
											float sinus1;
											float shit1[2];
											shit1[0] = lensData[g][0] - lightcords[0];
											shit1[1] = lensData[g][1] - lightcords[1];
											cosin1 = cos((360 - lensData[g][2]) * 0.0175);
											sinus1 = sin((360 - lensData[g][2]) * 0.0175);
											float shhhit1[2];
											shhhit1[0] = shit1[0] * cosin1 - shit1[1] * sinus1;
											shhhit1[1] = shit1[0] * sinus1 + shit1[1] * cosin1;
											if (abs(shhhit1[0]) <= 1 && abs(shhhit1[1]) <= 100)
												shit = true;
											else
												shit = false;
										}
										lightcords[0] += move[0];
										lightcords[1] += move[1];
										point_s[1].setPosition(int(lightcords[0]), int(lightcords[1]));
										window.draw(point_s[1]);
									}
								}

								if (show)
								{
									if (g == choosenlens)
										point_s[0].setColor(Color(220, 220, 220, 100));
									else
										point_s[0].setColor(Color(255, 255, 255, 100));
									float fuck = 0.05;
									float refr = ((refractiveIndexLenses[g] / 8) + 9 * refractiveIndexLenses[g] / abs(refractiveIndexLenses[g]));
									float cosin = cos(herelensData[2] * 0.0175);
									float sinus = sin(herelensData[2] * 0.0175);
									if (refractiveIndexLenses[g] > 0)
										for (int i = -100; i < 101; i += 2)
										{
											int shit = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
											if (shit >= refr) shit = refr - 1;
											for (int u = -(shit * (1 - fuck)) - refr * fuck; u < (shit * (1 - fuck)) + refr * fuck; u++)
											{
												point_s[0].setPosition(herelensData[0] + u, herelensData[1] + i);
												window.draw(point_s[0]);
											}
										}
									else
									{
										refr = abs(refr);
										for (int i = -100; i < 101; i += 2)
										{
											int shit = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
											shit -= refr;
											shit = abs(shit);
											if (shit >= refr - 2) shit = refr - 2;
											if (shit <= 1) shit = 1;
											for (int u = (-(shit * (1 - fuck)) - refr * fuck); u < ((shit * (1 - fuck)) + refr * fuck); u++)
											{
												point_s[0].setPosition(herelensData[0] + u, herelensData[1] + i);
												window.draw(point_s[0]);
											}
										}
									}
									point_s[1].setColor(Color(255, 0, 0));
									point_s[1].setScale(5, 5);
									point_s[1].setPosition(int(herelensData[0] - shhhit[0]), int(herelensData[1] - shhhit[1]));
									window.draw(point_s[1]);
									point_s[1].setScale(1, 1);
									point_s[1].setColor(Color(255, 255, 255));
									for (int i = 0; i < 25; i++)
									{
										point_s[1].setPosition(int(herelensData[0] - shhhit[0] + heremove[0] * i), int(herelensData[1] - shhhit[1] + heremove[1] * i));
										window.draw(point_s[1]);
									}

									point_s[1].setColor(Color(0, 255, 255));
									for (int i = 0; i < refractiveIndexLenses[g] / heremove[0]; i++)
									{
										point_s[1].setPosition(int(herelensData[0] + heremove[0] * i), int(herelensData[1] + heremove[1] * i));
										window.draw(point_s[1]);
									}
									point_s[1].setColor(Color(255, 255, 255));
									for (int i = 0; abs(i) < abs(fuckingshit[1]); i++)
									{
										point_s[1].setPosition(int(herelensData[0] + refractiveIndexLenses[g]), int(herelensData[1] + i * fuckingshit[1] / abs(fuckingshit[1])));
										window.draw(point_s[1]);
									}
									for (int i = 0; i < fuckingshit[0]; i++)
									{
										point_s[1].setPosition(int(herelensData[0] + i), int(herelensData[1]));
										window.draw(point_s[1]);
									}
									point_s[1].setColor(Color(0, 0, 255));
									for (int i = 0; i < 90; i++)
									{
										point_s[1].setPosition(int(herelensData[0] - shhhit[0] + newmove[0] * i), int(herelensData[1] - shhhit[1] + newmove[1] * i));
										window.draw(point_s[1]);
									}
									point_s[1].setColor(Color(255, 255, 255));
								}
							}
						}
					}

					float pastlightcords[2];
					pastlightcords[0] = lightcords[0];
					pastlightcords[1] = lightcords[1];
					float shitt = 1000;
					for (int g = 0; g < countLenses; g++)
					{
						float sshitt = sqrt(pow(lightcords[0] - lensData[g][0], 2) + pow(lightcords[1] - lensData[g][1], 2));
						if (shitt > sshitt) shitt = sshitt;
					}
					if (shitt > 250)
					{
						for (int g = 0; g < 65; g++)
						{
							lightcords[0] += move[0] * 2;
							lightcords[1] += move[1] * 2;
							point_s[1].setPosition(int(lightcords[0]), int(lightcords[1]));
							window.draw(point_s[1]);
						}
						lightcords[0] += move[0] * 2;
						lightcords[1] += move[1] * 2;
					}
					else
					{
						lightcords[0] += move[0] * 2;
						lightcords[1] += move[1] * 2;
					}
					point_s[1].setPosition(int(lightcords[0]), int(lightcords[1]));
					window.draw(point_s[1]);
				}
			}
		}

		window.draw(s_lighter);

		if (true)
		{
			for (int g = 0; g < countLenses; g++)
			{
				if (g == choosenlens)
					s_len[g].setColor(Color(220, 220, 220, 180));
				else
					s_len[g].setColor(Color(255, 255, 255, 180));

				if (lensData[g][3] == 1)
				{
					for (int i = 0; i < 200; i++)
						for (int u = 0; u < 500; u++)
							len_image.setPixel(u, i, Color(255, 255, 255));
					len_image.createMaskFromColor(Color(255, 255, 255));
					int shiiit = 0;
					float fuck = 0.05;
					float refr = ((refractiveIndexLenses[g] / 8) + 9 * refractiveIndexLenses[g] / abs(refractiveIndexLenses[g]));
					if (refractiveIndexLenses[g] > 0)
						for (int i = -100; i < 100; i += 1)
						{
							if (abs(i) > 90)
								shiiit = 1;
							else
								shiiit = 3;
							int shit = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
							if (shit >= refr) shit = refr - 1;
							int w = int(((shit * (1 - fuck)) - refr * fuck) * 2);

							for (int u = (-(shit * (1 - fuck)) - refr * fuck) + 250; u < 250 + ((shit * (1 - fuck)) + refr * fuck); u++)
							{
								if (shiiit == 1) len_image.setPixel(u, i + 100, Color(85, 150, 170));
								if (shiiit == 3) len_image.setPixel(u, i + 100, Color(150, 215, 235));
							}
							len_t[g].loadFromImage(len_image);
							s_len[g].setTexture(len_t[g]);
							s_len[g].setColor(Color(255, 255, 255, 180));
							window.draw(s_len[g]);
						}
					else
					{
						refr = abs(refr);

						for (int i = -100; i < 100; i += 1)
						{
							if (abs(i) > 90)
								shiiit = 1;
							else
								shiiit = 3;
							int shit = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
							shit -= refr;
							shit = abs(shit);
							if (shit >= refr - 2) shit = refr - 2;
							if (shit <= 1) shit = 1;
							for (int u = (-(shit * (1 - fuck)) - refr * fuck) + 250; u < ((shit * (1 - fuck)) + refr * fuck) + 250; u++)
							{
								if (shiiit == 1) len_image.setPixel(u, i + 100, Color(85, 150, 170));
								if (shiiit == 3) len_image.setPixel(u, i + 100, Color(150, 215, 235));
							}
							len_t[g].loadFromImage(len_image);
							s_len[g].setTexture(len_t[g]);
							s_len[g].setColor(Color(255, 255, 255, 180));
							window.draw(s_len[g]);
						}
					}
					lensData[g][3] = 0;
				}
			}
		}

		for (int g = 0; g < countLenses; g++)
			window.draw(s_len[g]);

		for (int i = 0; i < qofstuff; i++) window.draw(s_stuff[i]);

		smalls_bg.setPosition(getviewx() - int(W / 2) + int(W / 455), getviewy() - int(H / 2));
		window.draw(smalls_bg);

		std::ostringstream thing;
		thing << int((2000 / time) / 2);
		thing << " fps";
		text.setString(thing.str());//задаем строку тексту и вызываем сформированную выше строку методом .str() 
		text.setPosition(getviewx() - int(W / 2) + 21, getviewy() - int(H / 2) + 6);//задаем позицию текста, отступая от центра камеры
		window.draw(text);

		s_exit.setPosition(getviewx() + int(W / 2) - 126, getviewy() - int(H / 2) + stuffy);
		window.draw(s_exit);
		window.display();
		// 1 к 9
	}
	return 0;
}