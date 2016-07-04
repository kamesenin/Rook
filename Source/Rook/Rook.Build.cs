using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class Rook : ModuleRules
    {
        public Rook(TargetInfo target)
        {
            PrivateIncludePaths.AddRange(new string[] { "Rook/Private" });

            PublicIncludePaths.AddRange(
                new string[] {
                    "Rook/Public",
                    Path.Combine( OpenALSoftPath, "include" )
                }
            );

            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "Engine",
                    "InputCore",
                    "Core",
                    "EditorStyle",
                    "CoreUObject"
                }
            );

            if (UEBuildConfiguration.bBuildEditor == true)
            {
                PublicDependencyModuleNames.AddRange(
                    new string[] {
                        "UnrealEd",
                        "LevelEditor",
                    }
                );
            }
        }

        private string ModulePath
        {
            get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
        }

        private string OpenALSoftPath
        {
            get { return Path.GetFullPath(Path.Combine(ModulePath, "OpenALSoft")); }
        }
    }
}