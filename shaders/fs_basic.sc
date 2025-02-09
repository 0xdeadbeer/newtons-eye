$input f_position

#include <bgfx_shader.sh>
#include <shaderlib.sh>

void main() {
  float a = sin(f_position.y/10);
  float red = clamp(-pow((a+1), 2)+1, 0, 1);
  float green = clamp(-pow(a, 2)+1, 0, 1);
  float blue = clamp(-pow((a-1), 2)+1, 0, 1);
  gl_FragColor = vec4(red, green, blue, 1.0f);
}
