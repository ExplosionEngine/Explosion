#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>

#include <Common/File.h>
#include <Editor/Project/ProjectLauncher.h>
#include <Editor/Project/ProjectService.h>
#include <Editor/Project/RecentProjectStore.h>
#include <Test/Test.h>

namespace Editor::Test {
    class ProjectTest : public testing::Test {
    protected:
        void SetUp() override;
        void TearDown() override;
        void WriteFile(const std::filesystem::path& inPath, const std::string& inText) const;
        CreateProjectDesc CreateDesc(const std::string& inName) const;

        std::filesystem::path directory;
        std::filesystem::path templates;
        std::filesystem::path projectTemplate;
    };
}

namespace Editor::Test {
    void ProjectTest::SetUp()
    {
        const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
        directory = std::filesystem::temp_directory_path() / std::format("ExplosionProjectTest-{}", uniqueId);
        templates = directory / "Templates";
        projectTemplate = templates / "Default";
        WriteFile(projectTemplate / "CMakeLists.txt.tpl", "cmake_minimum_required(VERSION @cmakeMinVersion@)\nproject(@projectName@)\n");
        WriteFile(projectTemplate / "Game" / "Src" / "Main.cpp", "int main() { return 0; }\n");
    }

    void ProjectTest::TearDown()
    {
        std::error_code error;
        std::filesystem::remove_all(directory, error);
    }

    void ProjectTest::WriteFile(const std::filesystem::path& inPath, const std::string& inText) const
    {
        std::filesystem::create_directories(inPath.parent_path());
        std::ofstream stream(inPath, std::ios::binary);
        stream << inText;
    }

    CreateProjectDesc ProjectTest::CreateDesc(const std::string& inName) const
    {
        return { inName, Common::Path(directory / "Projects with spaces"), Common::Path(projectTemplate) };
    }

    TEST_F(ProjectTest, CreatesAValidatedProjectFromTemplates)
    {
        ProjectService service { Common::Path(templates) };
        const auto result = service.CreateProject(CreateDesc("MyProject"));
        ASSERT_TRUE(result.IsOk()) << result.Error();
        const auto project = result.Value();
        EXPECT_TRUE(ProjectService::ValidateProject(project).IsOk());
        EXPECT_EQ(Common::FileUtils::ReadTextFile((project / "CMakeLists.txt").String()).Unwrap(), "cmake_minimum_required(VERSION 3.25)\nproject(MyProject)\n");
        EXPECT_EQ(Common::FileUtils::ReadTextFile((project / "Game/Src/Main.cpp").String()).Unwrap(), "int main() { return 0; }\n");
        EXPECT_FALSE((project / "CMakeLists.txt.tpl").Exists());
    }

    TEST_F(ProjectTest, DoesNotOverwriteExistingProjectsOrAllowPathNames)
    {
        ProjectService service { Common::Path(templates) };
        const auto result = service.CreateProject(CreateDesc("Existing"));
        ASSERT_TRUE(result.IsOk());
        WriteFile(std::filesystem::path(result.Value().String()) / "keep.txt", "keep");
        EXPECT_TRUE(service.CreateProject(CreateDesc("Existing")).IsErr());
        EXPECT_EQ(Common::FileUtils::ReadTextFile((result.Value() / "keep.txt").String()).Unwrap(), "keep");
        EXPECT_TRUE(service.CreateProject(CreateDesc("../Escape")).IsErr());
        EXPECT_TRUE(service.CreateProject(CreateDesc("My Project")).IsErr());
        EXPECT_TRUE(service.CreateProject(CreateDesc("")).IsErr());
    }

    TEST_F(ProjectTest, RollsBackAnIncompleteProjectWhenRenderingFails)
    {
        WriteFile(projectTemplate / "Game" / "Broken.tpl", "@missingVariable@");
        ProjectService service { Common::Path(templates) };
        EXPECT_TRUE(service.CreateProject(CreateDesc("Broken")).IsErr());
        EXPECT_FALSE((CreateDesc("Broken").parentDirectory / "Broken").Exists());
        std::filesystem::remove(projectTemplate / "Game" / "Broken.tpl");
        EXPECT_TRUE(service.CreateProject(CreateDesc("Broken")).IsOk());
    }

    TEST_F(ProjectTest, RejectsDirectoriesWithoutTheProjectStructure)
    {
        EXPECT_TRUE(ProjectService::ValidateProject(Common::Path(directory)).IsErr());
        EXPECT_TRUE(ProjectService::ValidateProject(Common::Path(directory / "Missing")).IsErr());
        ProjectService service { Common::Path(templates) };
        WriteFile(templates / "Invalid" / "readme.txt", "not a template");
        const auto result = service.ListTemplates();
        ASSERT_TRUE(result.IsOk());
        ASSERT_EQ(result.Value().size(), 1);
        EXPECT_EQ(result.Value().front().name, "Default");
    }

    TEST_F(ProjectTest, RecentProjectsPreserveLegacyJsonAndDeduplicateCanonicalPaths)
    {
        const auto firstProject = directory / "First";
        const auto secondProject = directory / "Second";
        std::filesystem::create_directory(firstProject);
        std::filesystem::create_directory(secondProject);
        const Common::Path storeFile(directory / "RecentProjects.json");
        RecentProjectStore store(storeFile);
        ASSERT_TRUE(store.Load().IsOk());
        ASSERT_TRUE(store.Record(Common::Path(firstProject)).IsOk());
        ASSERT_TRUE(store.Record(Common::Path(secondProject)).IsOk());
        ASSERT_TRUE(store.Record(Common::Path(firstProject / ".")).IsOk());
        ASSERT_EQ(store.Projects().size(), 2);
        EXPECT_EQ(store.Projects().back().name, "First");

        const auto document = Common::FileUtils::ReadJsonFile(storeFile.String()).Unwrap();
        ASSERT_TRUE(document.IsArray());
        EXPECT_STREQ(document[1]["name"].GetString(), "First");
        RecentProjectStore reloaded(storeFile);
        ASSERT_TRUE(reloaded.Load().IsOk());
        ASSERT_EQ(reloaded.Projects().size(), 2);
        EXPECT_EQ(reloaded.Projects().back().path, Common::Path(firstProject).Canonical().String());
    }

    TEST_F(ProjectTest, ReportsInvalidRecentProjectsAndWriteFailures)
    {
        const Common::Path storeFile(directory / "RecentProjects.json");
        WriteFile(storeFile.String(), "not JSON");
        RecentProjectStore store(storeFile);
        EXPECT_TRUE(store.Load().IsErr());
        WriteFile(storeFile.String(), "[{\"path\": 42}]");
        EXPECT_TRUE(store.Load().IsErr());

        RecentProjectStore unwritable { Common::Path(directory) };
        EXPECT_TRUE(unwritable.Record(Common::Path(directory)).IsErr());
        EXPECT_TRUE(unwritable.Projects().empty());
    }

    TEST_F(ProjectTest, LauncherPassesPathsAsArgumentsAndPropagatesFailures)
    {
        const Common::Path project("C:/Projects with spaces/A&B");
        bool launched = false;
        ProjectLauncher launcher(
            EditorLaunchOptions { "Editor with spaces.exe", "vulkan", true, true },
            [&](const std::string& inExecutable, const std::vector<std::string>& inArguments) -> Common::Result<void, std::string> {
                launched = true;
                EXPECT_EQ(inExecutable, "Editor with spaces.exe");
                EXPECT_EQ(inArguments, (std::vector<std::string> { "-project", project.String(), "-rhi", "vulkan", "-gpuDebug", "-softwareGpu" }));
                return Common::Err(std::string("launch failed"));
            });
        const auto result = launcher.Launch(project);
        EXPECT_TRUE(launched);
        ASSERT_TRUE(result.IsErr());
        EXPECT_EQ(result.Error(), "launch failed");
    }

    TEST_F(ProjectTest, LauncherOmitsDisabledFlags)
    {
        ProjectLauncher launcher(
            EditorLaunchOptions { "Editor.exe", "dx12", false, false },
            [](const std::string&, const std::vector<std::string>& inArguments) -> Common::Result<void, std::string> {
                EXPECT_EQ(inArguments, (std::vector<std::string> { "-project", "Project", "-rhi", "dx12" }));
                return Common::Ok();
            });
        EXPECT_TRUE(launcher.Launch(Common::Path("Project")).IsOk());
        EXPECT_TRUE(Common::Process::Launch((directory / "MissingExecutable").string(), {}).IsErr());
    }
}
