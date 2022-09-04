#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in float textureIndex;
layout(location = 4) in float tilingFactor;

layout (std140) uniform CameraBlock
{
  mat4 uViewProjection;
};

uniform float uZIndex;

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
  gl_Position = uViewProjection * vec4(position, uZIndex, 1.0);
}
