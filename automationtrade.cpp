#include "automationtrade.h"
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::AutomationTrade
/// \param parent
///
AutomationTrade::AutomationTrade(QObject *parent)
    : QThread{parent}
{
    err_singal = 0;


}
AutomationTrade::~AutomationTrade(){

    deleteLater();
}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::run
///
void AutomationTrade::run()
{

    excel = new QAxObject("Excel.Application");

    if (!excel->isNull()) {
        excel->setProperty("Visible", false); // Tornar o Excel visível (opcional)
        //excel->setProperty("AutomationSecurity", 1); // Permitir todas as automações

        // Abrir o workbook
        workbooks = excel->querySubObject("Workbooks");

        if (!workbooks->isNull()) {

            workbook = workbooks->querySubObject("Open(const QString&)", workbookName);

            if (!workbook->isNull()) {
                // Obter a planilha ativa
                sheets = workbook->querySubObject("Sheets");
                sheet = sheets->querySubObject("Item(int)", 1); // Planilha 1

                if (!sheet->isNull()) {
                    // Escrever na célula C2
                    cell = sheet->querySubObject("Range(const QString&)", "C2");
                    cell->setProperty("Value", "falso");
                    // Escrever na célula L2
                    cell = sheet->querySubObject("Range(const QString&)", "L2");
                    cell->setProperty("Value", "falso");

                } else {
                    qDebug() << "sheet é nulo";
                        err_singal = 4;
                }

            } else {
                qDebug() << "workbook é nulo";
                    err_singal = 3;
            }

        } else {
            qDebug() << "workbooks é nulo";
                err_singal = 2;
        }

    } else {
        qDebug() << "Erro ao iniciar a automação do Excel.";
        err_singal = 1;
    }

    // Cria uma conexão com o tryd para receber dados de posição
    connect(&tcpTrydClient, &QTcpSocket::readyRead, this, &AutomationTrade::on_SocketTrydEvent);
    connect(&tcpTrydClient, &QTcpSocket::connected, this, &AutomationTrade::on_trydConnected);
    connect(&tcpTrydClient, &QTcpSocket::errorOccurred, this, &AutomationTrade::connectionError);
    connect(&tcpTrydClient, &QTcpSocket::disconnected, this, &AutomationTrade::on_trydDisconnected);

    tcpTrydClient.connectToHost("localhost",12002);



    while (!isInterruptionRequested()) // Verifica se a thread deve ser interrompida
    {
        // Coloque sua lógica de tarefa aqui
        if (operationEntry){
                // Escrever na célula C2
                cell = sheet->querySubObject("Range(const QString&)", "C2");
                cell->setProperty("Value", "falso");
                // Escrever na célula L2
                cell = sheet->querySubObject("Range(const QString&)", "L2");
                cell->setProperty("Value", "falso");
                // Escrever na célula A2
                // Escreve o ativo, porem teria q implementar uma logica pra verificar o ativo da planilha
                // e tmb verificar em qual linha ta o ativo desejado, pois a classe suporta varios ativos
                cell = sheet->querySubObject("Range(const QString&)", "A2");
                cell->setProperty("Value", dadosDeOperacao.ativo);

                if (dadosDeOperacao.lado == "Compra"){
                    // Escrever na célula F2
                    cell = sheet->querySubObject("Range(const QString&)", "F2");
                    cell->setProperty("Value", dadosDeOperacao.lote);
                    // Escrever na célula G2
                    cell = sheet->querySubObject("Range(const QString&)", "G2");
                    cell->setProperty("Value", dadosDeOperacao.preco);
                    // Escrever na célula C2
                    cell = sheet->querySubObject("Range(const QString&)", "C2");
                    cell->setProperty("Value", "OK");

                }else if (dadosDeOperacao.lado == "Venda"){
                    // Escrever na célula F2
                    cell = sheet->querySubObject("Range(const QString&)", "I2");
                    cell->setProperty("Value", dadosDeOperacao.lote);
                    // Escrever na célula G2
                    cell = sheet->querySubObject("Range(const QString&)", "H2");
                    cell->setProperty("Value", dadosDeOperacao.preco);
                    // Escrever na célula C2
                    cell = sheet->querySubObject("Range(const QString&)", "L2");
                    cell->setProperty("Value", "OK");
                }
                dadosDeOperacao.lote = 0; //para garantir que nenhuma ordem sera executada posterior

        }




        operationEntry = false;
        // Insira um atraso de 10 ms (0,01 segundo)
        QThread::msleep(10);

        ////////////////////////////////////////////////////////
        /// zerar uma posição em caso de ultrapassar o limite de perda
        if (!contaInfo.isEmpty() && !limitePorAtivo.isEmpty() && !stop_execute){
            for(int i = 0; i <= contaInfo.size()-1; i++){ //verifico a toda lista de posições
                if (contaInfo[i].posLote != 0){ // caso esteja posicionado
                    for (int j = 0; j <= limitePorAtivo.size()-1; j++){ //verifico a lista de permissões
                        if (contaInfo[i].ativo.left(3) == limitePorAtivo[j].ativoPermitido.left(3)){ //quando encontra o mesmo ativo
                            if ((contaInfo[i].resAberto+contaInfo[i].resFechado) <= (-1.0*limitePorAtivo[j].perdaMax)){ //caso ultrapasse o limite
                                float p = contaInfo[i].posPreco;
                                if (contaInfo[i].posLote > 0){
                                    p -= 100;
                                    qDebug() << "Te stopei malandro";
                                    stop_execute = true;
                                    ativoToStop = limitePorAtivo[j].ativoPermitido;
                                    executaStop(ativoToStop, p, contaInfo[i].posLote,"Venda");
                                }else{
                                    p += 100;
                                    qDebug() << "Te stopei malandro";
                                    stop_execute = true;
                                    ativoToStop = limitePorAtivo[j].ativoPermitido;
                                    executaStop(ativoToStop, p, -1.0*contaInfo[i].posLote,"Compra");
                                }
                            }
                        }
                    }
                }
            }
        }







    }



    // Salvar o workbook (opcional)
    workbook->dynamicCall("Save()");
    // Fechar o workbook (opcional)
    workbook->dynamicCall("Close()");
    delete cell;
    delete sheet;
    delete sheets;
    // Fechar o Excel (opcional)
    excel->dynamicCall("Quit()");
    delete workbook;
    delete workbooks;
    delete excel;

//    if (tcpTrydClient.state() == QAbstractSocket::ConnectedState) {
//    // Encerre a conexão antes de encerrar a thread
    tcpTrydClient.disconnectFromHost();
    tcpTrydClient.waitForDisconnected();
//    }

}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::on_trydConnected
///
void AutomationTrade::on_trydConnected(){
    qDebug() << "Conectou no tryd pela thread";

    QString msg_send;

    msg_send = "RST$S|WDOFUT|123|0|#";

    tcpTrydClient.write(msg_send.toLocal8Bit().constData());
}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::on_SocketTrydEvent
///
void AutomationTrade::on_SocketTrydEvent(){
    qDebug() << "Recebeu algo";

    QByteArray recData = tcpTrydClient.readAll();
    QString receivedData = QString::fromUtf8(recData);

    qDebug() << receivedData;

    ////////////////////////////////////////////////////////
    /// Implementar logica para separa os dados recebidos
    ///
    int posStart;
    int posEnd;
    informacaoDeConta aux;

    posStart = receivedData.indexOf('!'); //Encontra 1° separador
    posEnd = receivedData.indexOf('|', posStart + 1); //Encontra 2° separador
    aux.ativo = receivedData.mid(posStart + 1, 3); //Copia a String entre os separadores

    posStart = posEnd;
    posEnd = receivedData.indexOf('|', posStart + 1);
    aux.conta = receivedData.mid(posStart + 1, posEnd - posStart - 1); //Copia a String entre os separadores

    posStart = posEnd;
    posEnd = receivedData.indexOf('|', posStart + 1);
    aux.resAberto = receivedData.mid(posStart + 1, posEnd - posStart - 1).replace(',', '.').toFloat(); //Copia a String entre os separadores

    posStart = posEnd;
    posEnd = receivedData.indexOf('|', posStart + 1);
    aux.resFechado = receivedData.mid(posStart + 1, posEnd - posStart - 1).replace(',', '.').toFloat(); //Copia a String entre os separadores

    //// pulo 4 barras
    posStart = receivedData.indexOf('|', posEnd + 1);
    posStart = receivedData.indexOf('|', posStart + 1);
    posStart = receivedData.indexOf('|', posStart + 1);
    posStart = receivedData.indexOf('|', posStart + 1);
    ////
    posEnd = receivedData.indexOf('|', posStart + 1);
    aux.posLote = receivedData.mid(posStart + 1, posEnd - posStart - 1).toInt(); //Copia a String entre os separadores

    posStart = posEnd;
    posEnd = receivedData.indexOf('|', posStart + 1);
    aux.posPreco = receivedData.mid(posStart + 1, posEnd - posStart - 1).replace(',', '.').toFloat(); //Copia a String entre os separadores

    if (contaInfo.isEmpty()){
        contaInfo.append(aux);
        return;
    }
    for (int i = 0; i <= contaInfo.size()-1; i++){
        if (contaInfo[i].ativo == aux.ativo && contaInfo[i].conta == aux.conta){
            contaInfo[i] = aux;
            if (stop_execute){
                if(ativoToStop.left(3) == aux.ativo.left(3)){
                    if(aux.posLote == 0) stop_execute = false;
                }
            }
            return;
        }
    }
    contaInfo.append(aux);

}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::connectionError
/// \param socketError
///
void AutomationTrade::connectionError(QAbstractSocket::SocketError socketError) {
    qDebug() << "Erro de conexão:" << socketError;
    emit err_singal = -10;
}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::on_trydDisconnected
///
void AutomationTrade::on_trydDisconnected(){
    qDebug() << "Desconectou";
}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::executaOperacao
/// \param ativo
/// \param preco
/// \param lote
/// \return
///
void AutomationTrade::executaOperacao(QString ativof, float precof, int lotef, QString ladof){

    if(limitePorAtivo.isEmpty()) return emit err_signal(11); //lista de ativos habilitados vazia

    bool ativoExiste = false;
    int indexDoAtivo = 0;
    /////////////////////////////////////////////////////////////
    /// Verifico se o ativo esta na lista limitePorAtivo
    for (int i = 0; i <= limitePorAtivo.size() - 1; i++){
        if (ativof.left(3) == limitePorAtivo[i].ativoPermitido.left(3)){
                ativoExiste = true;
                indexDoAtivo = i;
                break;
        }
    }
    if (ativoExiste == false) return emit err_signal(12); //Ativo não habilitado para operação.

    if (lotef > limitePorAtivo[indexDoAtivo].loteMax) return emit err_signal(13); //Lote solicitado exede o limite permitido

    if (ladof != "Compra" && ladof != "Venda") {
        return emit err_signal(14); // O lado solicitado não esta correto ou não existe
    }

    /////////////////////////////////////////////////////////////
    /// Apos passar pela verificação de limite, é verificado as
    /// posições abertas e resultado.
    if (!contaInfo.isEmpty()){
        for (int i = 0; i <= contaInfo.size()-1; i++){
            if (contaInfo[i].ativo.left(3) == ativof.left(3)){
                if (ladof == "Compra"){
                    if ((contaInfo[i].resFechado + contaInfo[i].resAberto) <= -1*limitePorAtivo[indexDoAtivo].perdaMax)
                        if (contaInfo[i].posLote > 0) return emit err_signal(15); // impede uma operação caso já tenha atingido o limite de perda
                    if (contaInfo[i].posLote + lotef > limitePorAtivo[indexDoAtivo].loteMax)
                        return emit err_signal(16);
                }
                if (ladof == "Venda"){
                    if ((contaInfo[i].resFechado + contaInfo[i].resAberto) <= -1*limitePorAtivo[indexDoAtivo].perdaMax)
                        if (contaInfo[i].posLote < 0) return emit err_signal(15); // impede uma operação caso já tenha atingido o limite de perda
                    if (contaInfo[i].posLote - lotef < -1*limitePorAtivo[indexDoAtivo].loteMax)
                        return emit err_signal(16);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////
    /// Operação permitida para execução
    /// Será realizada no run() quando operationEntry for true.
    dadosDeOperacao.ativo = ativof;
    dadosDeOperacao.preco = precof;
    dadosDeOperacao.lote = lotef;
    dadosDeOperacao.lado = ladof;
    operationEntry = true;

    return; //Operação requisitada com sucesso.
    // para verificar se a operação foi aberta, solicite o metodo getPosition()
}
void AutomationTrade::executaStop(QString ativof, float precof, int lotef, QString ladof){

    /////////////////////////////////////////////////////////////
    /// Operação permitida para execução
    /// Será realizada no run() quando operationEntry for true.
    dadosDeOperacao.ativo = ativof;
    dadosDeOperacao.preco = precof;
    dadosDeOperacao.lote = lotef;
    dadosDeOperacao.lado = ladof;
    operationEntry = true;

    return;
}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::addAtivoPermitido
/// \param ativof
/// \param loteMaxf
/// \return
///
int AutomationTrade::adicionaAtivoPermitido(QString ativof, int loteMaxf, float perdaMaxf){

    limites aux;

    if(limitePorAtivo.isEmpty()){
        aux.ativoPermitido = ativof;
        if (loteMaxf > 0) aux.loteMax = loteMaxf;
        else aux.loteMax = 0;
        if (perdaMaxf > 0) aux.perdaMax = perdaMaxf;
        else aux.perdaMax = 0;
        limitePorAtivo.append(aux);
    }else{
        for (int i = 0; i <= limitePorAtivo.size() - 1; i++){
            if (ativof == limitePorAtivo[i].ativoPermitido){
                limitePorAtivo[i].loteMax = loteMaxf;
                limitePorAtivo[i].perdaMax = perdaMaxf;
                return 0;
            }
        }
        aux.ativoPermitido = ativof;
        if (loteMaxf > 0) aux.loteMax = loteMaxf;
        else aux.loteMax = 0;
        if (perdaMaxf > 0) aux.perdaMax = perdaMaxf;
        else aux.perdaMax = 0;
        limitePorAtivo.append(aux);
    }

    return 0;
}
/////////////////////////////////////////////////////////////
/// \brief AutomationTrade::removeAtivoPermitido
/// \param ativof
/// \return
///
int AutomationTrade::removeAtivoPermitido(QString ativof){

    limites aux;

    if(limitePorAtivo.isEmpty()){
        return 1; // Lista vazia
    }else{
        if (ativof == "ALL"){ // limpa a lista
            limitePorAtivo.clear();
            return 0; //Ativos removidos
        }
        for (int i = 0; i <= limitePorAtivo.size() - 1; i++){
            if (ativof == limitePorAtivo[i].ativoPermitido){
                limitePorAtivo.removeAt(i);
                return 0; // Ativo removido
            }
        }
    }

    return 2; // Ativo não encontrado na lista
}


