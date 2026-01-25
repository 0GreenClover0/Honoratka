#pragma once

#include "CoreMinimal.h"
#include "Timer.h"
#include "Blueprint/UserWidget.h"
#include "HonoratkaHUDWidget.generated.h"

USTRUCT(BlueprintType)
struct FBPCustomerTypeInstance
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsMale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Accessory1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Accessory2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Accessory3;
};

struct FCustomerTypeInstance;
/**
 *
 */
UCLASS()
class HONORATKA_API UHonoratkaHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UFUNCTION()
    void SetTimer(ATimer* Timer);

    UFUNCTION()
    void RegisterCustomer(const FCustomerTypeInstance& CustomerTypeInstance);

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTimerWidget> TimerWidget;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UAlbumWidget> AlbumWidget;
};
