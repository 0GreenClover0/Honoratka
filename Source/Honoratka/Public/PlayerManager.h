#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerManager.generated.h"

class AInteractable;
class UPlayerCursor;

USTRUCT(BlueprintType)
struct FSelectedObject
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<AInteractable> Object = nullptr;

    UPROPERTY()
    TObjectPtr<UTexture2D> DisplayedIcon = nullptr;
};

UCLASS()
class HONORATKA_API APlayerManager : public AActor
{
    GENERATED_BODY()

public:
    APlayerManager();

    virtual void Tick(float DeltaTime) override;

    void Interact(TObjectPtr<AInteractable> Object, TObjectPtr<UTexture2D> Icon);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UPlayerCursor> PlayerCursorWidgetClass;

public:
    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> DefaultCursorTexture;

    UPROPERTY(EditAnywhere)
    TObjectPtr<UTexture2D> GrabCursorTexture;

private:
    UPROPERTY()
    TObjectPtr<UPlayerCursor> PlayerCursor;

    TOptional<FSelectedObject> SelectedObject;
};
