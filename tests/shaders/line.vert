#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

uniform mat4 uViewProjection;
uniform float uZIndex;

out vec4 vColor;

void main()
{
  vColor = color;
  gl_Position = uViewProjection * vec4(position, uZIndex, 1.0);
}
