#include "Interactable.h"

#include "PlayerManager.h"
#include "Kismet/GameplayStatics.h"

AInteractable::AInteractable()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AInteractable::BeginPlay()
{
    Super::BeginPlay();

    PlayerManager = Cast<APlayerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerManager::StaticClass()));
}

void AInteractable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AInteractable::Interact_Implementation()
{
}

void AInteractable::Uninteract_Implementation()
{
}
