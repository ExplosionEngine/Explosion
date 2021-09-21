//
// Created by Zach Lee on 2021/7/10.
//

#include "ShaderBuilder.h"

#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>

#include <memory>

#include "ShaderGenerator.h"
#include <Common/Exception.h>

using namespace glslang;

namespace Explosion::Builder {
    const TBuiltInResource defaultTBuiltInResource = {
        /* .MaxLights = */ 32,
        /* .MaxClipPlanes = */ 6,
        /* .MaxTextureUnits = */ 32,
        /* .MaxTextureCoords = */ 32,
        /* .MaxVertexAttribs = */ 64,
        /* .MaxVertexUniformComponents = */ 4096,
        /* .MaxVaryingFloats = */ 64,
        /* .MaxVertexTextureImageUnits = */ 32,
        /* .MaxCombinedTextureImageUnits = */ 80,
        /* .MaxTextureImageUnits = */ 32,
        /* .MaxFragmentUniformComponents = */ 4096,
        /* .MaxDrawBuffers = */ 32,
        /* .MaxVertexUniformVectors = */ 128,
        /* .MaxVaryingVectors = */ 8,
        /* .MaxFragmentUniformVectors = */ 16,
        /* .MaxVertexOutputVectors = */ 16,
        /* .MaxFragmentInputVectors = */ 15,
        /* .MinProgramTexelOffset = */ -8,
        /* .MaxProgramTexelOffset = */ 7,
        /* .MaxClipDistances = */ 8,
        /* .MaxComputeWorkGroupCountX = */ 65535,
        /* .MaxComputeWorkGroupCountY = */ 65535,
        /* .MaxComputeWorkGroupCountZ = */ 65535,
        /* .MaxComputeWorkGroupSizeX = */ 1024,
        /* .MaxComputeWorkGroupSizeY = */ 1024,
        /* .MaxComputeWorkGroupSizeZ = */ 64,
        /* .MaxComputeUniformComponents = */ 1024,
        /* .MaxComputeTextureImageUnits = */ 16,
        /* .MaxComputeImageUniforms = */ 8,
        /* .MaxComputeAtomicCounters = */ 8,
        /* .MaxComputeAtomicCounterBuffers = */ 1,
        /* .MaxVaryingComponents = */ 60,
        /* .MaxVertexOutputComponents = */ 64,
        /* .MaxGeometryInputComponents = */ 64,
        /* .MaxGeometryOutputComponents = */ 128,
        /* .MaxFragmentInputComponents = */ 128,
        /* .MaxImageUnits = */ 8,
        /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
        /* .MaxCombinedShaderOutputResources = */ 8,
        /* .MaxImageSamples = */ 0,
        /* .MaxVertexImageUniforms = */ 0,
        /* .MaxTessControlImageUniforms = */ 0,
        /* .MaxTessEvaluationImageUniforms = */ 0,
        /* .MaxGeometryImageUniforms = */ 0,
        /* .MaxFragmentImageUniforms = */ 8,
        /* .MaxCombinedImageUniforms = */ 8,
        /* .MaxGeometryTextureImageUnits = */ 16,
        /* .MaxGeometryOutputVertices = */ 256,
        /* .MaxGeometryTotalOutputComponents = */ 1024,
        /* .MaxGeometryUniformComponents = */ 1024,
        /* .MaxGeometryVaryingComponents = */ 64,
        /* .MaxTessControlInputComponents = */ 128,
        /* .MaxTessControlOutputComponents = */ 128,
        /* .MaxTessControlTextureImageUnits = */ 16,
        /* .MaxTessControlUniformComponents = */ 1024,
        /* .MaxTessControlTotalOutputComponents = */ 4096,
        /* .MaxTessEvaluationInputComponents = */ 128,
        /* .MaxTessEvaluationOutputComponents = */ 128,
        /* .MaxTessEvaluationTextureImageUnits = */ 16,
        /* .MaxTessEvaluationUniformComponents = */ 1024,
        /* .MaxTessPatchComponents = */ 120,
        /* .MaxPatchVertices = */ 32,
        /* .MaxTessGenLevel = */ 64,
        /* .MaxViewports = */ 16,
        /* .MaxVertexAtomicCounters = */ 0,
        /* .MaxTessControlAtomicCounters = */ 0,
        /* .MaxTessEvaluationAtomicCounters = */ 0,
        /* .MaxGeometryAtomicCounters = */ 0,
        /* .MaxFragmentAtomicCounters = */ 8,
        /* .MaxCombinedAtomicCounters = */ 8,
        /* .MaxAtomicCounterBindings = */ 1,
        /* .MaxVertexAtomicCounterBuffers = */ 0,
        /* .MaxTessControlAtomicCounterBuffers = */ 0,
        /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
        /* .MaxGeometryAtomicCounterBuffers = */ 0,
        /* .MaxFragmentAtomicCounterBuffers = */ 1,
        /* .MaxCombinedAtomicCounterBuffers = */ 1,
        /* .MaxAtomicCounterBufferSize = */ 16384,
        /* .MaxTransformFeedbackBuffers = */ 4,
        /* .MaxTransformFeedbackInterleavedComponents = */ 64,
        /* .MaxCullDistances = */ 8,
        /* .MaxCombinedClipAndCullDistances = */ 8,
        /* .MaxSamples = */ 4,
        /* .maxMeshOutputVerticesNV = */ 256,
        /* .maxMeshOutputPrimitivesNV = */ 512,
        /* .maxMeshWorkGroupSizeX_NV = */ 32,
        /* .maxMeshWorkGroupSizeY_NV = */ 1,
        /* .maxMeshWorkGroupSizeZ_NV = */ 1,
        /* .maxTaskWorkGroupSizeX_NV = */ 32,
        /* .maxTaskWorkGroupSizeY_NV = */ 1,
        /* .maxTaskWorkGroupSizeZ_NV = */ 1,
        /* .maxMeshViewCountNV = */ 4,
        /* .maxDualSourceDrawBuffersEXT = */ 1,
        /* .limits = */ {
        /* .nonInductiveForLoops = */ true,
        /* .whileLoops = */ true,
        /* .doWhileLoops = */ true,
        /* .generalUniformIndexing = */ true,
        /* .generalAttributeMatrixVectorIndexing = */ true,
        /* .generalVaryingIndexing = */ true,
        /* .generalSamplerIndexing = */ true,
        /* .generalVariableIndexing = */ true,
        /* .generalConstantMatrixVectorIndexing = */ true,
        }
    };

    EShLanguage StageCast(RHI::ShaderStageBits type)
    {
        switch (type) {
        case RHI::ShaderStageBits::VERTEX: return EShLangVertex;
        case RHI::ShaderStageBits::FRAGMENT: return EShLangFragment;
        case RHI::ShaderStageBits::COMPUTE: return EShLangCompute;
        default: return EShLangVertex;
        }
    }

    void ShaderBuilder::Initialize()
    {
        glslang::InitializeProcess();
    }

    void ShaderBuilder::Finalize()
    {
        glslang::FinalizeProcess();
    }

    static void BuildReflection(TShader& shader, Shader& outShader)
    {
        auto program = std::make_unique<TProgram>();
        program->addShader(&shader);
        if (!program->link((EShMessages)(EShMsgSpvRules | EShMsgVulkanRules))) {
            LOGE("program link error:%s", program->getInfoLog());
        }
        if (!program->buildReflection(EShReflectionSeparateBuffers | EShReflectionSharedStd140SSBO | EShReflectionSharedStd140UBO)) {
            LOGE("program build reflection failed:%s", program->getInfoLog());
            return;
        }

        uint32_t sbo = program->getNumBufferBlocks();
        uint32_t ubo = program->getNumUniformBlocks();
        uint32_t uvs = program->getNumUniformVariables();

        for (uint32_t i = 0; i < sbo; ++i) {
            auto& ref = program->getBufferBlock(i);
            auto type = ref.getType();
            auto& desSet = outShader.reflection.descriptorSets[ref.index];
            desSet.bufferBlock.emplace_back();
            auto& sbp = desSet.bufferBlock.back();
            sbp.name = ref.name;
            sbp.binding = (uint32_t)ref.getBinding();
            LOGE("storage buffer block-%s, set-%d, binding-%u, stride-%d size-%d",
                 sbp.name.c_str(),
                 ref.index,
                 sbp.binding,
                 ref.arrayStride,
                 ref.size);
        }

        for (uint32_t i = 0; i < ubo; ++i) {
            auto& ref = program->getUniformBlock(i);
            auto& desSet = outShader.reflection.descriptorSets[ref.index];
            desSet.uniformBlock.emplace_back();
            UniformBlockParam& ubp = desSet.uniformBlock.back();
            ubp.name = ref.name;
            ubp.binding = (uint32_t)ref.getBinding();
            ubp.size = (uint32_t)ref.size;
            LOGE("uniform buffer block-%s, set-%d, binding-%u, size-%u",
                 ubp.name.c_str(),
                 ref.index,
                 ubp.binding,
                 ubp.size);
        }

        for (uint32_t i = 0; i < uvs; ++i) {
            auto& ref = program->getUniform(i);
            auto type = ref.getType();
            auto& desSet = outShader.reflection.descriptorSets[ref.index];
            if (type->getBasicType() == EbtSampler) {

            } else {
                auto& varMap = desSet.uniformMap;
                auto& attribute = varMap[ref.name];
                attribute.offset = static_cast<uint32_t>(ref.offset);

                LOGE("name-%s, offset-%u, set-%d",
                     ref.name.c_str(),
                     attribute.offset,
                     ref.index);
            }
        }

        LOGE("bufferblock-%u, uniformbuffer-%u, uniformvariable-%u", sbo, ubo, uvs);
    }

    Shader* ShaderBuilder::Load(const std::string& url, RHI::ShaderStageBits type)
    {
        std::string source = ShaderGenerator::Generate(url);

        std::unique_ptr<TProgram> program = std::make_unique<TProgram>();
        std::unique_ptr<TShader> shader = std::make_unique<TShader>(StageCast(type));
        const char* srcData = source.c_str();

        shader->setStringsWithLengths(&srcData, nullptr, 1);
        shader->setEntryPoint("main");
        if (!shader->parse(&defaultTBuiltInResource, 100, false, (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules))) {
            LOGE("shader compile stage-%d, error:%s", (uint32_t)type, shader->getInfoLog());
            return nullptr;
        }

        Shader* result = new Shader();

        glslang::GlslangToSpv(*shader->getIntermediate(), result->bin);

        BuildReflection(*shader, *result);
        return result;
    }

}