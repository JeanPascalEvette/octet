//////////////////////////////////////////////////////////////////////////////////////////
//
// default frament shader for solid colours
//

// inputs
varying vec4 color_;
varying vec3 model_pos_;
uniform float height;

void main() {

	vec3 origin = vec3(0,0,0);
	float dist = length(origin - model_pos_);

	float innerMargin = 0.3;


	if(dist < innerMargin) 
	dist = innerMargin;
	float fraction = dist / height;

	//Gradient effect on the holes
	gl_FragColor = vec4(vec3(0.03,0.03,0.03) * fraction, 1);

}

