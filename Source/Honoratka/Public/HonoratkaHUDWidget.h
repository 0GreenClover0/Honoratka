#pragma once

#include "CoreMinimal.h"
#include "Timer.h"
#include "Blueprint/UserWidget.h"
#include "HonoratkaHUDWidget.generated.h"

/**
 *
 */
UCLASS()
class HONORATKA_API UHonoratkaHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTimerWidget> TimerWidget;

    UFUNCTION()
    void SetTimer(ATimer* Timer);
};
