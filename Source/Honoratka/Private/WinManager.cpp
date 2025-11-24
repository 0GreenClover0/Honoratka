// Fill out your copyright notice in the Description page of Project Settings
#include "WinManager.h"

#include "Timer.h"
#include "Kismet/GameplayStatics.h"

AWinManager::AWinManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWinManager::BeginPlay()
{
    Super::BeginPlay();

    ensure(TimerRef);

    if (TimerRef->IsValidLowLevel())
    {
        if (ATimer* Timer = Cast<ATimer>(TimerRef))
        {
            Timer->OnTimerFinished.AddDynamic(this, &AWinManager::HandleTimerFinished);
        }
        else
        {
            ensure(Timer);
        }
    }
}

void AWinManager::HandleTimerFinished()
{
    //UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}

void AWinManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

