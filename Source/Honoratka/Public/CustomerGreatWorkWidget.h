#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CustomerGreatWorkWidget.generated.h"

class UTextBlock;
class UImage;

UCLASS(abstract)
class HONORATKA_API UCustomerGreatWorkWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	UImage* GetPosterImage();

	UFUNCTION(BlueprintImplementableEvent)
	UTextBlock* GetDescriptionText();
};
