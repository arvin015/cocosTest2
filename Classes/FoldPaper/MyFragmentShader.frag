uniform sampler2D diffuse;
uniform mediump vec3 lightDir;

varying mediump vec2 texCoord;
varying mediump vec3 Normal;
varying mediump vec4 fragColor;

void main (void)
{
    mediump vec3 n = normalize(Normal);
    mediump float dp = max(dot(lightDir, n), 0.0);
    mediump vec4 c0 = fragColor * texture2D(diffuse, texCoord.xy);
    gl_FragColor = (0.8 + 0.2 * dp) * c0;
    gl_FragColor.a = c0.a;
}
