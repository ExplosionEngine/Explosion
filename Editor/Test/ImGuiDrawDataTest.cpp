#include <cstring>

#include <Editor/ImGui/ImGuiDrawData.h>
#include <Test/Test.h>

namespace Editor::Test {
    TEST(ImGuiDrawDataTest, SnapshotsTextureIdsAndOwnedCallbackPayloads)
    {
        ImTextureData texture;
        texture.SetTexID(123);
        ImDrawList source(nullptr);
        source._CallbacksDataBuf.resize(sizeof(int));
        const int originalValue = 42;
        std::memcpy(source._CallbacksDataBuf.Data, &originalValue, sizeof(int));
        ImDrawCmd command;
        command.TexRef = texture.GetTexRef();
        command.UserCallback = [](const ImDrawList*, const ImDrawCmd*) -> void {};
        command.UserCallbackData = source._CallbacksDataBuf.Data;
        command.UserCallbackDataSize = sizeof(int);
        command.UserCallbackDataOffset = 0;
        source.CmdBuffer.push_back(command);
        ImDrawData sourceData;
        sourceData.CmdListsCount = 1;
        sourceData.CmdLists.push_back(&source);

        ImGuiDrawData snapshot(sourceData);
        texture.SetTexID(456);
        const int changedValue = 99;
        std::memcpy(source._CallbacksDataBuf.Data, &changedValue, sizeof(int));
        const ImDrawCmd& cloned = snapshot.Get().CmdLists[0]->CmdBuffer[0];
        EXPECT_EQ(cloned.GetTexID(), 123);
        EXPECT_EQ(cloned.TexRef._TexData, nullptr);
        EXPECT_NE(cloned.UserCallbackData, command.UserCallbackData);
        int clonedValue = 0;
        std::memcpy(&clonedValue, cloned.UserCallbackData, sizeof(int));
        EXPECT_EQ(clonedValue, originalValue);
    }

    TEST(ImGuiDrawDataTest, KeepsClonedCommandsAfterTheOriginalFrameIsReused)
    {
        ImGuiContext* context = ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.DisplaySize = ImVec2(640.0f, 480.0f);
        unsigned char* pixels = nullptr;
        int width = 0;
        int height = 0;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        ImGui::NewFrame();
        ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(10.0f, 20.0f), ImVec2(30.0f, 40.0f), IM_COL32_WHITE);
        ImGui::Render();
        {
            ImGuiDrawData snapshot(*ImGui::GetDrawData());
            ASSERT_EQ(snapshot.Get().CmdListsCount, 1);
            EXPECT_NE(snapshot.Get().CmdLists[0], ImGui::GetDrawData()->CmdLists[0]);
            const int vertexCount = snapshot.Get().TotalVtxCount;
            ImGui::NewFrame();
            ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(100.0f, 200.0f), ImVec2(300.0f, 400.0f), IM_COL32_WHITE);
            ImGui::Render();
            EXPECT_EQ(snapshot.Get().TotalVtxCount, vertexCount);
            EXPECT_FLOAT_EQ(snapshot.Get().CmdLists[0]->VtxBuffer[0].pos.x, 10.0f);
            EXPECT_FLOAT_EQ(snapshot.Get().CmdLists[0]->VtxBuffer[0].pos.y, 20.0f);
        }
        ImGui::DestroyContext(context);
    }
}
