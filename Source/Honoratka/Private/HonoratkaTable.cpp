#include "HonoratkaTable.h"
#include "Customer.h"
#include "CustomerManager.h"
#include "CustomerWork.h"
#include "DrawDebugHelpers.h"
#include "GameManager.h"
#include "Honoratka.h"
#include "PlayerManager.h"
#include "Kismet/GameplayStatics.h"

AHonoratkaTable::AHonoratkaTable()
{
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    SeatComponents.Reserve(MaxSeats);
    for (int32 i = 0; i < MaxSeats; ++i)
    {
        FString Name = FString::Printf(TEXT("Seat%d"), i);
        USceneComponent* SeatComponent = CreateDefaultSubobject<USceneComponent>(FName(Name));
        SeatComponent->SetupAttachment(RootComponent);
        SeatComponents.Add(SeatComponent);
    }
}

void AHonoratkaTable::BeginPlay()
{
    Super::BeginPlay();

    GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    CustomerManager = Cast<ACustomerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACustomerManager::StaticClass()));
    PlayerManager = Cast<APlayerManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerManager::StaticClass()));

    GetComponents(MeshComponents);

    InitializeSeats();
}

void AHonoratkaTable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FGreatWorkRequirements CustomersAtTable = {};
    TArray<ACustomer*> Customers = GetCustomers();

    for (int32 i = 0; i < Customers.Num(); ++i)
    {
        FCustomerTypeInstance CustomerTypeInstance = Customers[i]->GetCustomerTypeInstance();

        // Unfamiliar customers do not contribute to making movies.
        if (!CustomerManager->IsCustomerFamiliar(CustomerTypeInstance))
        {
            continue;
        }

        // Customers that have already created a great work, can't create another.
        if (Customers[i]->bHasMadeAGreatWork)
        {
            continue;
        }

        if (CustomerTypeInstance.Type == static_cast<int32>(ECustomerType::Actor))
        {
            if (CustomerTypeInstance.IsMale)
            {
                CustomersAtTable.MaleActors += 1;
            }
            else
            {
                CustomersAtTable.FemaleActors += 1;
            }
        }
        else if (CustomerTypeInstance.Type == static_cast<int32>(ECustomerType::Director))
        {
            CustomersAtTable.Directors += 1;
        }
        else if (CustomerTypeInstance.Type == static_cast<int32>(ECustomerType::Screenwriter))
        {
            CustomersAtTable.Screenwriters += 1;
        }
    }

    for (int32 i = 0; i < CustomerManager->GreatWorks.Num(); ++i)
    {
        UCustomerWork* CustomerWork = CustomerManager->GreatWorks[i];
        const TArray<FGreatWorkRequirements>& Requirements = CustomerWork->GetRequirements();

        for (int32 r = 0; r < Requirements.Num(); ++r)
        {
            if (Requirements[r] == CustomersAtTable)
            {
                bool Advancing = CustomerWork->AdvanceWork(DeltaTime);

                if (Advancing && CustomerWork->IsFinished())
                {
                    for (int32 c = 0; c < Customers.Num(); ++c)
                    {
                        Customers[c]->bHasMadeAGreatWork = true;
                    }
                }

                if (Advancing)
                {
                    return;
                }
            }
        }
    }
}

void AHonoratkaTable::NotifyActorBeginCursorOver()
{
    Super::NotifyActorBeginCursorOver();

    if (PlayerManager->HasSelectedObject())
    {
        return;
    }

    for (int32 i = 0; i < MeshComponents.Num(); ++i)
    {
        MeshComponents[i]->SetOverlayMaterial(OutlineMaterial);
        MeshComponents[i]->bDisallowNanite = true;
    }
}

void AHonoratkaTable::NotifyActorEndCursorOver()
{
    Super::NotifyActorEndCursorOver();

    for (int32 i = 0; i < MeshComponents.Num(); ++i)
    {
        MeshComponents[i]->SetOverlayMaterial(nullptr);
        MeshComponents[i]->bDisallowNanite = false;
    }
}

void AHonoratkaTable::InitializeSeats()
{
    Seats.Empty();
    for (int32 i = 0; i < MaxSeats; ++i)
    {
        FTableSeat Seat;
        Seat.SeatPosition = SeatComponents[i]->GetComponentLocation();
        Seat.SeatType = static_cast<ESeatType>(i);
        Seats.Add(Seat);
    }
}

bool AHonoratkaTable::CanSeatCustomers(int32 Count) const
{
    return GetAvailableSeats() >= Count;
}

bool AHonoratkaTable::SeatCustomers(TArray<ACustomer*> Customers)
{
    ensure(CanSeatCustomers(Customers.Num()));

    int32 SeatIndex = 0;
    for (ACustomer* Customer : Customers)
    {
        ensure(Customer);

        // Find next available seat
        while (SeatIndex < Seats.Num() && Seats[SeatIndex].IsOccupied())
        {
            SeatIndex++;
        }

        if (SeatIndex >= Seats.Num())
        {
            return false;
        }

        Seats[SeatIndex].Customer = Customer;
        Customer->SeatCustomer(this, Seats[SeatIndex].SeatPosition);
        SeatIndex++;
    }

    return true;
}

void AHonoratkaTable::RemoveCustomer(ACustomer* Customer)
{
    for (FTableSeat& Seat : Seats)
    {
        if (Seat.Customer == Customer)
        {
            Seat.Customer = nullptr;
            Customer->Table = nullptr;
            return;
        }
    }
}

void AHonoratkaTable::ClearTable()
{
    for (FTableSeat& Seat : Seats)
    {
        Seat.Customer = nullptr;
    }
}

int32 AHonoratkaTable::GetAvailableSeats() const
{
    int32 Available = 0;
    for (const FTableSeat& Seat : Seats)
    {
        if (!Seat.IsOccupied())
        {
            Available++;
        }
    }
    return Available;
}

int32 AHonoratkaTable::GetOccupiedSeats() const
{
    return MaxSeats - GetAvailableSeats();
}

TArray<ACustomer*> AHonoratkaTable::GetCustomers() const
{
    TArray<ACustomer*> Customers;

	for (const FTableSeat& Seat : Seats)
    {
        if (Seat.Customer != nullptr)
        {
            Customers.Add(Seat.Customer);
        }
    }

    return Customers;
}

bool AHonoratkaTable::HasCustomer(ACustomer* Customer) const
{
    for (const FTableSeat& Seat : Seats)
    {
        if (Seat.Customer == Customer)
        {
            return true;
        }
    }

    return false;
}

FTableSeat AHonoratkaTable::GetCustomerSeat(ACustomer* Customer) const
{
    for (const FTableSeat& Seat : Seats)
    {
        if (Seat.Customer == Customer)
        {
            return Seat;
        }
    }

    ensure(false);
    return {};
}

void AHonoratkaTable::NotifyActorOnClicked(FKey ButtonPressed)
{
    if (GameManager->IsPaused())
    {
        return;
    }

    Super::NotifyActorOnClicked(ButtonPressed);

    GameManager->OnTableClicked(this);
}
