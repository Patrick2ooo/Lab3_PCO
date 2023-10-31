#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcomutex.h>

WindowInterface* Wholesale::interface = nullptr;
extern bool Run;

PcoMutex mtxWholesaleStocks;
PcoMutex mtxWholesaleMoney;
PcoMutex mtxWholesaleTrade;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

void Wholesale::setSellers(std::vector<Seller*> sellers) {
    this->sellers = sellers;

    for(Seller* seller: sellers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

void Wholesale::buyResources() {
    auto s = Seller::chooseRandomSeller(sellers);
    auto m = s->getItemsForSale();
    auto i = Seller::chooseRandomItem(m);

    if (i == ItemType::Nothing) {
        /* Nothing to buy... */
        return;
    }

    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(i);

    interface->consoleAppendText(uniqueId, QString("I would like to buy %1 of ").arg(qty) %
                                 getItemName(i) % QString(" which would cost me %1").arg(price));
    /* TODO */
    /* done */
    mtxWholesaleStocks.lock();
    if(m.at(i) >= qty){
        int price = s->trade(i, qty);
        money -= price;
        stocks.at(i) += qty;
    }
    mtxWholesaleStocks.unlock();
}

void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr << "You have to give factories and mines to a wholeseler before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (Run) {
        buyResources();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 100000);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");


}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

int Wholesale::trade(ItemType it, int qty) {
    if(getItemsForSale().at(it) >= qty && qty > 0){
        stocks.at(it) -= qty;
        int order = getCostPerUnit(it)*qty;
        this->money += order;
        return order;
    }
    return 0;
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
