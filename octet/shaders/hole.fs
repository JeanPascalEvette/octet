//////////////////////////////////////////////////////////////////////////////////////////
//
// default frament shader for solid colours
//

// inputs
varying vec4 color_;
varying vec3 model_pos_;
uniform float height;
uniform vec3 pos;

void main() {

	vec3 origin = vec3(0,0,0);
	float dist = length(origin - model_pos_);
	float fraction = dist / height;


	gl_FragColor = vec4(vec3(0.2f,0.2f,0.2f) * fraction, 1);

}

