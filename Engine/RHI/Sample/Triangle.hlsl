struct PixelInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInput VSMain(float4 position : SV_POSITION, float4 color : COLOR)
{
    PixelInput pixelInput;
    pixelInput.position = position;
    pixelInput.color = color;
    return pixelInput;
}

float4 PSMain(PixelInput input) : SV_TARGET
{
    return input.color;
}
