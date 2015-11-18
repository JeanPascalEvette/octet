////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#include <sstream>
#include "tinyxml2.h"

namespace octet {
	class Model
	{
		std::string axiom;
		float angle;
		std::map<std::string, std::vector<std::pair<std::string, int>>> rules;
		int iterations;
		bool noFSystem;



	public:
		Model() {}
		Model(std::string myAxiom, float myAngle, std::map<std::string, std::vector<std::pair<std::string, int>>> myRules, int myIterations)
		{
			axiom = myAxiom;
			angle = myAngle;
			rules = myRules;
			iterations = myIterations;
			noFSystem = true;
			std::map<std::string, std::vector<std::pair<std::string, int>>>::iterator it;
			for (it = myRules.begin(); it != myRules.end();)
			{
				std::vector<std::pair<std::string, int>>::iterator it2;
				for (it2 = it->second.begin(); it2 != it->second.end();)
				{
					if (it2->first.find('F') != -1)
						noFSystem = false;
					it2++;
				}
				it++;
			}
		}
		std::string getAxiom() { return axiom; }
		float getAngle() {
			return angle;
		}
		std::map<std::string, std::vector<std::pair<std::string, int>>> getRules() {
			return rules;
		}
		int getIterations() {
			return iterations;
		}
		void increaseAngle() { angle += 5.0f; }
		void reduceAngle() { angle -= 5.0f; }
		bool isNoFSystem() {
			return noFSystem;
		}
	};

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

	std::vector<std::pair<vec3, float>> savedPointStack;
	std::vector<std::pair<vec3, float>> listLines;

	Model currentModel;


	vec3 nextPoint;
	int currentFile;
	int currentIteration;
	float additionalThickness;
	float lineHalfLength;
	const int FILENUMBER = 10;

	float highestY, lowestY;
	float highestX, lowestX;

	enum ColorScheme {
		ALTERNATING,
		TREELIKE,
		RANDOM
	};
	ColorScheme colorSchemeType;

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
	  lineHalfLength = 1.0f;
	  additionalThickness = 0.0f;
	  listLines = std::vector<std::pair<vec3, float>>();


	  highestY = 0;
	  lowestY = 0;
	  highestX = 0;
	  lowestX = 0;


	  aabb bb(vec3(144.5f, 305.0f, 0.0f), vec3(256, 64, 0));
	  myText = new text_overlay();
	  myInfoText = new mesh_text(myText->get_default_font(), "", &bb);
	  myText->add_mesh_text(myInfoText);



	  loadFile();


    }

	// Loading data from specified file. The file loaded depends on the currentFile variable.
	void loadFile()
	{

		std::vector<std::pair<std::string, int>> rules = std::vector<std::pair<std::string, int>>();
		std::map<std::string, std::vector<std::pair<std::string, int>>> decodedRules = std::map<std::string, std::vector<std::pair<std::string, int>>>();

		tinyxml2::XMLDocument doc;
		char aChar =  65 + currentFile;
		std::string fileName = "data";
		fileName += aChar;
		fileName += ".xml";
		doc.LoadFile(fileName.c_str());
		std::string  axiom = doc.FirstChildElement("Data")->FirstChildElement("Axiom")->GetText();
		float setupAngle = atof(doc.FirstChildElement("Data")->FirstChildElement("Angle")->GetText());
		tinyxml2::XMLNode * el = doc.FirstChildElement("Data")->FirstChildElement("Rules")->FirstChildElement();
		while (el != nullptr)
		{
			int percentage = 100;
			el->ToElement()->QueryIntAttribute("chance", &percentage);
			rules.push_back(std::pair<std::string, int>(el->ToElement()->GetText(), percentage));
			el = el->NextSibling();
		}

		//Storing the data in a vector
		//Structure : 
		//	{ F ->	{	{F[[XF]F]	, 20},
		//				{FF			, 80}	 }
		//	  F>X ->{	{FXX		, 100}	 }
		//	}
		//
		for (int i = 0;i < rules.size(); i++)
		{
			std::string pre = (rules[i].first.substr(0, rules[i].first.find("->")));
			std::pair<std::string, int> post = std::pair<std::string, int>(rules[i].first.substr(rules[i].first.find("->") + 2, rules[i].first.size()), rules[i].second);
			decodedRules[pre].push_back(post);

		}
		int iterations = atoi(doc.FirstChildElement("Data")->FirstChildElement("Iterations")->GetText());
		currentModel = Model(axiom, setupAngle, decodedRules, iterations);
		generateTree();
	}

	void generateTree()
	{
		currentMaterial = 0;
		material *red = new material(vec4(1, 0, 0, 1));
		material *green = new material(vec4(0, 1, 0, 1));
		material *blue = new material(vec4(0, 0, 1, 1));
		material *white = new material(vec4(1, 1, 1, 1));
		material *black = new material(vec4(0, 0, 0, 1));
		darkGreen = new material(vec4(0, 0.5f, 0, 1));
		brown = new material(vec4(0.2f, 0.2f, 0.2f,1));
		savedPointStack = std::vector<std::pair<vec3, float>>();
		currentTreeMaterial = brown;


		listOfMaterials = std::vector<material*>();

		listOfMaterials.push_back(red);
		listOfMaterials.push_back(green);
		listOfMaterials.push_back(blue);
		listOfMaterials.push_back(white);
		listOfMaterials.push_back(black);
		std::string startingAxiom = currentModel.getAxiom();
		std::map<std::string, std::vector<std::pair<std::string, int>>> rules = currentModel.getRules();
		nextPoint = vec3(0, 0, 0);
		//For each iteration
		for (int i = 0; i < currentIteration; i++)
		{
			std::string newAxiom = "";
			//And for each letter in the current string
			for (int u = 0; u < startingAxiom.size(); u++)
			{
				std::string currentChar = std::string(1,startingAxiom[u]);
				bool found = false;

				//Go through each rule and apply the one that corresponds
				std::map<std::string, std::vector<std::pair<std::string, int>>>::iterator it;
				for (it = rules.begin(); it != rules.end();)
				{
					std::string prev = "";
					std::string post = "";
					if(it->first.find("<") != -1)
						prev = it->first.substr(0, it->first.find("<"));
					if (it->first.find(">") != -1)
						post = it->first.substr(it->first.find(">") + 1, it->first.size() - (it->first.find(">")+1));

					//Checking for context sensitive properties
					bool b1 = (it->first.find("<") == -1 || (u >= prev.size() && startingAxiom.substr(u - prev.size(), prev.size()) == prev));
					bool b2 = (it->first.find(">") == -1 || (u + post.size() <= startingAxiom.size() && startingAxiom.substr(u + 1, u + 1 + post.size()) == post));
					bool b3 = (it->first == currentChar || it->first.substr(prev.size() + 1, 1) == (currentChar));
					if( 
						b1 &&
						b2 && 
						b3
						)
					{

						//Checking for stochastic properties
						int maxPercent = 0;
						for (int i = 0; i < it->second.size(); i++)
							maxPercent += it->second[i].second;
						int percent = rand() % maxPercent;
						int cumulatedPercent = 0;
						for (int i = 0; i < it->second.size(); i++)
						{
							cumulatedPercent += it->second[i].second;
							if(percent <= cumulatedPercent)
								newAxiom += it->second[i].first;
						}
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

		//Then when the final string has been generated - draw the tree

		//First try to guess where the branches are for the coloring
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

		//Then apply the right step depending on the letter
		for (int i = 0; i < startingAxiom.size(); i++)
		{
			if (startingAxiom[i] == 'F' || (currentModel.isNoFSystem() && (startingAxiom[i] == 'A')))
			{
				for (int u = 0; u < listBranches.size(); u++)
					if (listBranches[u] == i)
						currentTreeMaterial = darkGreen;
				nextPoint = drawLine(nextPoint, angle);
			}
			else if (startingAxiom[i] == '+')
			{
				angle += currentModel.getAngle();
			}
			else if (startingAxiom[i] == '-')
			{
				angle -= currentModel.getAngle();
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

		std::pair<vec3, float> retVal = savedPointStack[savedPointStack.size() - 1];
		savedPointStack.pop_back();
		savedPointStack.shrink_to_fit();
		return retVal;
	}

	void rememberPoint(vec3 point, float angle)
	{
		std::pair<vec3, float> newPoint = std::pair<vec3, float>(point, angle);
		savedPointStack.push_back(newPoint);
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
		
			midPoint = startingPoint;
			midPoint.x() = midPoint.x() + lineHalfLength *cos((angle + 90) * CL_M_PI / 180);
			midPoint.y() = midPoint.y() + lineHalfLength *sin((angle + 90)* CL_M_PI / 180);
			endPoint = startingPoint;
			endPoint.x() = endPoint.x() + 2.0f*lineHalfLength *cos((angle + 90) * CL_M_PI / 180);
			endPoint.y() = endPoint.y() + 2.0f*lineHalfLength *sin((angle + 90) * CL_M_PI / 180);
		

			//This is designed to prevent drawing the same line twice to improve performance
			std::pair<vec3, float> currentLine = std::pair<vec3, float>(startingPoint, angle);
			std::vector<std::pair<vec3, float>>::iterator it;
			for (it = listLines.begin(); it != listLines.end();)
			{
				if (it->first.x() == currentLine.first.x() &&
					it->first.y() == currentLine.first.y() && 
					it->first.z() == currentLine.first.z() && 
					it->second == currentLine.second)
				{
					return endPoint;
				}
				it++;
			}
			listLines.push_back(currentLine);
			if (startingPoint.y() > highestY)
				highestY = startingPoint.y();
			if (startingPoint.y() < lowestY)
				lowestY = startingPoint.y();
			if (endPoint.y() > highestY)
				highestY = endPoint.y();
			if (endPoint.y() < lowestY)
				lowestY = endPoint.y();

			if (startingPoint.x() > highestX)
				highestX = startingPoint.x();
			if (startingPoint.x() < lowestX)
				lowestX = startingPoint.x();
			if (endPoint.x() > highestX)
				highestX = endPoint.x();
			if (endPoint.x() < lowestX)
				lowestX = endPoint.x();

		mat4t mat = mat4t();
		mat.loadIdentity();
		mat.rotate(90.0f, 1, 0, 0);

		mesh_box *line;
		if (currentModel.getIterations() == 7)
			line = new mesh_box(vec3(additionalThickness + 0.5f, 0.1f, lineHalfLength), mat);
		else if (currentModel.getIterations() == 6)
			line = new mesh_box(vec3(additionalThickness + 0.3f, 0.1f, lineHalfLength), mat);
		else
			line = new mesh_box(vec3(additionalThickness + 0.2f, 0.1f, lineHalfLength), mat);
		scene_node *node = new scene_node();
		app_scene->add_child(node);
		app_scene->add_mesh_instance(new mesh_instance(node, line, color));
		node->translate(midPoint);
		node->rotate(angle, vec3(0, 0, 1));



		return endPoint;
	}



	void checkCamera(int vx, int vy)
	{
		float margin = lineHalfLength;

		float sceneHeightY = highestY - lowestY;
		float sceneHeightX = highestX - lowestX;
		float highestHeight = sceneHeightY;
		if (sceneHeightX > highestHeight) highestHeight = sceneHeightX;

		vec3 currentCamPos = app_scene->get_camera_instance(0)->get_node()->get_position();
		vec3 newCamPos = -currentCamPos + vec3(highestX - sceneHeightX / 2, highestY - sceneHeightY / 2 , highestHeight / 80.0f * 100.0f);
		app_scene->get_camera_instance(0)->get_node()->translate(newCamPos);

		return;


		// No longer used - this is the previous way I implemented the placement of the camera.

		vec2 fov = app_scene->get_camera_instance(0)->getFov();
		vec3 cameraPos = app_scene->get_camera_instance(0)->get_node()->get_position();
		cameraPos.y() = 0;
		vec3 cameraOnPlane = vec3(cameraPos.x(), 0, 0);

		float lengthAdj = (cameraPos - cameraOnPlane).length();
		float lengthOppY = 2*(tan(fov.y()/2 * CL_M_PI / 180) * lengthAdj);


		float newLength = 4.0f * app_scene->get_camera_instance(0)->get_node()->get_position().z() / 34.6410179f;


		while (highestY - margin   < -lengthOppY || highestY + margin   > lengthOppY)
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 2, 5.0f));
			cameraPos = app_scene->get_camera_instance(0)->get_node()->get_position();
			cameraPos.y() = 0;
			cameraOnPlane = vec3(cameraPos.x(), 0, 0);

			lengthAdj = (cameraPos - cameraOnPlane).length();
			lengthOppY = 2 * (tan(fov.y() / 2 * CL_M_PI / 180) * lengthAdj);
		}


		while (lowestY < -newLength)
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.0f));
			newLength = 4.0f * app_scene->get_camera_instance(0)->get_node()->get_position().z() / 34.6410179f;
		}
	}

	void reset()
	{
		highestY = 0;
		lowestY = 0;
		highestX = 0;
		lowestX = 0;

		app_scene->reset();
		app_scene->release();
		app_scene = new visual_scene();
		app_scene->create_default_camera_and_lights();
		app_scene->get_camera_instance(0)->set_far_plane(1000000000.0f);

		listLines.clear();
	}

	void handleInputs()
	{
		if (is_key_going_down(key_up))
		{
			currentFile = (currentFile + 1) % FILENUMBER;
			currentIteration = 1;
			reset();
			loadFile();
		}
		else if (is_key_going_down(key_down))
		{
			currentFile = (currentFile - 1);
			currentIteration = 1;
			if (currentFile < 0) currentFile = FILENUMBER - 1;
			reset();
			loadFile();
		}
		else if (is_key_going_down(key_right))
		{
			if (currentIteration == currentModel.getIterations()) return;
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
		else if (is_key_going_down(key_f1))
		{
			lineHalfLength += 0.2f;
			reset();
			generateTree();
		}
		else if (is_key_going_down(key_f2))
		{
			lineHalfLength -= 0.2f;
			reset();
			generateTree();
		}
		else if (is_key_going_down(key_f3))
		{
			currentModel.increaseAngle();
			reset();
			generateTree();
		}
		else if (is_key_going_down(key_f4))
		{
			currentModel.reduceAngle();
			reset();
			generateTree();
		}
		else if (is_key_going_down(key_f5))
		{
			additionalThickness += 0.1f;
			reset();
			generateTree();
		}
		else if (is_key_going_down(key_f6))
		{
			additionalThickness -= 0.1f;
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

	  handleInputs();

	  checkCamera(vx, vy);

	  updateText(vx, vy);
    }
  };
}
