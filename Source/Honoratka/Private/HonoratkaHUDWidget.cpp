#include "HonoratkaHUDWidget.h"
#include "TimerWidget.h"
#include "AlbumWidget.h"
#include "Customer.h"
#include "CustomerManager.h"
#include "HonoratkaPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UHonoratkaHUDWidget::SetTimer(ATimer* Timer)
{
    if (TimerWidget)
    {
        TimerWidget->SetTimer(Timer);
    }
}

void UHonoratkaHUDWidget::RegisterCustomer(const FCustomerTypeInstance& CustomerTypeInstance)
{
    bool bIsMale = CustomerTypeInstance.IsMale != 0;
    FBPCustomerTypeInstance BPCustomerTypeInstance =
    {
        CustomerTypeInstance.Type,
        bIsMale,
        CustomerTypeInstance.Accessory1,
        CustomerTypeInstance.Accessory2,
        CustomerTypeInstance.Accessory3,
    };

    const AHonoratkaPlayerController* PC = Cast<AHonoratkaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    const ACustomerManager* CM = PC->CustomerManagerPtr;
    FCustomerWidgetPayload Payload;

    for (int32 k = 0; k < CM->CustomerTypes.Num(); ++k)
    {
        if (CM->CustomerTypes[k].CustomerType != static_cast<ECustomerType>(CustomerTypeInstance.Type) || CM->CustomerTypes[k].bIsMale != static_cast<bool>(CustomerTypeInstance.IsMale))
        {
            continue;
        }

        Payload.MainBody = CM->CustomerTypes[k].BaseImage;

        if (CM->CustomerTypes[k].Accessory1Images.Num() > 0 && CustomerTypeInstance.Accessory1 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory1;
            Payload.Accessory1 = CM->CustomerTypes[k].Accessory1Images[AccessoryId];
        }
        else
        {
            Payload.Accessory1 = CM->TransparentImage;
        }

        if (CM->CustomerTypes[k].Accessory2Images.Num() > 0 && CustomerTypeInstance.Accessory2 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory2;
            Payload.Accessory2 = CM->CustomerTypes[k].Accessory2Images[AccessoryId];
        }
        else
        {
            Payload.Accessory2 = CM->TransparentImage;
        }

        if (CM->CustomerTypes[k].Accessory3Images.Num() > 0 && CustomerTypeInstance.Accessory3 >= 0)
        {
            const int32 AccessoryId = CustomerTypeInstance.Accessory3;
            Payload.Accessory3 = CM->CustomerTypes[k].Accessory3Images[AccessoryId];
        }
        else
        {
            Payload.Accessory3 = CM->TransparentImage;
        }
    }

    AlbumWidget->AddCustomerToScrollBox(Payload);
}
