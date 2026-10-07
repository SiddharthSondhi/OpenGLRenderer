#version 460 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gEmission;
layout (location = 4) out vec4 gMisc; // (metallic, roughness, AO, shading model)	

in VS_OUT{
	vec3 fragPos;
    vec2 texCoords;
	vec3 normal;
	mat3 TBN;
} fs_in;

struct PhongMaterial{
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
	bool hasEmissionMap;
};

struct PBRMaterial{
	sampler2D texture_albedo;
	sampler2D texture_metallic;
	sampler2D texture_roughness;
	sampler2D texture_AO;
	sampler2D texture_normal;

	vec3 albedoColor;
	float metallic;
	float roughness;

	bool hasAlbedoMap;
	bool hasMetallicMap;
	bool hasRoughnessMap;
	bool hasAOMap;
	bool hasNormalMap;
};

uniform PhongMaterial phongMat;
uniform PBRMaterial pbrMat;

// 0 = phong, 1 = PBR
uniform float matType;

void main(){
	// ---------------------------------------------------- All Materials ----------------------------------------------------
	gMisc.a = matType;
	gPosition = vec4(fs_in.fragPos, 1.0);
	
	// ---------------------------------------------------- Phong Material ----------------------------------------------------
	if(matType == 0){
		// normals
		if (phongMat.hasNormalMap){
			// sample normal from normal map, will be in range [0, 1]
			vec3 norm = texture(phongMat.texture_normal, phongMat.textureScale * fs_in.texCoords).rgb;
			// transform normal vector to range [-1,1] , transform to view space using TBN, and normalize
			gNormal = vec4(normalize(fs_in.TBN * (norm * 2.0 - 1.0)), 1.0);
		}
		else 
			gNormal = vec4(normalize(fs_in.normal), 1.0);

		// diffuse
		gAlbedoSpec.rgb = texture(phongMat.texture_diffuse, phongMat.textureScale * fs_in.texCoords).rgb * phongMat.diffuseColor;

		// specular
		if (phongMat.hasSpecularMap)
			gAlbedoSpec.a = (texture(phongMat.texture_specular, phongMat.textureScale * fs_in.texCoords).rgb * phongMat.specularColor).r;
		else 
			gAlbedoSpec.a = 0.3;

		// emission
		if (phongMat.hasEmissionMap)
			gEmission = vec4(texture(phongMat.texture_emission, phongMat.textureScale * fs_in.texCoords).rgb * phongMat.emissionColor, 1.0);
		else 
			gEmission = vec4(phongMat.emissionColor, 1.0);

		// initialize unused
		gMisc.rgb = vec3(0);
	}

	// ---------------------------------------------------- PBR Material -------------------------------------------------------
	else if (matType == 1){
		//albedo
		if (pbrMat.hasAlbedoMap)
			gAlbedoSpec.rgb = texture(pbrMat.texture_albedo, fs_in.texCoords).rgb * pbrMat.albedoColor;
		else
			gAlbedoSpec.rgb = pbrMat.albedoColor;

		//metallic
		if (pbrMat.hasMetallicMap)
			gMisc.r = texture(pbrMat.texture_metallic, fs_in.texCoords).r;
		else 
			gMisc.r = pbrMat.metallic;

		//roughness
		if (pbrMat.hasRoughnessMap)
			gMisc.g = texture(pbrMat.texture_roughness, fs_in.texCoords).r;
		else 
			gMisc.g = pbrMat.roughness;

		//AO
		if (pbrMat.hasAOMap)
			gMisc.b = texture(pbrMat.texture_AO, fs_in.texCoords).r;
		else 
			gMisc.b = 1.0;

		//normals
		if (pbrMat.hasNormalMap){
			// sample normal from normal map, will be in range [0, 1]
			vec3 norm = texture(pbrMat.texture_normal, fs_in.texCoords).rgb;
			// transform normal vector to range [-1,1] , transform to view space using TBN, and normalize
			gNormal = vec4(normalize(fs_in.TBN * (norm * 2.0 - 1.0)), 1.0);
		}
		else 
			gNormal = vec4(normalize(fs_in.normal), 1.0);


		// initialize unused
		gEmission = vec4(0.0);
		gAlbedoSpec.a = 1.0;
	}
}