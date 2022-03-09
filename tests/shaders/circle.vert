#version 450 core

layout(location = 0) in vec2 worldPosition;
layout(location = 1) in vec2 localPosition;
layout(location = 2) in vec4 color;
layout(location = 3) in float thickness;
layout(location = 4) in float fade;

uniform mat4 uViewProjection;
uniform float uZIndex;

out vec2 vLocalPosition;
out vec4 vColor;
out float vThickness;
out float vFade;

void main()
{
  vLocalPosition = localPosition;
  vColor = color;
  vThickness = thickness;
  vFade = fade;

  gl_Position = uViewProjection * vec4(worldPosition, uZIndex, 1.0);
}
