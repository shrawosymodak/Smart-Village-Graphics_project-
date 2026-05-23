#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 ourColor;
out vec3 Normal;
out vec2 TexCoord;
out vec3 vertexLitColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uvScale;

// ==================== GOURAUD SHADING UNIFORMS ====================
uniform vec3 objectColor;
uniform float shininess;
uniform vec3 viewPos;

uniform bool enableDirectionalLight;
uniform bool enablePointLights;
uniform bool enableSpotlight;
uniform bool enableAmbient;
uniform bool enableDiffuse;
uniform bool enableSpecular;

// Directional Light
uniform vec3 sunDirection;
uniform vec3 sunAmbient;
uniform vec3 sunDiffuse;
uniform vec3 sunSpecular;

// Point Lights
#define MAX_POINT_LIGHTS 16
uniform int numPointLights;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightColor[MAX_POINT_LIGHTS];
uniform float pointLightConstant;
uniform float pointLightLinear;
uniform float pointLightQuadratic;

// Spotlight
uniform bool spotlightActive;
uniform vec3 spotlightPos;
uniform vec3 spotlightDir;
uniform vec3 spotlightColor;
uniform float spotlightCutOff;
uniform float spotlightOuterCutOff;
uniform float spotlightIntensity;

void main()
{
    // World-space position for lighting
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Pass vertex color to fragment shader
    ourColor = aColor;

    // Transform normal to world space
    // Using normal matrix to handle non-uniform scaling correctly
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass texture coordinate to fragment shader
    float actualUvScale = (uvScale <= 0.0) ? 1.0 : uvScale;
    TexCoord = aTexCoord * actualUvScale;

    // Final clip-space position
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // ==================== GOURAUD SHADING (for Mode 2) ====================
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lighting = vec3(0.0);

    // Directional Light
    if (enableDirectionalLight) {
        vec3 lightDir = normalize(-sunDirection);
        vec3 ambient = vec3(0.0);
        if (enableAmbient) ambient = sunAmbient;
        vec3 diffuse = vec3(0.0);
        if (enableDiffuse) {
            float diff = max(dot(norm, lightDir), 0.0);
            diffuse = diff * sunDiffuse;
        }
        vec3 specular = vec3(0.0);
        if (enableSpecular) {
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
            specular = spec * sunSpecular;
        }
        lighting += ambient + diffuse + specular;
    }

    // Point Lights
    if (enablePointLights) {
        int lightCount = min(numPointLights, MAX_POINT_LIGHTS);
        for (int i = 0; i < lightCount; i++) {
            vec3 lPos = pointLightPos[i];
            vec3 lColor = pointLightColor[i];
            vec3 lightDir = normalize(lPos - FragPos);
            float dist = length(lPos - FragPos);
            float attenuation = 1.0 / (pointLightConstant + pointLightLinear * dist + pointLightQuadratic * dist * dist);

            vec3 ambient = vec3(0.0);
            if (enableAmbient) ambient = 0.1 * lColor * attenuation;
            vec3 diffuse = vec3(0.0);
            if (enableDiffuse) {
                float diff = max(dot(norm, lightDir), 0.0);
                diffuse = diff * lColor * attenuation;
            }
            vec3 specular = vec3(0.0);
            if (enableSpecular) {
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                specular = spec * lColor * 0.5 * attenuation;
            }
            lighting += ambient + diffuse + specular;
        }
    }

    // Spotlight
    if (enableSpotlight && spotlightActive) {
        vec3 lightDir = normalize(spotlightPos - FragPos);
        float theta = dot(lightDir, normalize(-spotlightDir));
        float epsilon = spotlightCutOff - spotlightOuterCutOff;
        float spotInt = clamp((theta - spotlightOuterCutOff) / epsilon, 0.0, 1.0);

        if (theta >= spotlightOuterCutOff) {
            float dist = length(spotlightPos - FragPos);
            float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
            float finalIntensity = attenuation * spotInt * spotlightIntensity;

            vec3 ambient = vec3(0.0);
            if (enableAmbient) ambient = 0.1 * spotlightColor * finalIntensity;
            vec3 diffuse = vec3(0.0);
            if (enableDiffuse) {
                float diff = max(dot(norm, lightDir), 0.0);
                diffuse = diff * spotlightColor * finalIntensity;
            }
            vec3 specular = vec3(0.0);
            if (enableSpecular) {
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
                specular = spec * spotlightColor * finalIntensity;
            }
            lighting += ambient + diffuse + specular;
        }
    }

    // Gouraud result: lighting * objectColor
    vertexLitColor = lighting * objectColor;
}