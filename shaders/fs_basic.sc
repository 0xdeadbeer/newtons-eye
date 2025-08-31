$input f_position

uniform vec4 u_params; 
uniform vec4 u_sparams; 
uniform mat4 u_positions; 
uniform vec4 u_initialSpeed;
uniform mat4 u_mass; 

uniform mat3 u_colors1;
uniform mat3 u_colors2;
uniform mat3 u_colors3;

#include <bgfx_shader.sh>
#include <shaderlib.sh>

float bodyRadius = u_params.w; 
float pointMass =  u_params.x; 
int planets = int(u_sparams.x); 

vec3 color_stripe[8] = {
  u_colors1[0],
  u_colors1[1],
  u_colors1[2],
  u_colors2[0],
  u_colors2[1],
  u_colors2[2],
  u_colors3[0],
  u_colors3[1]
};

float masses[8] = {
  u_mass[0].x * pow(10, u_mass[0].y), 
  u_mass[1].x * pow(10, u_mass[1].y), 
  u_mass[2].x * pow(10, u_mass[2].y), 
  u_mass[3].x * pow(10, u_mass[3].y),
  u_mass[0].z * pow(10, u_mass[0].w), 
  u_mass[1].z * pow(10, u_mass[1].w), 
  u_mass[2].z * pow(10, u_mass[2].w), 
  u_mass[3].z * pow(10, u_mass[3].w) 
};

vec4 pos[8] = {
  vec4(u_positions[0].xy, 0.0f, 0.0f), 
  vec4(u_positions[1].xy, 0.0f, 0.0f),
  vec4(u_positions[2].xy, 0.0f, 0.0f),
  vec4(u_positions[3].xy, 0.0f, 0.0f),
  vec4(u_positions[0].zw, 0.0f, 0.0f), 
  vec4(u_positions[1].zw, 0.0f, 0.0f),
  vec4(u_positions[2].zw, 0.0f, 0.0f),
  vec4(u_positions[3].zw, 0.0f, 0.0f)
};

float g = 6.6 * pow(10, -11);

float speedClamp = u_params.y; 
vec2 pointSpeed = u_initialSpeed.xy;
vec2 pointPosition = f_position.xy;
float counterLimit = u_params.z;
float counter = 0; 

int followPath() {
  do {
    float distances[8] = {
      distance(pointPosition, pos[0].xy),
      distance(pointPosition, pos[1].xy),
      distance(pointPosition, pos[2].xy),
      distance(pointPosition, pos[3].xy),
      distance(pointPosition, pos[4].xy),
      distance(pointPosition, pos[5].xy),
      distance(pointPosition, pos[6].xy),
      distance(pointPosition, pos[7].xy)
    };

    for (int i = 0; i < planets; i++) {
      if (distances[i] < bodyRadius) {
        return i; 
      }
    }

    vec2 a = vec2(0.0f); 

    for (int i = 0; i < planets; i++) {
      a += (g * pointMass * masses[i] * (pos[i].xy - pointPosition)) / pow(distances[i], 3);
    }

    if (length(pointSpeed+a) > speedClamp) {
      pointSpeed += a; 
      pointSpeed = normalize(pointSpeed) * speedClamp; 
    } else {
      pointSpeed += a; 
    }

    pointPosition += pointSpeed;

    counter++; 
  } while (counter < counterLimit); 

  return -1; 
}

void main() {
  for (int i = 0; i < planets; i++) {
    if (distance(f_position.xy, pos[i].xy) <= bodyRadius) {
      gl_FragColor = vec4(0.0f);
      return; 
    }
  }

  int hit = followPath(); 
  if (hit == -1) {
    gl_FragColor = vec4(0.0f); 
    return; 
  }
  float computationCoefficient = counter/counterLimit; 

  gl_FragColor = vec4(color_stripe[hit].x-computationCoefficient*color_stripe[hit].x, color_stripe[hit].y-computationCoefficient*color_stripe[hit].y, color_stripe[hit].z-computationCoefficient*color_stripe[hit].z, 1.0f);
}
