#version 330 core

layout(location = 0) out vec4 color;

in vec2 vTextureCoord;
uniform sampler2D uLightTexture;
uniform sampler2D uRendererTexture;
uniform float uDarkness;

void main()
{
  color = mix(
    texture(uRendererTexture, vTextureCoord), 
    texture(uRendererTexture, vTextureCoord) * texture(uLightTexture, vec2(vTextureCoord.x, 1.0 - vTextureCoord.y)), 
    uDarkness
  );
}
