#pragma once

#include "CoreMinimal.h"
#include "Timer.h"
#include "Blueprint/UserWidget.h"
#include "TimerWidget.generated.h"

/**
 *
 */
UCLASS()
class HONORATKA_API UTimerWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION()
    void SetTimer(ATimer* InTimer);

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<ATimer> TimerRef;
};
