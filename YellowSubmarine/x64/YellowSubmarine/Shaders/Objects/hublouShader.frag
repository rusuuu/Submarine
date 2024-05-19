#version 330 core

in vec2 TexCoords;
uniform vec3 lightColor;

out vec4 color;

uniform sampler2D texture_diffuse;
uniform float Ka = 1.0;

void main( )
{
    vec3 ambient = Ka * lightColor;
    vec3 result = ambient * vec3( texture( texture_diffuse, TexCoords ));
    color = vec4(result, 0.2);
}
