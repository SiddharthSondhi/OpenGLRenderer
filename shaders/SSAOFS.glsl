#version 460 core
out float occlusion;

in vec2 texCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

const int NUM_SAMPLES = 64;
uniform vec3 samples[NUM_SAMPLES];

layout(std140, binding = 0) uniform Matrices{
	mat4 view;
	mat4 projection;
};

uniform float radius = 0.5;
uniform float bias = .025;

const vec2 noiseScale = textureSize(gPosition, 0) / textureSize(noiseTexture, 0);

void main(){
	vec3 fragPos = texture(gPosition, texCoords).xyz;
	vec3 normal  = texture(gNormal, texCoords).rgb;
	vec3 randomVec = texture(noiseTexture, texCoords * noiseScale).xyz;  

	// construct TBN to transform samples from tangent space to view space with a random rotation about the +z axis
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal); 

	occlusion = 0.0;
	for (int i = 0; i < NUM_SAMPLES; i++){
		// transform sample from tangent space to view space using TBN
		vec3 samplePos = TBN * samples[i];
		samplePos = fragPos + samplePos * radius;

		//transform sample to screen space and store it so it can be used to sample gPosition
		vec4 sampleScreenSpace = vec4(samplePos, 1.0);
		sampleScreenSpace  = projection * sampleScreenSpace;    // from view to clip-space
		sampleScreenSpace.xyz /= sampleScreenSpace.w;               // perspective divide
		sampleScreenSpace.xyz  = sampleScreenSpace.xyz * 0.5 + 0.5; // transform from range -1.0 - 1.0 to range 0.0 - 1.0  

		float fragmentDepth = texture(gPosition, sampleScreenSpace.xy).z; 

		// add range check so geometry far away from the sample doesn't affect occlusion
		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - fragmentDepth));
		// In OpenGL view space, larger Z is closer to the camera. If samplePos is behind the fragment depth (has a smaller z value), it is occluded.
		occlusion += (samplePos.z + bias <= fragmentDepth ? 1.0 : 0.0) * rangeCheck;  
	} 

	occlusion = 1.0 - (occlusion / NUM_SAMPLES);
}
