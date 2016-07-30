/***
Rook Audio Plugin
Created by Tomasz 'kamesenin' Witczak - kamesenin@gmail.com
**/
#pragma once
#include "Runtime/Core/Public/Async/TaskGraphInterfaces.h"

class RookAudioDataLoadingTask {
public:
	RookAudioDataLoadingTask( TArray<TWeakObjectPtr<class USoundWave>> Assets ):AudioAssets( Assets ) {};

	static const TCHAR* GetTaskName() {
		return TEXT("RookAudioDataLoadingTask");
	}

	FORCEINLINE static TStatId GetStatId() {
		RETURN_QUICK_DECLARE_CYCLE_STAT( RookAudioDataLoadingTask, STATGROUP_TaskGraphTasks );
	}

	static ENamedThreads::Type GetDesiredThread() {
		return ENamedThreads::AnyThread;
	}

	static ESubsequentsMode::Type GetSubsequentsMode() {
		return ESubsequentsMode::FireAndForget;
	}

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent);

private:
	void LoadData();
	UPROPERTY()
	TArray< TWeakObjectPtr<class USoundWave> > AudioAssets;
};