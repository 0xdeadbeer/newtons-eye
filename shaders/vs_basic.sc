$input a_position
$output f_position

uniform vec4 u_position;
uniform vec4 u_rotation;
uniform vec4 u_scale;

#include <bgfx_shader.sh>
#include <shaderlib.sh>

void main() {
  // rotation
  mat3 x_rot = mat3(1, 0, 0, 
                  0, cos(u_rotation.x), -sin(u_rotation.x),
                  0, sin(u_rotation.x), cos(u_rotation.x));
  mat3 y_rot = mat3(cos(u_rotation.y), 0, sin(u_rotation.y), 
                  0, 1, 0, 
                  -sin(u_rotation.y), 0, cos(u_rotation.y));
  mat3 z_rot = mat3(cos(u_rotation.z), -sin(u_rotation.z), 0, 
                  sin(u_rotation.z), cos(u_rotation.z), 0, 
                  0, 0, 1);

  vec3 r_position = a_position;
  r_position = mul(r_position, x_rot);
  r_position = mul(r_position, y_rot);
  r_position = mul(r_position, z_rot);

  vec4 scale = mul(u_scale, vec4(r_position, 1.0f));
  vec4 view = mul(u_view, scale);
  vec4 pos = view + u_position;

  gl_Position = mul(u_proj, pos);
  f_position = a_position;
}
