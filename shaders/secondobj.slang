struct Matrix {
    float4x4 proj;
}

[[vk::push_constant]]
Matrix mtx;

static float3 positions[36] = float3[](
	// Front face (z = 4)
	float3(-1, -1, 4), float3( 1, -1, 4), float3( 1,  1, 4),
	float3(-1, -1, 4), float3( 1,  1, 4), float3(-1,  1, 4),
	// Back face (z = 6)
	float3(-1, -1, 6), float3(-1,  1, 6), float3( 1,  1, 6),
	float3(-1, -1, 6), float3( 1,  1, 6), float3( 1, -1, 6),
	// Top face (y = 1)
	float3(-1,  1, 4), float3( 1,  1, 4), float3( 1,  1, 6),
	float3(-1,  1, 4), float3( 1,  1, 6), float3(-1,  1, 6),
	// Bottom face (y = -1)
	float3(-1, -1, 4), float3(-1, -1, 6), float3( 1, -1, 6),
	float3(-1, -1, 4), float3( 1, -1, 6), float3( 1, -1, 4),
	// Right face (x = 1)
	float3( 1, -1, 4), float3( 1, -1, 6), float3( 1,  1, 6),
	float3( 1, -1, 4), float3( 1,  1, 6), float3( 1,  1, 4),
	// Left face (x = -1)
	float3(-1, -1, 4), float3(-1,  1, 4), float3(-1,  1, 6),
	float3(-1, -1, 4), float3(-1,  1, 6), float3(-1, -1, 6)
);

static float3 colors[36] = float3[](
	// Front - red
	float3(1, 0, 0), float3(1, 0, 0), float3(1, 0, 0),
	float3(1, 0, 0), float3(1, 0, 0), float3(1, 0, 0),
	// Back - green
	float3(0, 1, 0), float3(0, 1, 0), float3(0, 1, 0),
	float3(0, 1, 0), float3(0, 1, 0), float3(0, 1, 0),
	// Top - blue
	float3(0, 0, 1), float3(0, 0, 1), float3(0, 0, 1),
	float3(0, 0, 1), float3(0, 0, 1), float3(0, 0, 1),
	// Bottom - yellow
	float3(1, 1, 0), float3(1, 1, 0), float3(1, 1, 0),
	float3(1, 1, 0), float3(1, 1, 0), float3(1, 1, 0),
	// Right - magenta
	float3(1, 0, 1), float3(1, 0, 1), float3(1, 0, 1),
	float3(1, 0, 1), float3(1, 0, 1), float3(1, 0, 1),
	// Left - cyan
	float3(0, 1, 1), float3(0, 1, 1), float3(0, 1, 1),
	float3(0, 1, 1), float3(0, 1, 1), float3(0, 1, 1)
);

struct VertexOutput {
    float4 sv_position : SV_Position;
    float3 color;
}

[shader("vertex")]
VertexOutput vertsMain(uint vid : SV_VertexID) {
    VertexOutput output;
    output.sv_position = mul(mtx.proj, float4(positions[vid].xy, positions[vid].z, 1.0).add(float4(-4.5, 0.0, 10.0, 0.0)));

    output.color = colors[vid];
    return output;
}

[shader("fragment")]
float4 fragsMain(VertexOutput vertIn) : SV_Target {
    return float4(vertIn.color, 1.0);
}