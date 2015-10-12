//////////////////////////////////////////////////////////////////////////////////////////
//
// default frament shader for solid colours
//

// inputs
varying vec4 color_;
uniform vec2 uResolution;

void main() {
// 1
vec2 center = vec2(uResolution.x/2., uResolution.y/2.);
 
// 2
float radius = uResolution.x/2.;
 
// 3
vec2 position = gl_FragCoord.xy - center;
 
// 4
if (length(position) > radius) {
  gl_FragColor = vec4(vec3(0.), 1.);
} else {
  gl_FragColor = vec4(vec3(1.), 1.);
}

  gl_FragColor = vec4(0,0,0,1);
}

