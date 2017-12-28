using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class Rook : ModuleRules
    {
        public Rook(ReadOnlyTargetRules target) : base(target)
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
                    "CoreUObject"
                }
            );

            if (UEBuildConfiguration.bBuildEditor == true)
            {
                PublicDependencyModuleNames.AddRange(
                    new string[] {
                        "UnrealEd",
                        "LevelEditor",
                        "EditorStyle"
                    }
                );
            }
        }
        
        private string ModulePath
        {
            get { return ModuleDirectory; }
        }

        private string OpenALSoftPath
        {
            get { return Path.GetFullPath(Path.Combine(ModulePath, "OpenALSoft")); }
        }
    }
}