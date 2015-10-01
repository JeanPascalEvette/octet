////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class example_shapes : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	btDiscreteDynamicsWorld* world;

  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

    ~example_shapes() {
    }

    /// this is called once OpenGL is initialized
	void app_init() {
		app_scene = new visual_scene();
		world = app_scene->get_world();
		app_scene->create_default_camera_and_lights();
		app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));

		btRigidBody* rigid_body = NULL;
		btRigidBody* rigid_body2 = NULL;
		material *red = new material(vec4(1, 0, 0, 1));
		material *green = new material(vec4(0, 1, 0, 1));
		material *blue = new material(vec4(0, 0, 1, 1));

		mat4t mat;
		mat.loadIdentity();
		mat.translate(-3, 6, 0);
		rigid_body = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_sphere(vec3(2, 2, 2), 2), 1.0f, true);


		mat.loadIdentity();
		mat.translate(0, 15, 0);
		rigid_body2 = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_sphere(vec3(2,2,2),2),1.0f, true);

		btHingeConstraint* hinge = new btHingeConstraint(*rigid_body, *rigid_body2, btVector3(-3, 6, 0), btVector3(0, 15, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
		hinge->setLimit(0, 0.5f);
		world->addConstraint(hinge, true);


	  //app_scene->add_shape(mat, new mesh_box(vec3(2, 2, 2)), red, true);
	  //scene_node* box = app_scene->get_mesh_instance(1)->get_scene_node();


      mat.loadIdentity();
      mat.translate( 3, 6, 0);
      app_scene->add_shape(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), red, true);

      // ground
      mat.loadIdentity();
      mat.translate(0, -1, 0);
      app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), green, false);


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
    }
  };
}
