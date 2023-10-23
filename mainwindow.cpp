#include "mainwindow.h"
#include "ui_mainwindow.h"

//////////////////////////////////////////////////////////////////////////////
/// O que deve ser feito:
///
/*
 *<>Desenvolver uma forma de calcular a correlação entre DOLAR e os demais
 *
 *<>Desenvolver uma analise de probabilidade de o dolar subir ou cair dado
 *<>a movimentação de cada ativo.
 *
 */

//////////////////////////////////////////////////////////////////////////////
/// Variaveis globais para uso neste arquivo
///
#define BOOKDEPH 20
#define NEGOCIOS "NEGS$S|"
#define BOOKF "LVL2$S|"
#define COTACAO "COT$S|"
#define GRAFICO "GRF$S|"

TCHAR MapObj_WorkEvent_reader[] = TEXT("WorkEvent");
TCHAR MapObj_BackEvent_reader[] = TEXT("BackEvent");
HANDLE workHandle;
HANDLE backHandle;
TCHAR MapObjName[]=TEXT("MyPythonConection");
HANDLE hMapFile;
LPCTSTR pBuf;

QString DOLAR = "DOLFUT";
QString miniDOLAR = "WDOFUT";
QString INDICE = "INDFUT";
QString miniINDICE = "WINFUT";
QString VALE = "VALE3";
QString PETR = "PETR4";
QString ITUB = "ITUB4";
QString BBDC = "BBDC4";
QString ABEV = "ABEV3";
QString B3SA = "B3SA3";
QString WEGE = "WEGE3";
QString DI24 = "DI1F24";
QString DI25 = "DI1F25";
QString DI26 = "DI1F26";
QString DI27 = "DI1F27";
QString DI29 = "DI1F29";
QString DI31 = "DI1F31";
QString DI33 = "DI1F33";
//////////////////////////////////////////////////////////////////////////////
/// \brief MainWindow::MainWindow
///
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //////////////////////////////////////////////////
    ui->setupUi(this);
    loadSettings();
    //////////////////////////////////////////////////
    //desabilita o menu de estrategia
    ui->actionEstrategy->setEnabled(false);
    //////////////////////////////////////////////////
    /// \brief connect
    ///

    //////////////////////////////////////////////////
    /// inicialização de variaveis
    /// 7 variaveis monitoradas: Dol; Ind; MXN; DX; Sp; DeslDOL; DeslWDO;
    /// 4 diferenças: Dolar x MXN; Dol x SP; Dol x DX; Dol x Ind
    dataAtivo.resize(tdp.tamAtivo);
    dataDif.resize(tdp.tamAtivo);
    tempoDoTimer = 1000; //defino o tempo do timer do plot em ms
    pointsOnGraphic = 600; //defino o numero de pontos a serem plotados no grafico
    lenMedVar = 60; //numero de amostras para calcular a media curta
    mediaCurta = 60*10; //numero de pontos na media curta
    amosPorMin = 60*1000/tempoDoTimer;
    //////////////////////////////////////////////////
}
void MainWindow::loadSettings()
{
    // Configurações do arquivo myTradeSettings.ini
    QCoreApplication::setOrganizationName("AWI");
    QCoreApplication::setApplicationName("Trader Helper");

    // Restaurando o estado anterior da janela
    QSettings settings("myTradeSettings.ini", QSettings::IniFormat);
    settings.beginGroup("MainWindow");

    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    const QByteArray state = settings.value("windowState", QByteArray()).toByteArray();
    if (!state.isEmpty()) {
        restoreState(state);
    }
    QString dolaux = settings.value("dolar", QString()).toString();
    if (!dolaux.isEmpty()) {
        DOLAR = dolaux;
        miniDOLAR = settings.value("minidolar", QString()).toString();
        INDICE = settings.value("indice", QString()).toString();
        miniINDICE = settings.value("miniindice", QString()).toString();
    }


}
void MainWindow::saveSettings()
{
    // Salvando o estado atual da janela
    QSettings settings("myTradeSettings.ini", QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("dolar", DOLAR);
    settings.setValue("minidolar", miniDOLAR);
    settings.setValue("indice", INDICE);
    settings.setValue("miniindice", miniINDICE);
    settings.endGroup();
}
MainWindow::~MainWindow()
{
    saveSettings();

    if (automationTrade != nullptr)
        if (automationTrade->isRunning()){
            // Pare a thread no destrutor da classe principal
            emit stopAutomationTrade(); // Emita o sinal para parar a thread
            automationTrade->wait(); // Aguarde a thread terminar
        }

    delete ui;
}
//////////////////////////////////////////////////////////////////////////////
/// \brief Janelas
///
void MainWindow::on_actionShow_TT_triggered()
{
    if (janelaTT != nullptr) return janelaTT->raise();

    janelaTT = new MyJanelaTTDialog();

    QPoint pos = this->pos();
    pos.setX(pos.x() - 440);
    janelaTT->move(pos);

    janelaTT->show();

    janelaTT->setAttribute(Qt::WA_DeleteOnClose);

    // conectar o sinal finished() ao slot que irá definir o ponteiro para nullptr
    connect(janelaTT, &QDialog::finished, this, [this]() {
        janelaTT = nullptr;
    });
}
void MainWindow::on_actionShow_TToriginal_triggered()
{
    if (janelaTTo != nullptr) return janelaTTo->raise();

    janelaTTo = new MyJanelaTTDialog();

    QPoint pos = this->pos();
    pos.setX(pos.x() - 440);
    janelaTTo->move(pos);

    janelaTTo->show();

    janelaTTo->setAttribute(Qt::WA_DeleteOnClose);

    // conectar o sinal finished() ao slot que irá definir o ponteiro para nullptr
    connect(janelaTTo, &QDialog::finished, this, [this]() {
        janelaTTo = nullptr;
    });
}
void MainWindow::on_actionShow_Msg_triggered()
{
    if (janelaMSG != nullptr) return janelaMSG->raise();

    janelaMSG = new MyJanelaMSGDialog();

    janelaMSG->show();

    janelaMSG->setAttribute(Qt::WA_DeleteOnClose);

    // conectar o sinal finished() ao slot que irá definir o ponteiro para nullptr
    connect(janelaMSG, &QDialog::finished, this, [this]() {
        janelaMSG = nullptr;
    });
}
void MainWindow::on_actionShow_Msg_Int_triggered()
{
    if (janelaInt != nullptr) return janelaInt->raise();

    janelaInt = new MyJanelaMSGDialog();

    janelaInt->resize(200,400);
    janelaInt->show();

    janelaInt->setAttribute(Qt::WA_DeleteOnClose);

    // conectar o sinal finished() ao slot que irá definir o ponteiro para nullptr
    connect(janelaInt, &QDialog::finished, this, [this]() {
        janelaInt = nullptr;
    });
}
void MainWindow::on_actionShow_Cota_o_triggered()
{
    if (janelaCOT != nullptr) return janelaCOT->raise();

    janelaCOT = new MyJanelaCOTDialog();

    janelaCOT->resize(200,400);
    janelaCOT->show();

    janelaCOT->setAttribute(Qt::WA_DeleteOnClose);

    // conectar o sinal finished() ao slot que irá definir o ponteiro para nullptr
    connect(janelaCOT, &QDialog::finished, this, [this]() {
        janelaCOT = nullptr;
    });
}
void MainWindow::on_actionSettings_triggered()
{
    settingsDialog = new SettingsDialog(this);
    settingsDialog->lineEditDolar->setText(DOLAR);
    settingsDialog->lineEditIndice->setText(INDICE);
    connect(settingsDialog, &SettingsDialog::settingsAccepted, this, &MainWindow::onSettingsAccepted);
    settingsDialog->show();
}
void MainWindow::onSettingsAccepted(const QString &dolar, const QString &indice){
    DOLAR = dolar;
    miniDOLAR = "WDO" + dolar.mid(3, 3);
    INDICE = indice;
    miniINDICE = "WIN" + indice.mid(3, 3);

    saveSettings();


}
//////////////////////////////////////////////////////////////////////////////
/// \brief TRYD
///
void MainWindow::on_actionTryd_Connect_triggered()
{
    if (ui->actionTryd_Connect->isChecked()) {
        connect(&tcpTrydClient, &QTcpSocket::readyRead, this, &MainWindow::onSocketTrydEvent);
        connect(&tcpTrydClient, &QTcpSocket::connected, this, &MainWindow::trydConnected);
        connect(&tcpTrydClient, &QTcpSocket::disconnected, this, &MainWindow::trydDisconnected);
        connect(&tcpTrydClient, &QTcpSocket::errorOccurred, this, &MainWindow::onSocketTrydEvent);

        tcpTrydClient.connectToHost("localhost",12002);
    } else {
        tcpTrydClient.disconnectFromHost();
    }
}
void MainWindow::onSocketTrydEvent()
{
    QString msg_print;

    QByteArray recData = tcpTrydClient.readAll();
    QString receivedData = QString::fromUtf8(recData);

    QVector<bool> sizeChange = tdp.processaDados(&receivedData);

    if (janelaMSG != nullptr) janelaMSG->textEditMSG->append(receivedData);
    if (janelaCOT != nullptr){
        QString ativoSelecionado = janelaCOT->comboBoxAtivo->currentText();
        Ativos ativo = tdp.ativoToNum(ativoSelecionado);
        if (ativo != nulo) janelaCOT->textEditMSG->append(tdp.getPrintCOT(ativo));
    }

    if (janelaTT != nullptr && sizeChange[WDO] == true)
    {
        msg_print = tdp.getPrintTT(tdp.sizeTTChange[WDO], WDO);
        janelaTT->textEditTT->append(msg_print);
        msg_print = tdp.getPrintBF(8, WDO);
        janelaTT->textEditBF->append(msg_print);
    }
    if (janelaTTo != nullptr && sizeChange[WDO] == true && tdp.sizeTToFChange[WDO] > 0)
    {
        msg_print = tdp.getPrintTToF(tdp.sizeTToFChange[WDO], WDO);
        janelaTTo->textEditTT->append(msg_print);
        msg_print = tdp.getPrintBF(8, WDO);
        janelaTTo->textEditBF->append(msg_print);
    }
}
void MainWindow::trydConnected()
{
    QString msg_send;

    //    ui->textEdit_1->append("Tryd Conectado via Socket!");
    ui->statusbar->showMessage("Solicitando dados do Tryd...", 5000);

    for (int i=0;i<tdp.numAtivoBF;i++)
        tdp.dadosBF[i].resize(BOOKDEPH);

    msg_send = tdp.solicitaDados(DOLAR, NEGOCIOS);
    msg_send.append(tdp.solicitaDados(miniDOLAR, NEGOCIOS));
    msg_send.append(tdp.solicitaDados(DOLAR, BOOKF, BOOKDEPH));
    msg_send.append(tdp.solicitaDados(miniDOLAR, BOOKF, BOOKDEPH));

    msg_send.append(tdp.solicitaDados(DOLAR, COTACAO));
    msg_send.append(tdp.solicitaDados(INDICE, COTACAO));
    msg_send.append(tdp.solicitaDados(miniDOLAR, COTACAO));
    msg_send.append(tdp.solicitaDados(miniINDICE, COTACAO));

    msg_send.append(tdp.solicitaDados(VALE, COTACAO));
    msg_send.append(tdp.solicitaDados(PETR, COTACAO));
    msg_send.append(tdp.solicitaDados(ITUB, COTACAO));
    msg_send.append(tdp.solicitaDados(BBDC, COTACAO));
    msg_send.append(tdp.solicitaDados(ABEV, COTACAO));
    msg_send.append(tdp.solicitaDados(B3SA, COTACAO));

    msg_send.append(tdp.solicitaDados(DI24, COTACAO));
    msg_send.append(tdp.solicitaDados(DI25, COTACAO));
    msg_send.append(tdp.solicitaDados(DI26, COTACAO));
    msg_send.append(tdp.solicitaDados(DI27, COTACAO));
    msg_send.append(tdp.solicitaDados(DI29, COTACAO));
    msg_send.append(tdp.solicitaDados(DI31, COTACAO));
    msg_send.append(tdp.solicitaDados(DI33, COTACAO));


    tcpTrydClient.write(msg_send.toLocal8Bit().constData());

}
void MainWindow::trydDisconnected()
{
    ui->statusbar->showMessage("Desconectou do tryd", 5000);
}
void MainWindow::trydSocketError()
{
    ui->statusbar->showMessage("Conexão com tryd falhou", 5000);
}
void MainWindow::on_actionAutomatizador_triggered()
{
    if (ui->actionAutomatizador->isChecked()) {

        automationTrade = new AutomationTrade(this);

        // conecta um sinal(MainWindow::stopAutomationTrade) a um slot(AutomationTrade::requestInterruption)
        // para indicar que ocorreu uma solicitação de interrupção da thread
        connect(this, &MainWindow::stopAutomationTrade, automationTrade, &AutomationTrade::requestInterruption);
        connect(automationTrade, &AutomationTrade::err_signal, this, &MainWindow::err_SignalOnAutomationTrade);


        automationTrade->start();
        automationTrade->adicionaAtivoPermitido("WDOX23",4,400);

        //habilita o menu de estrategia
        ui->actionEstrategy->setEnabled(true);
    }else{
        if(automationTrade != nullptr){
            automationTrade->removeAtivoPermitido("ALL");
            emit stopAutomationTrade(); // Emita o sinal para parar a thread
        }
        ui->actionEstrategy->setEnabled(false);
    }
}
void MainWindow::on_actionAbrir_Boleta_triggered()
{
    if (automationTrade == nullptr) return ui->statusbar->showMessage("automationTrade não inicializado", 5000);

    if (!automationTrade->isRunning()) return ui->statusbar->showMessage("automationTrade não está em execução", 5000);

    dialogBoleta = new InfoDialog(this);

    connect(dialogBoleta, &InfoDialog::accepted, this, &MainWindow::onOKButtonDialogBoleta);

    dialogBoleta->show();
}
void MainWindow::onOKButtonDialogBoleta() {
    QString ativo = dialogBoleta->getAtivo();
    float preco = dialogBoleta->getPreco();
    int lote = dialogBoleta->getLote();
    QString lado = dialogBoleta->getLado();

    // Agora você pode usar as variáveis 'ativo', 'preco', 'lote' e 'lado' como desejar
    automationTrade->executaOperacao(ativo, preco, lote, lado);

}
void MainWindow::err_SignalOnAutomationTrade(int errr){

    qDebug() << "Erro:" << errr ;

}
//////////////////////////////////////////////////////////////////////////////
/// \brief MT5
///
void MainWindow::on_actionMT5_Connect_triggered()
{
    if (ui->actionMT5_Connect->isChecked()) {
        tcpMt5Server = new QTcpServer(this);

        connect(tcpMt5Server, &QTcpServer::newConnection, this, &MainWindow::mt5Connected);

        if (!tcpMt5Server->listen(QHostAddress::LocalHost, 9090))
        {
            ui->statusbar->showMessage("Erro ao escutar porta do MT5", 5000);
        }
        else
        {
            ui->statusbar->showMessage("Criando Server do MT5",  5000);
        }
    } else {
        tcpMt5Server->close();
        delete tcpMt5Server;
        ui->statusbar->showMessage("Fechando Server do MT5", 5000);
    }
}
void MainWindow::onSocketMt5Event()
{
    QTcpSocket *socket = static_cast<QTcpSocket*>(sender());

    QByteArray recData = socket->readAll();
    QString receivedData = QString::fromUtf8(recData);

    if (janelaInt != nullptr) janelaInt->textEditMSG->append(receivedData);

    tdp.processaDadosInt(&receivedData);

    if (janelaCOT != nullptr){
        QString ativoSelecionado = janelaCOT->comboBoxAtivo->currentText();
        Ativos ativo = tdp.ativoToNumInt(ativoSelecionado);
        if (ativo != nulo) janelaCOT->textEditMSG->append(tdp.getPrintCOT(ativo));
    }

}
void MainWindow::mt5Connected()
{
    QTcpSocket *mt5Socket = tcpMt5Server->nextPendingConnection();
    connect(mt5Socket, SIGNAL(readyRead()), this, SLOT(onSocketMt5Event()));
    connect(mt5Socket, SIGNAL(disconnected()), mt5Socket, SLOT(deleteLater()));

    ui->statusbar->showMessage("Client connected", 5000);
}
//////////////////////////////////////////////////////////////////////////////
/// \brief PYTHON
///
void MainWindow::on_actionPython_Service_triggered()
{
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE,
                                 NULL,
                                 PAGE_READWRITE,
                                 0,
                                 sizeof(MyPyStruct),
                                 MapObjName);
    if(hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE){
        qDebug() << "Cannot create file mapping";
        return;
    }
    else qDebug() << "Create File Mapping VALUE success!";

    pStruct = (MyPyStruct*)MapViewOfFile(hMapFile,
                                           FILE_MAP_ALL_ACCESS,
                                           0,
                                           0,
                                           sizeof(MyPyStruct));
    //Check if Create success
    if(pStruct==NULL){
        qDebug() << "Cannot Create Map View of File";
        CloseHandle(hMapFile);
        return;
    }

    connect(this,SIGNAL(plotDone()),this,SLOT(writePyInfo()));

    workHandle = CreateEvent(NULL, TRUE, FALSE, MapObj_WorkEvent_reader); // Manually. no signal
    if (workHandle == NULL){
        qDebug() << "CreateEvent failed %d\n";
        return;
    }

    if (!SetEvent(workHandle)) qDebug() << "backHandle failed!";


}
void MainWindow::writePyInfo(){

    pStruct->dol = tdp.dadosCOT[DOL].variacao_p;
    pStruct->win = tdp.dadosCOT[WIN].variacao_p;
    pStruct->sp = tdp.dadosCOT[SP500].variacao_p;
    pStruct->dx = tdp.dadosCOT[DXY].variacao_p;
    pStruct->mxn = tdp.dadosCOT[MEX].variacao_p;
    pStruct->idpd = diferenca;
    pStruct->idpw = diferencaW;

    if (!SetEvent(workHandle)) qDebug() << "backHandle failed!";
}
//////////////////////////////////////////////////////////////////////////////
/// \brief DOLAR Analiser
///
void MainWindow::on_actionDolar_Analiser_triggered()
{
    if (!ui->actionDolar_Analiser->isChecked())
    {
        timerAnaliser->stop();
        return;
    }

    //////////////////////////////////////////////////

    timerAnaliser = new QTimer(this);
    timerAnaliser->setInterval(1000); // intervalo de 1 segundo
    QObject::connect(timerAnaliser, &QTimer::timeout, this, &MainWindow::onTimerAnaliserEvent);

    timerAnaliserW = new QTimer(this);
    timerAnaliserW->setInterval(1000); // intervalo de 1 segundo
    QObject::connect(timerAnaliserW, &QTimer::timeout, this, &MainWindow::onTimerAnaliserWEvent);

    //////////////////////////////////////////////////

    holdTTChange = true;
    holdTTChangeW = true;

    lastbook = tdp.dadosBF[DOL];
    lastTTpos = tdp.dadosTT[DOL].size() - 1;
    if (lastTTpos < 0) lastTTpos = 0;

    atualTTpos = tdp.dadosTT[DOL].size() - 1;
    if (atualTTpos < 0) atualTTpos = 0;

    lastbookW = tdp.dadosBF[WDO];
    lastTTposW = tdp.dadosTT[WDO].size() - 1;
    lastTToposW = tdp.dadosTTo[WDO].size() - 1;
    lastTToFposW = tdp.dadosTToF[WDO].size() - 1;
    if (lastTTposW < 0) lastTTposW = 0;
    if (lastTToposW < 0) lastTToposW = 0;
    if (lastTToFposW < 0) lastTToFposW = 0;

    atualTTposW = tdp.dadosTT[WDO].size() - 1;
    atualTToposW = tdp.dadosTTo[WDO].size() - 1;
    atualTToFposW = tdp.dadosTToF[WDO].size() - 1;
    if (atualTTposW < 0) atualTTposW = 0;
    if (atualTToposW < 0) atualTToposW = 0;
    if (atualTToFposW < 0) atualTToFposW = 0;

    timerAnaliser->start(); // inicia o timer

    timerAnaliserW->start(); // inicia o timer
}
void MainWindow::onTimerAnaliserEvent()
{
    //////////////////////////////////////////////////
    /// Verifica se book e TT não estão vazios
    if (lastbook.empty()){
        lastbook = tdp.dadosBF[DOL];
        return ui->statusbar->showMessage("Book vazio", 1100);
    }
    if (tdp.dadosTT[DOL].empty()) return ui->statusbar->showMessage("TT vazio", 1100);

    //////////////////////////////////////////////////
    /// a cada segundo do timer eu paro neste if para verificar se entrou um dado novo no TT
    /// Esse intervalo de 1 segundo é + ou - como eu leio o fluxo (a cada evento)
    /// Caso tenha um dado novo, espero mais um pouco o book ou TT se estabilizar
    /// Se não teve dado novo eu pulo
    if (((tdp.dadosTT[DOL].size() - 1) > lastTTpos)){ //se houve mudança no TT

        if (holdTTChange == true){ //verifica se é a primeira vez que entrou apos 1 segundo
            atualTTpos = tdp.dadosTT[DOL].size() - 1; //ultima posição do TT
            timerAnaliser->setInterval(500); //seta um intervalo menor
            holdTTChange = false;
            return;
        } else {
            if (atualTTpos != (tdp.dadosTT[DOL].size() - 1)) //verifico se entrou mais dados no TT em 500ms
            {
                atualTTpos = tdp.dadosTT[DOL].size() - 1;
                return;
            }
        }

        atualbook = tdp.dadosBF[DOL]; //Captura o book atual

        //////////////////////////////////////////////////
        /// Inicializo variaveis locais
        ///


        int negEmSpread = 0; //negocios que sairam no preço de spread
        int negsVendedor = 0; //soma de todos negocios abaixo do BID
        int negsComprador = 0; //soma de todos negocios acima do ASK

        int lotesNaCompra = 0; //soma de todos lotes do book q foram consumidos
        int lotesNaVenda = 0;

        int deslocamentoNaCompra = 0; //valor correspondente ao deslocamento momentaneo
        int deslocamentoNaVenda = 0;

        int deslocamentoBookAsk = 0; //o quanto deslocou em ticks
        int deslocamentoBookBid = 0;

        //////////////////////////////////////////////////
        /// Na variação de dados novos do TT, conto lotes comprador e vendedor
        for (int i = lastTTpos + 1; i <= atualTTpos; i++)
        {
            if (tdp.dadosTT[DOL][i].preco <= lastbook[0].precoComprador){ //
                if (tdp.dadosTT[DOL][i].Agressor == "Comprador"){
                    negsVendedor += tdp.dadosTT[DOL][i].lote;
                }else if (tdp.dadosTT[DOL][i].Agressor == "Vendedor"){
                    negsVendedor += tdp.dadosTT[DOL][i].lote;
                }
            }else if (tdp.dadosTT[DOL][i].preco >= lastbook[0].precoVendedor){
                if (tdp.dadosTT[DOL][i].Agressor == "Vendedor"){
                    negsComprador += tdp.dadosTT[DOL][i].lote;
                }else if (tdp.dadosTT[DOL][i].Agressor == "Comprador"){
                    negsComprador += tdp.dadosTT[DOL][i].lote;
                }
            }else{
                negEmSpread++;
            }
        }

        //////////////////////////////////////////////////
        /// calculo o deslocamento BID e ASK
        deslocamentoBookBid = (int)(2 * (lastbook[0].precoComprador - atualbook[0].precoComprador));
        deslocamentoBookAsk = (int)(2 * (atualbook[0].precoVendedor - lastbook[0].precoVendedor));

        //////////////////////////////////////////////////
        /// Protege as variaveis caso o movimento seja maior q o permitido
        if (deslocamentoBookBid >= BOOKDEPH) deslocamentoBookBid = BOOKDEPH - 1;
        if (deslocamentoBookAsk >= BOOKDEPH) deslocamentoBookAsk = BOOKDEPH - 1;

        //////////////////////////////////////////////////
        /// logica principal, onde calculo o numero de lote que tinha e o que restou
        /// compara esse valor ainda com os negocios fechados
        if (deslocamentoBookBid >= 0)
        {
            for (int j = 0; j <= deslocamentoBookBid; j++)
            {
                lotesNaCompra += lastbook[j].loteComprador;
            }
            deslocamentoNaVenda = atualbook[0].loteComprador - lotesNaCompra + negsVendedor;
            totalDeslocamentoV += deslocamentoNaVenda;
        }

        if (deslocamentoBookAsk >= 0)
        {
            for (int j = 0; j <= deslocamentoBookAsk; j++)
            {
                lotesNaVenda += lastbook[j].loteVendedor;
            }
            deslocamentoNaCompra = atualbook[0].loteVendedor - lotesNaVenda + negsComprador;
            totalDeslocamentoC += deslocamentoNaCompra;
        }

        diferenca = totalDeslocamentoV - totalDeslocamentoC;

        /////////////////////////////////////////////////////////
        /// salvo as variaveis e atualizo para a proxima chamada do timer
        lastbook = atualbook;
        lastTTpos = atualTTpos;
        timerAnaliser->setInterval(1000); //reajusta o intervalo para 1s
        holdTTChange = true;
    }

}
void MainWindow::onTimerAnaliserWEvent()
{
    if (lastbookW.empty()){
        lastbookW = tdp.dadosBF[WDO];
        return ui->statusbar->showMessage("Book W vazio", 1100);
    }
    if (tdp.dadosTToF[WDO].empty()) return ui->statusbar->showMessage("TT W vazio", 1100);

    //////////////////////////////////////////////////

    // a cada segundo do timer eu paro neste if para verificar se entrou um dado novo no TT
    //
    if (((tdp.dadosTToF[WDO].size() - 1) > lastTToFposW)){

        if (holdTTChangeW == true){
            atualTToFposW = tdp.dadosTToF[WDO].size() - 1;
            timerAnaliserW->setInterval(500);
            holdTTChangeW = false;
            return;
        } else {
            if (atualTToFposW != (tdp.dadosTToF[WDO].size() - 1))
            {
                atualTToFposW = tdp.dadosTToF[WDO].size() - 1;
                return;
            }
        }

        atualbookW = tdp.dadosBF[WDO];
        atualTTposW = tdp.dadosTT[WDO].size() - 1;
        ///////////////////

        int negEmSpread = 0;
        int negsVendedor = 0;
        int negsComprador = 0;

        int lotesNaCompra = 0;
        int lotesNaVenda = 0;

        int deslocamentoNaCompra = 0;
        int deslocamentoNaVenda = 0;

        int deslocamentoBookAsk = 0;
        int deslocamentoBookBid = 0;

        ///////////////////

        for (int i = lastTTposW + 1; i <= atualTTposW; i++)
        {
            if (tdp.dadosTT[WDO][i].preco <= lastbookW[0].precoComprador){
                if (tdp.dadosTT[WDO][i].Agressor == "Comprador"){
                    negsVendedor += tdp.dadosTT[WDO][i].lote;
                }else if (tdp.dadosTT[WDO][i].Agressor == "Vendedor"){
                    negsVendedor += tdp.dadosTT[WDO][i].lote;
                }
            }else if (tdp.dadosTT[WDO][i].preco >= lastbookW[0].precoVendedor){
                if (tdp.dadosTT[WDO][i].Agressor == "Vendedor"){
                    negsComprador += tdp.dadosTT[WDO][i].lote;
                }else if (tdp.dadosTT[WDO][i].Agressor == "Comprador"){
                    negsComprador += tdp.dadosTT[WDO][i].lote;
                }
            }else{
                negEmSpread++;
            }
        }

        ///////////////////

        deslocamentoBookBid = (int)(2 * (lastbookW[0].precoComprador - atualbookW[0].precoComprador));
        deslocamentoBookAsk = (int)(2 * (atualbookW[0].precoVendedor - lastbookW[0].precoVendedor));

        ///////////////////

        if (deslocamentoBookBid >= BOOKDEPH) deslocamentoBookBid = BOOKDEPH - 1;
        if (deslocamentoBookAsk >= BOOKDEPH) deslocamentoBookAsk = BOOKDEPH - 1;

        if (deslocamentoBookBid >= 0)
        {
            for (int j = 0; j <= deslocamentoBookBid; j++)
            {
                lotesNaCompra += lastbookW[j].loteComprador;
            }
            deslocamentoNaVenda = atualbookW[0].loteComprador - lotesNaCompra + negsVendedor;
            totalDeslocamentoVW += deslocamentoNaVenda;
        }

        if (deslocamentoBookAsk >= 0)
        {
            for (int j = 0; j <= deslocamentoBookAsk; j++)
            {
                lotesNaVenda += lastbookW[j].loteVendedor;
            }
            deslocamentoNaCompra = atualbookW[0].loteVendedor - lotesNaVenda + negsComprador;
            totalDeslocamentoCW += deslocamentoNaCompra;
        }

        diferencaW = totalDeslocamentoVW - totalDeslocamentoCW;

        ///////////////////

        lastbookW = atualbookW;
        lastTTposW = atualTTposW;
        timerAnaliserW->setInterval(1000);
        holdTTChangeW = true;
    }

}
//////////////////////////////////////////////////////////////////////////////
/// \brief Estratégia
///
void MainWindow::on_actionEstrategy_triggered()
{
    if (ui->actionEstrategy->isChecked()) {

        estrategyThread = new Estrategy(this);

        connect(estrategyThread, &Estrategy::enviarOperacao, automationTrade, &AutomationTrade::executaOperacao);

        estrategyThread->start();

    }else{
        if(estrategyThread != nullptr){
            emit stopEstrategy(); // Emita o sinal para parar a thread
        }
    }
}
//////////////////////////////////////////////////////////////////////////////
/// \brief PLAY
///
void MainWindow::on_actionPlay_triggered()
{
    //////////////////////////////////////////////////
    QPen customPen1(QColor("blue")); // blue line
    customPen1.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    QPen customPen2(QColor("red")); // red line
    customPen2.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    //////////////////////////////////////////////////
    //adiciono grafico ao customPlot
    ui->customPlot1->addGraph(); //dolar
    ui->customPlot1->graph(0)->setPen(customPen1);
    ui->customPlot1->addGraph(ui->customPlot1->xAxis, ui->customPlot1->yAxis2); //dolar
    ui->customPlot1->graph(1)->setPen(customPen2);
    ui->customPlot2->addGraph(); //meu indicador dol
    ui->customPlot2->graph(0)->setPen(customPen1);
    ui->customPlot2->addGraph(ui->customPlot2->xAxis, ui->customPlot2->yAxis2); //meu indicador wdo
    ui->customPlot2->graph(1)->setPen(customPen2);
    ui->customPlot3->addGraph(); //DXY
    ui->customPlot3->graph(0)->setPen(customPen1);
    ui->customPlot3->addGraph(ui->customPlot3->xAxis, ui->customPlot3->yAxis2); //MEX
    ui->customPlot3->graph(1)->setPen(customPen2);
    ui->customPlot4->addGraph(); //SP500
    ui->customPlot4->graph(0)->setPen(customPen1);
    ui->customPlot4->addGraph(ui->customPlot4->xAxis, ui->customPlot4->yAxis2); //NASDAQ
    ui->customPlot4->graph(1)->setPen(customPen2);

    //////////////////////////////////////////////////
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    //////////////////////////////////////////////////
    ui->customPlot1->xAxis->setTicker(timeTicker);
    ui->customPlot1->yAxis->setRange(-0.1, 0.1);
    ui->customPlot1->yAxis2->setRange(-0.1, 0.1);
    ui->customPlot1->yAxis2->setVisible(true);
    ui->customPlot2->xAxis->setTicker(timeTicker);
    ui->customPlot2->yAxis->setRange(-0.1, 0.1);
    ui->customPlot2->yAxis2->setRange(-0.1, 0.1);
    ui->customPlot2->yAxis2->setVisible(true);
    ui->customPlot3->xAxis->setTicker(timeTicker);
    ui->customPlot3->yAxis->setRange(-0.1, 0.1);
    ui->customPlot3->yAxis2->setRange(-0.1, 0.1);
    ui->customPlot3->yAxis2->setVisible(true);
    ui->customPlot4->xAxis->setTicker(timeTicker);
    ui->customPlot4->yAxis->setRange(-0.1, 0.1);
    ui->customPlot4->yAxis2->setRange(-0.1, 0.1);
    ui->customPlot4->yAxis2->setVisible(true);
    //////////////////////////////////////////////////

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->customPlot1->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot1->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot1->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot1->yAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot2->yAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot3->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot3->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot3->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot3->yAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot4->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot4->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot4->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot4->yAxis2, SLOT(setRange(QCPRange)));

    // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
    connect(&dataTimerPlot, SIGNAL(timeout()), this, SLOT(realtimeDataPlotSlot()));
    dataTimerPlot.start(tempoDoTimer); // Interval 0 means to refresh as fast as possible
}
void MainWindow::realtimeDataPlotSlot()
{
    float yUplimit;
    float yDwlimit;
    float yUplimita;
    float yDwlimita;
    //////////////////////////////////////////////////
    // Declare um QVector para armazenar os últimos X pontos
    static QVector<float> lastPoints1;
    static QVector<float> lastPoints1a;
    static QVector<float> lastPoints2;
    static QVector<float> lastPoints2a;
    static QVector<float> lastPoints3;
    static QVector<float> lastPoints3a;
    static QVector<float> lastPoints4;
    static QVector<float> lastPoints4a;
    //////////////////////////////////////////////////
    /// \brief chamo as funções para calcular outras coisas dentro do timer
    ///

    saveCot();
    calcule_variacao();
    calcule_diferenca();

    ///

    //////////////////////////////////////////////////
    // add data to lines:
    double newValue1 = dataAtivo[WDO].perA.last();
    double newValue1a = dataAtivo[WDO].cotA.last();
    double newValue2 = diferenca;
    double newValue2a = diferencaW;
    double newValue3 = dataAtivo[DXY].perA.last();
    double newValue3a = dataAtivo[MEX].perA.last();
    double newValue4 = dataAtivo[SP500].perA.last();
    double newValue4a = dataAtivo[WIN].perA.last();

    //////////////////////////////////////////////////
    /// \brief timeStart
    ///
    QTime timeStart = QTime::currentTime();
    // calculate two new data points:
    double key = timeStart.msecsSinceStartOfDay()/1000.0; // time elapsed since start of demo, in seconds
    //////////////////////////////////////////////////
    /// inicio a plotagem
    ///
    ui->customPlot1->graph(0)->addData(key, newValue1);
    ui->customPlot1->graph(1)->addData(key, newValue1a);
    lastPoints1.append(newValue1);
    lastPoints1a.append(newValue1a);
    ui->customPlot2->graph(0)->addData(key, newValue2);
    ui->customPlot2->graph(1)->addData(key, newValue2a);
    lastPoints2.append(newValue2);
    lastPoints2a.append(newValue2a);
    ui->customPlot3->graph(0)->addData(key, newValue3);
    ui->customPlot3->graph(1)->addData(key, newValue3a);
    lastPoints3.append(newValue3);
    lastPoints3a.append(newValue3a);
    ui->customPlot4->graph(0)->addData(key, newValue4);
    ui->customPlot4->graph(1)->addData(key, newValue4a);
    lastPoints4.append(newValue4);
    lastPoints4a.append(newValue4a);

    //////////////////////////////////////////////////
    if (lastPoints1.size() > pointsOnGraphic) {
        lastPoints1.pop_front(); // Remova o ponto mais antigo
        lastPoints1a.pop_front(); // Remova o ponto mais antigo
        lastPoints2.pop_front(); // Remova o ponto mais antigo
        lastPoints2a.pop_front(); // Remova o ponto mais antigo
        lastPoints3.pop_front(); // Remova o ponto mais antigo
        lastPoints3a.pop_front(); // Remova o ponto mais antigo
        lastPoints4.pop_front(); // Remova o ponto mais antigo
        lastPoints4a.pop_front(); // Remova o ponto mais antigo
    }
    if (reg == -1){
        lastPoints1.clear();
        lastPoints1a.clear();
        lastPoints2.clear();
        lastPoints2a.clear();
        lastPoints3.clear();
        lastPoints3a.clear();
        lastPoints4.clear();
        lastPoints4a.clear();

        ui->customPlot1->graph(0)->data()->clear();
        ui->customPlot1->graph(1)->data()->clear();
        ui->customPlot2->graph(0)->data()->clear();
        ui->customPlot2->graph(1)->data()->clear();
        ui->customPlot3->graph(0)->data()->clear();
        ui->customPlot3->graph(1)->data()->clear();
        ui->customPlot4->graph(0)->data()->clear();
        ui->customPlot4->graph(1)->data()->clear();

        reg = 0;
    }
    //////////////////////////////////////////////////
    yUplimit = *std::max_element(lastPoints1.begin(), lastPoints1.end());
    yDwlimit = *std::min_element(lastPoints1.begin(), lastPoints1.end());
    yUplimita = *std::max_element(lastPoints1a.begin(), lastPoints1a.end());
    yDwlimita = *std::min_element(lastPoints1a.begin(), lastPoints1a.end());
    ui->customPlot1->yAxis->setRange(yDwlimit, yUplimit);
    ui->customPlot1->yAxis2->setRange(yDwlimita, yUplimita);
    ui->customPlot1->xAxis->setRange(key, pointsOnGraphic, Qt::AlignRight);
    /////////////
    yUplimit = *std::max_element(lastPoints2.begin(), lastPoints2.end());
    yDwlimit = *std::min_element(lastPoints2.begin(), lastPoints2.end());
    yUplimita = *std::max_element(lastPoints2a.begin(), lastPoints2a.end());
    yDwlimita = *std::min_element(lastPoints2a.begin(), lastPoints2a.end());
    ui->customPlot2->yAxis->setRange(yDwlimit, yUplimit);
    ui->customPlot2->yAxis2->setRange(yDwlimita, yUplimita);
    ui->customPlot2->xAxis->setRange(key, pointsOnGraphic, Qt::AlignRight);
    /////////////
    yUplimit = *std::max_element(lastPoints3.begin(), lastPoints3.end());
    yDwlimit = *std::min_element(lastPoints3.begin(), lastPoints3.end());
    yUplimita = *std::max_element(lastPoints3a.begin(), lastPoints3a.end());
    yDwlimita = *std::min_element(lastPoints3a.begin(), lastPoints3a.end());
    ui->customPlot3->yAxis->setRange(yDwlimit, yUplimit);
    ui->customPlot3->yAxis2->setRange(yDwlimita, yUplimita);
    ui->customPlot3->xAxis->setRange(key, pointsOnGraphic, Qt::AlignRight);
    /////////////
    yUplimit = *std::max_element(lastPoints4.begin(), lastPoints4.end());
    yDwlimit = *std::min_element(lastPoints4.begin(), lastPoints4.end());
    yUplimita = *std::max_element(lastPoints4a.begin(), lastPoints4a.end());
    yDwlimita = *std::min_element(lastPoints4a.begin(), lastPoints4a.end());
    ui->customPlot4->yAxis->setRange(yDwlimit, yUplimit);
    ui->customPlot4->yAxis2->setRange(yDwlimita, yUplimita);
    ui->customPlot4->xAxis->setRange(key, pointsOnGraphic, Qt::AlignRight);
    //////////////////////////////////////////////////
    ui->customPlot1->replot();
    ui->customPlot2->replot();
    ui->customPlot3->replot();
    ui->customPlot4->replot();

    return emit plotDone();
}
void MainWindow::on_actionClean_Graphics_triggered()
{
    reg = -1;
}
void MainWindow::on_actionSave_dataGraphics_triggered(){

    QString basePath = "C:\\Users\\Aldair\\OneDrive\\Mercado Financeiro\\SoftwareProjetos\\TraderHelperSoftware";
    QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString filename = basePath + "\\pasdata_" + currentDateTime + ".csv";

    // Abra o arquivo CSV para gravação
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // Itere sobre seus dados e escreva cada linha no arquivo CSV
        // construa cabeçalho
        int n = dataAtivo.size(); //dataAtivo
        for (int i = 0; i < n; i++) {
            out << dataAtivo[i].ativo;
            out << "," << ",";
        }
        n = dataDif.size(); //dataDif
        for (int i = 0; i < n; i++) {
            out << dataDif[i].ativos;
            out << "," << "," << "," << "," << "," << ",";
        }
        out << "dataDolarFluxo";
        out << "," << "," << "," << "," << "," << ",";
        out << "\n";
        // primeira linha do cabeçalho
        n = dataAtivo.size(); //dataAtivo
        for (int i = 0; i < n; i++) {
            out << "cotA" << "," << "perA" << ",";
        }
        n = dataDif.size(); //dataAtivo
        for (int i = 0; i < n; i++) {
            out << "dif1m" << "," << "dif2m" << "," << "dif5m" << "," << "dif15m" << "," << "dif30m" << "," << "difD" << ",";
        }
        out << "deslDOL" << "," << "deslWDO" << "," << "deslDOL_varR1m" << "," << "deslWDO_varR1m" << "," << "deslDOL_varR5m" << "," << "deslWDO_varR5m" << ",";
        out << "\n";
        // fim do cabeçalho

        n = dataAtivo[0].cotA.size(); // Número de elementos em cotA (ou perA)

        for (int i = 0; i < n; i++) {
            for (const dadosDeImporPorAtivo& ativo : dataAtivo) {
                out << ativo.cotA[i] << "," << ativo.perA[i] << ",";
            }

            for (const dadosDeDiferenca& ativos : dataDif) {
                out << ativos.dif1m[i] << "," << ativos.dif2m[i] << ",";
                out << ativos.dif5m[i] << "," << ativos.dif15m[i] << ",";
                out << ativos.dif30m[i] << "," << ativos.difD[i] << ",";
            }

            out << dataDolarFluxo.deslDOL[i] << "," << dataDolarFluxo.deslWDO[i] << ",";
            out << dataDolarFluxo.deslDOL_varR1m[i] << "," << dataDolarFluxo.deslWDO_varR1m[i] << ",";
            out << dataDolarFluxo.deslDOL_varR5m[i] << "," << dataDolarFluxo.deslWDO_varR5m[i];

            out << "\n";
        }

        // Feche o arquivo CSV
        file.close();

        if (file.error() == QFile::NoError) {
            qDebug() << "Dados salvos com sucesso em " << filename;
        } else {
            qDebug() << "Erro ao salvar os dados no arquivo CSV: " << file.errorString();
        }
    } else {
        qDebug() << "Erro ao abrir o arquivo CSV para gravação: " << file.errorString();
    }


}
void MainWindow::saveCot(){

    dataDolarFluxo.deslDOL.append(diferenca);
    dataDolarFluxo.deslWDO.append(diferencaW);

    for (int i = 0; i < tdp.tamAtivo; i++){
        if (dataAtivo[i].cotA.isEmpty()){
            Ativos ativo = static_cast<Ativos>(i);
            dataAtivo[i].ativo = tdp.nomeDoAtivo(ativo);
        }
        dataAtivo[i].cotA.append(tdp.dadosCOT[i].ultima);
        dataAtivo[i].perA.append(tdp.dadosCOT[i].variacao_p);
    }

}
void MainWindow::calcule_variacao(){

    static float DOLmedia1m = 0;
    static float WDOmedia1m = 0;
    static float DOLmedia5m = 0;
    static float WDOmedia5m = 0;

    dataDolarFluxo.deslDOL_varR1m.append(dataDolarFluxo.deslDOL.last() - DOLmedia1m);
    DOLmedia1m = ((amosPorMin*1 - 1) * DOLmedia1m + dataDolarFluxo.deslDOL.last())/(amosPorMin*1);
    dataDolarFluxo.deslWDO_varR1m.append(dataDolarFluxo.deslWDO.last() - WDOmedia1m);
    WDOmedia1m = ((amosPorMin*1 - 1) * WDOmedia1m + dataDolarFluxo.deslWDO.last())/(amosPorMin*1);
    dataDolarFluxo.deslDOL_varR5m.append(dataDolarFluxo.deslDOL.last() - DOLmedia5m);
    DOLmedia5m = ((amosPorMin*1 - 1) * DOLmedia5m + dataDolarFluxo.deslDOL.last())/(amosPorMin*1);
    dataDolarFluxo.deslWDO_varR5m.append(dataDolarFluxo.deslWDO.last() - WDOmedia5m);
    WDOmedia5m = ((amosPorMin*1 - 1) * WDOmedia5m + dataDolarFluxo.deslDOL.last())/(amosPorMin*1);
}
void MainWindow::calcule_diferenca(){

    static float media1m[tdp.tamAtivo];
    static float media2m[tdp.tamAtivo];
    static float media5m[tdp.tamAtivo];
    static float media15m[tdp.tamAtivo];
    static float media30m[tdp.tamAtivo];

    int atRef = DOL;

    for(int j = 0; j < tdp.tamAtivo; j++){
        if (dataDif[j].dif1m.isEmpty()){
            Ativos ativo1 = static_cast<Ativos>(DOL);
            Ativos ativo2 = static_cast<Ativos>(j);
            dataDif[j].ativos = tdp.nomeDoAtivo(ativo1) + "_" + tdp.nomeDoAtivo(ativo2);
        }

        float lastDif = dataAtivo[atRef].perA.last() - dataAtivo[j].perA.last();
        dataDif[j].difD.append(lastDif);

        if(dataDif[j].dif1m.isEmpty()){
            dataDif[j].dif1m.append(lastDif);
            media1m[j] = lastDif;
            dataDif[j].dif2m.append(lastDif);
            media2m[j] = lastDif;
            dataDif[j].dif5m.append(lastDif);
            media5m[j] = lastDif;
            dataDif[j].dif15m.append(lastDif);
            media15m[j] = lastDif;
            dataDif[j].dif30m.append(lastDif);
            media30m[j] = lastDif;
        }else{
            dataDif[j].dif1m.append(lastDif - media1m[j]);
            media1m[j] = ((amosPorMin*1 - 1) * media1m[j] + lastDif)/(amosPorMin*1);
            dataDif[j].dif2m.append(lastDif - media2m[j]);
            media2m[j] = ((amosPorMin*2 - 1) * media2m[j] + lastDif)/(amosPorMin*2);
            dataDif[j].dif5m.append(lastDif - media5m[j]);
            media5m[j] = ((amosPorMin*1 - 1) * media5m[j] + lastDif)/(amosPorMin*5);
            dataDif[j].dif15m.append(lastDif - media15m[j]);
            media15m[j] = ((amosPorMin*15 - 1) * media15m[j] + lastDif)/(amosPorMin*15);
            dataDif[j].dif30m.append(lastDif - media30m[j]);
            media30m[j] = ((amosPorMin*30 - 1) * media30m[j] + lastDif)/(amosPorMin*30);
        }
    }
}







