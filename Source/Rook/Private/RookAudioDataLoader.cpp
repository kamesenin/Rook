/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#include "RookPrivatePCH.h"
#include "RookAudioDataLoader.h"

bool URookAudioDataLoader::IsTickable() const
{
	return true;
}

TStatId URookAudioDataLoader::GetStatId() const
{
	return GetStatID();
}

void URookAudioDataLoader::Tick(float DeltaTime)
{
	if (LoadingGraph.Num() == 0) { return; }
	
	if (LoadingGraph[0] != nullptr)
	{
		if (LoadingGraph[0]->IsComplete()) 
			LoadingGraph.RemoveAt(0);			
	}
	else 
	{
		LoadingGraph.RemoveAt(0);
	}	
}