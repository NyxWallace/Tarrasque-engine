#include "TE_Engine.h"
#include "LeapListener.h"
#include "Ball.h"
#include "Block.h"
#include <cstdlib>

TE_Node* root_load;
TE_List* scene_list;
float barPosition = 0;

TE_Node* bar;
TE_Camera* camera;
Ball* ball;
Block *first[7];
Block *second[6];
Block *third[7];

void displayCallback(){
	//Move bar with leap motion
	TE_MAT4 mat = bar->get_position();
	mat[3][0] = barPosition;
	bar->set_position(mat);

	//Move ball
	ball->Update(barPosition);

	if (ball->isLost())
		ball = new Ball(root_load->find_node("Ball"));

	TE_VEC2 vec;
	vec[0] = ball->get_position()[3][0];
	vec[1] = ball->get_position()[3][1];

	if (vec[1] > 106 && vec[1] < 114)
	{
		for (int i = 0; i < 7; ++i)
		{
			if (!first[i]->removed)
			{
				switch (first[i]->isHit(vec, root_load)){
				case 1:
					ball->Direction[1] *= -1;
					break;
				case 2:
					ball->Direction[0] *= -1;
					break;
				}
			}
		}
	}

	if (vec[1] > 116 && vec[1] < 124)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (!second[i]->removed)
			{
				switch (second[i]->isHit(vec, root_load)){
				case 1:
					ball->Direction[1] *= -1;
					break;
				case 2:
					ball->Direction[0] *= -1;
					break;
				}
			}
		}
	}

	if (vec[1] > 126 && vec[1] < 134)
	{
		for (int i = 0; i < 7; ++i)
		{
			if (!third[i]->removed)
			{
				switch (third[i]->isHit(vec, root_load)){
				case 1:
					ball->Direction[1] *= -1;
					break;
				case 2:
					ball->Direction[0] *= -1;
					break;
				}
			}
		}
	}

	bool isVictory = true;
	for (int i = 0; i < 7; ++i)
		isVictory &= first[i]->removed;
	for (int i = 0; i < 6; ++i)
		isVictory &= second[i]->removed;
	for (int i = 0; i < 7; ++i)
		isVictory &= third[i]->removed;
	if (isVictory)
		exit(42);

	scene_list->get_istances()->clear(); // Clear the list
	scene_list->pass(root_load); // Pass the root to the list
}

void specialCallback(int key, int mouseX, int mouseY)
{
	std::cout << "[key pressed]" << std::endl;
	TE_Engine* engine = TE_Engine::get_istance();

	switch (key)
	{
	case GLUT_KEY_RIGHT:
		root_load->rotate(5.0f, TE_VEC3(0.0f, 0.0f, 1.0f));
		scene_list->get_istances()->clear();
		//Update all position matrix in the list
		scene_list->pass(root_load);
		break;
	case GLUT_KEY_LEFT:
		root_load->rotate(-5.0f, TE_VEC3(0.0f, 0.0f, 1.0f));
		scene_list->get_istances()->clear();
		//Update all position matrix in the list
		scene_list->pass(root_load);
		break;
	case GLUT_KEY_UP:
		//ball->Direction[0] *= -1;
		scene_list->get_istances()->clear();
		//Update all position matrix in the list
		scene_list->pass(root_load);
		engine->set_eye_separation(engine->get_eye_separation() +0.3f);
		//std::cout << "eye separation: " << engine->get_eye_separation() << std::endl;
		break;
	case GLUT_KEY_DOWN:
		//ball->Direction[1] *= -1;
		scene_list->get_istances()->clear();
		//Update all position matrix in the list
		scene_list->pass(root_load);
		engine->set_eye_separation(engine->get_eye_separation() - 0.3f);
		//std::cout << "eye separation: " << engine->get_eye_separation() << std::endl;
		break;
	default:
		break;
	}
}

void keyboard(unsigned char key, int mouseX, int mouseY)										   
{
	std::cout << "[key pressed]" << std::endl;
	TE_Engine* engine = TE_Engine::get_istance();

	switch (key){
	case 'k':
		engine->toggle_stereo_rendering();
		break;
	case '+':
		engine->set_convergence(engine->get_convergence() + 0.3f);
		std::cout << "convergence: " << engine->get_convergence() << std::endl;
		break;
	case '-':
		engine->set_convergence(engine->get_convergence() - 0.3f);
		std::cout << "convergence: " << engine->get_convergence() << std::endl;
		break;
	case '9':
		engine->set_far_plane(engine->get_far_plane() + 0.3f);
		engine->set_fbo_perspective(glm::perspective(glm::radians(engine->get_fov()), (float)engine->m_width / (float)engine->m_height, engine->get_near_plane(), engine->get_far_plane()));
		std::cout << "far plane: " << engine->get_far_plane() << std::endl;
		break;
	case '8':
		engine->set_far_plane(engine->get_far_plane() - 0.3f);
		engine->set_fbo_perspective(glm::perspective(glm::radians(engine->get_fov()), (float)engine->m_width / (float)engine->m_height, engine->get_near_plane(), engine->get_far_plane()));
		std::cout << "far plane: " << engine->get_far_plane() << std::endl;
		break;
	case 'w':
		camera->translate(TE_VEC3(0.0f, 0.0, -0.3f));
		break;
	case 's':
		camera->translate(TE_VEC3(0.0f, 0.0, 0.3f));
		break;
	case 'a':
		camera->rotate(10.0f, TE_VEC3(0.0f, 1.0f, 0.0f));
		break;
	case 'd':
		camera->rotate(-10.0f, TE_VEC3(0.0f, 1.0f, 0.0f));
		break;
	case 'q':
		camera->rotate(10.0f, TE_VEC3(1.0f, 0.0f, 0.0f));
		break;
	case 'e':
		camera->rotate(-10.0f, TE_VEC3(1.0f, 0.0f, 0.0f));
		break;

	}
}

int main(int argc, char *argv[]){
	//Initialize random
	srand(time(NULL));

	//Initialize engine
	TE_Engine::init();

	//Start LeapMotion controller      *https://developer.leapmotion.com/documentation/cpp/devguide/Sample_Tutorial.html*
	LeapListener listener(&barPosition);
	Controller controller;

	controller.addListener(listener);

	//Set callbacks
	TE_Engine::set_special_callback(specialCallback);
	TE_Engine::set_keyboard_callback(keyboard);
	TE_Engine::set_display_callback(displayCallback);

	//Load scene
	root_load = TE_Engine::load_scene("../DAE/Arkanoid.DAE");
	if (root_load == nullptr){
		std::cout << "Wrong file name" << std::endl;
		return 1;
	}
	
	//Initialize utility nodes
	scene_list = new TE_List();
	camera = new TE_Camera("camera1");
	bar = root_load->find_node("Bar");
	ball = new Ball(root_load->find_node("Ball"));

	for (int i = 1; i <= 7; ++i)
	{
		std::stringstream s;
		s << "Box" << 1 << i;
		std::string str = s.str();
		first[i-1] = new Block(root_load->find_node(str));
	}

	for (int i = 1; i <= 6; ++i)
	{
		std::stringstream s;
		s << "Box" << 2 << i;
		std::string str = s.str();
		second[i-1] = new Block(root_load->find_node(str));
	}
	for (int i = 1; i <= 7; ++i)
	{
		std::stringstream s;
		s << "Box" << 3 << i;
		std::string str = s.str();
		third[i-1] = new Block(root_load->find_node(str));
	}
	//Move the scene away from the camera
	root_load->translate(TE_VEC3(0.0f, 0.0f, -130.0f));
	root_load->translate(TE_VEC3(0.0f, -30.0f,.0f));
	root_load->rotate(-75.0f, TE_VEC3(1.0f, 0.0f, 0.0f));

	//Update all position matrix in the list
	scene_list->pass(root_load);

	//Render and start main loop
	//camera->set_position(root_load->get_position());
	TE_Engine::render(camera, scene_list);

	//Wait for Leap controller to go on-line
	int count = 1;
	while (!controller.isConnected()){
		std::cout << "Warning: Leap controller not connected, wait for connection" << std::endl;
		count++;
		Sleep(500);
		if (count == 5){
			std::cout << "Error: Leap Motion not found, starting Engine without motion caption" << std::endl;
			break;
		}
	}

	listener.setEngineStarted(true);
	TE_Engine::start();

	controller.removeListener(listener);

	return 0;
}