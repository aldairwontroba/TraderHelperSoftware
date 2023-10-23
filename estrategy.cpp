#include "estrategy.h"

Estrategy::Estrategy(QObject *parent)
    : QThread{parent}
{

}

Estrategy::~Estrategy(){
    deleteLater();
}

void Estrategy::run(){

    emit enviarOperacao("WDOX23", 5049, 1, "Compra");

    while(1){


    };
}
