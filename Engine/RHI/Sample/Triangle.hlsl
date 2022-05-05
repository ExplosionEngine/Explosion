struct FragmentInput {
    float4 position;
    float4 color;
};

FragmentInput VSMain(float4 position : SV_POSITION, float4 color : SV_COLOR)
{
    FragmentInput fragmentInput;
    fragmentInput.position = position;
    fragmentInput.color = color;
    return fragmentInput;
}

float4 FSMain(FragmentInput input) : SV_TARGET
{
    return input.color;
}
