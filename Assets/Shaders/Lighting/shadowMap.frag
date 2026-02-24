#version 460 core
#extension GL_ARB_gpu_shader_int64 : enable
#extension GL_ARB_bindless_texture : require

in vec2 texCoord;
uniform sampler2D BlueNoiseTex;
uniform uint64_t texture_diffuse_Handle;
uniform uint64_t bayerMatrixHandle;

void blueNoiseOpacity(float Threshold) // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
{
	vec2 noiseUV = vec2(gl_FragCoord.xy) / vec2(textureSize(BlueNoiseTex, 0)); // new uvec2
	float noise = texture(BlueNoiseTex, noiseUV).r;

	// normal ranges should be 0.0f-1.0f;
	if (noise > Threshold) discard;
}

void BayerNoiseOpacity(float Threshold) // for fade out or opacity (cheap) (could fade out near farplane or nearplane)
{
	sampler2D baySamp = sampler2D(bayerMatrixHandle);
	vec2 bayUV = vec2(gl_FragCoord.xy) / vec2(textureSize(baySamp, 0)); // new uvec2
	float bayer = texture(baySamp, bayUV).r;

	float clampedThreshold = clamp(Threshold, 0.2, 1.0);

	// normal ranges should be 0.0f-1.0f;
	if (bayer > clampedThreshold || Threshold <= 0) discard;
}


void main()
{

	sampler2D Sampler = sampler2D(texture_diffuse_Handle);
	float diffuseTex = texture(Sampler, texCoord).a;

	if (diffuseTex <= 0.0)
	discard;


	blueNoiseOpacity(diffuseTex);
	//BayerNoiseOpacity(diffuseTex);
}