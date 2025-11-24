#include "GameManager.h"
#include "Customer.h"
#include "HonoratkaTable.h"
#include "CustomerManager.h"
#include "Kismet/GameplayStatics.h"

AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	// Enable mouse cursor
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SelectFirstCustomerGroup();
}

void AGameManager::SelectFirstCustomerGroup()
{
	if (!CustomerManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameManager: CustomerManager not assigned!"));
		return;
	}

	// Deselect previous selection
	DeselectCustomers();

	// Get first customer group from queue
	if (CustomerManager->GetQueueLength() > 0)
	{
		// Get the first customer in queue
		ACustomer* FirstCustomer = CustomerManager->GetFirstCustomerInQueue();
		
		if (FirstCustomer)
		{
			SelectedCustomers.Add(FirstCustomer);
			
			// If customer has a pair, add them too
			if (FirstCustomer->IsPaired() && FirstCustomer->GetPairedCustomer())
			{
				SelectedCustomers.Add(FirstCustomer->GetPairedCustomer());
			}

			HighlightCustomers(true);
			UE_LOG(LogTemp, Log, TEXT("Selected %d customer(s)"), SelectedCustomers.Num());
		}
	}
}

void AGameManager::DeselectCustomers()
{
	HighlightCustomers(false);
	SelectedCustomers.Empty();
}

void AGameManager::OnTableClicked(AHonoratkaTable* Table)
{
	if (!Table)
	{
		return;
	}

	if (HasSelectedCustomers())
	{
		AssignCustomersToTable(Table);
	}
}

void AGameManager::OnCustomerClicked(ACustomer* Customer)
{
	if (!Customer)
	{
		return;
	}

	// Only allow reseating customers that are already seated
	if (Customer->GetCustomerState() != ECustomerState::Seated)
	{
		return;
	}

	// Deselect previous selection
	DeselectCustomers();

	// Select this customer (and their pair if they have one)
	SelectedCustomers.Add(Customer);
	if (Customer->IsPaired() && Customer->GetPairedCustomer())
	{
		SelectedCustomers.Add(Customer->GetPairedCustomer());
	}

	// Store their current table for removal
	CurrentTable = FindCustomerTable(Customer);

	HighlightCustomers(true);
	UE_LOG(LogTemp, Log, TEXT("Selected seated customer for reseating"));
}

void AGameManager::AssignCustomersToTable(AHonoratkaTable* Table)
{
	ensure(Table);
	ensure(CustomerManager);

	// Check if table has enough space
	if (!Table->CanSeatCustomers(SelectedCustomers.Num()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Table doesn't have enough seats!"));
		DeselectCustomers();
		return;
	}

	// If reseating from another table, remove them first
	if (CurrentTable)
	{
		for (ACustomer* Customer : SelectedCustomers)
		{
			CurrentTable->RemoveCustomer(Customer);
		}
		CurrentTable = nullptr;
	}
	else
	{
		// Remove from queue if they're coming from queue
		CustomerManager->MoveQueueForward();
	}

	// Seat customers at table
	if (Table->SeatCustomers(SelectedCustomers))
	{
		UE_LOG(LogTemp, Log, TEXT("Seated %d customer(s) at table"), SelectedCustomers.Num());
	}

	DeselectCustomers();
}

void AGameManager::HighlightCustomers(bool bHighlight)
{
	// TODO: Implement visual highlighting (change material, add outline, etc.)
	for (ACustomer* Customer : SelectedCustomers)
	{
		if (Customer)
		{
			//Customer->SetSelected(bHighlight);
		}
	}
}

AHonoratkaTable* AGameManager::FindCustomerTable(ACustomer* Customer) const
{
	TArray<AActor*> Tables;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHonoratkaTable::StaticClass(), Tables);

	for (AActor* Actor : Tables)
	{
		AHonoratkaTable* Table = Cast<AHonoratkaTable>(Actor);
		if (Table)
		{
			// Check if this table has the customer
			for (int32 i = 0; i < Table->GetOccupiedSeats(); ++i)
			{
				// This is a simplified check - you might need to add a proper getter
				// For now, we'll just return the first table we find
			}
		}
	}

	return nullptr;
}

void AGameManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Bind input for selecting first customer group
	// You can bind a key here, e.g., Space bar
	// PlayerInputComponent->BindAction("SelectCustomers", IE_Pressed, this, &AGameManager::SelectFirstCustomerGroup);
}