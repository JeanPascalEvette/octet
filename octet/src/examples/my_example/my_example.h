////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "tinyxml2.h"
 
namespace octet {
  /// Scene containing a box with octet.
  class my_example : public app {
	  collada_builder loader;

	  // Variables used for the mobile camera
	  mouse_look mouse_look_helper;
	  helper_fps_controller fps_helper;
	  ref<camera_instance> the_camera;
	  ref<scene_node> player_node;
	  enum CameraType {
		  TOPDOWN,
		  SIDEWAYS,
		  OBLIQUE,
		  MOBILE
	  };

	  ref<scene_node> whiteBall;
	  ref<scene_node> rayDrag;
	  std::vector<ref<scene_node>> redBalls;
	  std::vector<ref<scene_node>> myHoles;
	  float holesRadius;
	  int currentLevel;

	  int xMousePos, yMousePos;
	  int currentNode, currentNodePostInit;
	  time_t resetTime;

	  int attemptLimit;

	  struct my_vertex {
		  vec3p pos;
		  uint32_t color;
	  };
	  

    // scene for drawing box
    ref<visual_scene> app_scene; 
	// this function converts three floats into a RGBA 8 bit color
	static uint32_t make_color(float r, float g, float b) {
		return 0xff000000 + ((int)(r*255.0f) << 0) + ((int)(g*255.0f) << 8) + ((int)(b*255.0f) << 16);
	}
  public:
    /// this is called when we construct the class before everything is initialised.
    my_example(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
		enable_cursor();
		currentLevel = 0;
	  currentNode = 0;
	  resetTime = 0;
      app_scene =  new visual_scene();
	  holesRadius = 1.4f;
	  xMousePos = -999, yMousePos = -999;
	  mat4t mat;
      app_scene->create_default_camera_and_lights();

	  // Setup camera - make sure it is ready if mobile mode is enabled
	  mouse_look_helper.init(this, 200.0f / 360.0f, false);
	  fps_helper.init(this);
	  the_camera = app_scene->get_camera_instance(0);
	  the_camera->set_far_plane(10000);
	  the_camera->get_node()->translate(vec3(0, 0, -35)); // Have to move the camera for it to be centered
	  mesh_instance *mi;


	  // Set the camera depending on camera type
	  CameraType CameraPosition = CameraType::TOPDOWN;
	  if (CameraPosition == CameraType::TOPDOWN)
	  {
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 60, 0));
		  app_scene->get_camera_instance(0)->get_node()->rotate(-90, vec3(1, 0, 0));
	  }
	  else if (CameraPosition == CameraType::SIDEWAYS)
	  {
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 50));
	  }
	  else if (CameraPosition == CameraType::OBLIQUE)
	  {
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 50));
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 30, 0));
		  app_scene->get_camera_instance(0)->get_node()->rotate(-45, vec3(1, 0, 0));
	  }
	  else if (CameraPosition == CameraType::MOBILE)
	  {
		  float player_height = 1.83f;
		  float player_radius = 0.0f;
		  float player_mass = 90.0f;

		  mat.loadIdentity();
		  mat.translate(0, player_height*0.5f, 15);

		   mi = app_scene->add_shape(
			  mat,
			  new mesh_sphere(vec3(0), player_radius),
			  new material(vec4(0, 0, 1, 1)),
			  true, player_mass,
			  new btCapsuleShape(0.25f, player_height)
			  );
		   player_node = mi->get_node();
		   currentNode++;
	  }


	  // Generate materials
	  material *green = new material(vec4(0, 1, 0, 1));
	  material *darkgreen = new material(vec4(0, 0.5f, 0, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
      
	  // Generate Ground
	  mat.loadIdentity();
	  mat.translate(0, -1.5f, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(100.0f, 1.0f, 100.0f)), blue, false, 999.0f);
	  // Generate field
	  mat.loadIdentity();
	  mat.translate(0, -1, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(16.0f, 1.0f, 20.0f)), green, false, 999.0f);

	  // Generate 4 Walls of pool table
	  mat.loadIdentity();
	  mat.translate(15, -1, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(1.0f, 3.5f, 21.0f)), darkgreen, false, 999.0f);
	  mat.loadIdentity();
	  mat.translate(-15, -1, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(1.0f, 3.5f, 21.0f)), darkgreen, false, 999.0f);
	  mat.loadIdentity();
	  mat.translate(0, -1, -20);
	  app_scene->add_shape(mat, new mesh_box(vec3(16.0f, 3.5f, 1.0f)), darkgreen, false, 999.0f);
	  mat.loadIdentity();
	  mat.translate(0, -1, 20);
	  app_scene->add_shape(mat, new mesh_box(vec3(16.0f, 3.5f, 1.0f)), darkgreen, false, 999.0f);


	  // Create scene_nodes corresponding to the meshes created
	  scene_node *ground = app_scene->get_mesh_instance(currentNode++)->get_node();
	  scene_node *field = app_scene->get_mesh_instance(currentNode++)->get_node();
	  scene_node *wall = app_scene->get_mesh_instance(currentNode++)->get_node();
	  scene_node *wall2 = app_scene->get_mesh_instance(currentNode++)->get_node();
	  scene_node *wall3 = app_scene->get_mesh_instance(currentNode++)->get_node();
	  scene_node *wall4 = app_scene->get_mesh_instance(currentNode++)->get_node();


	  // Generate the 6 holes of the pool table
	  myHoles = std::vector<ref<scene_node>>();
	  generateHole(vec3(13.5f, 3.5f, 0.0f), holesRadius);
	  generateHole(vec3(-13.5f, 3.5f, 0.0f), holesRadius);
	  generateHole(vec3(13.0f, 3.5f, 18.0f), holesRadius);
	  generateHole(vec3(13.0f, 3.5f, -18.0f), holesRadius);
	  generateHole(vec3(-13.0f, 3.5f, 18.0f), holesRadius);
	  generateHole(vec3(-13.0f, 3.5f, -18.0f), holesRadius);




	  // Also set some friction and restitution values. Those need to be modified to make it more realistic
	  


	  
	  

	  field->set_resitution(1.0f);
	 // field->set_friction(1.0f);
	  field->set_rolling_friction(1.0f);
	  wall->set_resitution(1.0f);
	  wall2->set_resitution(1.0f);
	  wall3->set_resitution(1.0f);
	  wall4->set_resitution(1.0f);

	  currentNodePostInit = currentNode;
	  loadDataFromFile();

    }


	void loadDataFromFile()
	{
		currentNode = currentNodePostInit;
		mat4t mat;
		material *red = new material(vec4(1, 0, 0, 1));
		material *white = new material(vec4(1, 1, 1, 1));
		char levelName[30];
		strcpy(levelName, "Level");
		strcat(levelName, std::to_string(currentLevel).c_str());
		strcat(levelName, ".xml");

		tinyxml2::XMLDocument doc;	
		doc.LoadFile(levelName);
		attemptLimit = atof(doc.FirstChildElement("Data")->FirstChildElement("AttemptLimit")->GetText());;
		float whiteBallLocX = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Location")->FirstChildElement("x")->GetText());
		float whiteBallLocY = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Location")->FirstChildElement("y")->GetText());
		float whiteBallLocZ = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Location")->FirstChildElement("z")->GetText());

		float whiteBallVelX = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Velocity")->FirstChildElement("x")->GetText());
		float whiteBallVelY = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Velocity")->FirstChildElement("y")->GetText());
		float whiteBallVelZ = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Velocity")->FirstChildElement("z")->GetText());

		// Generate white ball (This will be modified to position the ball based on some input data later)
		mat.loadIdentity();
		mat.translate(whiteBallLocX, whiteBallLocY, whiteBallLocZ);
		app_scene->add_shape(mat, new mesh_sphere(vec3(0), 1), white, true, 10.0f);
		whiteBall = app_scene->get_mesh_instance(currentNode++)->get_node();
		whiteBall->set_linear_velocity(vec3(whiteBallVelX, whiteBallVelY, whiteBallVelZ));
		//whiteBall->set_friction(0.5f);
		whiteBall->set_rolling_friction(0.5f);
		whiteBall->set_resitution(0.75f);
		whiteBall->activateForever();

		redBalls = std::vector<ref<scene_node>>();
		tinyxml2::XMLNode * el = doc.FirstChildElement("Data")->FirstChildElement("ListOfRedBall")->FirstChildElement();
		while (el != nullptr)
		{
			float redBallLocX = atof(el->FirstChildElement("Location")->FirstChildElement("x")->GetText());
			float redBallLocY = atof(el->FirstChildElement("Location")->FirstChildElement("y")->GetText());
			float redBallLocZ = atof(el->FirstChildElement("Location")->FirstChildElement("z")->GetText());

			float redBallVelX = atof(el->FirstChildElement("Velocity")->FirstChildElement("x")->GetText());
			float redBallVelY = atof(el->FirstChildElement("Velocity")->FirstChildElement("y")->GetText());
			float redBallVelZ = atof(el->FirstChildElement("Velocity")->FirstChildElement("z")->GetText());
			mat.loadIdentity();
			mat.translate(redBallLocX, redBallLocY, redBallLocZ);
			app_scene->add_shape(mat, new mesh_sphere(vec3(0), 1), red, true, 10.0f);
			scene_node *redBall = app_scene->get_mesh_instance(currentNode++)->get_node();
			redBall->set_linear_velocity(vec3(redBallVelX, redBallVelY, redBallVelZ));
			//redBall->set_friction(0.5f);
			redBall->set_rolling_friction(0.5f);
			redBall->set_resitution(0.75f);
			redBalls.push_back(redBall);
			el = el->NextSibling();

		}

		//whiteBall->set_linear_velocity(vec3(24, 0, 14));
	}

	/// This function generates a black disc to be used as a hole. It is based on the helix object created on example_geometry
	void generateHole(vec3 position, float radius)
	{
		// use a shader that just outputs the color_ attribute.
		param_shader *shader = new param_shader("shaders/default.vs", "shaders/hole.fs");

		material *black = new material(vec4(0, 0, 0, 1), shader);
		float val = radius * 2;
		black->add_uniform(&val, atom_height, GL_FLOAT, 1, param::stage_fragment);
		black->add_uniform(&position, atom_pos, GL_FLOAT_VEC3, 1, param::stage_fragment);


		mesh *hole = new mesh();

		// number of steps in helix
		size_t num_steps = 160;

		// allocate vertices and indices into OpenGL buffers
		size_t num_vertices = num_steps * 2 + 2;
		size_t num_indices = num_steps * 6;
		hole->allocate(sizeof(my_vertex) * num_vertices, sizeof(uint32_t) * num_indices);
		hole->set_params(sizeof(my_vertex), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);

		// describe the structure of my_vertex to OpenGL
		hole->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
		hole->add_attribute(attribute_color, 4, GL_UNSIGNED_BYTE, 12, GL_TRUE);


		{
			// these write-only locks give access to the vertices and indices.
			// they will be released at the next } (the end of the scope)
			gl_resource::wolock vl(hole->get_vertices());
			my_vertex *vtx = (my_vertex *)vl.u8();
			gl_resource::wolock il(hole->get_indices());
			uint32_t *idx = il.u32();

			// make the vertices
			for (size_t i = 0; i != num_steps + 1; ++i) {
				float r = 0, g = 0, b = 0;
				//float r = 0.0f, g = 1.0f * i / num_steps, b = 1.0f;
				//float y = i * (height / num_steps) - height * 0.5f;
				float angle = i * (2.0f * 3.14159265f / num_steps);
				vtx->pos = vec3p(0, 0, 0);
				vtx->color = make_color(r, g, b);
				log("%f %f %f %08x\n", r, g, b, vtx->color);
				vtx++;
				vtx->pos = vec3p(cosf(angle) * radius, 0, sinf(angle) * radius);
				vtx->color = make_color(r, g, b);
				vtx++;
			}
			// make the triangles
			uint32_t vn = 0;
			for (size_t i = 0; i != num_steps; ++i) {
				// 0--2
				// | \|
				// 1--3
				idx[0] = vn + 0;
				idx[1] = vn + 3;
				idx[2] = vn + 1;
				idx += 3;

				idx[0] = vn + 0;
				idx[1] = vn + 2;
				idx[2] = vn + 3;
				idx += 3;

				vn += 2;
			}
		}


		//Add the hole to the app_scene
		scene_node *node = new scene_node();
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, hole, black));

		//Move the hole to the required location, then add it to the list of holes.
		node->translate(position);
		myHoles.push_back(node);
		currentNode++;
	}



	/// This function generates a black disc to be used as a hole. It is based on the helix object created on example_geometry
	void generateRay(vec3 position1, vec3 position2, float width)
	{
		// use a shader that just outputs the color_ attribute.
		param_shader *shader = new param_shader("shaders/default.vs", "shaders/simple_color.fs");
		material *black = new material(vec4(0, 0, 0, 1), shader);


		mesh *ray = new mesh();

		// number of steps in helix
		size_t num_steps = 160;

		// allocate vertices and indices into OpenGL buffers
		size_t num_vertices = num_steps * 2 + 2;
		size_t num_indices = num_steps * 6;
		ray->allocate(sizeof(my_vertex) * num_vertices, sizeof(uint32_t) * num_indices);
		ray->set_params(sizeof(my_vertex), num_indices, num_vertices, GL_TRIANGLES, GL_UNSIGNED_INT);

		// describe the structure of my_vertex to OpenGL
		ray->add_attribute(attribute_pos, 3, GL_FLOAT, 0);
		ray->add_attribute(attribute_color, 4, GL_UNSIGNED_BYTE, 12, GL_TRUE);


		{
			// these write-only locks give access to the vertices and indices.
			// they will be released at the next } (the end of the scope)
			gl_resource::wolock vl(ray->get_vertices());
			my_vertex *vtx = (my_vertex *)vl.u8();
			gl_resource::wolock il(ray->get_indices());
			uint32_t *idx = il.u32();

			vec3 step = (position2 - position1) / num_steps;
			// make the vertices
			for (size_t i = 0; i != num_steps + 1; ++i) {
				float r = 0, g = 0, b = 0;
				//float r = 0.0f, g = 1.0f * i / num_steps, b = 1.0f;
				//float y = i * (height / num_steps) - height * 0.5f;
				vec3 middlePoint = vec3p(position1.x() + step.x()*i, position1.y(), position1.z() + step.z()*i);
				vec3 crossProduct = step.cross(vec3(0, 1, 0)) / (step.cross(vec3(0, 1, 0))).length();
				vtx->pos = middlePoint + width*crossProduct;
				vtx->color = make_color(r, g, b);
				log("%f %f %f %08x\n", r, g, b, vtx->color);
				vtx++;
				vtx->pos = middlePoint - width*crossProduct;
				vtx->color = make_color(r, g, b);
				vtx++;
			}
			// make the triangles
			uint32_t vn = 0;
			for (size_t i = 0; i != num_steps; ++i) {
				// 0--2
				// | \|
				// 1--3
				idx[0] = vn + 0;
				idx[1] = vn + 3;
				idx[2] = vn + 1;
				idx += 3;

				idx[0] = vn + 0;
				idx[1] = vn + 2;
				idx[2] = vn + 3;
				idx += 3;

				vn += 2;
			}
		}


		//Add the hole to the app_scene
		scene_node *node = new scene_node();
		rayDrag = node;
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, ray, black));
	}


	/// this is called to remove the ray
	void eraseRay()
	{
		if (rayDrag)
		{
			app_scene->delete_mesh_instance(app_scene->get_first_mesh_instance(rayDrag));
			rayDrag.~ref();
		}
	}


	/// Helper function. a bit cleaner to read.
	float getRandomFloat(int max)
	{
		return float(rand() % max);
	}

	/// This function takes a position and a circle's position and radius, and returns whether or not the first position is inside the circle.
	bool vecInsideOfCircle(vec3 position, vec3 circlePos, float circleRadius, float marginOfError = 0.0f)
	{
		circleRadius += marginOfError;

		float xMember = (position.x() - circlePos.x())*(position.x() - circlePos.x());
		float zMember = (position.z() - circlePos.z())*(position.z() - circlePos.z());

		return (xMember + zMember <= (circleRadius*circleRadius));
	}

	void goToNextLevel()
	{
		currentLevel++;
		resetBoard();
	}

	/// This function checks whether any ball is in a pocket and removes in from play if that is the case.
	void checkIfBallIsInPocket()
	{
		std::vector<ref<scene_node>>::iterator it;
		std::vector<ref<scene_node>>::iterator it2;
		for (it = redBalls.begin(); it != redBalls.end();)
		{
			if ((*it)->get_linear_velocity().y() != 0.0f)
			{
				vec3 linVel = (*it)->get_linear_velocity();
				(*it)->set_linear_velocity(vec3(linVel.x(), 0, linVel.z()));
			}
			bool hasBallBeenDeleted = false;
			vec3 ballPosition = (*it)->get_position();
			for (it2 = myHoles.begin(); it2 != myHoles.end();)
			{
				if (vecInsideOfCircle(ballPosition, (*it2)->get_position(), holesRadius, -0.5f))
				{
					(*it)->set_position(vec3(0, -10.0f, 0)); // Hides the bal until I can figure out how to properly delete it.
					app_scene->delete_mesh_instance(app_scene->get_first_mesh_instance((*it)));
					it = redBalls.erase(it);
					hasBallBeenDeleted = true;
					printf("Ball has been deleted.");
					break;
				}
				else
					++it2;
			}
			if(!hasBallBeenDeleted)
				++it;
		}
		redBalls.shrink_to_fit();

		if(whiteBall)
		{
			if ((whiteBall)->get_linear_velocity().y() != 0.0f)
			{
				vec3 linVel = (whiteBall)->get_linear_velocity();
				(whiteBall)->set_linear_velocity(vec3(linVel.x(), 0, linVel.z()));
			}
			vec3 playerPosition = whiteBall->get_position();
			for (it2 = myHoles.begin(); it2 != myHoles.end();)
			{
				if (vecInsideOfCircle(playerPosition, (*it2)->get_position(), holesRadius))
				{
					whiteBall->set_position(vec3(0, -10.0f, 0)); // Hides the ball until I can figure out how to properly delete it.
					app_scene->delete_mesh_instance(app_scene->get_first_mesh_instance(whiteBall));
					whiteBall.~ref();
					printf("White Ball has been deleted.");
					break;
				}
				else
					++it2;
			}
		}

		if (redBalls.size() == 0)
			goToNextLevel();
	}

	void resetBoard()
	{
		if (resetTime < clock())
			resetTime = clock() + 1000;
		else
			return;
		if(whiteBall)
		{
		whiteBall->set_position(vec3(0, -10.0f, 0)); // Hides the ball until I can figure out how to properly delete it.
		app_scene->delete_mesh_instance(app_scene->get_first_mesh_instance(whiteBall));
		whiteBall.~ref();
		printf("White Ball has been deleted.");
		}
		std::vector<ref<scene_node>>::iterator it;
		for (it = redBalls.begin(); it != redBalls.end();)
		{
			(*it)->set_position(vec3(0, -10.0f, 0)); // Hides the ball until I can figure out how to properly delete it.
			app_scene->delete_mesh_instance(app_scene->get_first_mesh_instance((*it)));
			it = redBalls.erase(it);
			printf("Ball has been deleted.");
		}

		loadDataFromFile();

	}


	vec3 convertScreenToWorld(vec3 position)
	{
		return vec3((position.x() * 30.0f / 450.0f) - 25.0f, 3.0f, (position.z() * 40.0f / 580.0f) - 25.5f);
		//return vec3(position.x() / 15 - 25, 3, position.y() / 15 - 25);
	}

	/// this is called to handle inputs
	void handleInputs()
	{
		if (is_key_down(key_space))
		{
			resetBoard();
			return;
		}
		if (!whiteBall) return;
		if (attemptLimit <= 0) return;
		if (is_key_down(key_ctrl))
		{
			whiteBall->set_linear_velocity(vec3(24,0,14));
			return;
		}
		else 
		{
			eraseRay();
			int newX, newY;
			get_mouse_pos(newX, newY);
			vec3 mouseWorldPos = convertScreenToWorld(vec3(newX, 3, newY));
			if (is_key_down(key_lmb))
				printf("X: %d Y: %d\n X2: %f Y2: %f\n\n", newX, newY, mouseWorldPos.x(), mouseWorldPos.z());
			if (is_key_down(key_lmb) && xMousePos == -999 && yMousePos == -999
				&& vecInsideOfCircle(vec3(mouseWorldPos.x(), 3, mouseWorldPos.z()), whiteBall->get_position(), 1))
			{
				get_mouse_pos(xMousePos, yMousePos);
			}
			else if (!is_key_down(key_lmb) && xMousePos != -999 && yMousePos != -999)
			{

				vec2 dirVector = vec2(newX - xMousePos, newY - yMousePos);
				int vectorLength = sqrt(pow(dirVector.x(), 2) + pow(dirVector.y(), 2));
				vec2 unitVector = dirVector / vectorLength;

				if (vectorLength > 700)
					vectorLength = 70;
				else
					vectorLength /= 10;

				if (whiteBall)
					whiteBall->set_linear_velocity(-1 * vec3(unitVector.x(), 0, unitVector.y()) * vectorLength);
				attemptLimit--;

				xMousePos = -999;
				yMousePos = -999;
			}
			else if (is_key_down(key_lmb) && xMousePos != -999 && yMousePos != -999)
			{
				generateRay(convertScreenToWorld(vec3(xMousePos, 3, yMousePos)), convertScreenToWorld(vec3(newX, 3, newY)), 0.1f);
			}
		}
	}

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);


      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);


      // draw the scene
      app_scene->render((float)vx / vy);
	  

	  // Player_node is only initialized when on MOBILE camera type
	  if (player_node)
	  {
		  scene_node *camera_node = the_camera->get_node();
		  mat4t &camera_to_world = camera_node->access_nodeToParent();
		  mouse_look_helper.update(camera_to_world);
		  fps_helper.update(player_node, camera_node);
	  }
	  // handle balls in pockets
	  checkIfBallIsInPocket();

	  handleInputs();

	  //std::string whiteBallHeight = std::to_string(whiteBall->get_position().y()) + " \n";
	  //printf(whiteBallHeight.c_str());
    }
  };
}
