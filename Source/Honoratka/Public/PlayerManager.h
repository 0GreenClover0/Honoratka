#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerManager.generated.h"

class UPlayerCursor;

USTRUCT(BlueprintType)
struct FSelectedObject
{
    GENERATED_BODY()

    UPROPERTY()
    TObjectPtr<UTexture2D> DisplayedIcon;
};

UCLASS()
class HONORATKA_API APlayerManager : public AActor
{
    GENERATED_BODY()

public:
    APlayerManager();

    virtual void Tick(float DeltaTime) override;

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
};
