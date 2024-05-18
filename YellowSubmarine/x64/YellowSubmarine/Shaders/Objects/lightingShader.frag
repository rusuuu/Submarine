#version 330 core

in vec2 TexCoords;
uniform vec3 lightColor;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform float Ka = 1.0;

void main( )
{
    vec3 ambient = Ka * lightColor;
    vec4 texColor = texture(texture_diffuse, TexCoords);
    vec3 result = ambient * vec3(texColor);
    color = vec4(vec3(texColor), 1.0);
}
