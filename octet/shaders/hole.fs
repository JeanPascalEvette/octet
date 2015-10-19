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


  gl_FragColor = vec4(0,0,0,1);
}

