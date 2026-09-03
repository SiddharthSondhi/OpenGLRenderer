#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT{
	vec3 fragPos;
    vec2 texCoords;
	vec3 normal;
	mat3 TBN;
} fs_in;

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;

	vec2 textureScale;
	bool hasNormalMap;
	float shininess;
};

uniform Material material;

void main(){
	gPosition = vec4(fs_in.fragPos, 1.0);
	
	if (material.hasNormalMap){
		// sample normal from normal map, will be in range [0, 1]
		vec3 norm = texture(material.texture_normal1, fs_in.texCoords).rgb;
		// transform normal vector to range [-1,1] , transform to view space using TBN, and normalize
		gNormal = vec4(normalize(fs_in.TBN * (norm * 2.0 - 1.0)), 1.0);
	}
	else{
		gNormal = vec4(normalize(fs_in.normal), 1.0);
	}
	gAlbedoSpec.rgb = texture(material.texture_diffuse1, material.textureScale * fs_in.texCoords).rgb;
	gAlbedoSpec.a = texture(material.texture_specular1, material.textureScale * fs_in.texCoords).r;
}