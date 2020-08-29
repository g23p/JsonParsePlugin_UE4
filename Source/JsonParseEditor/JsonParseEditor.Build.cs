// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class JsonParseEditor : ModuleRules
{
	public JsonParseEditor(ReadOnlyTargetRules ROTargetRules) : base(ROTargetRules)
	{
		PrivateDependencyModuleNames.AddRange(
				new string[] {
					"Core",
					"CoreUObject",
					"UnrealEd",
					"EditorStyle",
					"Engine",
					"Slate",
					"SlateCore",
					"GraphEditor",
					"BlueprintGraph",
					"KismetCompiler",
					"JsonParsePlugin",
				});
	}
}
