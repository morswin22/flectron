#version 330 core

layout(location = 0) in vec2 position;

out vec2 vTextureCoord;

void main()
{
  vTextureCoord = (position + 1.0) * 0.5;
  gl_Position = vec4(position, 0.0, 1.0);
}
