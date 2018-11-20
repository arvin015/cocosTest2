attribute highp vec4 vertex;
attribute mediump vec4 uv;
attribute mediump vec4 normal;
attribute mediump vec4 color;

varying mediump vec2 texCoord;
varying mediump vec3 Normal;
varying mediump vec4 fragColor;

void main(void)
{
   texCoord = uv.st;
   highp vec4 n = vec4(normal.xyz, 0.0);
   Normal = n.xyz;
   fragColor = color;
   gl_Position = CC_MVPMatrix * vertex;
}
