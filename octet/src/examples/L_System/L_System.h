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
  public:
    /// this is called when we construct the class before everything is initialised.
    L_System(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();

	  currentMaterial = 0;
	  material *red = new material(vec4(1, 0, 0, 1));
	  material *green = new material(vec4(0, 1, 0, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
	  material *white = new material(vec4(1, 1, 1, 1));
	  material *black = new material(vec4(0, 0, 0, 1));





	  listOfMaterials.push_back(red);
	  listOfMaterials.push_back(green);
	  listOfMaterials.push_back(blue);
	  listOfMaterials.push_back(white);
	  listOfMaterials.push_back(black);

	  vec3 nextPoint = drawLine(vec3(0),0.0f);
	  nextPoint = drawLine(nextPoint, 0.0f);
	  nextPoint = drawLine(nextPoint, 0.0f);
	  nextPoint = drawLine(nextPoint, 0.0f);
	  nextPoint = drawLine(nextPoint, 0.0f);
	  nextPoint = drawLine(nextPoint, 45.0f);
	  nextPoint = drawLine(nextPoint, 22.0f);
	  nextPoint = drawLine(nextPoint, -15.0f);
	  nextPoint = drawLine(nextPoint, 25.0f);
	  nextPoint = drawLine(nextPoint, -315.0f);
	  nextPoint = drawLine(nextPoint, 45.0f);
	  nextPoint = drawLine(nextPoint, -435.0f);
	  nextPoint = drawLine(nextPoint, 4445.0f);
	  nextPoint = drawLine(nextPoint, -451.0f);
    }

	vec3 drawLine(vec3 startingPoint, float angle = 0.0f)
	{
		material * color = listOfMaterials[currentMaterial];
		currentMaterial = (currentMaterial + 1) % listOfMaterials.size();
		float halfSize = 1.0f;
		vec3 midPoint, endPoint;
		if (angle == 0.0f)
		{
			midPoint = startingPoint;
			midPoint.y() = midPoint.y() + halfSize;
			endPoint = startingPoint;
			endPoint.y() = endPoint.y() + 2.0f*halfSize;
		}
		else
		{
			midPoint = startingPoint;
			midPoint.x() = midPoint.x() + halfSize*cos((angle + 90) * CL_M_PI / 180);
			midPoint.y() = midPoint.y() + halfSize*sin((angle + 90)* CL_M_PI / 180);
			endPoint = startingPoint;
			endPoint.x() = endPoint.x() + 2.0f*halfSize*cos((angle + 90) * CL_M_PI / 180);
			endPoint.y() = endPoint.y() + 2.0f*halfSize*sin((angle + 90) * CL_M_PI / 180);
		}
		mat4t mat = mat4t();
		mat.loadIdentity();
		mat.rotate(90.0f, 1, 0, 0);
		mesh_cylinder *line = new mesh_cylinder(zcylinder(vec3(0), 0.1f, halfSize), mat);
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
		if (listOfLines.size() == 0) return;
		vec3 linePos = listOfLines[listOfLines.size() - 1]->get_position();

		vec2 fov = app_scene->get_camera_instance(0)->getFov();
		vec3 cameraPos = app_scene->get_camera_instance(0)->get_node()->get_position();
		vec3 cameraOnPlane = vec3(cameraPos.x(), cameraPos.y(), 0);

		float lengthAdj = (cameraPos - cameraOnPlane).length();
		float lengthOppY = tan(fov.y() / 2 * CL_M_PI / 180) * lengthAdj;
		//tan(fov.y()) = Opp/Adj
		//TOA


		if (linePos.y() < -lengthOppY || linePos.y() > lengthOppY)
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0,0,5.0f));
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
