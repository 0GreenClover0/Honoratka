// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Timer.generated.h"

UCLASS()
class HONORATKA_API ATimer : public AActor
{
    GENERATED_BODY()

public:
    ATimer();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
};
