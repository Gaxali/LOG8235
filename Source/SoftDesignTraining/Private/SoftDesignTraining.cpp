// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SoftDesignTraining.h"
//#include "profiler/profiler.h"
//#include "StatsData.h"

static FDelegateHandle WorldTickStartHandle;
static FDelegateHandle WorldCleanupHandle;
static FDelegateHandle WorldLogFrameStatsHandle;

void SoftDesignTrainingModuleImpl::StartupModule()
{
    // TODO: Re-enable profiler
	//MicroProfileSetEnableAllGroups(true);
	//MicroProfileSetForceMetaCounters(true);
    //WorldTickStartHandle = FWorldDelegates::OnWorldTickStart.AddStatic(WorldTickStart);

    WorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddStatic(WorldCleanup);
}

void SoftDesignTrainingModuleImpl::WorldTickStart(ELevelTick, float)
{

}

void SoftDesignTrainingModuleImpl::WorldBeginPlay()
{
    // TODO: Re-enable profiler
    //MicroProfileSetForceEnable(true);
    //StatsMasterEnableAdd();

    //FStatsThreadState& Stats = FStatsThreadState::GetLocalState();
    //WorldLogFrameStatsHandle = Stats.NewFrameDelegate.AddStatic(WorldLogFrameStats);
}

void SoftDesignTrainingModuleImpl::WorldLogFrameStats(int64 Frame)
{
    // TODO: Re-enable profiler
    //if (!MicroProfileGetForceEnable())
    //    return;

    //ProfilerConvertStats();
}

void SoftDesignTrainingModuleImpl::WorldCleanup(UWorld*, bool, bool)
{
    // TODO: Re-enable profiler
    //if (!MicroProfileGetForceEnable())
    //    return;

    //StatsMasterEnableSubtract();
    //MicroProfileSetForceEnable(false);

    //FStatsThreadState& Stats = FStatsThreadState::GetLocalState();
    //Stats.NewFrameDelegate.Remove(WorldLogFrameStatsHandle);

}

void SoftDesignTrainingModuleImpl::ShutdownModule()
{
    // TODO: Re-enable profiler
	//MicroProfileShutdown();
    FWorldDelegates::OnWorldTickStart.Remove(WorldTickStartHandle);
    FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupHandle);
}


IMPLEMENT_PRIMARY_GAME_MODULE(SoftDesignTrainingModuleImpl, SoftDesignTraining, "SoftDesignTraining");

DEFINE_LOG_CATEGORY(LogSoftDesignTraining)
 