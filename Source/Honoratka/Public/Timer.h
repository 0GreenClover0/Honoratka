// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Timer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimerFinishedDelegate);

UCLASS()
class HONORATKA_API ATimer : public AActor
{
    GENERATED_BODY()

public:
    ATimer();
    void SetTimer();

    UPROPERTY(EditAnywhere)
    float MaxTime = 30.0f; // The time of a "round", let's say

    UFUNCTION(BlueprintPure)
    FString GetTimeLeftReadable();

    // Public delegate others can bind to
    UPROPERTY(BlueprintAssignable)
    FTimerFinishedDelegate OnTimerFinished;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void OnTimerFired();
    void FinishTimer(); // will broadcast
    FTimerHandle TimerHandle = {};
    float TimerInterval = 1.0f;
    float TimeLeft = 0.0f;
};
