#version 330 core

layout(location = 0) out vec4 color;

in vec2 vTextureCoord;
uniform sampler2D uRendererTexture;
uniform float uDarkness;

uniform vec2 uCameraPosition;
uniform vec2 uWindowSize;
uniform vec4 uBaseColor;

uniform vec4 uLightColor[128];
uniform vec3 uLightData[128];
uniform int uLightCount;

void main()
{    
  vec4 lightColor = uBaseColor;
  vec2 coords = vec2(
    ((vTextureCoord.x - 0.5) * uWindowSize.x) + uCameraPosition.x,
    ((vTextureCoord.y - 0.5) * uWindowSize.y) + uCameraPosition.y
  );

  float weights = 0;
  float maxWeight = 0.0;
  vec4 combinedColor = vec4(0.0, 0.0, 0.0, 0.0);
  for (int i = 0; i < uLightCount; i++) 
  {
    float dist = distance(uLightData[i].xy, coords);
    if (dist < uLightData[i].z)
    {
      float x = dist / uLightData[i].z;
      float weight = 1.0 - x*x*x;
      combinedColor += uLightColor[i] * weight;
      weights += weight;
      if (weight > maxWeight)
      {
        maxWeight = weight;
      }
    }
  }

  if (weights > 0)
  {
    // TODO fix the slightly visible darkness when lights overlap
    // the maxWeight currently is the max distance from the pixel to the edge of light
    // and it should take into account that when lights overlap, that distance should be
    // calculated as the max distance from the pixel to the closest edge of the combined lights
    lightColor = mix(vec4(uBaseColor.rgb, 1.0), combinedColor / weights, maxWeight);
  }

  color = mix(
    texture(uRendererTexture, vTextureCoord), 
    texture(uRendererTexture, vTextureCoord) * vec4(lightColor.rgb, uBaseColor.a), 
    uDarkness
  );
}
