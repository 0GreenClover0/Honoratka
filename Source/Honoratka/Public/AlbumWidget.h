#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlbumWidget.generated.h"

struct FBPCustomerTypeInstance;

/**
 *
 */
UCLASS()
class HONORATKA_API UAlbumWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void AddCustomerToScrollBox(const FBPCustomerTypeInstance& CustomerTypeInstance);
};
