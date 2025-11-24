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

    ECustomerState State = Customer->GetCustomerState();

    // If customer is in queue, only allow selecting if they're at the front
    if (State == ECustomerState::WaitingInQueue || State == ECustomerState::MovingForward)
    {
        ensure(CustomerManager);

        // Check if this customer is the first in queue or paired with the first
        ACustomer* FirstCustomer = CustomerManager->GetFirstCustomerInQueue();
        if (!FirstCustomer)
        {
            return;
        }

        // Allow selection only if this is the first customer OR their pair
        bool IsFirstCustomer = (Customer == FirstCustomer);
        bool IsPairOfFirst = (FirstCustomer->GetPairedCustomer() == Customer);

        if (!IsFirstCustomer && !IsPairOfFirst)
        {
            UE_LOG(LogTemp, Warning, TEXT("Can only seat customers at the front of the queue!"));
            return;
        }
    }

    // Deselect previous selection
    DeselectCustomers();

    // Select this customer
    SelectedCustomers.Add(Customer);

    // Store their current table if they're seated
    if (Customer->GetCustomerState() == ECustomerState::Seated)
    {
        PreviousTable = FindCustomerTable(Customer);
    }

    HighlightCustomers(true);

    if (State == ECustomerState::WaitingInQueue || State == ECustomerState::MovingForward)
    {
        UE_LOG(LogTemp, Log, TEXT("Selected customer from queue"));
    }
    else if (State == ECustomerState::Seated)
    {
        UE_LOG(LogTemp, Log, TEXT("Selected seated customer for reseating"));
    }
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
    if (PreviousTable)
    {
        for (ACustomer* Customer : SelectedCustomers)
        {
            PreviousTable->RemoveCustomer(Customer);
        }

        PreviousTable = nullptr;
    }
    else
    {
        // Coming from queue - remove only the selected customer from queue
        if (SelectedCustomers.Num() > 0 && SelectedCustomers[0])
        {
            ECustomerState State = SelectedCustomers[0]->GetCustomerState();
            if (State == ECustomerState::WaitingInQueue || State == ECustomerState::MovingForward)
            {
                CustomerManager->RemoveCustomerFromQueue(SelectedCustomers[0]);
            }
        }
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
            Customer->SetCustomerSelected(bHighlight);
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
        if (Table && Table->HasCustomer(Customer))
        {
            return Table;
        }
    }

    return nullptr;
}
