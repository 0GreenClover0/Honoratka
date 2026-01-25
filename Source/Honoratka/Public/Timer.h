// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Timer.generated.h"

class AGameManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimerFinishedDelegate);

UCLASS()
class HONORATKA_API ATimer : public AActor
{
    GENERATED_BODY()

public:
    ATimer();
    void SetTimer();

    float GetTimeLeft() const;

    UFUNCTION(BlueprintPure)
    FString GetTimeLeftReadable();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void OnTimerFired();
    void FinishTimer(); // will broadcast

public:
    UPROPERTY(EditAnywhere)
    float MaxTime = 120.0f; // The time of a "round", let's say

    // Public delegate others can bind to
    UPROPERTY(BlueprintAssignable)
    FTimerFinishedDelegate OnTimerFinished;

private:
    FTimerHandle TimerHandle = {};
    float TimerInterval = 1.0f;
    float TimeLeft = 0.0f;

    float WarningTime = 8.5f;
    bool bHasPlayedWarningSound = false;

    TObjectPtr<AGameManager> GameManager = nullptr;
};
