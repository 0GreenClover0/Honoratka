#include "HonoratkaTable.h"
#include "Customer.h"
#include "GameManager.h"
#include "Kismet/GameplayStatics.h"

AHonoratkaTable::AHonoratkaTable()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AHonoratkaTable::BeginPlay()
{
    Super::BeginPlay();
    InitializeSeats();
}

void AHonoratkaTable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHonoratkaTable::InitializeSeats()
{
    Seats.Empty();
    for (int32 i = 0; i < MaxSeats; ++i)
    {
        FTableSeat Seat;
        Seat.SeatPosition = CalculateSeatPosition(i);
        Seats.Add(Seat);
    }
}

FVector AHonoratkaTable::CalculateSeatPosition(int32 SeatIndex) const
{
    float AngleStep = 360.0f / MaxSeats;
    float Angle = AngleStep * SeatIndex;
    float Radians = FMath::DegreesToRadians(Angle);

    FVector Offset(
        FMath::Cos(Radians) * SeatRadius,
        FMath::Sin(Radians) * SeatRadius,
        0.0f
    );

    return GetActorLocation() + Offset;
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
        Customer->SetCustomerState(ECustomerState::Seated);
        Customer->SetShowingBubble();
        Customer->SetTargetPosition(Seats[SeatIndex].SeatPosition);
        Customer->Table = this;
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

void AHonoratkaTable::NotifyActorOnClicked(FKey ButtonPressed)
{
    Super::NotifyActorOnClicked(ButtonPressed);

    AGameManager* GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
    if (GameManager)
    {
        GameManager->OnTableClicked(this);
    }
}
