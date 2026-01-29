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
}

void APlayerManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APlayerManager::SelectObject(TObjectPtr<AInteractable> Object, TObjectPtr<UTexture2D> Icon)
{
    SelectedObject = {
        .Object = Object,
        .DisplayedIcon = Icon,
    };

    PlayerCursor->UpdateCursor(GrabCursorTexture);

    PlayerCursor->UpdateHeldObjectIcon(Icon);
}

void APlayerManager::UnselectObject()
{
    SelectedObject = {
        .Object = nullptr,
        .DisplayedIcon = nullptr,
    };

    PlayerCursor->UpdateCursor(DefaultCursorTexture);

    PlayerCursor->UpdateHeldObjectIcon(nullptr);
}

bool APlayerManager::HasSelectedObject() const
{
    return SelectedObject.IsSet() && SelectedObject->Object != nullptr;
}
