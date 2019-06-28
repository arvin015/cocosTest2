uniform mediump vec3 lightDir;

varying mediump vec2 texCoord;
varying mediump vec3 Normal;
varying mediump vec4 fragColor;

vec3 computeLighting(vec3 normalVector, vec3 lightDirection, vec3 lightColor, float attenuation)
{
    float diffuse = max(dot(normalVector, lightDirection), 0.0);
    vec3 diffuseColor = lightColor * diffuse * attenuation;
    
    return diffuseColor;
}

void main (void)
{
    mediump vec3 n = normalize(Normal);
    
    vec4 combinedColor = vec4(0.9, 0.9, 0.9, 1.0);
    
    vec3 lightDirection = normalize(lightDir * 2.0);
    combinedColor.xyz += computeLighting(n, -lightDirection, vec3(0.15, 0.15, 0.15), 1.0);

    gl_FragColor = texture2D(CC_Texture0, texCoord) * fragColor * combinedColor;
}
