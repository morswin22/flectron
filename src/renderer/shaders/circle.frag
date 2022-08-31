#version 450 core

layout(location = 0) out vec4 color;

in vec2 vLocalPosition;
in vec4 vColor;
in float vThickness;
in float vFade;

void main()
{
  float dist = 1.0 - length(vLocalPosition);
  float circle = smoothstep(0.0, vFade, dist);
  circle *= smoothstep(vThickness + vFade, vThickness, dist);

  if (circle == 0.0)
    discard;

  color = vColor;
}