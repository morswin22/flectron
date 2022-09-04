#version 450 core

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 uViewProjection;
uniform float uZIndex;

in vec4 vColor[];
in float vThickness[];
out vec4 fColor;

void main() {
  fColor = vColor[0];

  vec2 axis = normalize(gl_in[1].gl_Position.xy - gl_in[0].gl_Position.xy) * vThickness[0];
  vec2 normal = normalize(vec2(-axis.y, axis.x)) * vThickness[0];

  gl_Position = uViewProjection * vec4(gl_in[0].gl_Position.xy - normal - axis, uZIndex, 1.0);
  EmitVertex();

  gl_Position = uViewProjection * vec4(gl_in[0].gl_Position.xy + normal - axis, uZIndex, 1.0);
  EmitVertex();

  gl_Position = uViewProjection * vec4(gl_in[1].gl_Position.xy - normal + axis, uZIndex, 1.0);
  EmitVertex();

  gl_Position = uViewProjection * vec4(gl_in[1].gl_Position.xy + normal + axis, uZIndex, 1.0);
  EmitVertex();

  EndPrimitive();
}
