#version 330 core

layout(location = 0) out vec4 color;

in vec4 vColor;
in vec2 vTextureCoord;
in float vTextureIndex;
in float vTilingFactor;

// TODO set this value dynamicaly not hardcoded
uniform sampler2D uTextures[32];

void main()
{
  color = texture(uTextures[int(vTextureIndex)], vTextureCoord * vTilingFactor) * vColor;
}
