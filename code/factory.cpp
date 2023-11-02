#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>
#include <pcosynchro/pcomutex.h>

WindowInterface* Factory::interface = nullptr;

extern bool Run;

PcoMutex mtxFactoryTrade;
PcoMutex mtxFactoryMoney;
PcoMutex mtxFactoryStocks;

Factory::Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), resourcesNeeded(resourcesNeeded), itemBuilt(builtItem), nbBuild(0)
{
    assert(builtItem == ItemType::Chip ||
           builtItem == ItemType::Plastic ||
           builtItem == ItemType::Robot);

    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");
}

void Factory::setWholesalers(std::vector<Wholesale *> wholesalers) {
    Factory::wholesalers = wholesalers;

    for(Seller* seller: wholesalers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

ItemType Factory::getItemBuilt() {
    return itemBuilt;
}

int Factory::getMaterialCost() {
    return getCostPerUnit(itemBuilt);
}

bool Factory::verifyResources() {
    for (auto item : resourcesNeeded) {
        if (stocks[item] == 0) {
            return false;
        }
    }

    return true;
}

void Factory::buildItem() {

    // TODO
    /*  - regarder quelle type de produit l'on fabrique
     *  - commander un employé avec getEmployeeThatProduces et getEmployeeSalary
     *  - checker s'il on a assez d'argent pour commander un employé.
     *  - si assez d'argent mettre à jour les ressource et l'argent.
     */
    ItemType item = Factory::getItemBuilt();
    EmployeeType myEmployee = getEmployeeThatProduces(item);
    int salary = getEmployeeSalary(myEmployee);
    mtxFactoryMoney.lock();
    if(salary <= money){
        //mtxFactoryStocks.lock();
        money -= salary;
        nbBuild++;
        //mtxFactoryStocks.unlock();
        //Temps simulant l'assemblage d'un objet.
        PcoThread::usleep((rand() % 100) * 100000);

        // TODO
        /* mettre à jour les stock de nos produit créé*/
        //mtxFactoryStocks.lock();
        for(ItemType items : resourcesNeeded){
            this->stocks[items] -= 1;
        }
        this->stocks[item] += 1;
        //mtxFactoryStocks.unlock();
        interface->consoleAppendText(uniqueId, "Factory have build a new object");
    }
    mtxFactoryMoney.unlock();
}

void Factory::orderResources() {

    // TODO - Itérer sur les resourcesNeeded et les wholesalers disponibles
    /* - faire ici le système de trade pour commander les ressource
     * - pas oublié d'implémenter le trade avant
     */
    for(Wholesale* wholesaler : wholesalers){
        for(ItemType item : resourcesNeeded){
            mtxFactoryTrade.lock();
            if(stocks[item] == 0 && wholesaler->getItemsForSale()[item] > 0 && getCostPerUnit(item) <= this->money){
                int price = getCostPerUnit(item);
                interface->consoleAppendText(uniqueId, QString("I would like to buy %1 of ").arg(1) %
                                             getItemName(item) % QString(" which would cost me %1").arg(price));        
                int cost = wholesaler->trade(item, 1);
                if(cost != 0){                  
                    money -= cost;
                    stocks[item]++;
                    interface->consoleAppendText(uniqueId, QString("Achat réussi"));
                }
                else{
                    interface->consoleAppendText(uniqueId, QString("pas de stock"));
                }

            }
            mtxFactoryTrade.unlock();
        }
    }

    //Temps de pause pour éviter trop de demande
    PcoThread::usleep(10 * 100000);



}

void Factory::run() {
    if (wholesalers.empty()) {
        std::cerr << "You have to give to factories wholesalers to sales their resources" << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");

    while (Run) {
        if (verifyResources()) {
            buildItem();
        } else {
            orderResources();
        }
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

std::map<ItemType, int> Factory::getItemsForSale() {
    return std::map<ItemType, int>({{itemBuilt, stocks[itemBuilt]}});
}

int Factory::trade(ItemType it, int qty) {
    if(getItemsForSale()[it] >= qty && qty > 0){
        //mtxFactoryMoney.lock();
        stocks[it] -= qty;
        //mtxFactoryMoney.unlock();
        int order = getCostPerUnit(it)*qty;
        //mtxFactoryMoney.lock();
        money += order;
        //mtxFactoryMoney.unlock();
        return order;
    }
    return 0;
}

int Factory::getAmountPaidToWorkers() {
    return Factory::nbBuild * getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
}

void Factory::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

PlasticFactory::PlasticFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Plastic, {ItemType::Petrol}) {}

ChipFactory::ChipFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Chip, {ItemType::Sand, ItemType::Copper}) {}

RobotFactory::RobotFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Robot, {ItemType::Chip, ItemType::Plastic}) {}
