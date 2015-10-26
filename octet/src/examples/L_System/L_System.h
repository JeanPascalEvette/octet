////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#include <sstream>
#include "tinyxml2.h"
namespace octet {
  /// Scene containing a box with octet.
  class L_System : public app {
    // scene for drawing box
	  ref<text_overlay> myText;
	  ref<mesh_text> myInfoText;
    ref<visual_scene> app_scene;
	material* darkGreen;
	material* brown;
	material* currentTreeMaterial;
	std::vector<material*> listOfMaterials;
	int currentMaterial;

	std::vector<ref<scene_node>> listOfLines;
	std::vector<vec3> savedPointStack;
	std::vector<float> savedAngleStack;

	std::vector<std::string> rules;
	std::map<char, std::string> decodedRules;
	std::string axiom;
	float setupAngle;
	int iterations;


	vec3 nextPoint;
	int currentFile;
	int currentIteration;

	enum ColorScheme {
		ALTERNATING,
		TREELIKE,
		RANDOM
	};
	ColorScheme colorSchemeType;

	const float HALFSIZE = 1.0f;
  public:
    /// this is called when we construct the class before everything is initialised.
    L_System(int argc, char **argv) : app(argc, argv) {
    }
    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  app_scene->get_camera_instance(0)->set_far_plane(100000.0f);
	  currentFile = 0;
	  currentIteration = 1;
	  colorSchemeType = ALTERNATING;



	  aabb bb(vec3(144.5f, 305.0f, 0.0f), vec3(256, 64, 0));
	  myText = new text_overlay();
	  myInfoText = new mesh_text(myText->get_default_font(), "", &bb);
	  myText->add_mesh_text(myInfoText);



	  loadFile();


    }

	void loadFile()
	{


		rules = std::vector<std::string>();
		decodedRules = std::map<char, std::string>();

		tinyxml2::XMLDocument doc;
		char aChar =  65 + currentFile;
		std::string fileName = "data";
		fileName += aChar;
		fileName += ".xml";
		doc.LoadFile(fileName.c_str());
		axiom = doc.FirstChildElement("Data")->FirstChildElement("Axiom")->GetText();
		setupAngle = atof(doc.FirstChildElement("Data")->FirstChildElement("Angle")->GetText());
		tinyxml2::XMLNode * el = doc.FirstChildElement("Data")->FirstChildElement("Rules")->FirstChildElement();
		while (el != nullptr)
		{
			rules.push_back(el->ToElement()->GetText());
			el = el->NextSibling();
		}

		for (int i = 0;i < rules.size(); i++)
		{
			char pre = (rules[i].substr(0, rules[i].find_first_of("->"))[0]);
			std::string post = rules[i].substr(rules[i].find_first_of("->") + 2, rules[i].size());
			decodedRules[pre] = post;

		}
		iterations = atoi(doc.FirstChildElement("Data")->FirstChildElement("Iterations")->GetText());

		generateTree();
	}

	void generateTree()
	{
		app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 14, 0));
		currentMaterial = 0;
		material *red = new material(vec4(1, 0, 0, 1));
		material *green = new material(vec4(0, 1, 0, 1));
		material *blue = new material(vec4(0, 0, 1, 1));
		material *white = new material(vec4(1, 1, 1, 1));
		material *black = new material(vec4(0, 0, 0, 1));
		darkGreen = new material(vec4(0, 0.5f, 0, 1));
		brown = new material(vec4(0.2f, 0.2f, 0.2f,1));
		savedPointStack = std::vector<vec3>();
		savedAngleStack = std::vector<float>();
		currentTreeMaterial = brown;


		listOfMaterials = std::vector<material*>();

		listOfMaterials.push_back(red);
		listOfMaterials.push_back(green);
		listOfMaterials.push_back(blue);
		listOfMaterials.push_back(white);
		listOfMaterials.push_back(black);
		std::string startingAxiom = axiom;
		nextPoint = vec3(0, 0, 0);
		for (int i = 0; i < currentIteration; i++)
		{
			std::string newAxiom = "";
			for (int u = 0; u < startingAxiom.size(); u++)
			{
				char currentChar = startingAxiom[u];
				bool found = false;
				std::map<char, std::string>::iterator it;
				for (it = decodedRules.begin(); it != decodedRules.end();)
				{
					if (it->first == currentChar)
					{
						newAxiom += it->second;
						found = true;
						break;
					}
					it++;
				}
				if (!found)
					newAxiom += currentChar;

			}
			startingAxiom = newAxiom;
		}

		float angle = 0.0f;
		std::vector<int> listBranches = std::vector<int>();
		for (int i = 0; i < startingAxiom.size(); i++)
		{
			if (startingAxiom[i] == ']')
			{
				for (int u = i; u >= 0; u--)
				{
					if(startingAxiom[u] == 'F' && u != 0)
					{
						listBranches.push_back(u);
						break;
					}
				}
			}
		}
		for (int i = 0; i < startingAxiom.size(); i++)
		{
			if (startingAxiom[i] == 'F')
			{
				for (int u = 0; u < listBranches.size(); u++)
					if (listBranches[u] == i)
						currentTreeMaterial = darkGreen;
				nextPoint = drawLine(nextPoint, angle);
			}
			else if (startingAxiom[i] == '+')
			{
				angle += setupAngle;
			}
			else if (startingAxiom[i] == '-')
			{
				angle -= setupAngle;
			}
			else if (startingAxiom[i] == '[')
			{
				//currentTreeMaterial = darkGreen;
				rememberPoint(nextPoint, angle);
			}
			else if (startingAxiom[i] == ']')
			{
				currentTreeMaterial = brown;
				std::pair<vec3, float> newVal = retrievePoint();
				nextPoint = newVal.first;
				angle = newVal.second;
			}
		}
	}

	std::pair<vec3, float> retrievePoint()
	{
		vec3 point = savedPointStack[savedPointStack.size() - 1];
		savedPointStack.pop_back();
		savedPointStack.shrink_to_fit();
		float angle = savedAngleStack[savedAngleStack.size() - 1];
		savedAngleStack.pop_back();
		savedAngleStack.shrink_to_fit();
		std::pair<vec3, float> retVal = std::pair<vec3, float>(point, angle);
		return retVal;
	}

	void rememberPoint(vec3 point, float angle)
	{
		savedPointStack.push_back(point);
		savedAngleStack.push_back(angle);
	}

	vec3 drawLine(vec3 startingPoint, float angle = 0.0f)
	{
		material * color = listOfMaterials[currentMaterial];
		if (colorSchemeType == TREELIKE)
			color = currentTreeMaterial;
		else if(colorSchemeType == ALTERNATING)
			currentMaterial = (currentMaterial + 1) % listOfMaterials.size();
		else if(colorSchemeType == RANDOM)
			currentMaterial = rand() % listOfMaterials.size();
		vec3 midPoint, endPoint;
		if (angle == 0.0f)
		{
			midPoint = startingPoint;
			midPoint.y() = midPoint.y() + HALFSIZE;
			endPoint = startingPoint;
			endPoint.y() = endPoint.y() + 2.0f*HALFSIZE;
		}
		else
		{
			midPoint = startingPoint;
			midPoint.x() = midPoint.x() + HALFSIZE *cos((angle + 90) * CL_M_PI / 180);
			midPoint.y() = midPoint.y() + HALFSIZE *sin((angle + 90)* CL_M_PI / 180);
			endPoint = startingPoint;
			endPoint.x() = endPoint.x() + 2.0f*HALFSIZE *cos((angle + 90) * CL_M_PI / 180);
			endPoint.y() = endPoint.y() + 2.0f*HALFSIZE *sin((angle + 90) * CL_M_PI / 180);
		}
		mat4t mat = mat4t();
		mat.loadIdentity();
		mat.rotate(90.0f, 1, 0, 0);
		mesh_cylinder *line;
		if (iterations == 7)
			line = new mesh_cylinder(zcylinder(vec3(0), 0.5f, HALFSIZE), mat);
		else if (iterations == 6)
			line = new mesh_cylinder(zcylinder(vec3(0), 0.3f, HALFSIZE), mat);
		else
			line = new mesh_cylinder(zcylinder(vec3(0), 0.2f, HALFSIZE), mat);
		scene_node *node = new scene_node();
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, line, color));
		node->translate(midPoint);
		listOfLines.push_back(node);
		node->rotate(angle, vec3(0, 0, 1));



		return endPoint;
	}



	void checkCamera(int vx, int vy)
	{
		float margin = HALFSIZE;
		if (listOfLines.size() == 0) return;

		float highestY = 0;
		for (int i = 0; i < listOfLines.size(); i++)
			if (listOfLines[i]->get_position().y() > highestY)
				highestY = listOfLines[i]->get_position().y();

		vec2 fov = app_scene->get_camera_instance(0)->getFov();
		vec3 cameraPos = app_scene->get_camera_instance(0)->get_node()->get_position();
		cameraPos.y() = 0;
		vec3 cameraOnPlane = vec3(cameraPos.x(), 0, 0);

		float lengthAdj = (cameraPos - cameraOnPlane).length();
		float lengthOppY = 2*(tan(fov.y()/2 * CL_M_PI / 180) * lengthAdj);
		//tan(fov.y()) = Opp/Adj
		//TOA


		while (highestY - margin   < -lengthOppY || highestY + margin   > lengthOppY)
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 2, 5.0f));
			cameraPos = app_scene->get_camera_instance(0)->get_node()->get_position();
			cameraPos.y() = 0;
			cameraOnPlane = vec3(cameraPos.x(), 0, 0);

			lengthAdj = (cameraPos - cameraOnPlane).length();
			lengthOppY = 2 * (tan(fov.y() / 2 * CL_M_PI / 180) * lengthAdj);
		}
	}

	void reset()
	{
		app_scene->reset();
		std::vector<ref<scene_node>>::iterator it;
		for (it = listOfLines.begin(); it != listOfLines.end();)
		{
			it = listOfLines.erase(it);
		}
		listOfLines.shrink_to_fit();
		app_scene = new visual_scene();
		app_scene->create_default_camera_and_lights();
		app_scene->get_camera_instance(0)->set_far_plane(100000.0f);
	}

	void handleInputs()
	{
		if (is_key_going_down(key_up))
		{
			currentFile = (currentFile + 1) % 6;
			currentIteration = 1;
			reset();
			loadFile();
		}
		else if (is_key_going_down(key_down))
		{
			currentFile = (currentFile - 1);
			currentIteration = 1;
			if (currentFile < 0) currentFile = 5;
			reset();
			loadFile();
		}
		else if (is_key_going_down(key_right))
		{
			if (currentIteration == iterations) return;
			currentIteration++;
			reset();
			generateTree();
		}
		else if (is_key_going_down(key_left))
		{
			if (currentIteration == 0) return;
			currentIteration--;
			reset();
			generateTree();
		}
		else if (is_key_going_down(key_space))
		{
			if (colorSchemeType == ALTERNATING) colorSchemeType = TREELIKE;
			else if (colorSchemeType == TREELIKE) colorSchemeType = RANDOM;
			else if (colorSchemeType == RANDOM) colorSchemeType = ALTERNATING;
			reset();
			generateTree();
		}
	}



	void updateText(int vx, int vy)
	{
		myInfoText->clear();
		// write some text to the overlay
		char buf[3][256];
		sprintf(buf[0], "%9d", currentFile+1);
		sprintf(buf[1], "%9d", currentIteration);
		sprintf(buf[2], "%9d", colorSchemeType + 1);

		myInfoText->format(
			"Current Tree: %s\n"
			"Current Iteration: %s\n"
			"Color Scheme: %s\n",
			buf[0],
			buf[1],
			buf[2]
			);

		// convert it to a mesh.
		myInfoText->update();
		// draw the text overlay
		myText->render(vx, vy);
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

	  checkCamera(vx, vy);

	  handleInputs();

	  updateText(vx, vy);
    }
  };
}
