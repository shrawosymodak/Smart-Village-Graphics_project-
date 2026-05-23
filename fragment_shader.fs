#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 ourColor;
in vec3 Normal;
in vec2 TexCoord;
in vec3 vertexLitColor;

// Material properties
uniform vec3 objectColor;
uniform float shininess;
uniform vec3 viewPos;
uniform bool isNightMode;

// Texture
uniform sampler2D ourTexture;
uniform int textureMode;
uniform bool useTriplanar;
uniform float triplanarScale;

// ==================== LIGHT TOGGLE UNIFORMS ====================
uniform bool enableDirectionalLight;
uniform bool enablePointLights;
uniform bool enableSpotlight;
uniform bool enableAmbient;
uniform bool enableDiffuse;
uniform bool enableSpecular;

// ==================== DIRECTIONAL LIGHT (SUN/MOON) ====================
uniform vec3 sunDirection;
uniform vec3 sunAmbient;
uniform vec3 sunDiffuse;
uniform vec3 sunSpecular;

// ==================== POINT LIGHTS (STREET LAMPS) ====================
#define MAX_POINT_LIGHTS 16
uniform int numPointLights;
uniform vec3 pointLightPos[MAX_POINT_LIGHTS];
uniform vec3 pointLightColor[MAX_POINT_LIGHTS];
uniform float pointLightConstant;
uniform float pointLightLinear;
uniform float pointLightQuadratic;

// ==================== SPOTLIGHT ====================
uniform bool spotlightActive;
uniform vec3 spotlightPos;
uniform vec3 spotlightDir;
uniform vec3 spotlightColor;
uniform float spotlightCutOff;
uniform float spotlightOuterCutOff;
uniform float spotlightIntensity;

// ==================== EMISSIVE ====================
uniform bool isEmissive;
uniform vec3 emissiveColor;

// ==================== PHONG LIGHTING FUNCTIONS ====================

vec3 CalcDirectionalLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-sunDirection);
    
    // Ambient component
    vec3 ambient = vec3(0.0);
    if (enableAmbient) {
        ambient = sunAmbient;
    }
    
    // Diffuse component (Lambertian)
    vec3 diffuse = vec3(0.0);
    if (enableDiffuse) {
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse = diff * sunDiffuse;
    }
    
    // Specular component (Phong)
    // Formula: Specular = Is * (V � R)^n where R = reflect(-L, N)
    vec3 specular = vec3(0.0);
    if (enableSpecular) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        specular = spec * sunSpecular;
    }
    
    return ambient + diffuse + specular;
}

vec3 CalcPointLight(int index, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // Bounds check to prevent array access issues
    if (index < 0 || index >= MAX_POINT_LIGHTS) {
        return vec3(0.0);
    }
    
    vec3 lightPos = pointLightPos[index];
    vec3 lightColor = pointLightColor[index];
    
    vec3 lightDir = normalize(lightPos - fragPos);
    
    // Attenuation
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * (distance * distance));
    
    // Ambient
    vec3 ambient = vec3(0.0);
    if (enableAmbient) {
        ambient = 0.1 * lightColor * attenuation;
    }
    
    // Diffuse
    vec3 diffuse = vec3(0.0);
    if (enableDiffuse) {
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse = diff * lightColor * attenuation;
    }
    
    // Specular (Phong reflection model)
    vec3 specular = vec3(0.0);
    if (enableSpecular) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        specular = spec * lightColor * 0.5 * attenuation;
    }
    
    return ambient + diffuse + specular;
}

vec3 CalcSpotlight(vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(spotlightPos - fragPos);
    
    // Check spotlight cone
    float theta = dot(lightDir, normalize(-spotlightDir));
    float epsilon = spotlightCutOff - spotlightOuterCutOff;
    float intensity = clamp((theta - spotlightOuterCutOff) / epsilon, 0.0, 1.0);
    
    // Outside the cone
    if (theta < spotlightOuterCutOff) {
        return vec3(0.0);
    }
    
    // Distance attenuation
    float distance = length(spotlightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));
    
    // Combine intensity factors
    float finalIntensity = attenuation * intensity * spotlightIntensity;
    
    // Ambient
    vec3 ambient = vec3(0.0);
    if (enableAmbient) {
        ambient = 0.1 * spotlightColor * finalIntensity;
    }
    
    // Diffuse
    vec3 diffuse = vec3(0.0);
    if (enableDiffuse) {
        float diff = max(dot(normal, lightDir), 0.0);
        diffuse = diff * spotlightColor * finalIntensity;
    }
    
    // Specular (Phong)
    vec3 specular = vec3(0.0);
    if (enableSpecular) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        specular = spec * spotlightColor * finalIntensity;
    }
    
    return ambient + diffuse + specular;
}

// ==================== PHONG LIGHTING HELPER ====================
vec3 CalcPhongLighting(vec3 norm, vec3 viewDir)
{
    vec3 result = vec3(0.0);

    if (enableDirectionalLight)
        result += CalcDirectionalLight(norm, viewDir);

    if (enablePointLights) {
        int lightCount = min(numPointLights, MAX_POINT_LIGHTS);
        for (int i = 0; i < lightCount; i++)
            result += CalcPointLight(i, norm, FragPos, viewDir);
    }

    if (enableSpotlight && spotlightActive)
        result += CalcSpotlight(norm, FragPos, viewDir);

    return result;
}

void main()
{
    // Handle emissive objects first (sun, lamp bulbs, sky)
    if (isEmissive) {
        vec4 emTex = texture(ourTexture, TexCoord);
        FragColor = vec4(emissiveColor * emTex.rgb, 1.0);
        return;
    }

    // Sample texture
    vec4 texColor;
    if (useTriplanar) {
        vec2 uv;
        vec3 absN = abs(normalize(Normal));
        
        if (absN.x > absN.y && absN.x > absN.z) {
            uv = FragPos.yz;
        } else if (absN.y > absN.x && absN.y > absN.z) {
            uv = FragPos.xz;
        } else {
            uv = FragPos.xy;
        }
        
        float scale = (triplanarScale > 0.0) ? triplanarScale : 0.5;
        texColor = texture(ourTexture, uv * scale);
    } else {
        texColor = texture(ourTexture, TexCoord);
    }
    
    // Detect if a real texture is bound (vs default 1x1 white)
    bool hasRealTexture = !(texColor.r > 0.95 && texColor.g > 0.95 && texColor.b > 0.95);

    if (textureMode == 1) {
        // ===== Mode 1 (N): Simple Texture + Phong Lighting =====
        // Formula: FragColor = texture * lightingResult
        // No color blending with vertex/object color
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 lighting = CalcPhongLighting(norm, viewDir);

        if (hasRealTexture) {
            FragColor = vec4(texColor.rgb * lighting, texColor.a);
        } else {
            // Fallback: show objectColor with lighting
            FragColor = vec4(objectColor * lighting, 1.0);
        }

    } else if (textureMode == 2) {
        // ===== Mode 2 (M): Blended Gouraud (Vertex Calculation) =====
        // Formula: FragColor = texture * vec4(interpolatedVertexColor, 1.0)
        // Lighting was computed per-vertex in the Vertex Shader
        if (hasRealTexture) {
            FragColor = texColor * vec4(vertexLitColor, 1.0);
        } else {
            // Fallback: show Gouraud-shaded color
            FragColor = vec4(vertexLitColor, 1.0);
        }

    } else {
        // ===== Mode 3 (O): Blended Phong (Fragment Calculation) =====
        // Formula: FragColor = texture * vec4(objectColor, 1.0) * lightingResult
        // Lighting computed per-fragment (Phong Shading)
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 lighting = CalcPhongLighting(norm, viewDir);

        if (hasRealTexture) {
            FragColor = vec4(texColor.rgb * objectColor * lighting, texColor.a);
        } else {
            // Fallback: objectColor with Phong lighting
            FragColor = vec4(objectColor * lighting, 1.0);
        }
    }

    // Clamp final output
    FragColor = vec4(clamp(FragColor.rgb, 0.0, 1.0), FragColor.a);
}