struct Matrix {
    float4x4 proj;
    float3 pos;
}

[[vk::push_constant]]
Matrix mtx;

struct VSInput {
    float3 pos;
    float3 col;
}

struct VertexOutput {
	float4 sv_position : SV_Position; /*just denotes the output i.e this struct member is going to be used as part of the rasterization stage, : is a specifier indicating its a special gpu property*/
	float3 color; 
}

[shader("vertex")]
VertexOutput vertMain(VSInput input) {
    VertexOutput output;
    output.sv_position = mul(mtx.proj, float4(input.pos.xyz, 1.0).add(float4(mtx.pos, 0.0)));
	output.color = input.col;
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput vertIn) : SV_Target {
	float3 color = vertIn.color;
	return float4(color, 1.0);
}

/*SV_Position — vertex shader output. Tells the GPU "this is where the vertex is in clip space." GPU uses it for perspective divide, rasterization, depth testing.

SV_Target — fragment shader output. Tells the GPU "write this color to the framebuffer." That's what actually shows up on screen.*/