// Fill out your copyright notice in the Description page of Project Settings.
#include "CoatRack.h"
#include <CustomerManager.h>
#include <Kismet/GameplayStatics.h>

ACoatRack::ACoatRack()
{
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(TEXT("/Game/Honoratka/Materials/M_OutlineShader.M_OutlineShader"));
    OutlineMaterial = MatFinder.Object;
}

void ACoatRack::BeginPlay()
{
    Super::BeginPlay();

    RackModel = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("SkeletalMesh")));
    RackCollider = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Collider")));
    ResetTimeToFall();

    RackCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    RackCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
    RackCollider->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    RackCollider->OnBeginCursorOver.AddDynamic(this, &ACoatRack::OnHoverBegin);
    RackCollider->OnEndCursorOver.AddDynamic(this, &ACoatRack::OnHoverEnd);
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
    if (FallRotation < 89.0f)
    {
        FallRotation = FMath::Lerp(FallRotation, 90.0f, 0.05f);
    }
    else
    {
        if (FallRotation != 90.0f)
        {
            FallRotation = 90.0f;
            ACustomerManager* CustomerManager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
            CustomerManager->ModifyHappiness(-CustomerManager->NumberOfCustomersInside);
        }
    }

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

void ACoatRack::OnHoverBegin(UPrimitiveComponent* TouchedComponent)
{
    RackModel->SetOverlayMaterial(OutlineMaterial);
}

void ACoatRack::OnHoverEnd(UPrimitiveComponent* TouchedComponent)
{
    RackModel->SetOverlayMaterial(nullptr);
}