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

//расстояние между точками (длина вектора)
float getLengthVector(float x[2], float y[2])
{
	return sqrt(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2));
}


int main()
{
	setlocale(LC_ALL, "ru");
	srand(time(NULL));

	//длина/ширина экрана
	int W = GetSystemMetrics(SM_CXSCREEN);
	int H = GetSystemMetrics(SM_CYSCREEN);
	

	const int countLenses = 15; //количество линз
	int count = 1;
	int refractiveIndexLenses[countLenses]; //показатель преломления
	int lensData[15][4] = { int(W / 2), int(H / 2), 0, 1}; //данные о линзах (координаты, угол, ?)

	int lightParam[3] = { 100, int(H / 2), 0 };//параметры источника света, координаты и угол относительно x
	int lightFolow = 0;//флаг следования, чтобы двигать прелметы

	//расстявляю имеющиеся линзы на экране
	for (int i = 0; i < countLenses; i++)
	{
		lensData[i][0] = int(W / 2) + i * 30;
		lensData[i][1] = int(H / 2)+30;
		lensData[i][2] = 0;
		refractiveIndexLenses[i] = 15;
	}

	//источник света (световой меч)
	Image lighter_image;
	lighter_image.loadFromFile("images/light2.png");
	lighter_image.createMaskFromColor(Color(255, 255, 255));
	Texture lighter_t;
	lighter_t.loadFromImage(lighter_image);
	Sprite s_lighter;
	s_lighter.setTexture(lighter_t);
	s_lighter.setTextureRect(IntRect(0, 2, 150, 58));
	s_lighter.setPosition(lightParam[0], lightParam[1]);
	s_lighter.setRotation(lightParam[2]);

	
	//лучи
	Image point_image;
	point_image.loadFromFile("images/light2.png");
	point_image.createMaskFromColor(Color(255, 255, 255, 100));
	Texture point_t;
	point_t.loadFromImage(point_image);

	Sprite point_s[2];
	point_s[0].setTexture(point_t);
	point_s[1].setTexture(point_t);
	point_s[0].setTextureRect(IntRect(0, 0, 1, 1));
	point_s[1].setTextureRect(IntRect(0, 0, 2, 2));
	
	point_s[0].setPosition(0, 0);
	point_s[1].setPosition(0, 0);
	point_s[0].setColor(Color(255, 255, 255, 140));
	point_s[1].setColor(Color(255, 255, 255, 180));
	

	//задаю окно
	RenderWindow window(sf::VideoMode(W, H), "KursProject");
	//задаю экран
	view.reset(sf::FloatRect(0, 0, W, H));
	//задаю позицию окна
	window.setPosition(Vector2i(0, 0));

	Clock clock;

	getplayercoordinateforview(int(W / 2), int(H / 2));
	//коордлинаты указателя
	float mousePlace[2] = { 0, 0 };
	//?
	float optimiz = 8;

   //индекс активной линзы
	int choosenLens = -1;

	//флаг активности источника
	int choosenLight = 0;

	//?
	bool flag1 = false;

	//изображение линз
	Image len_image;
	len_image.create(500, 200, Color(255, 0, 255));
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
		lensData[i][3] = 1; //?
	}

//
//НАЧАЛО ЦИКЛА
//
	while (window.isOpen())
	{
		//для работы без глюков
		float time = clock.getElapsedTime().asMilliseconds();  //дать прошедшее время в микросекундах
		clock.restart(); //перезагружает время


		Vector2i pixelPos = Mouse::getPosition(window); //получение координат мыши
		Vector2f mousePosition = window.mapPixelToCoords(pixelPos); //конвертируем координаты
		Event event;

		//
		//ЦИКЛ СОБЫТИЙ
		//
		while (window.pollEvent(event))
		{
			//закрыть 
			if (event.type == sf::Event::Closed)
				window.close();

			//если нажата клавиша мыши
			if (event.type == Event::MouseButtonPressed)
				if (event.key.code == Mouse::Left) 
				{
					// координата курсора попадает в спрайт источника света
					if (s_lighter.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
					{
						choosenLight = 1;
						lightFolow = 1;  //привязка объекта к мыши
						choosenLens = -1; //разактивация линз, если были активные
						s_lighter.setColor(Color(220, 220, 220)); //выделение светом источника
					}

					//проверяю спрайты линз
					int offsetSprite = 30;
					for (int i = 0; i < count; i++)
						if (mousePosition.x > lensData[i][0] -( offsetSprite) && mousePosition.x < lensData[i][0] + (offsetSprite) && mousePosition.y > lensData[i][1] - (3*offsetSprite) && mousePosition.y < lensData[i][1] + (3*offsetSprite))
						{
							lightFolow = 2 + i;//указываем индекс активного бъекта
							choosenLens = i;
							choosenLight = 0;
						}
		}
			if (event.type == Event::MouseButtonReleased) //кнопка мыши выпущена
				if (event.key.code == Mouse::Left) //а зажата была левая
					lightFolow = 0; //обнуляем выбранный элемент

			//вращение линз
			int direction = 1;

				if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))&& (choosenLens != -1|| choosenLight!=0))
			{

					if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
						direction = -1;
				if (choosenLight != 0)
				{
					lightParam[2] += direction * event.key.code * 5;
					while (lightParam[2] > 360) lightParam[2] -= 360;
					while (lightParam[2] < 0) lightParam[2] += 360;
					s_lighter.setRotation( lightParam[2]);
				}
				if (choosenLens != -1)
				{
					lensData[choosenLens][2] += direction *event.key.code * 5;
					while (lensData[choosenLens][2] > 360) lensData[choosenLens][2] -= 360;
					while (lensData[choosenLens][2] < 0) lensData[choosenLens][2] += 360;
					s_len[choosenLens].setRotation(lensData[choosenLens][2]);
				}

				
			}

				//уменьшаю толщину линзы
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) && choosenLens != -1)
				{
					int secondrefr = refractiveIndexLenses[choosenLens];
					if (Keyboard::isKeyPressed(Keyboard::LControl)) refractiveIndexLenses[choosenLens] -= 6;
					else if (Keyboard::isKeyPressed(Keyboard::LShift)) refractiveIndexLenses[choosenLens] -= 24;
					else refractiveIndexLenses[choosenLens] -= 12;

					float refr = ((refractiveIndexLenses[choosenLens] / 8) + 9 * refractiveIndexLenses[choosenLens] / abs(refractiveIndexLenses[choosenLens]));
					int helpContein = int(sqrt(abs(pow(refr, 2))) / 10000);

					if (abs(int(((helpContein * (1 - 0.05)) - refr * 0.05))) > 250)
						refractiveIndexLenses[choosenLens] = secondrefr;
					else
						lensData[choosenLens][3] = 1;

				}

				//увеличиваю толщину линзы
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) && choosenLens != -1)
				{
					int secondrefr = refractiveIndexLenses[choosenLens];
					if (Keyboard::isKeyPressed(Keyboard::LControl)) refractiveIndexLenses[choosenLens] += 6;
					else if (Keyboard::isKeyPressed(Keyboard::LShift)) refractiveIndexLenses[choosenLens] += 24;
					else refractiveIndexLenses[choosenLens] += 12;
					float refr = ((refractiveIndexLenses[choosenLens] / 8) + 9 * refractiveIndexLenses[choosenLens] / abs(refractiveIndexLenses[choosenLens]));
					int helpContein = int(sqrt(abs(pow(refr, 2))) / 10000);
					if (abs(int(((helpContein * (1 - 0.05)) - refr * 0.05))) > 250)
						refractiveIndexLenses[choosenLens] = secondrefr;
					else
						lensData[choosenLens][3] = 1;
				}

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
				{
					if (countLenses > count)
					{
						count++;
					}
					
				}
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) optimiz = optimiz * 2;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift) && optimiz > 0) optimiz = optimiz / 2;

		}


		if (lightFolow != 0)
		{
			//перемещение источника света
			if (lightFolow == 1)
			{
				lightParam[0] += mousePosition.x - mousePlace[0];
				lightParam[1] += mousePosition.y - mousePlace[1];
				s_lighter.setPosition(lightParam[0], lightParam[1]);
			}
			else
			{//перемещение линз
				lensData[choosenLens][0] += mousePosition.x - mousePlace[0];
				lensData[choosenLens][1] += mousePosition.y - mousePlace[1];
				s_len[choosenLens].setPosition(lensData[choosenLens][0], lensData[choosenLens][1]);
			}
		}
		else
		{
			s_lighter.setColor(Color(255, 255, 255));
			point_s[0].setColor(Color(255, 255, 255, 150));
		}
		//получаем позицию мыши
		mousePlace[0] = mousePosition.x;
		mousePlace[1] = mousePosition.y;


		window.setView(view);
		window.clear();
		window.getSystemHandle();


		if (true)
		{
			int light = -1; //пробегает по линзам
			//перечисляю лучи
			for (int i = 1; i < (56 / optimiz); i++)
			{
				light++;
				bool show = false;

				
				float cosin = cos(lightParam[2] * 0.0175);
				float sinus = sin(lightParam[2] * 0.0175);

				//вычисляю начало координат очередного луча
				float lightcords[2] = { int(lightParam[0] + int(((146 * cosin) - ((((i)*optimiz)) * sinus)))), int(lightParam[1] + int((150 * sinus) + ((((i)*optimiz)) * cosin))) };
				
				point_s[1].setPosition(lightcords[0], lightcords[1]);
				
				float move[2];
				move[0] = cos(lightParam[2] * 0.0175);
				move[1] = sin(lightParam[2] * 0.0175);

				if (light == choosenLight && flag1)
				{
					show = true;
					point_s[1].setColor(Color(255, 255, 255, 180));
				}
				else
					if (flag1) point_s[1].setColor(Color(255, 0, 0, 180));

				//проходим по длинне луча

				while (lightcords[0] > 0 && lightcords[0] < W && lightcords[1] > 0 && lightcords[1] < H)
				{
					bool lensishere = false; //флаг наличия линзы
					int whichlens = 0;//какая линза

					//проходим по линзам
					for (int g = 0; g < count; g++)
					{
						float helpContein[2];

						helpContein[0] = lensData[g][0] - lightcords[0];
						helpContein[1] = lensData[g][1] - lightcords[1];

						cosin = cos((360 - lensData[g][2]) * 0.0175);
						sinus = sin((360 - lensData[g][2]) * 0.0175);

						float arraySC[2];
						arraySC[0] = helpContein[0] * cosin - helpContein[1] * sinus;
						arraySC[1] = helpContein[0] * sinus + helpContein[1] * cosin;

						if (abs(arraySC[0]) <= 1 && abs(arraySC[1]) <= 100)
						{
							if (abs(arraySC[0]) <= 1 && abs(arraySC[1]) > 90)
							{
								lightcords[0] = -100;
								lightcords[1] = -100;
								g = count + 1;
							}
							else
							{
								bool vpered = true;
								if (true)
								{
									float array1[2];
									array1[0] = lightcords[0];
									array1[1] = lightcords[1];
									array1[0] += move[0] * 200;
									array1[1] += move[1] * 200;
									float fosuchelpContein[2];
									float fosuchelpContein1[2];
									cosin = cos((lensData[g][2]) * 0.0175);
									sinus = sin((lensData[g][2]) * 0.0175);
									fosuchelpContein[0] = lensData[g][0] + refractiveIndexLenses[g] * cosin;
									fosuchelpContein[1] = lensData[g][1] + refractiveIndexLenses[g] * sinus;
									fosuchelpContein1[0] = lensData[g][0] - refractiveIndexLenses[g] * cosin;
									fosuchelpContein1[1] = lensData[g][1] - refractiveIndexLenses[g] * sinus;
									if (getLengthVector(array1, fosuchelpContein1) < getLengthVector(array1, fosuchelpContein))
										vpered = false;
								}


								float newmove[2];
								float heremove[2];
								float fuckinghelpContein[2];
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
									arraySC[0] = -arraySC[0];
									arraySC[1] = -arraySC[1];
								}
								fuckinghelpContein[0] = refractiveIndexLenses[g];
								fuckinghelpContein[1] = (refractiveIndexLenses[g] / heremove[0]) * heremove[1];

								if (true)
								{
									float coord[2];
									float coord2[2];
									coord[0] = herelensData[0] - arraySC[0];
									coord[1] = herelensData[1] - arraySC[1];
									coord2[0] = herelensData[0] + fuckinghelpContein[0];
									coord2[1] = herelensData[1] + fuckinghelpContein[1];
									float dist = getLengthVector(coord, coord2);
									newmove[0] = -(coord[0] - coord2[0]) / dist;
									newmove[1] = -(coord[1] - coord2[1]) / dist;
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
									bool helpContein = true;
									while (helpContein)
									{
										if (true)
										{
											float cosin1;
											float sinus1;
											float helpContein1[2];
											helpContein1[0] = lensData[g][0] - lightcords[0];
											helpContein1[1] = lensData[g][1] - lightcords[1];
											cosin1 = cos((360 - lensData[g][2]) * 0.0175);
											sinus1 = sin((360 - lensData[g][2]) * 0.0175);
											float arraySC1[2];
											arraySC1[0] = helpContein1[0] * cosin1 - helpContein1[1] * sinus1;
											arraySC1[1] = helpContein1[0] * sinus1 + helpContein1[1] * cosin1;
											if (abs(arraySC1[0]) <= 1 && abs(arraySC1[1]) <= 100)
												helpContein = true;
											else
												helpContein = false;
										}
										lightcords[0] += move[0];
										lightcords[1] += move[1];
										point_s[1].setPosition(int(lightcords[0]), int(lightcords[1]));
										window.draw(point_s[1]);
									}
								}

								if (show)
								{
									if (g == choosenLens)
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
											int helpContein = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
											if (helpContein >= refr) helpContein = refr - 1;
											for (int u = -(helpContein * (1 - fuck)) - refr * fuck; u < (helpContein * (1 - fuck)) + refr * fuck; u++)
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
											int helpContein = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
											helpContein -= refr;
											helpContein = abs(helpContein);
											if (helpContein >= refr - 2) helpContein = refr - 2;
											if (helpContein <= 1) helpContein = 1;
											for (int u = (-(helpContein * (1 - fuck)) - refr * fuck); u < ((helpContein * (1 - fuck)) + refr * fuck); u++)
											{
												point_s[0].setPosition(herelensData[0] + u, herelensData[1] + i);
												window.draw(point_s[0]);
											}
										}
									}
									point_s[1].setColor(Color(255, 0, 0));
									point_s[1].setScale(5, 5);
									point_s[1].setPosition(int(herelensData[0] - arraySC[0]), int(herelensData[1] - arraySC[1]));
									window.draw(point_s[1]);
									point_s[1].setScale(1, 1);
									point_s[1].setColor(Color(255, 255, 255));
									for (int i = 0; i < 25; i++)
									{
										point_s[1].setPosition(int(herelensData[0] - arraySC[0] + heremove[0] * i), int(herelensData[1] - arraySC[1] + heremove[1] * i));
										window.draw(point_s[1]);
									}

									point_s[1].setColor(Color(0, 255, 255));
									for (int i = 0; i < refractiveIndexLenses[g] / heremove[0]; i++)
									{
										point_s[1].setPosition(int(herelensData[0] + heremove[0] * i), int(herelensData[1] + heremove[1] * i));
										window.draw(point_s[1]);
									}
									point_s[1].setColor(Color(255, 255, 255));
									for (int i = 0; abs(i) < abs(fuckinghelpContein[1]); i++)
									{
										point_s[1].setPosition(int(herelensData[0] + refractiveIndexLenses[g]), int(herelensData[1] + i * fuckinghelpContein[1] / abs(fuckinghelpContein[1])));
										window.draw(point_s[1]);
									}
									for (int i = 0; i < fuckinghelpContein[0]; i++)
									{
										point_s[1].setPosition(int(herelensData[0] + i), int(herelensData[1]));
										window.draw(point_s[1]);
									}
									point_s[1].setColor(Color(0, 0, 255));
									for (int i = 0; i < 90; i++)
									{
										point_s[1].setPosition(int(herelensData[0] - arraySC[0] + newmove[0] * i), int(herelensData[1] - arraySC[1] + newmove[1] * i));
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
					float helpConteint = 1000;
					for (int g = 0; g < count; g++)
					{
						float shelpConteint = sqrt(pow(lightcords[0] - lensData[g][0], 2) + pow(lightcords[1] - lensData[g][1], 2));
						if (helpConteint > shelpConteint) helpConteint = shelpConteint;
					}
					if (helpConteint > 250)
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
			for (int g = 0; g < count; g++)
			{
				if (g == choosenLens)
					s_len[g].setColor(Color(220, 220, 220, 255));
				else
					s_len[g].setColor(Color(192, 246, 254, 255));

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
							int helpContein = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
							if (helpContein >= refr) helpContein = refr - 1;
							int w = int(((helpContein * (1 - fuck)) - refr * fuck) * 2);

							for (int u = (-(helpContein * (1 - fuck)) - refr * fuck) + 250; u < 250 + ((helpContein * (1 - fuck)) + refr * fuck); u++)
							{
								if (shiiit == 1) len_image.setPixel(u, i + 100, Color(85, 150, 170));
								if (shiiit == 3) len_image.setPixel(u, i + 100, Color(150, 215, 235));
							}
							len_t[g].loadFromImage(len_image);
							s_len[g].setTexture(len_t[g]);
							s_len[g].setColor(Color(192, 246, 254, 255));
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
							int helpContein = int(sqrt(abs(pow(refr, 2)) - (i * i * abs(pow(refr, 2))) / 10000));
							helpContein -= refr;
							helpContein = abs(helpContein);
							if (helpContein >= refr - 2) helpContein = refr - 2;
							if (helpContein <= 1) helpContein = 1;
							for (int u = (-(helpContein * (1 - fuck)) - refr * fuck) + 250; u < ((helpContein * (1 - fuck)) + refr * fuck) + 250; u++)
							{
								if (shiiit == 1) len_image.setPixel(u, i + 100, Color(85, 150, 170));
								if (shiiit == 3) len_image.setPixel(u, i + 100, Color(150, 215, 235));
							}
							len_t[g].loadFromImage(len_image);
							s_len[g].setTexture(len_t[g]);
							s_len[g].setColor(Color(192, 246, 254, 255));
							window.draw(s_len[g]);
						}
					}
					lensData[g][3] = 0;
				}
			}
		}

		for (int g = 0; g < count; g++)
			window.draw(s_len[g]);


		window.display();
		
	}
	return 0;
}