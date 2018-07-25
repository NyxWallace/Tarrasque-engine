#include "TE_Engine.h"

// Rotation angle:
float angle = 0.0f;

// Nodes used to render the scene
TE_Node* root_load, *helper, *cube_core;
TE_Node* face_center;
TE_List* scene_list;
TE_Camera* camera;

vector<void(*)()> moves; // list of opposite move made by know (used to solve the cube)
vector<void(*)()> pending_moves; // list of sheduled moves

// Global variables to handle cube animation
TE_VEC3 axis; // vector of the axis of rotation
int k = 0; // face level on the axis (0 or 2)
char face_rotation_axis; // char of the axis (x,y,z)
bool animating = false; // true if the engine is animation
bool solving = false;

// Matrix of the rubik's cube
TE_Node* rubikMatrix[3][3][3] = {};

void displayText(){
	TE_Engine::bi_start(); // start 2D rendering
	TE_Engine::write_string("Commands:", 0.0f, 65.0f);
	TE_Engine::write_string("Rotate top: [q - w]", 0.0f, 50.0f);TE_Engine::write_string("Rotate bottom: [a - s]", 200.0f, 50.0f);TE_Engine::write_string("Rotate front: [y - x]", 400.0f, 50.0f);TE_Engine::write_string("Rotate back: [1 - 2]", 600.0f, 50.0f);TE_Engine::write_string("Rotate left: [Q - A]", 800.0f, 50.0f);TE_Engine::write_string("Rotate right: [W - S]", 1000.0f, 50.0f);
	TE_Engine::write_string("Sky light ON/OFF: [,]", 0.0f, 35.0f);	TE_Engine::write_string("Bulb ON/OFF: [.]", 200.0f, 35.0f);	TE_Engine::write_string("Table lamp ON/OFF: [-]", 400.0f, 35.0f);
	TE_Engine::write_string("Shuffle: [j]", 0.0f, 20.0f); TE_Engine::write_string("Solve: [k]", 200.0f, 20.0f);
	TE_Engine::bi_end(); // end 2D rendering

	// Shadow Rendering
	TE_Engine::depth_function(GL_LEQUAL);

	//Get a pointer to the trasparent plane
	TE_Mesh* plane = dynamic_cast<TE_Mesh*>(root_load->find_node("Plane001"));

	//Get the plane's children
	vector<TE_Node*>* plane_children = new vector<TE_Node*>();
	for (auto& item : *plane->get_children()){
		plane_children->push_back(item);
	}

	// Set the helper's node matrix to the plane trasformation
	TE_Node* tmp = new TE_Node("reflection_helper");
	tmp->set_position(plane->get_position());

	// Link the helper to the root and the plane's children to the helper
	root_load->link(tmp);
	for (int i = 0; i < plane_children->size(); i++){
		tmp->link(plane_children->at(i));
	}

	// Unlink the plane from the graph
	root_load->unlink(plane);

	// Unlink the bulb so that it doesn't cast shadow
	TE_Mesh* bulb = dynamic_cast<TE_Mesh*>(root_load->find_node("Bulb"));
	root_load->unlink(bulb);

	// Save material and change it to black (for shadows)
	TE_Mesh* cube = dynamic_cast<TE_Mesh*>(cube_core);
	TE_Material cube_material = *cube->get_material();
	cube->get_material()->set_material(TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f, nullptr);
	for (auto& item : *cube->get_children()){
		dynamic_cast<TE_Mesh*>(item)->get_material()->set_material(TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f, nullptr);
	}
	TE_Mesh* teapot = dynamic_cast<TE_Mesh*>(root_load->find_node("Teapot001"));
	TE_Material teapot_material = *teapot->get_material();
	teapot->get_material()->set_material(TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f, nullptr);
	TE_Mesh* tablelamp = dynamic_cast<TE_Mesh*>(root_load->find_node("Table_lamp"));
	TE_Material tablelamp_material = *tablelamp->get_material();
	tablelamp->get_material()->set_material(TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), TE_VEC4(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0.0f, nullptr);

	// Get the transform matrix of the root
	TE_MAT4 transformation = root_load->get_position();
	// Scale the root to create fake shadows
	root_load->scale(TE_VEC3(1.0f, 0.0f, 1.0f));
	// Clear the list and pass the root of the scene graph
	scene_list->get_istances()->clear();
	scene_list->pass(root_load);
	// Render the fake shadows
	scene_list->render(te_inverse(TE_Engine::get_camera()->get_position()));
	TE_Engine::depth_function(GL_LESS);
	TE_Engine::enable_lighting();

	// Restore materials after shadows have been rendered
	teapot->get_material()->set_material(&teapot_material);
	tablelamp->get_material()->set_material(&tablelamp_material);
	cube->get_material()->set_material(&cube_material);
	for (auto& item : *cube->get_children()){
		dynamic_cast<TE_Mesh*>(item)->get_material()->set_material(&cube_material);
	}

	// Relink bulb
	root_load->link(bulb);

	//Reset the root's trasformation matrix
	root_load->set_position(transformation);
	// Enable clockwise face culling so the reflections are
	// rendered in the correct way
	TE_Engine::cw_face_culling();

	// Scale the root to generate reflections
	root_load->scale(TE_VEC3(1.0f, -1.0f, 1.0f));

	// Clear the list, pass the root to the list and render the reflections
	scene_list->get_istances()->clear();
	scene_list->pass(root_load);
	scene_list->render(te_inverse(TE_Engine::get_camera()->get_position()));

	// Reflection rendering
	// Enable counter clockwise face culling to render the objects correctly
	TE_Engine::ccw_face_culling();
	// Scale the root again to have the objects in the original position
	root_load->scale(TE_VEC3(1.0f, -1.0f, 1.0f));
	// Relink the plane to the root
	root_load->link(plane);

	// Relink the plane's children to it
	vector<TE_Node*>* tmp_children = new vector<TE_Node*>();
	for (auto& item : *tmp->get_children()){
		tmp_children->push_back(item);
	}

	for (int i = 0; i < tmp_children->size(); i++){
		plane->link(tmp_children->at(i));
	}

	// Unlink the helper node and pass the root to render the scene
	root_load->unlink(tmp);
	scene_list->pass(root_load);

}

// Function to handle the animation of the rotating faces
void rotateFace(){
	if (!animating)
		//Check if there are sheduled moves
		if (pending_moves.size() > 0){
			(*pending_moves[0])();
			pending_moves.erase(pending_moves.begin());
			if (pending_moves.size() == 0 && solving){
				moves.clear();
				solving = false;
			}
		}
	TE_MAT4 inverse = te_inverse(face_center->get_position());
	TE_Node* tmp;
	if (angle > 0.0f){
		if (!animating){ //Link cubes to center of rotation, use helper to compensate over-translation and rotation
			helper->init_mat(); // Reset helper matrix
			helper->set_position(inverse); // Set the matrix to the inverse of the rotating face
			face_center->link(helper); // Link the helper to the rotating face center
			for (int i = 0; i < 3; ++i) // // Link the surrounding cubes to the helper
				for (int j = 0; j < 3; ++j)
					if (i != 1 || j != 1){
						switch (face_rotation_axis){
						case 'z':
							helper->link(rubikMatrix[i][j][k]);
							break;
						case 'x':
							helper->link(rubikMatrix[k][i][j]);
							break;
						case 'y':
							helper->link(rubikMatrix[i][k][j]);
							break;
						default:
							break;
						}
					}
		}
		animating = true;
		face_center->rotate(5.0f, axis); // Rotate the face center
		angle = angle - 5.0f; // Decrease the global angle
		scene_list->get_istances()->clear(); // Clear the list
		scene_list->pass(root_load); // Pass the root to the list
	}
	else{ //Link cubes to the core when animation is over
		if (animating){ // Check to relink only one time after the animation
			animating = false;
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					if (i != 1 || j != 1){
						switch (face_rotation_axis){
						case 'z':
							rubikMatrix[i][j][k]->trasform(te_rotate(90.0f, axis));
							cube_core->link(rubikMatrix[i][j][k]);
							break;
						case 'x':
							rubikMatrix[k][i][j]->trasform(te_rotate(90.0f, axis));
							cube_core->link(rubikMatrix[k][i][j]);
							break;
						case 'y':
							rubikMatrix[i][k][j]->trasform(te_rotate(90.0f, axis));
							cube_core->link(rubikMatrix[i][k][j]);
							break;
						default:
							break;
						}
					}

			//Shift cubes in the matrix
			switch (face_rotation_axis){
			case 'z':
				if (k == 2){
					if (axis.z == 1.0f){ //u
						//shift corners
						tmp = rubikMatrix[0][0][2];
						rubikMatrix[0][0][2] = rubikMatrix[0][2][2];
						rubikMatrix[0][2][2] = rubikMatrix[2][2][2];
						rubikMatrix[2][2][2] = rubikMatrix[2][0][2];
						rubikMatrix[2][0][2] = tmp;

						//Shift edges
						tmp = rubikMatrix[1][0][2];
						rubikMatrix[1][0][2] = rubikMatrix[0][1][2];
						rubikMatrix[0][1][2] = rubikMatrix[1][2][2];
						rubikMatrix[1][2][2] = rubikMatrix[2][1][2];
						rubikMatrix[2][1][2] = tmp;
					}
					else{ //u'
						tmp = rubikMatrix[0][0][2];
						rubikMatrix[0][0][2] = rubikMatrix[2][0][2];
						rubikMatrix[2][0][2] = rubikMatrix[2][2][2];
						rubikMatrix[2][2][2] = rubikMatrix[0][2][2];
						rubikMatrix[0][2][2] = tmp;

						tmp = rubikMatrix[1][0][2];
						rubikMatrix[1][0][2] = rubikMatrix[2][1][2];
						rubikMatrix[2][1][2] = rubikMatrix[1][2][2];
						rubikMatrix[1][2][2] = rubikMatrix[0][1][2];
						rubikMatrix[0][1][2] = tmp;
					}

				}
				else if (k == 0){
					if (axis.z == 1.0f){ //d
						tmp = rubikMatrix[0][0][0];
						rubikMatrix[0][0][0] = rubikMatrix[0][2][0];
						rubikMatrix[0][2][0] = rubikMatrix[2][2][0];
						rubikMatrix[2][2][0] = rubikMatrix[2][0][0];
						rubikMatrix[2][0][0] = tmp;

						tmp = rubikMatrix[1][0][0];
						rubikMatrix[1][0][0] = rubikMatrix[0][1][0];
						rubikMatrix[0][1][0] = rubikMatrix[1][2][0];
						rubikMatrix[1][2][0] = rubikMatrix[2][1][0];
						rubikMatrix[2][1][0] = tmp;
					}
					else{ //d'
						tmp = rubikMatrix[0][0][0];
						rubikMatrix[0][0][0] = rubikMatrix[2][0][0];
						rubikMatrix[2][0][0] = rubikMatrix[2][2][0];
						rubikMatrix[2][2][0] = rubikMatrix[0][2][0];
						rubikMatrix[0][2][0] = tmp;

						tmp = rubikMatrix[1][0][0];
						rubikMatrix[1][0][0] = rubikMatrix[2][1][0];
						rubikMatrix[2][1][0] = rubikMatrix[1][2][0];
						rubikMatrix[1][2][0] = rubikMatrix[0][1][0];
						rubikMatrix[0][1][0] = tmp;

					}
				}
				break;
			case 'x':
				if (k == 2){
					if (axis.x == 1.0f){ //r
						tmp = rubikMatrix[2][0][0];
						rubikMatrix[2][0][0] = rubikMatrix[2][0][2];
						rubikMatrix[2][0][2] = rubikMatrix[2][2][2];
						rubikMatrix[2][2][2] = rubikMatrix[2][2][0];
						rubikMatrix[2][2][0] = tmp;

						tmp = rubikMatrix[2][1][0];
						rubikMatrix[2][1][0] = rubikMatrix[2][0][1];
						rubikMatrix[2][0][1] = rubikMatrix[2][1][2];
						rubikMatrix[2][1][2] = rubikMatrix[2][2][1];
						rubikMatrix[2][2][1] = tmp;
					}
					else{ //r'
						tmp = rubikMatrix[2][0][0];
						rubikMatrix[2][0][0] = rubikMatrix[2][2][0];
						rubikMatrix[2][2][0] = rubikMatrix[2][2][2];
						rubikMatrix[2][2][2] = rubikMatrix[2][0][2];
						rubikMatrix[2][0][2] = tmp;

						tmp = rubikMatrix[2][1][0];
						rubikMatrix[2][1][0] = rubikMatrix[2][2][1];
						rubikMatrix[2][2][1] = rubikMatrix[2][1][2];
						rubikMatrix[2][1][2] = rubikMatrix[2][0][1];
						rubikMatrix[2][0][1] = tmp;
					}
				}
				else if (k == 0){
					if (axis.x == 1.0f){ //l
						tmp = rubikMatrix[0][0][0];
						rubikMatrix[0][0][0] = rubikMatrix[0][0][2];
						rubikMatrix[0][0][2] = rubikMatrix[0][2][2];
						rubikMatrix[0][2][2] = rubikMatrix[0][2][0];
						rubikMatrix[0][2][0] = tmp;

						tmp = rubikMatrix[0][1][0];
						rubikMatrix[0][1][0] = rubikMatrix[0][0][1];
						rubikMatrix[0][0][1] = rubikMatrix[0][1][2];
						rubikMatrix[0][1][2] = rubikMatrix[0][2][1];
						rubikMatrix[0][2][1] = tmp;
					}
					else{ //l'
						tmp = rubikMatrix[0][0][0];
						rubikMatrix[0][0][0] = rubikMatrix[0][2][0];
						rubikMatrix[0][2][0] = rubikMatrix[0][2][2];
						rubikMatrix[0][2][2] = rubikMatrix[0][0][2];
						rubikMatrix[0][0][2] = tmp;

						tmp = rubikMatrix[0][1][0];
						rubikMatrix[0][1][0] = rubikMatrix[0][2][1];
						rubikMatrix[0][2][1] = rubikMatrix[0][1][2];
						rubikMatrix[0][1][2] = rubikMatrix[0][0][1];
						rubikMatrix[0][0][1] = tmp;

					}
				}
				break;
			case 'y':
				if (k == 2){
					if (axis.y == 1.0f){ //b
						tmp = rubikMatrix[0][2][0];
						rubikMatrix[0][2][0] = rubikMatrix[2][2][0];
						rubikMatrix[2][2][0] = rubikMatrix[2][2][2];
						rubikMatrix[2][2][2] = rubikMatrix[0][2][2];
						rubikMatrix[0][2][2] = tmp;

						tmp = rubikMatrix[1][2][0];
						rubikMatrix[1][2][0] = rubikMatrix[2][2][1];
						rubikMatrix[2][2][1] = rubikMatrix[1][2][2];
						rubikMatrix[1][2][2] = rubikMatrix[0][2][1];
						rubikMatrix[0][2][1] = tmp;
					}
					else{ //b
						tmp = rubikMatrix[0][2][0];
						rubikMatrix[0][2][0] = rubikMatrix[0][2][2];
						rubikMatrix[0][2][2] = rubikMatrix[2][2][2];
						rubikMatrix[2][2][2] = rubikMatrix[2][2][0];
						rubikMatrix[2][2][0] = tmp;

						tmp = rubikMatrix[1][2][0];
						rubikMatrix[1][2][0] = rubikMatrix[0][2][1];
						rubikMatrix[0][2][1] = rubikMatrix[1][2][2];
						rubikMatrix[1][2][2] = rubikMatrix[2][2][1];
						rubikMatrix[2][2][1] = tmp;
					}

				}
				else if (k == 0){
					if (axis.y == 1.0f){ //f
						tmp = rubikMatrix[0][0][0];
						rubikMatrix[0][0][0] = rubikMatrix[2][0][0];
						rubikMatrix[2][0][0] = rubikMatrix[2][0][2];
						rubikMatrix[2][0][2] = rubikMatrix[0][0][2];
						rubikMatrix[0][0][2] = tmp;

						tmp = rubikMatrix[1][0][0];
						rubikMatrix[1][0][0] = rubikMatrix[2][0][1];
						rubikMatrix[2][0][1] = rubikMatrix[1][0][2];
						rubikMatrix[1][0][2] = rubikMatrix[0][0][1];
						rubikMatrix[0][0][1] = tmp;
					}
					else{ //f'
						tmp = rubikMatrix[0][0][0];
						rubikMatrix[0][0][0] = rubikMatrix[0][0][2];
						rubikMatrix[0][0][2] = rubikMatrix[2][0][2];
						rubikMatrix[2][0][2] = rubikMatrix[2][0][0];
						rubikMatrix[2][0][0] = tmp;

						tmp = rubikMatrix[1][0][0];
						rubikMatrix[1][0][0] = rubikMatrix[0][0][1];
						rubikMatrix[0][0][1] = rubikMatrix[1][0][2];
						rubikMatrix[1][0][2] = rubikMatrix[2][0][1];
						rubikMatrix[2][0][1] = tmp;

					}

				}
				break;
			default:
				break;
			}
			face_rotation_axis = 'n'; // Set a default rotation axis
			scene_list->get_istances()->clear(); // Clear the list
			scene_list->pass(root_load); // Pass the root to the list
		}
	}
}

void specialCallback(int key, int mouseX, int mouseY)
{
	std::cout << "[key pressed]" << std::endl;

	if (!animating){
		switch (key)
		{
		case GLUT_KEY_RIGHT:
			cube_core->rotate(5.0f, TE_VEC3(0.0f, 0.0f, 1.0f));
			scene_list->get_istances()->clear();
			scene_list->pass(root_load);
			break;
		case GLUT_KEY_LEFT:
			cube_core->rotate(-5.0f, TE_VEC3(0.0f, 0.0f, 1.0f));
			scene_list->get_istances()->clear();
			scene_list->pass(root_load);
			break;
		case GLUT_KEY_UP:
			cube_core->rotate(-5.0f, TE_VEC3(1.0f, 0.0f, 0.0f));
			scene_list->get_istances()->clear();
			scene_list->pass(root_load);
			break;
		case GLUT_KEY_DOWN:
			cube_core->rotate(5.0f, TE_VEC3(1.0f, 0.0f, 0.0f));
			scene_list->get_istances()->clear();
			scene_list->pass(root_load);
			break;
		default:
			break;
		}
	}
}
void rotate_up_cc();
void rotate_up_c(){
	face_center = root_load->find_node("cube112");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	// Set gloabl variables to animate the rotation
	face_rotation_axis = 'z';
	angle = 90.0f;
	k = 2;
	axis = TE_VEC3(0.0f, 0.0f, -1.0f);
	moves.push_back(rotate_up_cc);
}

void rotate_up_cc(){
	face_center = root_load->find_node("cube112");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}

	face_rotation_axis = 'z';
	angle = 90.0f;
	k = 2;
	axis = TE_VEC3(0.0f, 0.0f, 1.0f);
	moves.push_back(rotate_up_c);
}
void rotate_down_cc();
void rotate_down_c(){
	face_center = root_load->find_node("cube110");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	face_rotation_axis = 'z';
	angle = 90.0f;
	k = 0;
	axis = TE_VEC3(0.0f, 0.0f, 1.0f);
	moves.push_back(rotate_down_cc);
}

void rotate_down_cc(){
	face_center = root_load->find_node("cube110");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	face_rotation_axis = 'z';
	angle = 90.0f;
	k = 0;
	axis = TE_VEC3(0.0f, 0.0f, -1.0f);
	moves.push_back(rotate_down_c);
}
void rotate_front_cc();
void rotate_front_c(){
	face_center = root_load->find_node("cube101");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}

	face_rotation_axis = 'y';
	angle = 90.0f;
	k = 0;
	axis = TE_VEC3(0.0f, 1.0f, 0.0f);
	moves.push_back(rotate_front_cc);
}

void rotate_front_cc(){
	face_center = root_load->find_node("cube101");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}

	face_rotation_axis = 'y';
	angle = 90.0f;
	k = 0;
	axis = TE_VEC3(0.0f, -1.0f, 0.0f);
	moves.push_back(rotate_front_c);
}
void rotate_back_cc();
void rotate_back_c(){
	face_center = root_load->find_node("cube121");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}

	face_rotation_axis = 'y';
	angle = 90.0f;
	k = 2;
	axis = TE_VEC3(0.0f, -1.0f, 0.0f);
	moves.push_back(rotate_back_cc);
}

void rotate_back_cc(){
	face_center = root_load->find_node("cube121");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	face_rotation_axis = 'y';
	angle = 90.0f;
	k = 2;
	axis = TE_VEC3(0.0f, 1.0f, 0.0f);
	moves.push_back(rotate_back_c);
}
void rotate_left_cc();
void rotate_left_c(){
	face_center = root_load->find_node("cube011");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	face_rotation_axis = 'x';
	angle = 90.0f;
	k = 0;
	axis = TE_VEC3(1.0f, 0.0f, 0.0f);
	moves.push_back(rotate_left_cc);
}

void rotate_left_cc(){
	face_center = root_load->find_node("cube011");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	face_rotation_axis = 'x';
	angle = 90.0f;
	k = 0;
	axis = TE_VEC3(-1.0f, 0.0f, 0.0f);
	moves.push_back(rotate_left_c);
}
void rotate_right_cc();
void rotate_right_c(){
	face_center = root_load->find_node("cube211");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	face_rotation_axis = 'x';
	angle = 90.0f;
	k = 2;
	axis = TE_VEC3(-1.0f, 0.0f, 0.0f);
	moves.push_back(rotate_right_cc);
}

void rotate_right_cc(){
	face_center = root_load->find_node("cube211");
	if (face_center == nullptr){
		std::cout << "node not found" << std::endl;
		return;
	}
	face_rotation_axis = 'x';
	angle = 90.0f;
	k = 2;
	axis = TE_VEC3(1.0f, 0.0f, 0.0f);
	moves.push_back(rotate_right_c);
}

void shuffle(){
	// Schedules 10 rendom moves
	for (int i = 0; i < 10; ++i){
		int move = rand() % 12;
		switch (move){
		case 0:
			pending_moves.push_back(rotate_up_c);
			break;
		case 1:
			pending_moves.push_back(rotate_up_cc);
			break;
		case 2:
			pending_moves.push_back(rotate_down_c);
			break;
		case 3:
			pending_moves.push_back(rotate_down_cc);
			break;
		case 4:
			pending_moves.push_back(rotate_front_c);
			break;
		case 5:
			pending_moves.push_back(rotate_front_cc);
			break;
		case 6:
			pending_moves.push_back(rotate_back_c);
			break;
		case 7:
			pending_moves.push_back(rotate_back_cc);
			break;
		case 8:
			pending_moves.push_back(rotate_left_c);
			break;
		case 9:
			pending_moves.push_back(rotate_left_cc);
			break;
		case 10:
			pending_moves.push_back(rotate_right_c);
			break;
		case 11:
			pending_moves.push_back(rotate_right_cc);
			break;
		}
	}
}

void solve(){
	// Execute backward the opposite of all move made by now
	for (auto& item : moves){
		pending_moves.insert(pending_moves.begin(), item);
	}
	moves.clear();
	solving = true;
}

void keyboard(unsigned char key, int mouseX, int mouseY)
{
	TE_Light* light;
	TE_Mesh* bulb;
	std::cout << "[key pressed]" << std::endl;
	if (!animating){ // Disable keyboard events when cube is animating a rotation
		switch (key)
		{
		case 'q':
		case 'u':
			rotate_up_c();
			break;
		case 'w':
		case 'U':
			rotate_up_cc();
			break;
		case 's':
		case 'd':
			rotate_down_c();
			break;
		case 'a':
		case 'D':
			rotate_down_cc();
			break;
		case 'x':
		case 'f':
			rotate_front_c();
			break;
		case 'y':
		case 'F':
			rotate_front_cc();
			break;
		case '2':
		case 'b':
			rotate_back_cc();
			break;
		case '1':
		case 'B':
			rotate_back_c();
			break;
		case 'A':
		case 'l':
			rotate_left_c();
			break;
		case 'Q':
		case 'L':
			rotate_left_cc();
			break;
		case 'W':
		case 'r':
			rotate_right_c();
			break;
		case 'S':
		case 'R':
			rotate_right_cc();
			break;
		case 'p':
			TE_Engine::smooth_shading();
			break;
		case 'o':
			TE_Engine::flat_shading();
			break;
		case ',':
			light = dynamic_cast<TE_Light*>(root_load->find_node("Omni002"));
			if (light->is_enabled())
				light->disable();
			else
				light->enable();
			break;
		case '.':
			light = dynamic_cast<TE_Light*>(root_load->find_node("Omni003"));
			if (light->is_enabled()){
				light->disable();
				bulb = dynamic_cast<TE_Mesh*>(root_load->find_node("Bulb"));
				bulb->get_material()->set_emission(TE_VEC4(0.0f, 0.0f, 0.0f, 0.0f));
				bulb->get_material()->set_transparent(0.2f);
			}
			else{
				light->enable();
				bulb = dynamic_cast<TE_Mesh*>(root_load->find_node("Bulb"));
				bulb->get_material()->set_emission(TE_VEC4(1.0f, 1.0f, 0.0f, 0.0f));
				bulb->get_material()->set_transparent(1.0f);
			}
			break;
		case '-':
			light = dynamic_cast<TE_Light*>(root_load->find_node("Spot001"));
			if (light->is_enabled())
				light->disable();
			else
				light->enable();
			break;
		case 'j':
			shuffle();
			break;
		case 'k':
			solve();
			break;
		default:
			break;
		}

		std::cout << "end keyboard callback." << std::endl;
	}
}

int main(int argc, char *argv[]){
	//Initialize engine
	TE_Engine::init();

	//Set rand seed
	srand(time(NULL));

	//Set callbacks
	TE_Engine::set_special_callback(specialCallback);
	TE_Engine::set_keyboard_callback(keyboard);
	TE_Engine::set_idle_callback(rotateFace);
	TE_Engine::set_display_callback(displayText);

	//Load scene
	root_load = TE_Engine::load_scene("Rubik3D.DAE");
	if (root_load == nullptr){
		std::cout << "Wrong file name" << std::endl;
		return 1;
	}

	//Initialize utility nodes
	scene_list = new TE_List();
	camera = new TE_Camera("camera1");
	helper = new TE_Node("helper");
	face_center = new TE_Node("core");
	cube_core = new TE_Node("Cube_core");
	cube_core = root_load->find_node("cube111");

	//Set transparency to transparent objects (assimp won't export opacity)
	TE_Mesh* plane = dynamic_cast<TE_Mesh*>(root_load->find_node("Plane001"));
	plane->get_material()->set_transparent(0.8f);
	TE_Mesh* glass = dynamic_cast<TE_Mesh*>(root_load->find_node("Glass"));
	glass->get_material()->set_transparent(0.9f);
	TE_Mesh* glass2 = dynamic_cast<TE_Mesh*>(root_load->find_node("Glass_floor"));
	glass2->get_material()->set_transparent(0.9f);

	//Fill the rubik matrix with cube nodes using their names as index
	rubikMatrix[1][1][1] = root_load->find_node("cube111");
	for (vector<TE_Node*>::iterator iter = cube_core->get_children()->begin(); iter != cube_core->get_children()->end(); ++iter) {
		//Subtract 48 to the ASCII char value to get the correct integer value
		int x = (*iter)->get_name().at(4) - 48;
		int y = (*iter)->get_name().at(5) - 48;
		int z = (*iter)->get_name().at(6) - 48;
		rubikMatrix[x][y][z] = *iter;
	}

	//Move the scene away from the camera
	root_load->translate(TE_VEC3(0.0f, 0.0f, -260.0f));
	root_load->rotate(30.0f, TE_VEC3(1.0f, 0.0f, 0.0f));

	//Update all position matrix in the list
	scene_list->pass(root_load);

	//Render and start main loop
	TE_Engine::render(camera, scene_list);
	TE_Engine::start();
}