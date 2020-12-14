/*
 * main.cc
 *
 *  Created on: 14 ����. 2020 �.
 *      Author: ksbesova
 */

#include <iostream>
#include <memory>
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>
#include <SDL2/SDL_image.h>
#include <cmath>
constexpr double Pi = acos(-1.);
constexpr int WIDTH = 1280, HEIGHT = 720; //����

constexpr int rx = 600; //���
constexpr int ry = 300;

constexpr int fenceX = 50;//�����
constexpr int fenceY = 200;
//�����
constexpr int leftBorderW = WIDTH/2-rx/2-50;
constexpr int borderH = HEIGHT/2-60+fenceY;
constexpr int rightBorderW = WIDTH/2+rx/2+50;
std:: random_device rd;
std:: default_random_engine rnd {rd()};
std:: uniform_int_distribution <> pointX {0, leftBorderW};
std:: uniform_int_distribution <> pointXright {rightBorderW, WIDTH-50};
std:: uniform_int_distribution <> pointY {borderH, HEIGHT};
std:: uniform_int_distribution <> len {4, 25};
constexpr int k = 7;//���������� �����
constexpr int sizeArr = 4;
int lenght[] = {len(rnd), len(rnd), len(rnd), len(rnd)};
int xyLeft[] = {pointX(rnd), pointY(rnd),
		pointX(rnd), pointY(rnd),
		pointX(rnd), pointY(rnd),
		pointX(rnd), pointY(rnd),
		pointX(rnd), pointY(rnd),
		pointX(rnd), pointY(rnd),
		pointX(rnd), pointY(rnd)};
int xyRight[] = {pointXright(rnd), pointY(rnd),
		pointXright(rnd), pointY(rnd),
		pointXright(rnd), pointY(rnd),
		pointXright(rnd), pointY(rnd),
		pointXright(rnd), pointY(rnd),
		pointXright(rnd), pointY(rnd),
		pointXright(rnd), pointY(rnd)};

//������
int cloud_w = 820/4, cloud_h = 542/4;
int cloud_x1 = 0, cloud_y1 = 0;
int cloud_dx1 = 2;
int cloud_x2 = 0, cloud_y2 = cloud_h;
int cloud_dx2 = 1;

//����������� �����
int handSq_w = 2187/8, handSq_h = 1524/8;
int handSq_x = WIDTH - handSq_w, handSq_y = HEIGHT - handSq_h;

//���������
int chest_w = 400/2, chest_h = 309/2;
int chest_x = 100, chest_y = HEIGHT - chest_h * 1.65;

//������ ��������
int step = 6;
int space_k = 0;
bool space_f = false;

void DrawCircle (SDL_Renderer * ren, int cx, int cy, int r){
	int x = 0;
	int y = r;
	int delta = 1-2*r;
	int error = 0;
	while (y >= 0){
		SDL_RenderDrawPoint(ren, cx + x, cy + y);
		SDL_RenderDrawPoint(ren, cx + x, cy - y);
		SDL_RenderDrawPoint(ren, cx - x, cy + y);
		SDL_RenderDrawPoint(ren, cx - x, cy - y);
		error = 2 * (delta + y) - 1;
		if ((delta < 0) && (error <= 0))
			delta += 2* ++x +1;
		else if ((delta > 0) && (error > 0))
			delta -= 2* --y + 1;
		else
			delta += 2 * (++x - --y);
	}
}

int main(int, char **){

	std::cout << "������ ������ ���������" <<std::endl;

	SDL_Init(SDL_INIT_EVERYTHING);
auto win = std::shared_ptr<SDL_Window>(
	SDL_CreateWindow("��� ����� :)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN), SDL_DestroyWindow);

	if (win == nullptr){
		std::cerr <<"������ ��� �������� ����: " <<
				SDL_GetError() << std::endl;
		return 1;
	}

	auto ren = std::shared_ptr <SDL_Renderer>(
			SDL_CreateRenderer(win.get(), -1,
					SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC), SDL_DestroyRenderer);
	if (ren == nullptr){
		std::cerr << "������ ��� �������� �������: " <<
				SDL_GetError() << std::endl;
		return 1;
	}


	//������
	auto cloud_icon = std::shared_ptr <SDL_Texture>(IMG_LoadTexture(ren.get(), "cloud.png"), SDL_DestroyTexture);
	if (cloud_icon == nullptr){
		std::cerr << "�� ���� ��������� ��������: " <<
				SDL_GetError() <<std::endl;
		return 1;
	}
	//����� � ������
	auto girlSq_icon = std::shared_ptr <SDL_Texture>
			(IMG_LoadTexture(ren.get(), "girlSq.png"), SDL_DestroyTexture);
	if (girlSq_icon == nullptr){
			std::cerr << "�� ���� ��������� ��������: " <<
					SDL_GetError() <<std::endl;
			return 1;
		}

	//����������� �����
	auto handSq_icon = std::shared_ptr <SDL_Texture>
		(IMG_LoadTexture(ren.get(), "handSq.png"), SDL_DestroyTexture);
	if (handSq_icon == nullptr){
		std::cerr << "�� ���� ��������� ��������: " <<
				SDL_GetError() <<std::endl;
		return 1;
	}

	//���������
	auto chest1_icon = std::shared_ptr <SDL_Texture>
		(IMG_LoadTexture(ren.get(), "closeChest.png"), SDL_DestroyTexture);
	if (chest1_icon == nullptr){
			std::cerr << "�� ���� ��������� ��������: " <<
					SDL_GetError() <<std::endl;
			return 1;
		}
	auto chest2_icon = std::shared_ptr <SDL_Texture>
			(IMG_LoadTexture(ren.get(), "openChest.png"), SDL_DestroyTexture);
		if (chest2_icon == nullptr){
				std::cerr << "�� ���� ��������� ��������: " <<
						SDL_GetError() <<std::endl;
				return 1;
			}


	//�����
	auto keys =SDL_GetKeyboardState(nullptr);

	SDL_Event event;
	bool finish = false;

	while (not finish){
		while (SDL_PollEvent(&event)){
		if (event.type == SDL_QUIT)
			finish = true;
		}

		//������
		//��� ����
		SDL_SetRenderDrawColor(ren.get(), 120, 196, 255, 255);
		//��������� �������� - ��������������
		SDL_RenderClear(ren.get());//��������� �����������

		//��� �����

		SDL_Rect grass { 0, 330, WIDTH, HEIGHT-330};
		SDL_SetRenderDrawColor(ren.get(), 30, 240, 150, 255);
		SDL_RenderFillRect(ren.get(), &grass);

		//�����

		SDL_SetRenderDrawColor(ren.get(), 143, 120, 86, 255);
		SDL_RenderDrawLine (ren.get(), 0, HEIGHT/2-40, WIDTH, HEIGHT/2-40);
		SDL_RenderDrawLine (ren.get(), 0, HEIGHT/2+fenceY/2, WIDTH, HEIGHT/2+fenceY/2);
		for (int i = 0; i < WIDTH; i += fenceX+10){
			SDL_Rect fence { i, HEIGHT/2-70, fenceX, fenceY};
			SDL_SetRenderDrawColor(ren.get(), 230, 193, 138, 255);
			SDL_RenderFillRect(ren.get(), &fence);
		}

		//���
		constexpr int moveP = -50; //����� �����
		//�������
		SDL_Rect r { WIDTH/2-rx/2, HEIGHT/2+moveP, rx, ry};
		SDL_SetRenderDrawColor(ren.get(), 168, 150, 150, 255);
		SDL_RenderFillRect(ren.get(), &r);


		//���� �����
		constexpr int gap = rx / 5;
		constexpr int winX = rx / 5;
		constexpr int winY = ry * .75;
		SDL_Rect winLeft { WIDTH/2-gap - winX, HEIGHT/2+moveP+gap/2, winX, winY};
		SDL_SetRenderDrawColor(ren.get(), 186, 255, 249, 255);
		SDL_RenderFillRect(ren.get(), &winLeft);
		//���� ������
		SDL_Rect winRight { WIDTH/2+gap, HEIGHT/2+moveP+gap/2, winX, winY};
		SDL_RenderFillRect(ren.get(), &winRight);
		SDL_SetRenderDrawColor(ren.get(), 0, 0, 0, 255);
		//����� ������
		constexpr int bigRoof = rx * 0.55;
		SDL_SetRenderDrawColor(ren.get(), 168, 129, 110, 255);
				for (int i = 0; i< bigRoof/2; i++){
					SDL_RenderDrawLine(ren.get(), WIDTH/2-2*i, moveP+HEIGHT/2-bigRoof/2+i, WIDTH/2+2*i, moveP+HEIGHT/2-bigRoof/2+i);
				}
		//���� �� �����
		SDL_SetRenderDrawColor(ren.get(), 186, 255, 249, 255);
		constexpr int clockRad = 50;
		for (int i = clockRad; i>0; i--)
			DrawCircle(ren.get(), WIDTH/2, HEIGHT/2+moveP - bigRoof /4, i);
		SDL_SetRenderDrawColor(ren.get(), 0, 0, 0, 255);
		SDL_RenderDrawLine(ren.get(),WIDTH/2, HEIGHT/2+moveP - bigRoof /4 - clockRad, WIDTH/2, HEIGHT/2+moveP - bigRoof /4 + clockRad);
		SDL_RenderDrawLine(ren.get(),WIDTH/2 - clockRad, HEIGHT/2+moveP - bigRoof /4, WIDTH/2+clockRad, HEIGHT/2+moveP - bigRoof /4);

		//�����
		SDL_SetRenderDrawColor(ren.get(), 64, 92, 52, 255);

		constexpr int space = 10;
		int x;
		int xRight;

		for (int i = 0; i < k*2;i += 2){
			x = xyLeft[i];
			xRight = xyRight[i];
			for (int j = 0; j < sizeArr; j++){
				SDL_RenderDrawLine(ren.get(), x, xyLeft[i+1], x+space/2, xyLeft[i+1]-lenght[j]);
				SDL_RenderDrawLine(ren.get(), x+space/2, xyLeft[i+1]-lenght[j], x+space, xyLeft[i+1]);

				SDL_RenderDrawLine(ren.get(), xRight, xyRight[i+1], xRight+space/2, xyRight[i+1]-lenght[j]);
				SDL_RenderDrawLine(ren.get(), xRight+space/2, xyRight[i+1]-lenght[j], xRight+space, xyRight[i+1]);
				x += space;
				xRight += space;

			}
		}

		//����� � ������
		constexpr int gateRad = 80;
		if(handSq_x >= WIDTH/2 - gateRad and handSq_x <= WIDTH/2+gateRad+gap/2){
			//������
			SDL_SetRenderDrawColor(ren.get(), 250, 250, 240, 255);
			SDL_Rect gate { WIDTH/2 - gateRad, HEIGHT/2-moveP, gateRad*2, ry /2 +ry / 6};
			SDL_RenderFillRect(ren.get(), &gate);

			SDL_RenderCopy(ren.get(), girlSq_icon.get(),
					nullptr, &gate);
		}
		else{
			//������
			SDL_SetRenderDrawColor(ren.get(), 168, 129, 110, 255);
			SDL_Rect gate { WIDTH/2 - gateRad, HEIGHT/2-moveP, gateRad*2, ry /2 +ry / 6};
			SDL_RenderFillRect(ren.get(), &gate);
			//����� � �������
			SDL_SetRenderDrawColor(ren.get(), 0, 0, 0, 50);
			constexpr int handRad = 5;
			for (int i = handRad; i>0; i--)
				DrawCircle(ren.get(), WIDTH/2 - gateRad+0.3*gateRad, HEIGHT/2+moveP+ry/2+ry/4, i);
		}

		SDL_SetRenderDrawColor(ren.get(), 0, 0, 0, 50);
		//�������������� ����� �� �����
		SDL_RenderDrawLine(ren.get(), WIDTH/2-gap - winX, HEIGHT/2+moveP+gap,
				WIDTH/2-gap, HEIGHT/2+moveP+gap);
		SDL_RenderDrawLine(ren.get(), WIDTH/2+gap, HEIGHT/2+moveP+gap,
						WIDTH/2+gap+winX, HEIGHT/2+moveP+gap);
		//������������ ����� �� �����
		SDL_RenderDrawLine(ren.get(), WIDTH/2-gap/2 - winX, HEIGHT/2+moveP+gap/2,
				WIDTH/2-gap/2 - winX, HEIGHT/2+moveP+gap/2+winY);
		SDL_RenderDrawLine(ren.get(), WIDTH/2+gap + winX/2, HEIGHT/2+moveP+gap/2,
				WIDTH/2+gap + winX/2, HEIGHT/2+moveP+gap/2+winY);


		//���������
			if (handSq_x <= chest_x+chest_w){
				SDL_Rect chest2_rect{chest_x, chest_y,
						chest_w, chest_h};
				SDL_RenderCopy(ren.get(), chest2_icon.get(),
						nullptr, &chest2_rect);
			}
			else{
				SDL_Rect chest1_rect{chest_x, chest_y,
							chest_w, chest_h};
				SDL_RenderCopy(ren.get(), chest1_icon.get(),
							nullptr, &chest1_rect);
			}
		//����� �����������
		int pace = 8;
		int leftB = leftBorderW-handSq_w * .75;
		int rightB = rightBorderW - handSq_w * .45;
		int upB = HEIGHT/2+ry * .25;
		int fenceB = HEIGHT/2;
		if ((keys[SDL_SCANCODE_RIGHT] and ((handSq_x <= WIDTH - handSq_w and handSq_x >= rightB)
				or handSq_x <= leftB-pace
				or (handSq_x <= WIDTH - handSq_w and handSq_y >= upB)))){//������ ������� ������
			handSq_x += pace;
		}
		if (keys[SDL_SCANCODE_LEFT]and ((handSq_x >= 0 and handSq_x <= leftB)
				or handSq_x >= rightB+pace
				or (handSq_x >= 0 and handSq_y >= upB))){//������ ������� ������
			handSq_x -= pace;
		}
		if (keys[SDL_SCANCODE_UP] and (handSq_y >= upB+pace
				or (handSq_x <= leftB and handSq_y >= fenceB)
				or (handSq_x >= rightB and handSq_y >= fenceB))){//������ ������� ������
			handSq_y -= pace;
		}
		if (keys[SDL_SCANCODE_DOWN] and
				(handSq_y <= HEIGHT-handSq_h*.66)){//������ ������� ������
			handSq_y += pace;
		}

		if (keys[SDL_SCANCODE_SPACE]){
			space_f = true;
		}

		if (space_f and space_k != step){
			handSq_y -= pace*5;
			space_k++;
		}
		else if (space_k > 0){
			handSq_y += pace*5;
			space_k--;
			space_f = false;
		}



		SDL_Rect handSq_rect{handSq_x, handSq_y,
			handSq_w, handSq_h};
				SDL_RenderCopy(ren.get(), handSq_icon.get(),
					nullptr, &handSq_rect);

		//������//
		cloud_x1 += cloud_dx1;
		if(cloud_x1 >= WIDTH - cloud_w or cloud_x1 < 0)
			cloud_dx1 = -cloud_dx1;

		cloud_x2 += cloud_dx2;
		if(cloud_x2 >= WIDTH - cloud_w or cloud_x2 < 0)
			cloud_dx2 = -cloud_dx2;

		SDL_Rect cloud1_rect{cloud_x1, cloud_y1, cloud_w, cloud_h};
		SDL_RenderCopy(ren.get(), cloud_icon.get(),
			nullptr, &cloud1_rect);
		SDL_Rect cloud2_rect{cloud_x2, cloud_y2, cloud_w, cloud_h};
		SDL_RenderCopy(ren.get(), cloud_icon.get(),
						nullptr, &cloud2_rect);

		SDL_RenderPresent(ren.get());// ����� �����������

	}

	std::cout << "����� ������ ���������" << std::endl;

	return 0;
}


