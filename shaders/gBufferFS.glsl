#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gEmission;

in VS_OUT{
	vec3 fragPos;
    vec2 texCoords;
	vec3 normal;
	mat3 TBN;
} fs_in;

struct Material{
	sampler2D texture_diffuse;
	sampler2D texture_specular;
	sampler2D texture_normal;
	sampler2D texture_emission;

	vec3 diffuseColor;
	vec3 specularColor;
	vec3 emissionColor;

	vec2 textureScale;
	bool hasNormalMap;
	bool hasSpecularMap;
};

uniform Material material;

void main(){
	// position
	gPosition = vec4(fs_in.fragPos, 1.0);
	
	// normals
	if (material.hasNormalMap){
		// sample normal from normal map, will be in range [0, 1]
		vec3 norm = texture(material.texture_normal, material.textureScale * fs_in.texCoords).rgb;
		// transform normal vector to range [-1,1] , transform to view space using TBN, and normalize
		gNormal = vec4(normalize(fs_in.TBN * (norm * 2.0 - 1.0)), 1.0);
	}
	else{
		gNormal = vec4(normalize(fs_in.normal), 1.0);
	}

	// albedo
	gAlbedoSpec.rgb = texture(material.texture_diffuse, material.textureScale * fs_in.texCoords).rgb * material.diffuseColor;

	// specular
	if (material.hasSpecularMap)
		gAlbedoSpec.a = (texture(material.texture_specular, material.textureScale * fs_in.texCoords).rgb * material.specularColor).r;
	else
		gAlbedoSpec.a = 0.3;

	// emission
	gEmission = vec4(texture(material.texture_emission, material.textureScale * fs_in.texCoords).rgb * material.emissionColor, 1.0);
}