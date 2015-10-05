////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "tinyxml2.h"
#include "SoundSystem.h"

namespace octet {


	//Container classes to store data imported from XML file
	class predef_shape {

		string type;
		string color;
		vec3 location;
		std::vector<vec3> rotation;
		float weight;

	public :
		predef_shape(string _type, vec3 _location, std::vector<vec3> _rotation, float _weight, string _color) { type = _type; location = _location; rotation = _rotation; weight = _weight; color = _color; }

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
	scene_node * ground;
	int springIndex;

	SoundSystemClass sound;
	SoundClass bounceSound;
	FMOD::Channel* bounceChannel;
	vec3 lastCol;

	std::vector<ref<scene_node>> myObjects;

  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

    ~example_shapes() {
    }

    /// this is called once OpenGL is initialized
	void app_init() {
		myObjects = std::vector<ref<scene_node>>();
		lastCol = vec3(0);
		setUpSound();

		app_scene = new visual_scene();
		world = app_scene->get_world();
		app_scene->create_default_camera_and_lights();
		app_scene->get_camera_instance(0)->get_node()->translate(vec3(5, 4, 15));
		

		material *green = new material(vec4(0, 1, 0, 1));


		mat4t mat;
		// ground
		mat.loadIdentity();
		mat.translate(0, -1, 0);
		app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), green, false);
		ground = app_scene->get_child(0);
		


		loadDataFromFile();


    }

	

	/// Loads data from an XML file and generates shape based on it
	void loadDataFromFile()
	{
		//Spring & Hinge based of Bullet's demo -> http://bullet.googlecode.com/svn/trunk/Demos/ConstraintDemo/ConstraintDemo.cpp

		mat4t mat;
		springIndex = 0;
		material *red = new material(vec4(1, 0, 0, 1));
		material *green = new material(vec4(0, 1, 0, 1));
		material *blue = new material(vec4(0, 0, 1, 1));
		tinyxml2::XMLDocument doc;
		if(doc.LoadFile("data.xml") == tinyxml2::XML_NO_ERROR)
		{ 
			tinyxml2::XMLNode * el = doc.FirstChildElement("Data")->FirstChildElement("ShapeList")->FirstChildElement();
			std::vector<predef_shape> listOfShapes = std::vector<predef_shape>();
			std::vector<predef_link> listOfLinks = std::vector<predef_link>();
			std::vector<btRigidBody *> listOfRB = std::vector<btRigidBody *>();

			//Generate list of Shapes
			while (el != nullptr)
			{
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

				predef_shape myShape = predef_shape(type, location, myRotations, weight, color);
				listOfShapes.push_back(myShape);
				el = el->NextSibling();
			}

			//Generate list of links
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

			//Use list of shapes to generate actual shapes in OpenGL
			for (int curentShape = 0; curentShape < listOfShapes.size(); curentShape++)
			{
				btRigidBody * myRB = NULL;
				mat.loadIdentity();
				mat.translate(listOfShapes[curentShape].getLoc());
				material *myMat = NULL;

				if (listOfShapes[curentShape].getColor() == "red")
					myMat = red;
				else if (listOfShapes[curentShape].getColor() == "blue")
					myMat = blue;
				else if (listOfShapes[curentShape].getColor() == "green")
					myMat = green;
				else
					continue;


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

				scene_node* node = NULL;
				if (listOfShapes[curentShape].getType() == "Sphere")
				{
					node = app_scene->createNewObjectWithRigidBody(mat, myMat, new mesh_sphere(vec3(2, 2, 2), 2), listOfShapes[curentShape].getWeight(), true, &myRB);
				}
				else if (listOfShapes[curentShape].getType() == "Box")
				{
					node = app_scene->createNewObjectWithRigidBody(mat, myMat, new mesh_box(vec3(2, 2, 2)), listOfShapes[curentShape].getWeight(), true, &myRB);
				}
				else if (listOfShapes[curentShape].getType() == "Cylinder")
				{
					node = app_scene->createNewObjectWithRigidBody(mat, myMat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), listOfShapes[curentShape].getWeight(), true, &myRB);
				}
				myObjects.push_back(node);
				listOfRB.push_back(myRB);
			}


			//Use list of links to generate constraints
			for (int currentLink = 0; currentLink < listOfLinks.size(); currentLink++)
			{
				if (listOfShapes.size() <= listOfLinks[currentLink].getID1() || listOfShapes.size() <= listOfLinks[currentLink].getID2())
					continue;
				predef_shape shape1 = listOfShapes[listOfLinks[currentLink].getID1()];
				predef_shape shape2 = listOfShapes[listOfLinks[currentLink].getID2()];

				//Make sure that both shapes exist
				btVector3 axis = btVector3(1, 0, 0);

				if (listOfLinks[currentLink].getAxis() == "X")
					axis = btVector3(1, 0, 0);
				else if (listOfLinks[currentLink].getAxis() == "Y")
					axis = btVector3(0, 1, 0);
				else if (listOfLinks[currentLink].getAxis() == "Z")
					axis = btVector3(0, 0, 1);
			

				if (listOfLinks[currentLink].getType() == "Hinge")
				{
					btHingeConstraint* hinge = new btHingeConstraint(*listOfRB[listOfLinks[currentLink].getID1()], *listOfRB[listOfLinks[currentLink].getID2()], btVector3(shape1.getLoc().x(), shape1.getLoc().y(), shape1.getLoc().z()), btVector3(shape2.getLoc().x(), shape2.getLoc().y(), shape2.getLoc().z()), axis, axis);
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
					btGeneric6DofSpringConstraint* spring = new btGeneric6DofSpringConstraint(*listOfRB[listOfLinks[currentLink].getID1()], *listOfRB[listOfLinks[currentLink].getID2()], frameInA, frameInB, true);

					//Some of those values would idealy be parameters in the XML file
					spring->setLinearUpperLimit(btVector3(5., 0., 0.));
					spring->setLinearLowerLimit(btVector3(-5., 0., 0.));

					spring->setAngularLowerLimit(btVector3(0.f, 0.f, -1.5f));
					spring->setAngularUpperLimit(btVector3(0.f, 0.f, 1.5f));

					world->addConstraint(spring, false);

					spring->enableSpring(springIndex++, true);
					spring->setStiffness(0, 19.478f);

				}
			}
		}

	}


	void handleCollisions()
	{

		// Based on http://hamelot.co.uk/programming/using-bullet-only-for-collision-detection/
		world->performDiscreteCollisionDetection();
		int numManifolds = world->getDispatcher()->getNumManifolds();
		//For each contact manifold
		for (int i = 0; i < numManifolds; i++) {
			btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* obA = static_cast<const btCollisionObject*>(contactManifold->getBody0());
			const btCollisionObject* obB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
			scene_node * currentObjA = ((scene_node*)obA->getUserPointer());
			if (currentObjA->getIgnoreCol()) continue;
			scene_node * currentObjB = ((scene_node*)obB->getUserPointer());
			if (currentObjB->getIgnoreCol()) continue;
			contactManifold->refreshContactPoints(obA->getWorldTransform(), obB->getWorldTransform());
			int numContacts = contactManifold->getNumContacts();
			//For each contact point in that manifold
			for (int j = 0; j < numContacts; j++) {
				//Get the contact information
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				btVector3 ptA = pt.getPositionWorldOnA();
				btVector3 ptB = pt.getPositionWorldOnB();
				double ptdist = pt.getDistance();
				std::string dist = "Collision between objects at position (" + std::to_string(currentObjA->get_position().x()) + "," + std::to_string(currentObjA->get_position().y()) + "," + std::to_string(currentObjA->get_position().z()) + ") and (" + std::to_string(currentObjB->get_position().x()) + ", " + std::to_string(currentObjB->get_position().y()) + ", " + std::to_string(currentObjB->get_position().z()) + ") " + "\n";
				printf(dist.c_str());

				//Make sure the sound file is played only once at a time when the objects keep colliding with each other
				if (!bounceChannel)
					bounceChannel = sound.playSound(bounceSound, false);
				bool isChannelPlaying = false;
				bounceChannel->isPlaying(&isChannelPlaying);
				if(!isChannelPlaying)
					bounceChannel = sound.playSound(bounceSound, false);
				
			}
		}
	}

	void setUpSound()
	{
		// Initialize our sound system
		sound = SoundSystemClass();

		// http://www.freesfx.co.uk/download/?type=mp3&id=9971
		sound.createSound(&bounceSound, "bounce.mp3");
		bounceChannel = NULL;

	}
	/// this is called to handle inputs
	void handleInputs()
	{
		if (is_key_down(key_space))
		{
			reset();
			return;
		}
	}

	void reset()
	{

		std::vector<ref<scene_node>>::iterator it;
		for (it = myObjects.begin(); it != myObjects.end();)
		{
			(*it)->set_position(vec3(0, -10.0f, 0)); // Hides the ball until I can figure out how to properly delete it.
			app_scene->delete_mesh_instance(app_scene->get_first_mesh_instance((*it)));
			it = myObjects.erase(it);
			printf("Object has been deleted.");
		}

		loadDataFromFile();
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

	  handleCollisions();

	  handleInputs();

    }
  };
}
