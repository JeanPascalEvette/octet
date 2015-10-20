////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class L_System : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	std::vector<material*> listOfMaterials;
	int currentMaterial;

	std::vector<scene_node*> listOfLines;
	std::vector<vec3> savedPointStack;

	const float HALFSIZE = 1.0f;
  public:
    /// this is called when we construct the class before everything is initialised.
    L_System(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  app_scene->get_camera_instance(0)->set_far_plane(1000.0f);

	  currentMaterial = 0;
	  material *red = new material(vec4(1, 0, 0, 1));
	  material *green = new material(vec4(0, 1, 0, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
	  material *white = new material(vec4(1, 1, 1, 1));
	  material *black = new material(vec4(0, 0, 0, 1));
	  savedPointStack = std::vector<vec3>();





	  listOfMaterials.push_back(red);
	  listOfMaterials.push_back(green);
	  listOfMaterials.push_back(blue);
	  listOfMaterials.push_back(white);
	  listOfMaterials.push_back(black);

	  vec3 nextPoint = drawLine(vec3(0),0.0f);

	  for (int i = 0; i < 3; i ++){
	  nextPoint = drawLine(nextPoint, 0.0f);
	  nextPoint = drawLine(nextPoint, 0.0f);
	  nextPoint = drawLine(nextPoint, 0.0f);
	  rememberPoint(nextPoint);
	  nextPoint = drawLine(nextPoint, -15.0f);
	  nextPoint = retrievePoint();
	  nextPoint = drawLine(nextPoint, 15.0f);
	  rememberPoint(nextPoint);
	  nextPoint = drawLine(nextPoint, 0.0f);
	  nextPoint = drawLine(nextPoint, 45.0f);
	  nextPoint = drawLine(nextPoint, 22.0f);
	  nextPoint = drawLine(nextPoint, -15.0f);
	  rememberPoint(nextPoint);
	  nextPoint = drawLine(nextPoint, 25.0f);
	  nextPoint = drawLine(nextPoint, -315.0f);
	  nextPoint = drawLine(nextPoint, 45.0f);
	  nextPoint = drawLine(nextPoint, -435.0f);
	  nextPoint = retrievePoint();
	  nextPoint = drawLine(nextPoint, 4445.0f);
	  nextPoint = drawLine(nextPoint, -451.0f);
	  nextPoint = retrievePoint();
	  nextPoint = drawLine(nextPoint, 4445.0f);
	  nextPoint = drawLine(nextPoint, -451.0f);
	  }
    }

	vec3 retrievePoint()
	{
		vec3 point = savedPointStack[savedPointStack.size() - 1];
		savedPointStack.pop_back();
		savedPointStack.shrink_to_fit();
		return point;
	}

	void rememberPoint(vec3 point)
	{
		savedPointStack.push_back(point);
	}

	vec3 drawLine(vec3 startingPoint, float angle = 0.0f)
	{
		material * color = listOfMaterials[currentMaterial];
		currentMaterial = (currentMaterial + 1) % listOfMaterials.size();
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
		mesh_cylinder *line = new mesh_cylinder(zcylinder(vec3(0), 0.1f, HALFSIZE), mat);
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
		vec3 cameraOnPlane = vec3(cameraPos.x(), cameraPos.y(), 0);

		float lengthAdj = (cameraPos - cameraOnPlane).length();
		float lengthOppY = tan(fov.y() / 2 * CL_M_PI / 180) * lengthAdj;
		//tan(fov.y()) = Opp/Adj
		//TOA


		if (highestY - margin   < -lengthOppY || highestY + margin   > lengthOppY)
		{
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 5.0f));

			for (int i = 0; i < listOfLines.size();i++)
				listOfLines[i]->scale(vec3(1.1f, 1, 1.1f));
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

	  checkCamera(vx, vy);

    }
  };
}
