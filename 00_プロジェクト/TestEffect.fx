float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

technique textured
{
    pass p0
    {
        //VertexShader = compile vs_1_1 VS_TransformDiffuse();

        // Just use the color
        ColorArg1[0] = Diffuse;
        AlphaArg1[0] = Diffuse;
        ColorOp[0] = SelectArg1;
        AlphaOp[1] = SelectArg1;
    }
}