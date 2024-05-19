#version 330 core

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

out vec4 color;

uniform vec3 lightColor;
uniform vec3 sunlightPos;
uniform vec3 moonlightPos;
uniform vec3 goldLightPos1;
uniform vec3 goldLightPos2;
uniform vec3 goldLightPos3;
uniform vec3 goldLightPos4;
uniform vec3 goldLightPos5;
uniform vec3 goldLightPos6;
uniform vec3 viewPos;

uniform float Ka = 0.2;
uniform float sunKd = 0.8;
uniform float sunKs = 0.5;
uniform float sunN = 8;

uniform float moonKd = 0.5;
uniform float moonKs = 0.5;
uniform float moonN = 2;

uniform float goldKd = 0.6; // Difuzitate aur
uniform float goldKs = 0.9; // Specularitate aur
uniform float goldN = 128;  // Exponent specular aur

uniform sampler2D texture_diffuse;

uniform int indexTreasure = -1;

const float goldConstantAttenuation = 1.0;
const float goldLinearAttenuation = 0.09;
const float goldQuadraticAttenuation = 0.032;
float some_threshold_distance = 1.0;

void main( )
{
    vec3 ambient = Ka * lightColor;

    vec3 norm = normalize (Normal);
    vec3 sunlightDir = normalize(sunlightPos-FragPos);
    float sunDiff = sunKd * max(dot(norm, sunlightDir),0.0);
    vec3 moonlightDir = normalize(moonlightPos-FragPos);
    float moonDiff = moonKd * max(dot(norm, moonlightDir),0.0);
    vec3 goldlightDir1 = normalize(goldLightPos1-FragPos);
    float goldDiff1 = goldKd * max(dot(norm, goldlightDir1),0.0);
    vec3 goldlightDir2 = normalize(goldLightPos2-FragPos);
    float goldDiff2 = goldKd * max(dot(norm, goldlightDir2),0.0);
    vec3 goldlightDir3 = normalize(goldLightPos3-FragPos);
    float goldDiff3 = goldKd * max(dot(norm, goldlightDir3),0.0);
    vec3 goldlightDir4 = normalize(goldLightPos4-FragPos);
    float goldDiff4 = goldKd * max(dot(norm, goldlightDir4),0.0);
    vec3 goldlightDir5 = normalize(goldLightPos5-FragPos);
    float goldDiff5 = goldKd * max(dot(norm, goldlightDir5),0.0);
    vec3 goldlightDir6 = normalize(goldLightPos6-FragPos);
    float goldDiff6 = goldKd * max(dot(norm, goldlightDir6),0.0);

    vec3 viewDir = normalize (viewPos-FragPos);
    vec3 sunReflectDir = reflect (-sunlightDir, norm);
    float sunSpec = sunKs * pow(max(dot(viewDir, sunReflectDir),0.0),sunN);
    vec3 moonReflectDir = reflect (-moonlightDir, norm);
    float moonSpec = moonKs * pow(max(dot(viewDir, moonReflectDir),0.0),moonN);
    vec3 goldReflectDir1 = reflect (-goldlightDir1, norm);
    float goldSpec1 = goldKs * pow(max(dot(viewDir, goldReflectDir1),0.0),goldN);
    vec3 goldReflectDir2 = reflect (-goldlightDir2, norm);
    float goldSpec2 = goldKs * pow(max(dot(viewDir, goldReflectDir2),0.0),goldN);
    vec3 goldReflectDir3 = reflect (-goldlightDir3, norm);
    float goldSpec3 = goldKs * pow(max(dot(viewDir, goldReflectDir3),0.0),goldN);
    vec3 goldReflectDir4 = reflect (-goldlightDir4, norm);
    float goldSpec4 = goldKs * pow(max(dot(viewDir, goldReflectDir4),0.0),goldN);
    vec3 goldReflectDir5 = reflect (-goldlightDir5, norm);
    float goldSpec5 = goldKs * pow(max(dot(viewDir, goldReflectDir5),0.0),goldN);
    vec3 goldReflectDir6 = reflect (-goldlightDir6, norm);
    float goldSpec6 = goldKs * pow(max(dot(viewDir, goldReflectDir6),0.0),goldN);


    // Gold1 light attenuation
    float distance1 = length(goldLightPos1 - FragPos);
    float attenuation1 = 1.0 / (goldConstantAttenuation + goldLinearAttenuation * distance1 + goldQuadraticAttenuation * distance1 * distance1);

    goldDiff1 *= attenuation1;
    goldSpec1 *= attenuation1;

    if (indexTreasure==0)
        some_threshold_distance = 1.0;
    else
        some_threshold_distance = 0.0;
    if(distance1 > some_threshold_distance) {
        goldDiff1 = 0.0;
        goldSpec1 = 0.0;
    }

    // Gold2 light attenuation
    float distance2 = length(goldLightPos2 - FragPos);
    float attenuation2 = 1.0 / (goldConstantAttenuation + goldLinearAttenuation * distance2 + goldQuadraticAttenuation * distance2 * distance2);

    goldDiff2 *= attenuation2;
    goldSpec2 *= attenuation2;

    if (indexTreasure==1)
        some_threshold_distance = 1.0;
    else
        some_threshold_distance = 0.0;
    if(distance2 > some_threshold_distance) {
        goldDiff2 = 0.0;
        goldSpec2 = 0.0;
    }

    // Gold3 light attenuation
    float distance3 = length(goldLightPos3 - FragPos);
    float attenuation3 = 1.0 / (goldConstantAttenuation + goldLinearAttenuation * distance3 + goldQuadraticAttenuation * distance3 * distance3);

    goldDiff3 *= attenuation3;
    goldSpec3 *= attenuation3;

    if (indexTreasure==2)
        some_threshold_distance = 1.0;
    else
        some_threshold_distance = 0.0;
    if(distance3 > some_threshold_distance) {
        goldDiff3 = 0.0;
        goldSpec3 = 0.0;
    }

    // Gold4 light attenuation
    float distance4 = length(goldLightPos4 - FragPos);
    float attenuation4 = 1.0 / (goldConstantAttenuation + goldLinearAttenuation * distance4 + goldQuadraticAttenuation * distance4 * distance4);

    goldDiff4 *= attenuation4;
    goldSpec4 *= attenuation4;

    if (indexTreasure==3)
        some_threshold_distance = 1.0;
    else
        some_threshold_distance = 0.0;
    if(distance4 > some_threshold_distance) {
        goldDiff4 = 0.0;
        goldSpec4 = 0.0;
    }

    // Gold5 light attenuation
    float distance5 = length(goldLightPos5 - FragPos);
    float attenuation5 = 1.0 / (goldConstantAttenuation + goldLinearAttenuation * distance5 + goldQuadraticAttenuation * distance5 * distance5);

    goldDiff5 *= attenuation5;
    goldSpec5 *= attenuation5;

    if (indexTreasure==4)
        some_threshold_distance = 1.0;
    else
        some_threshold_distance = 0.0;
    if(distance5 > some_threshold_distance) {
        goldDiff5 = 0.0;
        goldSpec5 = 0.0;
    }

    // Gold6 light attenuation
    float distance6 = length(goldLightPos6 - FragPos);
    float attenuation6 = 1.0 / (goldConstantAttenuation + goldLinearAttenuation * distance6 + goldQuadraticAttenuation * distance6 * distance6);

    goldDiff6 *= attenuation6;
    goldSpec6 *= attenuation6;

    if (indexTreasure==5)
        some_threshold_distance = 1.0;
    else
        some_threshold_distance = 0.0;
    if(distance6 > some_threshold_distance) {
        goldDiff6 = 0.0;
        goldSpec6 = 0.0;
    }

    vec3 result = (ambient + sunDiff + sunSpec + moonDiff + moonSpec + goldDiff1 + goldSpec1 + goldDiff2 + goldSpec2 + goldDiff3 + goldSpec3 + goldDiff4 + goldSpec4 + goldDiff5 + goldSpec5 + goldDiff6 + goldSpec6) * vec3(texture(texture_diffuse, TexCoords));
    color = vec4(result, 1.0);

}
