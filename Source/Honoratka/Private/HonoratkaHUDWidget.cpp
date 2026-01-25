#include "HonoratkaHUDWidget.h"
#include "TimerWidget.h"
#include "AlbumWidget.h"
#include "Customer.h"

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

    AlbumWidget->AddCustomerToScrollBox(BPCustomerTypeInstance);
}
