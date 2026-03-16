static float2 positions[3] = float2[](
	float2(0.0, -0.5),
	float2(0.5, 0.5),
	float2(-0.5, 0.5)
);

static float3 colors[3] = float3[](
	float3(1.0, 0.0, 0.0),
	float3(0.0, 1.0, 0.0),
	float3(0.0, 0.0, 1.0)
);

struct VertexOutput {
	float4 sv_position : SV_Position; /*just denotes the output, : is a specifier indicating its a special gpu property*/
	float3 color; 
}

[shader("vertex")]
VertexOutput vertMain(uint vid: SV_VertexID) {
	VertexOutput output;
	output.sv_position = float4(positions[vid], 0.0, 1.0);
	output.color = colors[vid];
	return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput vertIn) : SV_Target {
	float3 color = vertIn.color;
	return float4(color, 1.0);
}

/*SV_Position — vertex shader output. Tells the GPU "this is where the vertex is in clip space." GPU uses it for perspective divide, rasterization, depth testing.

SV_Target — fragment shader output. Tells the GPU "write this color to the framebuffer." That's what actually shows up on screen.*/