#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

class APlayerManager;

UCLASS()
class HONORATKA_API AInteractable : public AActor
{
    GENERATED_BODY()

public:
    AInteractable();

    virtual void Tick(float DeltaTime) override;

    virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> IconWhenGrabbed;

private:
    UPROPERTY()
    TObjectPtr<APlayerManager> PlayerManager;
};
