$input a_position
$output f_position

uniform vec4 u_position;
uniform vec4 u_rotation;
uniform vec4 u_scale;

#include <bgfx_shader.sh>
#include <shaderlib.sh>

void main() {
  vec4 scale = mul(u_scale, vec4(a_position, 1.0f));
  vec4 view = mul(u_view, scale);
  vec4 pos = view + u_position;

  gl_Position = mul(u_proj, pos);
  f_position = a_position;
}
