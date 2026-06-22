#version 460 core

out vec4 fragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;


struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	float shininess;
};

struct DirLight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
	vec3 position;

	// attenuation constants
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight{
	vec3 position;
	vec3 direction;
	float innerCutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 diffuse;
	vec3 specular;
};

//uniforms
uniform Material material;
uniform DirLight dirLight;

uniform SpotLight spotLight;
uniform bool enableFlashLight = false;

#define NR_POINT_LIGHTS 3
uniform PointLight pointLights[NR_POINT_LIGHTS];


//prototypes
vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir); 
vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir);  
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);  


void main(){
    //------------------------- doing all calculations in VIEW SPACE -----------------------
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(-fragPos);
	vec3 result = vec3(0.0f);

	//directional light
	result += calcDirLight(dirLight, norm, viewDir);

	//point lights
	for(int i = 0; i < NR_POINT_LIGHTS; i++){
        result += calcPointLight(pointLights[i], norm, viewDir);  
    }

	//spot light
	if (enableFlashLight)
		result += calcSpotLight(spotLight, norm, viewDir);


	fragColor = vec4(result, 1.0f);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir){
	vec3 lightDir = normalize(-light.direction);

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0f);

	//specular 
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	//combine results
	vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));

	return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0f);

    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    
    return attenuation * (ambient + diffuse + specular);
} 


vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir){
	vec3 spotLightDir = normalize(-light.direction);
	vec3 fragToLightDir = normalize(light.position - fragPos);

	//diffuse
	float diff = max(dot(normal, fragToLightDir), 0.0f);

	//specular
	vec3 reflectDir = reflect(-fragToLightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	//attenuation
	float distance = length(light.position - fragPos);
	float attentuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//spotlight intentsity calculation
	float theta = dot(fragToLightDir, normalize(spotLightDir));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	//combine results
	vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));

	return attentuation * intensity * (diffuse + specular);
}