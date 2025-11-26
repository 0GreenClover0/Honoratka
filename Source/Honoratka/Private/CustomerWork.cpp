#include "CustomerWork.h"

#include "Customer.h"
#include "CustomerGreatWorkWidget.h"
#include "CustomerManager.h"
#include "HonoratkaTable.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

UCustomerWork::UCustomerWork()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCustomerWork::BeginPlay()
{
	Super::BeginPlay();

	Customer = Cast<ACustomer>(GetOwner());

	ensure(Customer);
}

void UCustomerWork::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bWorkFinished)
	{
		DebugTime -= DeltaTime;

		if (DebugTime < 0.0f && GreatWorkWidget != nullptr)
		{
			GreatWorkWidget->RemoveFromParent();
		}

		return;
	}

	bool bCustomerIsAtTable = Customer->Table != nullptr;

	if (!bCustomerIsAtTable)
	{
		return;
	}

	// TODO: Currently this requires any one other customer to seat at the table as well. Make it more sophisticated.

	auto Customers = Customer->Table->GetCustomers();
	
	bool bAnyOtherCustomer = false;
	for (const auto& OtherCustomer : Customers)
	{
		if (OtherCustomer != Customer)
		{
			bAnyOtherCustomer = true;
		}
	}

	if (bAnyOtherCustomer)
	{
		WorkProgress += WorkSpeed * DeltaTime;
	}

	if (WorkProgress >= 100.0f)
	{
		bWorkFinished = true;

		for (const auto& OtherCustomer : Customers)
		{
			if (OtherCustomer != Customer && OtherCustomer->GetComponentByClass<UCustomerWork>()->bWorkFinished)
			{
				return;
			}
		}

		ACustomerManager* CustomerManager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
		GreatWorkWidget = CreateWidget<UCustomerGreatWorkWidget>(GetWorld(), CustomerManager->GreatWorkWidgetClass);

		GreatWorkWidget->AddToViewport();
		GreatWorkWidget->GetPosterImage()->SetBrushFromTexture(GreatWork.PosterTexture);
		GreatWorkWidget->GetDescriptionText()->SetText(GreatWork.Description);
	}
}
