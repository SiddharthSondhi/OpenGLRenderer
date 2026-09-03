#version 460 core

in vec2 texCoords;

out vec4 fragColor;

// Create all light data structs using vec4 so that std140 UBO easy to use
struct DirLight{
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PointLight{
	vec4 position;

	vec4 attenuation; //constant, linear, quadratic

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct SpotLight{
	vec4 position;
	vec4 direction;

	vec4 cutOffs;     // innerCutOff, outerCutOff
	vec4 attenuation; //constant, linear, quadratic

	vec4 diffuse;
	vec4 specular;
};

#define NR_POINT_LIGHTS 8

layout(std140, binding = 1) uniform Lights{
	DirLight dirLight;
	SpotLight spotLight;
	PointLight pointLights[NR_POINT_LIGHTS];
	vec4 enableFlashLight; 
};

uniform sampler2D shadowMap;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform mat4 viewToDirLightSpaceMat;


//prototypes
vec3 calcDirLight(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec); 
vec3 calcPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec);  
vec3 calcSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec);  
float calcDirLightShadow(vec3 fragPos, vec3 normal, vec3 lightDirection);

void main(){
    //------------------------- doing all calculations in VIEW SPACE -----------------------
	vec3 fragPos = texture(gPosition, texCoords).rgb;

	// sample textures
	vec4 texDiff = vec4(texture(gAlbedoSpec, texCoords).rgb, 1.0);
	float specVal = texture(gAlbedoSpec, texCoords).a;
	vec4  texSpec = vec4(specVal, specVal, specVal, 1.0);
	
	//discard fragment if alpha below threshold
	//if(texDiff.a < 0.05)
       // discard;

	vec3 norm = texture(gNormal, texCoords).rgb;
	vec3 viewDir = normalize(-fragPos);
	vec3 result = vec3(0.0);

	//directional light
	result += calcDirLight(dirLight, fragPos, norm, viewDir, texDiff, texSpec);

	//point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += calcPointLight(pointLights[i], fragPos, norm, viewDir, texDiff, texSpec);  
    }

	//spot light
	if (enableFlashLight.x > 0)
		result += calcSpotLight(spotLight, fragPos, norm, viewDir, texDiff, texSpec);


	fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec){
	vec3 lightDir = normalize(-vec3(light.direction));

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	//specular 
    vec3 halfwayDir = normalize(lightDir + viewDir);
	//float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 256);


	//combine results
	vec3 ambient  = vec3(light.ambient)  * vec3(texDiff);
    vec3 diffuse  = vec3(light.diffuse)  * diff * vec3(texDiff);
    vec3 specular = vec3(light.specular) * spec * vec3(texSpec);

	float shadow = calcDirLightShadow(fragPos, normal, lightDir);
	return ambient + ((1.0 - shadow) * (diffuse + specular));
}

vec3 calcPointLight(PointLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec){
	// skip if the light has no color
	if (light.ambient == vec4(0.0) && light.diffuse == vec4(0.0) && light.specular == vec4(0.0)){
		return vec3(0.0);
	}

    vec3 lightDir = normalize(vec3(light.position) - fragPos);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 256);


    // attenuation
    float distance = length(vec3(light.position) - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));    
    
    // combine results
    vec3 ambient  = vec3(light.ambient)  * vec3(texDiff);
    vec3 diffuse  = vec3(light.diffuse)  * diff * vec3(texDiff);
    vec3 specular = vec3(light.specular) * spec * vec3(texSpec);
    
    return attenuation * (ambient + diffuse + specular);
} 


vec3 calcSpotLight(SpotLight light, vec3 fragPos, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec){
	vec3 spotLightDir = normalize(-vec3(light.direction));
	vec3 fragToLightDir = normalize(vec3(light.position) - fragPos);

	//diffuse
	float diff = max(dot(normal, fragToLightDir), 0.0);

	//specular
    vec3 halfwayDir = normalize(fragToLightDir + viewDir);
	//float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 256);


	//attenuation
	float distance = length(vec3(light.position) - fragPos);
	float attentuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));

	//spotlight intentsity calculation
	float theta = dot(fragToLightDir, normalize(spotLightDir));
    float epsilon = light.cutOffs.x - light.cutOffs.y;
    float intensity = clamp((theta - light.cutOffs.y) / epsilon, 0.0, 1.0);

	//combine results
	vec3 diffuse  = vec3(light.diffuse)  * diff * vec3(texDiff);
    vec3 specular = vec3(light.specular) * spec * vec3(texSpec);

	return attentuation * intensity * (diffuse + specular);
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