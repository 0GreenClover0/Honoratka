// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WinManager.generated.h"

class ATimer;

UCLASS()
class HONORATKA_API AWinManager : public AActor
{
    GENERATED_BODY()

public:
    AWinManager();
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere)
    ATimer* TimerRef = nullptr;

protected:
    virtual void BeginPlay() override;

private:

    UFUNCTION()
    void HandleTimerFinished();
};
