#include "PlayerManager.h"

#include "PlayerCursor.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

APlayerManager::APlayerManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void APlayerManager::BeginPlay()
{
    Super::BeginPlay();

    PlayerCursor = CreateWidget<UPlayerCursor>(GetWorld(), PlayerCursorWidgetClass);
    PlayerCursor->AddToViewport(1);

    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    PlayerController->SetShowMouseCursor(false);
}

void APlayerManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
