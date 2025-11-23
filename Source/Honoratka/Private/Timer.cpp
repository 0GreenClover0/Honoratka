// Fill out your copyright notice in the Description page of Project Settings.
#include "Timer.h"

ATimer::ATimer()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ATimer::SetTimer()
{
    GetWorld()->GetTimerManager().SetTimer
    (
        TimerHandle,                     // handle to cancel timer at a later time
        this,                               // the owning object
        &ATimer::OnTimerFired,              // function to call on elapsed
        TimerInterval,                      // float delay until elapsed
        true                                // looping?
    );

    TimeLeft = MaxTime;
}

FString ATimer::GetTimeLeftReadable()
{
    int32 TotalSeconds = FMath::FloorToInt(TimeLeft);
    int32 Minutes = TotalSeconds / 60;
    int32 Secs = TotalSeconds % 60;

    return FString::Printf(TEXT("%02d:%02d"), Minutes, Secs);
}

void ATimer::BeginPlay()
{
    Super::BeginPlay();
    SetTimer();
}

void ATimer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATimer::OnTimerFired()
{
    TimeLeft -= TimerInterval;

    if (TimeLeft <= TimerInterval)
    {
        FinishTimer();
    }
}

void ATimer::FinishTimer()
{
    OnTimerFinished.Broadcast();
}
