//////////////////////////////////////////////////////////////////////////////////////////
//
// custom frament shader
//

// inputs
varying vec3 normal_;
varying vec2 uv_;
varying vec4 color_;
varying vec3 model_pos_;
varying vec3 camera_pos_;

void main() { 
vec3 viewing = (model_pos_ - camera_pos_);
float edge = dot(normal_, -viewing);
// edge = clamp(edge, 0, 1);
// if(edge < 0.2)
// gl_FragColor = mix(vec3(0, 0, 0), diffuseColor, edge);


  gl_FragColor = color_;
}

