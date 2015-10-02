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
		string color;
		vec3 location;
		std::vector<vec3> rotation;
		float weight;

	public :
		predef_shape(int _id, string _type, vec3 _location, std::vector<vec3> _rotation, float _weight, string _color) { ID = _id; type = _type; location = _location; rotation = _rotation; weight = _weight; color = _color; }

		int getId() { return ID; }
		string getType() { return type; }
		vec3 getLoc() { return location; }
		std::vector<vec3> getRot() { return rotation; }
		float getWeight() { return weight; }
		string getColor() { return color; }
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
		app_scene->get_camera_instance(0)->get_node()->translate(vec3(5, 4, 15));


		material *green = new material(vec4(0, 1, 0, 1));


		mat4t mat;

		loadDataFromFile();
      // ground
      mat.loadIdentity();
      mat.translate(0, -1, 0);
      app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), green, false);


    }

	void loadDataFromFile()
	{
		//Spring & Hinge based of Bullet's demo -> http://bullet.googlecode.com/svn/trunk/Demos/ConstraintDemo/ConstraintDemo.cpp

		mat4t mat;
		material *red = new material(vec4(1, 0, 0, 1));
		material *green = new material(vec4(0, 1, 0, 1));
		material *blue = new material(vec4(0, 0, 1, 1));
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
			
			std::vector<vec3> myRotations = std::vector<vec3>();
			tinyxml2::XMLNode * el2 = el->FirstChildElement("RotationList")->FirstChildElement();
			while (el2 != nullptr)
			{
				vec3 rotation = vec3(atof(el2->FirstChildElement("x")->GetText()), atof(el2->FirstChildElement("y")->GetText()), atof(el2->FirstChildElement("z")->GetText()));
				myRotations.push_back(rotation);
				el2 = el2->NextSibling();
			}
			float weight = atoi(el->FirstChildElement("Weight")->GetText());
			string color = el->FirstChildElement("Color")->GetText();

			predef_shape myShape = predef_shape(ID, type, location, myRotations, weight, color);
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
			material *myMat = NULL;

			if (listOfShapes[curentShape].getColor() == "red")
				myMat = red;
			if (listOfShapes[curentShape].getColor() == "blue")
				myMat = blue;
			if (listOfShapes[curentShape].getColor() == "green")
				myMat = green;


			for (int currentRot = 0; currentRot < listOfShapes[curentShape].getRot().size(); currentRot++)
			{
				vec3 myRot = listOfShapes[curentShape].getRot()[currentRot];
				if (myRot.x() != 0)
					mat.rotateX(myRot.x());
				if (myRot.y() != 0)
					mat.rotateY(myRot.y());
				if (myRot.z() != 0)
					mat.rotateZ(myRot.z());
			}

			if (listOfShapes[curentShape].getType() == "Sphere")
			{
				myRB = app_scene->createNewObjectWithRigidBody(mat, myMat, new mesh_sphere(vec3(2, 2, 2), 2), listOfShapes[curentShape].getWeight(), true);
			}
			else if (listOfShapes[curentShape].getType() == "Box")
			{
				myRB = app_scene->createNewObjectWithRigidBody(mat, myMat, new mesh_box(vec3(2, 2, 2)), listOfShapes[curentShape].getWeight(), true);
			}
			else if (listOfShapes[curentShape].getType() == "Cylinder")
			{
				myRB = app_scene->createNewObjectWithRigidBody(mat, myMat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), listOfShapes[curentShape].getWeight(), true);
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
				btHingeConstraint* hinge = new btHingeConstraint(*listOfRB[listOfLinks[currentLink].getID1() - 1], *listOfRB[listOfLinks[currentLink].getID2() - 1], btVector3(shape1.getLoc().x(), shape1.getLoc().y(), shape1.getLoc().z()), btVector3(shape2.getLoc().x(), shape2.getLoc().y(), shape2.getLoc().z()), axis, axis);
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
