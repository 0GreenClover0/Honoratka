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

    UFUNCTION(BlueprintNativeEvent)
    void Interact();

    UFUNCTION(BlueprintNativeEvent)
    void Uninteract();

protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> IconWhenGrabbed;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<APlayerManager> PlayerManager;
};
