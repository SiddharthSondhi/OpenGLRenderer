#version 460 core

out vec4 fragColor;

in VS_OUT{
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
} fs_in;


struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	float shininess;
};

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

uniform Material material;


//prototypes
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec); 
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec);  
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec);  


void main(){
    //------------------------- doing all calculations in VIEW SPACE -----------------------
	// sample textures
	vec4 texDiff = texture(material.texture_diffuse1, fs_in.texCoords);
	vec4 texSpec = texture(material.texture_specular1, fs_in.texCoords);
	
	//discard fragment if alpha below threshold
	//if(texDiff.a < 0.05)
       // discard;

	vec3 norm = normalize(fs_in.normal);
	vec3 viewDir = normalize(-fs_in.fragPos);
	vec3 result = vec3(0.0);

	//directional light
	result += calcDirLight(dirLight, norm, viewDir, texDiff, texSpec);

	//point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += calcPointLight(pointLights[i], norm, viewDir, texDiff, texSpec);  
    }

	//spot light
	if (enableFlashLight.x > 0)
		result += calcSpotLight(spotLight, norm, viewDir, texDiff, texSpec);


	fragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec){
	vec3 lightDir = normalize(-vec3(light.direction));

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	//specular 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//combine results
	vec3 ambient  = vec3(light.ambient)  * vec3(texDiff);
    vec3 diffuse  = vec3(light.diffuse)  * diff * vec3(texDiff);
    vec3 specular = vec3(light.specular) * spec * vec3(texSpec);

	return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec){
	// skip if the light has no color
	if (light.ambient == vec4(0.0) && light.diffuse == vec4(0.0) && light.specular == vec4(0.0)){
		return vec3(0.0);
	}

    vec3 lightDir = normalize(vec3(light.position) - fs_in.fragPos);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(vec3(light.position) - fs_in.fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));    
    
    // combine results
    vec3 ambient  = vec3(light.ambient)  * vec3(texDiff);
    vec3 diffuse  = vec3(light.diffuse)  * diff * vec3(texDiff);
    vec3 specular = vec3(light.specular) * spec * vec3(texSpec);
    
    return attenuation * (ambient + diffuse + specular);
} 


vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec4 texDiff, vec4 texSpec){
	vec3 spotLightDir = normalize(-vec3(light.direction));
	vec3 fragToLightDir = normalize(vec3(light.position) - fs_in.fragPos);

	//diffuse
	float diff = max(dot(normal, fragToLightDir), 0.0);

	//specular
	vec3 reflectDir = reflect(-fragToLightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//attenuation
	float distance = length(vec3(light.position) - fs_in.fragPos);
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