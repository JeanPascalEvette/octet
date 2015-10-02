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
		app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 10));

		btRigidBody* rigid_body = NULL;
		btRigidBody* rigid_body2 = NULL;
		btRigidBody* rigid_body3 = NULL;
		btRigidBody* rigid_body4 = NULL;
		btRigidBody* rigid_body5 = NULL;
		material *red = new material(vec4(1, 0, 0, 1));
		material *green = new material(vec4(0, 1, 0, 1));
		material *blue = new material(vec4(0, 0, 1, 1));

		mat4t mat;
		mat.loadIdentity();
		mat.translate(-13, 6, 0);
		rigid_body = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_sphere(vec3(2, 2, 2), 2), 1.0f, true);


		mat.loadIdentity();
		mat.translate(-10, 15, 0);
		rigid_body2 = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_sphere(vec3(2,2,2),2),1.0f, true);

		btHingeConstraint* hinge = new btHingeConstraint(*rigid_body, *rigid_body2, btVector3(-3, 6, 0), btVector3(0, 15, 0), btVector3(1, 0, 0), btVector3(1, 0, 0));
		hinge->setLimit(0, 0.5f);
		world->addConstraint(hinge, true);


		mat.loadIdentity();
		mat.translate(8, 0, 5);
		rigid_body3 = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_box(vec3(2, 2, 2)), 1.0f, true);



		mat.loadIdentity();
		mat.translate(7, 5, 5);
		rigid_body4 = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_box(vec3(2, 2, 2)), 1.0f, true);


		mat.loadIdentity();
		mat.translate(13, 0, 5);
		rigid_body5 = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_box(vec3(2, 2, 2)), 51.0f, true);


		btHingeConstraint* hinge2 = new btHingeConstraint(*rigid_body3, *rigid_body5, btVector3(-3, 0, 5), btVector3(-3, 0, 10), btVector3(0, 1, 0), btVector3(0, 1, 0));
		hinge2->setLimit(0, 0.5f);
		world->addConstraint(hinge2, true);


		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.0f), btScalar(-5.0f), btScalar(0.0f)));
		btGeneric6DofSpringConstraint* spring = new btGeneric6DofSpringConstraint(*rigid_body3, *rigid_body4, frameInA, frameInB, true);

		spring->setLinearUpperLimit(btVector3(5., 0., 0.));
		spring->setLinearLowerLimit(btVector3(-5., 0., 0.));

		spring->setAngularLowerLimit(btVector3(0.f, 0.f, -1.5f));
		spring->setAngularUpperLimit(btVector3(0.f, 0.f, 1.5f));

		world->addConstraint(spring, true);
		spring->setDbgDrawSize(btScalar(5.f));

		spring->enableSpring(0, true);
		spring->setStiffness(0, 19.478f);
		spring->setDamping(0, 0.3f);
		spring->setEquilibriumPoint();

	  //app_scene->add_shape(mat, new mesh_box(vec3(2, 2, 2)), red, true);
	  //scene_node* box = app_scene->get_mesh_instance(1)->get_scene_node();


      mat.loadIdentity();
      mat.translate( 2, 0, -1.0f);
	  mat.rotate(90, 0, 1, 0);
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
