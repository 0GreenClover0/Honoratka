#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
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

    TObjectPtr<UMeshComponent> MainMesh;
    TObjectPtr<UBoxComponent> Collider;

    UFUNCTION()
    void OnHoverBegin(UPrimitiveComponent* TouchedComponent);

    UFUNCTION()
    void OnHoverEnd(UPrimitiveComponent* TouchedComponent);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> IconWhenGrabbed;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<APlayerManager> PlayerManager;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> OutlineMaterial;
};
