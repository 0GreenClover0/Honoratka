#include "CustomerAlbum.h"

bool ACustomerAlbum::IsCustomerFamiliar(const FCustomerTypeInstance& CustomerTypeInstance) const
{
    return FamiliarCustomers.ContainsByPredicate([&CustomerTypeInstance]
    (const FCustomerTypeInstance& Inst)
        {
            return CustomerTypeInstance.IsMale == Inst.IsMale
                && CustomerTypeInstance.Type == Inst.Type
                && CustomerTypeInstance.Accessory1 == Inst.Accessory1
                && CustomerTypeInstance.Accessory2 == Inst.Accessory2
                && CustomerTypeInstance.Accessory3 == Inst.Accessory3;
        });
}

void ACustomerAlbum::AddFamiliarCustomer(const FCustomerTypeInstance& CustomerTypeInstance)
{
    FamiliarCustomers.Add(CustomerTypeInstance);
}
