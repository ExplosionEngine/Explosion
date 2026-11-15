#include <Editor/ImGui/ImGuiDrawData.h>

namespace Editor {
    ImGuiDrawData::ImGuiDrawData(const ImDrawData& inDrawData)
    {
        drawData.Valid = inDrawData.Valid;
        drawData.CmdListsCount = inDrawData.CmdListsCount;
        drawData.TotalIdxCount = inDrawData.TotalIdxCount;
        drawData.TotalVtxCount = inDrawData.TotalVtxCount;
        drawData.DisplayPos = inDrawData.DisplayPos;
        drawData.DisplaySize = inDrawData.DisplaySize;
        drawData.FramebufferScale = inDrawData.FramebufferScale;
        drawData.CmdLists.reserve(inDrawData.CmdListsCount);
        for (int i = 0; i < inDrawData.CmdListsCount; ++i) {
            const ImDrawList& source = *inDrawData.CmdLists[i];
            ImDrawList* cloned = source.CloneOutput();
            drawData.CmdLists.push_back(cloned);
            cloned->_CallbacksDataBuf = source._CallbacksDataBuf;
            for (ImDrawCmd& command : cloned->CmdBuffer) {
                command.TexRef = ImTextureRef(command.TexRef.GetTexID());
                if (command.UserCallbackDataSize > 0) {
                    command.UserCallbackData = cloned->_CallbacksDataBuf.Data + command.UserCallbackDataOffset;
                }
            }
        }
    }

    ImGuiDrawData::~ImGuiDrawData()
    {
        for (ImDrawList* commandList : drawData.CmdLists) {
            IM_DELETE(commandList);
        }
    }

    ImDrawData& ImGuiDrawData::Get()
    {
        return drawData;
    }
}
