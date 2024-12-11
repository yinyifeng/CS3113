uniform sampler2D diffuse;
uniform vec2 lightPosition;
uniform float lightRadius; // Declare as a uniform to allow dynamic updates

varying vec2 texCoordVar;
varying vec2 varPosition;

// Adjust these values to control brightness and radius
const float brightnessFactor = 1.0;     // Lower brightness (0.0 to 1.0)
const float linearAttenuation = 8.0;    // Higher values reduce radius
const float quadraticAttenuation = 2.0; // Higher values reduce radius faster

float attenuate(float dist, float a, float b)
{
    return 1.0 / (1.0 + (a * dist) + (b * dist * dist));
}

void main()
{
    float dist = distance(lightPosition, varPosition);

    // Set brightness to 0 if outside light radius
    float brightness = (dist < lightRadius) ? attenuate(dist, linearAttenuation, quadraticAttenuation) * brightnessFactor : 0.0;

    vec4 color = texture2D(diffuse, texCoordVar);
    gl_FragColor = vec4(color.rgb * brightness, color.a);
}
