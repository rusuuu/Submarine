#version 330 core

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

out vec4 color;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float Ka = 0.0;
uniform float Kd = 0.5;
uniform float Ks = 0.5;
uniform float n = 16;

uniform sampler2D texture_diffuse;

void main( )
{
    vec3 ambient = Ka * lightColor;

    vec3 norm = normalize (Normal);
    vec3 lightDir = normalize(lightPos-FragPos);
    float diff = Kd * max(dot(norm, lightDir),0.0);

    vec3 viewDir = normalize (viewPos-FragPos);
    vec3 reflectDir = reflect (-lightDir, norm);
    float spec = Ks * pow(max(dot(viewDir, reflectDir),0.0),n);

    vec3 result = (ambient+diff+spec) * vec3(texture( texture_diffuse, TexCoords ));
    color = vec4( result, 1.0);

}
