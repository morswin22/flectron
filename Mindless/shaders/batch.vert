#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in float textureIndex;
layout(location = 4) in float tilingFactor;

uniform mat4 uMVP;

out vec4 vColor;
out vec2 vTextureCoord;
out float vTextureIndex;
out float vTilingFactor;

void main()
{
  vColor = color;
  vTextureCoord = textureCoord;
  vTextureIndex = textureIndex;
  vTilingFactor = tilingFactor;
  gl_Position = uMVP * vec4(position, 0.0, 1.0);
}
