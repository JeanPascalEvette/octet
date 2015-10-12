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
// 1
vec2 center = vec2(model_pos_.x, model_pos_.y);
 
// 2
float radius = height/2.;
 
// 3
vec2 position = gl_FragCoord.xy - center;
 
// 4
if (length(position) > radius+1.0) {
  gl_FragColor = vec4(vec3(0.), 1.);
} else {
  gl_FragColor = vec4(vec3(1.), 1.);
}

  //gl_FragColor = vec4(0,0,0,1);
}

