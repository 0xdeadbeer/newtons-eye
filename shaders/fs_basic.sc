$input f_position

#include <bgfx_shader.sh>
#include <shaderlib.sh>

void main() {
  gl_FragColor = vec4(f_position.xyz, 1.0f);
}
