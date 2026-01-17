// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include <Components/BoxComponent.h>
#include "CoatRack.generated.h"

UCLASS()
class HONORATKA_API ACoatRack : public AInteractable
{
    GENERATED_BODY()

public:

    void Tick(float DeltaTime) override;

    virtual void Interact_Implementation() override;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Time To Fall")
    float MinTimeToFall = 10.0f;

    UPROPERTY(EditAnywhere, Category = "Time To Fall")
    float MaxTimeToFall = 20.0f;

    TObjectPtr<USkeletalMeshComponent> RackModel;
    TObjectPtr<UBoxComponent> RackCollider;

private:
    float TimeToFall = 0.0f;
    float FallRotation = 0.0f;

    void ResetTimeToFall();
    void Shake(float DeltaTime);
    void Fall();
};
