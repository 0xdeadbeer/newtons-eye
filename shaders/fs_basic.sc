$input f_position

uniform vec4 u_firstBody;
uniform vec4 u_secondBody;
uniform vec4 u_thirdBody;
uniform vec4 u_fourthBody;
uniform vec4 u_initialSpeed;
uniform mat4 u_mass; 

#include <bgfx_shader.sh>
#include <shaderlib.sh>

float bodyRadius = 0.05f; 
float pointMass =  1; 
float firstBodyMass =  u_mass[0].x * pow(10, u_mass[0].y); 
float secondBodyMass = u_mass[1].x * pow(10, u_mass[1].y); 
float thirdBodyMass =  u_mass[2].x * pow(10, u_mass[2].y); 
float fourthBodyMass = u_mass[3].x * pow(10, u_mass[3].y); 

float g = 6.6 * pow(10, -11);

vec2 pointSpeed = u_initialSpeed.xy;
vec2 pointPosition = f_position.xy;
float counter = 0; 
float counterLimit = 500;

float ndist(float a, float b) {
  return max(a,b)-min(a,b); 
}
vec2 ndist(vec2 a, vec2 b) {
  return max(a,b)-min(a,b); 
}

int followPath() {
  do {
    float distanceToFirst = distance(pointPosition, u_firstBody.xy);
    float distanceToSecond = distance(pointPosition, u_secondBody.xy);
    float distanceToThird = distance(pointPosition, u_thirdBody.xy);
    float distanceToFourth = distance(pointPosition, u_fourthBody.xy);

    if (distanceToFirst < bodyRadius) {
      return 1;
    }

    if (distanceToSecond < bodyRadius) {
      return 2; 
    }

    if (distanceToThird < bodyRadius) {
      return 3; 
    }

    if (distanceToFourth < bodyRadius) {
      return 4; 
    }

    vec2 a = (g * firstBodyMass * (u_firstBody.xy - pointPosition))/pow(distanceToFirst, 3);
    a += (g * secondBodyMass * (u_secondBody.xy - pointPosition))/pow(distanceToSecond, 3);
    a += (g * thirdBodyMass * (u_thirdBody.xy - pointPosition))/pow(distanceToThird, 3);
    a += (g * fourthBodyMass * (u_fourthBody.xy - pointPosition))/pow(distanceToFourth, 3);

    pointSpeed += a; 

    pointPosition += pointSpeed;

    counter++; 
  } while (counter < counterLimit); 

  return 0; 
}

void main() {
  if (distance(f_position.xy, u_firstBody.xy) <= 0.01) {
    gl_FragColor = vec4(0.0f);
    return;
  }
  if (distance(f_position.xy, u_secondBody.xy) <= 0.01) {
    gl_FragColor = vec4(0.0f);
    return;
  }
  if (distance(f_position.xy, u_thirdBody.xy) <= 0.01) {
    gl_FragColor = vec4(0.0f); 
    return; 
  }
  if (distance(f_position.xy, u_fourthBody.xy) <= 0.01) {
    gl_FragColor = vec4(0.0f); 
    return; 
  }

  int hit = followPath(); 
  float computationCoefficient = counter/counterLimit; 

  if (hit == 1) {
    gl_FragColor = vec4(1.0f-computationCoefficient, 0.0f, 1.0f-computationCoefficient, 0.0f);
    return; 
  } 

  if (hit == 2) {
    gl_FragColor = vec4(0.0f, 1.0f-computationCoefficient, 0.0f, 1.0f);
    return; 
  }

  if (hit == 3) {
    gl_FragColor = vec4(1.0f-computationCoefficient, 0.0f, 0.0f, 1.0f);
    return; 
  }

  if (hit == 4) {
      gl_FragColor = vec4(0.6f-computationCoefficient, 0.8f-computationCoefficient, 0.0f, 1.0f);
    return; 
  }

  gl_FragColor = vec4(0.0f);
}
