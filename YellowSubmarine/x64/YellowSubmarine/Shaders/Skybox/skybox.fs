#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform vec3 lightColor;

uniform float Ka = 0.2;
uniform samplerCube skybox;

void main()
{    
    vec3 ambient = Ka * lightColor;
    vec3 result = ambient * vec3(texture(skybox, TexCoords));
    FragColor = vec4( result, 1.0);
}