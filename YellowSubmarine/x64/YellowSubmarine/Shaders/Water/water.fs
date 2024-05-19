#version 330 core

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

out vec4 color;

uniform vec3 lightColor;
uniform vec3 sunlightPos;
uniform vec3 moonlightPos;
uniform vec3 viewPos;

uniform float Ka = 0.2;
uniform float sunKd = 0.8;
uniform float sunKs = 0.5;
uniform float sunN = 8;

uniform float moonKd = 0.5;
uniform float moonKs = 0.5;
uniform float moonN = 2;

uniform sampler2D texture_diffuse;
uniform float opacity;

void main()
{
    vec3 ambient = Ka * lightColor;

    vec3 norm = normalize(Normal);
    vec3 sunlightDir = normalize(sunlightPos - FragPos);
    float sunDiff = sunKd * max(dot(norm, sunlightDir), 0.0);
    vec3 moonlightDir = normalize(moonlightPos - FragPos);
    float moonDiff = moonKd * max(dot(norm, moonlightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 sunReflectDir = reflect(-sunlightDir, norm);
    float sunSpec = sunKs * pow(max(dot(viewDir, sunReflectDir), 0.0), sunN);
    vec3 moonReflectDir = reflect(-moonlightDir, norm);
    float moonSpec = moonKs * pow(max(dot(viewDir, moonReflectDir), 0.0), moonN);

    vec3 result = (ambient + sunDiff + sunSpec + moonDiff + moonSpec) * vec3(texture(texture_diffuse, TexCoords));
    color = vec4(result, opacity);
 }