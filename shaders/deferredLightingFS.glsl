#version 460 core

in vec2 texCoords;

out vec4 fragColor;

// Create all light data structs using vec4 so that std140 UBO easy to use
struct DirLight{
    vec4 direction;
    vec4 diffuse;
    vec4 specular;
};

struct PointLight{
	vec4 position;
	vec4 attenuation; //constant, linear, quadratic
	vec4 color;
};

struct SpotLight{
	vec4 position;
	vec4 direction;

	vec4 cutOffs;     // innerCutOff, outerCutOff
	vec4 attenuation; //constant, linear, quadratic

	vec4 diffuse;
	vec4 specular;
};

#define MAX_NUMBER_POINT_LIGHTS 128
layout(std140, binding = 1) uniform Lights{
	DirLight dirLight;
	SpotLight spotLight;
	PointLight pointLights[MAX_NUMBER_POINT_LIGHTS];
	vec4 enableFlashLightNumPtLights; // {enable flash light, number of point lights, unused, unused}
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gEmission;
uniform sampler2D gMisc;
uniform sampler2D shadowMap;
uniform sampler2D SSAO;

uniform mat4 viewToDirLightSpaceMat;

//Blinn Phong
vec3 calcPhongDirLight(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec); 
vec3 calcPhongPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec);  
vec3 calcPhongSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec);  
float calcDirLightShadow(vec3 fragPos, vec3 normal, vec3 lightDirection);
vec3 calcBlinnFong(vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec, vec3 texEmission, float SSAOSample);

//PBR
vec3 fresnelSchlick(vec3 halfway, vec3 viewDir, vec3 F0);
vec3 calcPBR(vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, float AOPBRMat);
vec3 calcPBRPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0);
float NDF(vec3 normal, vec3 halfway, float roughness);
float Geometry(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
float GeometrySchlickGGX(float dotProduct, float roughness);

//other
float calcAttenuation(vec3 lightPos, vec3 fragPos, float constant, float linear, float quadratic);

// global vars
float shininess = 256.0;
const float PI = 3.14159265359;


void main(){
	//doing all calculations in VIEW SPACE
	vec3 fragPos = texture(gPosition, texCoords).rgb;
	vec3 normal = texture(gNormal, texCoords).rgb;
	vec3 viewDir = normalize(-fragPos); //fragment to camera

	vec4 gAlbedoSpecSample = texture(gAlbedoSpec, texCoords);
	vec3 texDiff = gAlbedoSpecSample.rgb;
	vec3 texSpec = vec3(gAlbedoSpecSample.a);
	vec3 texEmission = texture(gEmission, texCoords).rgb;
	float SSAOSample = texture(SSAO, texCoords).r;

	vec4 gMiscSample = texture(gMisc, texCoords);
	float metallic = gMiscSample.r;
	float roughness = gMiscSample.g;
	float AOPBRMat = gMiscSample.b;
	float matType = gMiscSample.a;

	vec3 result = vec3(0.0);

	// matType 0 = Phong, 1 = PBR
	if (matType == 0){
		result = calcBlinnFong(fragPos, normal, viewDir, texDiff, texSpec, texEmission, SSAOSample);
	}
	else if (matType == 1){
		result = calcPBR(fragPos, normal, viewDir, texDiff, metallic, roughness, AOPBRMat);
	}

	fragColor = vec4(result, 1.0);
}

// -------------------------------------------------------- PBR ----------------------------------------------------------------------------

vec3 calcPBR(vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, float AOPBRMat){
	vec3 result = vec3(0.0);

	// F0 is a physical property of the material that describes how reflective it is when directly looking at it or at normal incidence. 
	// For metallic objects, it is the color of albedo, for dielectric objects a good approximation is vec3(0.04) so use metallic value 
	// to interpolate between the two.
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// point Lights
	for(int i = 0; i < enableFlashLightNumPtLights.y; i++){
		result += calcPBRPointLight(pointLights[i], fragPos, normal, viewDir, albedo, metallic, roughness, F0);
	}

	vec3 ambient = vec3(0.03) * albedo * AOPBRMat;
    result += ambient;  

	return result;
}


vec3 calcPBRPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0){
	vec3 lightDir = normalize(vec3(light.position) - fragPos);
	vec3 halfway = normalize(viewDir + lightDir);

    float attenuation = calcAttenuation(vec3(light.position), fragPos, light.attenuation.x, light.attenuation.y, light.attenuation.z);
	vec3 radiance = vec3(light.color) * attenuation;

	//Cook Torrance BRDF
	float D = NDF(normal, halfway, roughness);
	float G = Geometry(normal, viewDir, lightDir, roughness);
	vec3 F = fresnelSchlick(halfway, viewDir, F0);

	vec3 numerator = D * F * G;
	// add 0.0001 to prevent divide by 0
	float denominator = 4.0  * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
	vec3 specular = numerator / denominator;

	vec3 kD = vec3(1.0) - F;
	kD *= 1.0 - metallic;
	vec3 result = (kD * albedo / PI + specular) * radiance * max(dot(normal, lightDir), 0.0);

	return result;
}

// approximation of fresnel equation (Fresnel-Schlick approximation). 
vec3 fresnelSchlick(vec3 halfway, vec3 viewDir, vec3 F0){
	float cosTheta = clamp(dot(halfway, viewDir), 0.0, 1.0);
	return F0 + (1.0 - F0) * pow(1 - cosTheta , 5.0);
}

// normal distribution function
float NDF(vec3 normal, vec3 halfway, float roughness){
	float a = roughness * roughness;
	float a2 = pow(a, 2);
	float NdotH = max(dot(normal, halfway), 0.0);
	return a2 / (PI * pow(pow(NdotH, 2) * (a2 - 1) + 1, 2));
}


float Geometry(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness){
	float ggx1 = GeometrySchlickGGX(max(dot(normal, viewDir), 0.0), roughness);
	float ggx2 = GeometrySchlickGGX(max(dot(normal, lightDir), 0.0), roughness);

	return ggx1 * ggx2;
}

float GeometrySchlickGGX(float dotProduct, float roughness){
	float k = pow(roughness + 1, 2) / 8.0;
	return dotProduct / (dotProduct * (1 - k)  + k);
}


// ----------------------------------------------------- Blinn Phong ------------------------------------------------------------------------

vec3 calcBlinnFong(vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec, vec3 texEmission, float SSAOSample){
	vec3 result = vec3(0.0);
	
	//directional light
	result += calcPhongDirLight(dirLight, fragPos, normal, viewDir, texDiff, texSpec);

	//point lights
	for(int i = 0; i < enableFlashLightNumPtLights.y; i++){
		result += calcPhongPointLight(pointLights[i], fragPos, normal, viewDir, texDiff, texSpec);  
	}

	//spot light
	if (enableFlashLightNumPtLights.x > 0)
		result += calcPhongSpotLight(spotLight, fragPos, normal, viewDir, texDiff, texSpec);

	//ambient light
	vec3 ambientLight = vec3(0.2);
	result += SSAOSample * ambientLight * texDiff.rgb;
	result += texEmission;

	return result;
}

vec3 calcPhongDirLight(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec){
	vec3 lightDir = normalize(-vec3(light.direction));

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	//specular 
    vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	//combine results
    vec3 diffuse  = vec3(light.diffuse)  * diff * texDiff;
    vec3 specular = vec3(light.specular) * spec * texSpec;

	float shadow = calcDirLightShadow(fragPos, normal, lightDir);
	return ((1.0 - shadow) * (diffuse + specular));
}

vec3 calcPhongPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec){
	// skip if the light has no color
	if (light.color == vec4(0.0)){
		return vec3(0.0);
	}

    vec3 lightDir = normalize(vec3(light.position) - fragPos);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    // attenuation
	float attenuation = calcAttenuation(vec3(light.position), fragPos, light.attenuation.x, light.attenuation.y, light.attenuation.z);
    
    // combine results
    vec3 diffuse  = vec3(light.color) * diff * texDiff;
    vec3 specular = vec3(light.color) * spec * texSpec;
    
    return attenuation * (diffuse + specular);
} 


vec3 calcPhongSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec){
	vec3 spotLightDir = normalize(-vec3(light.direction));
	vec3 fragToLightDir = normalize(vec3(light.position) - fragPos);

	//diffuse
	float diff = max(dot(normal, fragToLightDir), 0.0);

	//specular
    vec3 halfwayDir = normalize(fragToLightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

	//attenuation
	float attenuation = calcAttenuation(vec3(light.position), fragPos, light.attenuation.x, light.attenuation.y, light.attenuation.z);

	//spotlight intentsity calculation
	float theta = dot(fragToLightDir, normalize(spotLightDir));
    float epsilon = light.cutOffs.x - light.cutOffs.y;
    float intensity = clamp((theta - light.cutOffs.y) / epsilon, 0.0, 1.0);

	//combine results
	vec3 diffuse  = vec3(light.diffuse)  * diff * texDiff;
    vec3 specular = vec3(light.specular) * spec * texSpec;

	return attenuation * intensity * (diffuse + specular);
}

float calcDirLightShadow(vec3 fragPos, vec3 normal, vec3 lightDirection){
	//since frag pos in Light Space not passed through gl_Position, have to do perspective divide to transform it into NDC. 
	//However, since I am using orothgraphic projection, the w component is always 1, so it does not do anything.
	//But keeping it in case I use perspective projection in the future.
	vec4 fragPosDirLightSpace = viewToDirLightSpaceMat * vec4(fragPos, 1.0);
	vec3 lightSpaceCoords = fragPosDirLightSpace.xyz / fragPosDirLightSpace.w;

	// since shadow map is in range [0, 1], transform lightSpaceCoords from NDC [-1, 1] to normalized texture coordinates [0, 1] 
	// (so it can be used to sample the texture) 
	lightSpaceCoords = lightSpaceCoords * 0.5 + 0.5;
	float currentDepth = lightSpaceCoords.z;	

	// if the coords are further than the far plane of the light's frustum, the z value will always be greater than the depth value,
	// so just set the shadow to 0 so it is not always in shadow
	if(lightSpaceCoords.z > 1.0)
         return 0.0;

	// use bias to prevent shadow acne
	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);  
	float shadow = 0.0;

	// textureSize returns size of each texel in normalized texture coordinates at mipmap 0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

	// PCF : sample surrounding texels around the current fragment in light space and average it
	int sampleCount = 0;
	int samples = 1;
	for (int x = -samples;  x <= samples; x++){
		for(int y = -samples; y <= samples; y++){
			// .r because shadowMap has a GL_DEPTH_COMPONENT format which stores depth values as (depth, 0, 0, 1.0) -> (r, g, b, a)
			float closestDepthPCF = texture(shadowMap, lightSpaceCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > closestDepthPCF ? 1.0 : 0.0;
			sampleCount++;
		}
	}
	shadow /= sampleCount;

	return shadow;
}

float calcAttenuation(vec3 lightPos, vec3 fragPos, float constant, float linear, float quadratic){
	float dist = length(lightPos - fragPos);
	return 1.0 / (constant + linear * dist + quadratic * (dist * dist));
}