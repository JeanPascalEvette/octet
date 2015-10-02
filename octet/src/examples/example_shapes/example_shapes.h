////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "tinyxml2.h"
namespace octet {

	class predef_shape {

		int ID;
		string type;
		vec3 location;
		vec3 rotation;
		float weight;

	public :
		predef_shape(int _id, string _type, vec3 _location, vec3 _rotation, float _weight) { ID = _id; type = _type; location = _location; rotation = _rotation; weight = _weight; }

		int getId() { return ID; }
		string getType() { return type; }
		vec3 getLoc() { return location; }
		vec3 getRot() { return rotation; }
		float getWeight() { return weight; }
	};
	class predef_link {
		string type;
		int ID1;
		int ID2;
		string axis;
	public :
		predef_link(string _type, int _ID1, int _ID2, string _axis) { type = _type;ID1 = _ID1; ID2 = _ID2; axis = _axis; }
		string getType() { return type; }
		int getID1() { return ID1; }
		int getID2() { return ID2; }
		string getAxis() { return axis; }
	};

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

		//Spring & Hinge based of Bullet's demo -> http://bullet.googlecode.com/svn/trunk/Demos/ConstraintDemo/ConstraintDemo.cpp

		mat4t mat;

		tinyxml2::XMLDocument doc;
		doc.LoadFile("data.xml");
		tinyxml2::XMLNode * el = doc.FirstChildElement("Data")->FirstChildElement("ShapeList")->FirstChildElement();
		std::vector<predef_shape> listOfShapes = std::vector<predef_shape>();
		std::vector<predef_link> listOfLinks = std::vector<predef_link>();
		std::vector<btRigidBody *> listOfRB = std::vector<btRigidBody *>();
		while (el != nullptr)
		{
			int ID = atoi(el->FirstChildElement("ID")->GetText());
			string type = el->FirstChildElement("Type")->GetText();
			vec3 location = vec3(atof(el->FirstChildElement("Location")->FirstChildElement("x")->GetText()), atof(el->FirstChildElement("Location")->FirstChildElement("y")->GetText()), atof(el->FirstChildElement("Location")->FirstChildElement("z")->GetText()));
			vec3 rotation = vec3(atof(el->FirstChildElement("Rotation")->FirstChildElement("x")->GetText()), atof(el->FirstChildElement("Rotation")->FirstChildElement("y")->GetText()), atof(el->FirstChildElement("Rotation")->FirstChildElement("z")->GetText()));
			float weight = atoi(el->FirstChildElement("Weight")->GetText());

			predef_shape myShape = predef_shape(ID, type, location, rotation, weight);
			listOfShapes.push_back(myShape);
			el = el->NextSibling();
		}
		el = doc.FirstChildElement("Data")->FirstChildElement("LinkList")->FirstChildElement();
		while (el != nullptr)
		{
			string linkType = el->FirstChildElement("LinkType")->GetText();
			int ID1 = atoi(el->FirstChildElement("ID1")->GetText());
			int ID2 = atoi(el->FirstChildElement("ID2")->GetText());
			string axis = el->FirstChildElement("Axis")->GetText();
			

			predef_link myLink = predef_link(linkType, ID1, ID2, axis);
			listOfLinks.push_back(myLink);
			el = el->NextSibling();
		}

		for (int curentShape = 0; curentShape < listOfShapes.size(); curentShape++)
		{
			btRigidBody * myRB = NULL;
			mat.loadIdentity();
			mat.translate(listOfShapes[curentShape].getLoc());
			if (listOfShapes[curentShape].getType() == "Sphere")
			{
				myRB = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_sphere(vec3(2, 2, 2), 2), listOfShapes[curentShape].getWeight(), true);
			}
			else if (listOfShapes[curentShape].getType() == "Box")
			{
				myRB = app_scene->createNewObjectWithRigidBody(mat, blue, new mesh_box(vec3(2, 2, 2)), listOfShapes[curentShape].getWeight(), true);
			}
			else if (listOfShapes[curentShape].getType() == "Cylinder")
			{
				myRB = app_scene->createNewObjectWithRigidBody(mat, red, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), listOfShapes[curentShape].getWeight(), true);
			}
			listOfRB.push_back(myRB);
		}

		for (int currentLink = 0; currentLink < listOfLinks.size(); currentLink++)
		{
			predef_shape shape1 = listOfShapes[listOfLinks[currentLink].getID1() - 1];
			predef_shape shape2 = listOfShapes[listOfLinks[currentLink].getID2() - 1];
			btVector3 axis = btVector3(1, 0, 0);
			if (listOfLinks[currentLink].getAxis() == "Y")
				axis = btVector3(0, 1, 0);
			else if (listOfLinks[currentLink].getAxis() == "Z")
				axis = btVector3(0, 0, 1);

			if (listOfLinks[currentLink].getType() == "Hinge")
			{
				btHingeConstraint* hinge = new btHingeConstraint(*listOfRB[listOfLinks[currentLink].getID1()-1], *listOfRB[listOfLinks[currentLink].getID2() - 1], btVector3(shape1.getLoc().x(), shape1.getLoc().y(), shape1.getLoc().z()), btVector3(shape2.getLoc().x(), shape2.getLoc().y(), shape2.getLoc().z()), axis, axis);
				hinge->setLimit(0, 0.5f);
				world->addConstraint(hinge, true);

			}
			else if (listOfLinks[currentLink].getType() == "Spring")
			{
				btTransform frameInA, frameInB;
				frameInA = btTransform::getIdentity();
				frameInA.setOrigin(btVector3(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f)));
				frameInB = btTransform::getIdentity();
				frameInB.setOrigin(btVector3(btScalar(0.0f), btScalar(-5.0f), btScalar(0.0f)));
				btGeneric6DofSpringConstraint* spring = new btGeneric6DofSpringConstraint(*listOfRB[listOfLinks[currentLink].getID1() - 1], *listOfRB[listOfLinks[currentLink].getID2() - 1], frameInA, frameInB, true);

				spring->setLinearUpperLimit(btVector3(5., 0., 0.));
				spring->setLinearLowerLimit(btVector3(-5., 0., 0.));

				spring->setAngularLowerLimit(btVector3(0.f, 0.f, -1.5f));
				spring->setAngularUpperLimit(btVector3(0.f, 0.f, 1.5f));

				world->addConstraint(spring, true);

				spring->enableSpring(0, true);
				spring->setStiffness(0, 19.478f);

			}
		}

		/*
		float whiteBallLocX = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Location")->FirstChildElement("x")->GetText());
		float whiteBallLocY = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Location")->FirstChildElement("y")->GetText());
		float whiteBallLocZ = atof(doc.FirstChildElement("Data")->FirstChildElement("WhiteBall")->FirstChildElement("Location")->FirstChildElement("z")->GetText());






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
		
		spring->enableSpring(0, true);
		spring->setStiffness(0, 19.478f);
		

	  //app_scene->add_shape(mat, new mesh_box(vec3(2, 2, 2)), red, true);
	  //scene_node* box = app_scene->get_mesh_instance(1)->get_scene_node();


      mat.loadIdentity();
      mat.translate( 2, 0, -1.0f);
	  mat.rotate(90, 0, 1, 0);
      app_scene->add_shape(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), red, true);

	  */

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
