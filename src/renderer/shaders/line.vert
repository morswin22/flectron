#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float thickness;

out vec4 vColor;
out float vThickness;

void main()
{
  vColor = color;
  vThickness = thickness;
  gl_Position = vec4(position, 0.0, 1.0);
}
