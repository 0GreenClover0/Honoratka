// Fill out your copyright notice in the Description page of Project Settings.
#include "CoatRack.h"

void ACoatRack::BeginPlay()
{
    Super::BeginPlay();
    RackModel = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("SkeletalMesh")));
    RackCollider = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collider")));
    ResetTimeToFall();
}

void ACoatRack::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    TimeToFall -= DeltaTime;

    if (TimeToFall <= 0.0f)
    {
        Fall();
    }
    else if (TimeToFall <= 3.0f)
    {
        Shake(DeltaTime);
        RackCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

void ACoatRack::ResetTimeToFall()
{
    RackCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    TimeToFall = FMath::Lerp(MinTimeToFall, MaxTimeToFall, FMath::RandRange(0.0f, 1.0f));
    FallRotation = 0.0f;
    RackModel->SetRelativeRotation(FRotator(FallRotation, 0.0f, 0.0f));
}

void ACoatRack::Shake(float DeltaTime)
{
    FallRotation = -FMath::Sin(TimeToFall / 3.0f * 2 * PI) * 15.0f;

    RackModel->SetRelativeRotation(FRotator(FallRotation, 0.0f, 0.0f));
}

void ACoatRack::Fall()
{
    FallRotation = FMath::Lerp(FallRotation, 90.0f, 0.01f);

    RackModel->SetRelativeRotation(FRotator(FallRotation, 0.0f, 0.0f));
}

void ACoatRack::Interact_Implementation()
{
    Super::Interact_Implementation();

    if (TimeToFall <= 3.0f)
    {
        ResetTimeToFall();
    }
}