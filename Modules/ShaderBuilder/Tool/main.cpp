#include <ShaderBuilder.h>
#include <ShaderGenerator.h>

using namespace Explosion;

int main()
{
    Builder::ShaderBuilder::Initialize();

    ShaderGenerator::SetBasic("Shaders");

    Builder::ShaderBuilder::Load("Modules/ShaderBuilder/Tool/Test.vert", RHI::ShaderStageBits::VERTEX);

    Builder::ShaderBuilder::Finalize();
   return 0;
}