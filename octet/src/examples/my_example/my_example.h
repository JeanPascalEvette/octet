////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class my_example : public app {
	  mouse_look mouse_look_helper;
	  helper_fps_controller fps_helper;
	  ref<camera_instance> the_camera;
	  ref<scene_node> player_node;

	  std::vector<ref<scene_node>> redBalls;
	  std::vector<ref<scene_node>> myHoles;
	  float holesRadius;
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
      app_scene =  new visual_scene();
	  holesRadius = 1.4f;
	  mat4t mat;
      app_scene->create_default_camera_and_lights();

	  mouse_look_helper.init(this, 200.0f / 360.0f, false);
	  fps_helper.init(this);
	  the_camera = app_scene->get_camera_instance(0);
	  the_camera->set_far_plane(10000);


	  the_camera->get_node()->translate(vec3(0, 0, -35)); // Have to move the camera for it to be centered
	  mesh_instance *mi;
	  int CameraPosition = 0;
	  if (CameraPosition == 0) // 0 = topDown  - 1 = side - 2 = oblique - 3 = mobile
	  {
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 60, 0));
		  app_scene->get_camera_instance(0)->get_node()->rotate(-90, vec3(1, 0, 0));
	  }
	  else if (CameraPosition == 1)
	  {
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 50));
	  }
	  else if (CameraPosition == 2)
	  {
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 50));
		  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 30, 0));
		  app_scene->get_camera_instance(0)->get_node()->rotate(-45, vec3(1, 0, 0));
	  }
	  else if (CameraPosition == 3)
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
	  }
	  material *green = new material(vec4(0, 1, 0, 1));
	  material *darkgreen = new material(vec4(0, 0.5f, 0, 1));
	  material *red = new material(vec4(1, 0, 0, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
	  material *white = new material(vec4(1, 1, 1, 1));
      

	  mat.loadIdentity();
	  mat.translate(0, -1.5f, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(100, 1, 100)), blue, false);
	  mat.loadIdentity();
	  mat.translate(0, -1, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(16, 1, 20)), green, false);


	  mat.loadIdentity();
	  mat.translate(15, -1, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(1, 3, 21)), darkgreen, false);
	  mat.loadIdentity();
	  mat.translate(-15, -1, 0);
	  app_scene->add_shape(mat, new mesh_box(vec3(1, 3, 21)), darkgreen, false);
	  mat.loadIdentity();
	  mat.translate(0, -1, -20);
	  app_scene->add_shape(mat, new mesh_box(vec3(16, 3, 1)), darkgreen, false);
	  mat.loadIdentity();
	  mat.translate(0, -1, 20);
	  app_scene->add_shape(mat, new mesh_box(vec3(16, 3, 1)), darkgreen, false);

	  mat.loadIdentity();
	  mat.translate(0, 0, 0);
	  app_scene->add_shape(mat, new mesh_sphere(vec3(0), 1), white, true);

	  mat.loadIdentity();
	  mat.translate(12, 0, -3);
	  app_scene->add_shape(mat, new mesh_sphere(vec3(0), 1), red, true);

	  mat.loadIdentity();
	  mat.translate(5, 0, 6);
	  app_scene->add_shape(mat, new mesh_sphere(vec3(0), 1), red, true);

	  mat.loadIdentity();
	  mat.translate(1, 0, -8);
	  app_scene->add_shape(mat, new mesh_sphere(vec3(0), 1), red, true);



	  myHoles = std::vector<ref<scene_node>>();
	  generateHole(vec3(13.5f, 3.5f, 0.0f), holesRadius);
	  generateHole(vec3(-13.5f, 3.5f, 0.0f), holesRadius);
	  generateHole(vec3(13.0f, 3.5f, 18.0f), holesRadius);
	  generateHole(vec3(13.0f, 3.5f, -18.0f), holesRadius);
	  generateHole(vec3(-13.0f, 3.5f, 18.0f), holesRadius);
	  generateHole(vec3(-13.0f, 3.5f, -18.0f), holesRadius);



	  if (CameraPosition == 3)
		player_node = mi->get_node();


	  scene_node *ground = app_scene->get_mesh_instance(0)->get_node();
	  scene_node *field = app_scene->get_mesh_instance(1)->get_node();
	  scene_node *wall = app_scene->get_mesh_instance(2)->get_node();
	  scene_node *wall2 = app_scene->get_mesh_instance(3)->get_node();
	  scene_node *wall3 = app_scene->get_mesh_instance(4)->get_node();
	  scene_node *wall4 = app_scene->get_mesh_instance(5)->get_node();
	  scene_node *ball = app_scene->get_mesh_instance(6)->get_node();
	  scene_node *redBall1 = app_scene->get_mesh_instance(7)->get_node();
	  scene_node *redBall2 = app_scene->get_mesh_instance(8)->get_node();
	  scene_node *redBall3 = app_scene->get_mesh_instance(9)->get_node();

	  redBalls = std::vector<ref<scene_node>>();
	  redBalls.push_back(redBall1);
	  redBalls.push_back(redBall2);
	  redBalls.push_back(redBall3);
	  if (CameraPosition == 3)
		  redBalls.push_back(player_node);

	  ball->set_linear_velocity(vec3(getRandomFloat(30), 0, getRandomFloat(30)));
	  ball->set_friction(0.1f);
	  ball->set_resitution(1.0f);

	  redBall1->set_linear_velocity(vec3(getRandomFloat(30), 0, getRandomFloat(30)));
	  redBall1->set_friction(0.1f);
	  redBall1->set_resitution(1.0f);
	  redBall2->set_linear_velocity(vec3(getRandomFloat(30), 0, getRandomFloat(30)));
	  redBall2->set_friction(0.1f);
	  redBall2->set_resitution(1.0f);
	  redBall3->set_linear_velocity(vec3(getRandomFloat(30), 0, getRandomFloat(30)));
	  redBall3->set_friction(0.1f);
	  redBall3->set_resitution(1.0f);


	  field->set_resitution(1.0f);
	  wall->set_resitution(1.0f);
	  wall2->set_resitution(1.0f);
	  wall3->set_resitution(1.0f);
	  wall4->set_resitution(1.0f);

    }

	void generateHole(vec3 position, float radius)
	{
		//Based on example_geometry's helix
		// use a shader that just outputs the color_ attribute.
		param_shader *shader = new param_shader("shaders/default.vs", "shaders/simple_color.fs");
		material *black = new material(vec4(0, 0, 0, 1), shader);


		mesh *hole = new mesh();

		// number of steps in helix
		size_t num_steps = 160;

		// allocate vertices and indices into OpenGL buffers
		size_t num_vertices = num_steps * 2 + 2;
		size_t num_indices = num_steps*6;
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
		node->translate(position);
		myHoles.push_back(node);
	}

	float getRandomFloat(int max)
	{
		return rand() % max;
	}

	bool vecInsideOfCircle(vec3 position, vec3 circlePos, float circleRadius, float marginOfError = 0.0f)
	{
		circleRadius += marginOfError;

		float xMember = (position.x() - circlePos.x())*(position.x() - circlePos.x());
		float zMember = (position.z() - circlePos.z())*(position.z() - circlePos.z());

		return (xMember + zMember <= (circleRadius*circleRadius));
	}

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

	  if (player_node)
	  {
		  scene_node *camera_node = the_camera->get_node();
		  mat4t &camera_to_world = camera_node->access_nodeToParent();
		  mouse_look_helper.update(camera_to_world);
		  fps_helper.update(player_node, camera_node);
	  }

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

	  

      // draw the scene
      app_scene->render((float)vx / vy);
	  

	  for (int currentBall = 0; currentBall < redBalls.size(); currentBall++)
	  {
		  vec3 ballPosition = redBalls[currentBall]->get_position();
		  for (int currentHole = 0; currentHole < myHoles.size(); currentHole++)
		  {
			  if (vecInsideOfCircle(ballPosition, myHoles[currentHole]->get_position(), holesRadius, 0.5f)) // Delete Ball
			  {
				  redBalls[currentBall]->set_position(vec3(0, -10, 0));
			  }
		  }
	  }

    }
  };
}
