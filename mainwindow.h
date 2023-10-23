#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "trydDataProcess.h"
#include "automationtrade.h"
#include "estrategy.h"

#include <QtWidgets>
#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QTextEdit>
#include <QSettings>
#include <QDebug>
#include <QAxObject>

#include <QtCore/QMetaObject>
#include <QtCore/QDateTime>

#include <QtCharts/QCandlestickSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QChartView>
#include <QtCharts/QCandlestickSeries>
#include <QtCharts/QValueAxis>

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <ole2.h>
#include <cstdlib>
#include <cstdlib>
#include <cstring>
#include <qmath.h>
#include <cstdio>

//////////////////////////////////////////////////////////////////////////////

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//////////////////////////////////////////////////////////////////////////////
/// \brief The MyJanelaTTDialog class
///
class MyJanelaTTDialog : public QDialog
{
    Q_OBJECT

public:
    MyJanelaTTDialog(QWidget *parent = nullptr) : QDialog(parent) {

        QVBoxLayout *layout = new QVBoxLayout;

        textEditTT = new QTextEdit(this);
        textEditBF = new QTextEdit(this);
        textEditBF->setFixedSize(400, 160);

        layout->addWidget(textEditTT);
        layout->addWidget(textEditBF);

        setLayout(layout);
        setWindowTitle("Times & Trades");
        resize(400, 600);

    }

    QTextEdit *textEditTT;
    QTextEdit *textEditBF;

};
//////////////////////////////////////////////////////////////////////////////
/// \brief The MyJanelaMSGDialog class
///
class MyJanelaMSGDialog : public QDialog
{
    Q_OBJECT

public:
    MyJanelaMSGDialog(QWidget *parent = nullptr) : QDialog(parent) {

        QVBoxLayout *layout = new QVBoxLayout;

        textEditMSG = new QTextEdit(this);

        layout->addWidget(textEditMSG);

        setLayout(layout);
        setWindowTitle("Mensagem Recebida");
        resize(900, 900);

    }

    QTextEdit *textEditMSG;

};
//////////////////////////////////////////////////////////////////////////////
/// \brief The MyJanelaCOTDialog class
///
class MyJanelaCOTDialog : public QDialog
{
    Q_OBJECT

public:
    MyJanelaCOTDialog(QWidget *parent = nullptr) : QDialog(parent) {
        QVBoxLayout *layout = new QVBoxLayout;

        // Adicione uma caixa de seleção
        comboBoxAtivo = new QComboBox(this);
        comboBoxAtivo->addItem("DOL");  // Opção 1: DOL
        comboBoxAtivo->addItem("IND");  // Opção 2: IND
        comboBoxAtivo->addItem("WDO");  // Opção 1: DOL
        comboBoxAtivo->addItem("WIN");  // Opção 2: IND
        comboBoxAtivo->addItem("VALE3|");
        comboBoxAtivo->addItem("PETR4|");
        comboBoxAtivo->addItem("ITUB4|");
        comboBoxAtivo->addItem("BBDC4|");
        comboBoxAtivo->addItem("ABEV3|");
        comboBoxAtivo->addItem("B3SA3|");
        comboBoxAtivo->addItem("DI1F24");
        comboBoxAtivo->addItem("DI1F25");
        comboBoxAtivo->addItem("DI1F26");
        comboBoxAtivo->addItem("DI1F27");
        comboBoxAtivo->addItem("DI1F28");
        comboBoxAtivo->addItem("DI1F29");
        comboBoxAtivo->addItem("DI1F30");
        comboBoxAtivo->addItem("DI1F31");
        comboBoxAtivo->addItem("DI1F33");
        comboBoxAtivo->addItem("Usa500");
        comboBoxAtivo->addItem("DXYInd");
        comboBoxAtivo->addItem("LCrude");
        comboBoxAtivo->addItem("GOLDii");
        comboBoxAtivo->addItem("USDMXN");
        comboBoxAtivo->addItem("UsaTBD");
        comboBoxAtivo->addItem("VixVix");
        comboBoxAtivo->addItem("UsaTec");
        comboBoxAtivo->addItem("Euro50");
        comboBoxAtivo->addItem("Soybin");
        comboBoxAtivo->addItem("USDEUR");
        comboBoxAtivo->addItem("USDZAR");
        comboBoxAtivo->addItem("USDCAD");
        comboBoxAtivo->addItem("USDSEK");
        comboBoxAtivo->addItem("USDAUD");



        textEditMSG = new QTextEdit(this);

        layout->addWidget(comboBoxAtivo);
        layout->addWidget(textEditMSG);

        setLayout(layout);
        setWindowTitle("Cotação Recebida");
        resize(900, 900);
    }

    QTextEdit *textEditMSG;
    QComboBox *comboBoxAtivo;
};
//////////////////////////////////////////////////////////////////////////////
/// \brief The InfoDialog class
///
class InfoDialog : public QDialog
{
    Q_OBJECT

public:
    InfoDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setWindowTitle("Informe os Dados");

        // Crie widgets para as informações que você deseja capturar
        QLabel *ativoLabel = new QLabel("Ativo:");
        QLineEdit *ativoLineEdit = new QLineEdit;

        QLabel *precoLabel = new QLabel("Preço:");
            QDoubleSpinBox *precoSpinBox = new QDoubleSpinBox;
            precoSpinBox->setDecimals(2);   // Defina o número de casas decimais desejado
            precoSpinBox->setMinimum(0.00); // Defina o valor mínimo (por exemplo, 0.01)
            precoSpinBox->setMaximum(999999.99); // Defina o valor máximo (por exemplo, 9999.99)

        QLabel *loteLabel = new QLabel("Lote:");
        QSpinBox *loteSpinBox = new QSpinBox;

        QLabel *ladoLabel = new QLabel("Lado:");
        QComboBox *ladoComboBox = new QComboBox;
        ladoComboBox->addItem("Compra");
        ladoComboBox->addItem("Venda");

        // Layout para organizar os widgets
        QFormLayout *formLayout = new QFormLayout;
        formLayout->addRow(ativoLabel, ativoLineEdit);
        formLayout->addRow(precoLabel, precoSpinBox);
        formLayout->addRow(loteLabel, loteSpinBox);
        formLayout->addRow(ladoLabel, ladoComboBox);

        // Botões OK e Cancelar
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

        // Conectar os botões OK e Cancelar para aceitar ou rejeitar o diálogo
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

        // Adicionar o layout e os botões ao diálogo
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(buttonBox);
        setLayout(mainLayout);

        // Variáveis para armazenar os valores
        m_ativoLineEdit = ativoLineEdit;
        m_precoSpinBox = precoSpinBox;
        m_loteSpinBox = loteSpinBox;
        m_ladoComboBox = ladoComboBox;
    }

    QString getAtivo() const { return m_ativoLineEdit->text(); }
    float getPreco() const { return m_precoSpinBox->value(); }
    int getLote() const { return m_loteSpinBox->value(); }
    QString getLado() const { return m_ladoComboBox->currentText(); }

private:
    QLineEdit *m_ativoLineEdit;
    QDoubleSpinBox *m_precoSpinBox;
    QSpinBox *m_loteSpinBox;
    QComboBox *m_ladoComboBox;
};
//////////////////////////////////////////////////////////////////////////////
/// \brief The SettingsDialog class
///
class SettingsDialog : public QDialog
{
    Q_OBJECT

signals:
    void settingsAccepted(const QString &dolar, const QString &indice);

public:
    SettingsDialog(QWidget *parent = nullptr) : QDialog(parent)
    {
        setObjectName("JanelaConfig");
        setWindowTitle("Config");
        resize(366, 183);

        // Crie o layout e adicione os widgets
        gridLayout = new QGridLayout(this);  // Defina a janela como pai do layout

        label = new QLabel("Contrato do Dolar:");
        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_2 = new QLabel("Contrato do Indice:");
        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEditDolar = new QLineEdit();
        gridLayout->addWidget(lineEditDolar, 0, 1, 1, 1);

        lineEditIndice = new QLineEdit();
        gridLayout->addWidget(lineEditIndice, 1, 1, 1, 1);

        acceptButton = new QPushButton("OK");
        gridLayout->addWidget(acceptButton, 2, 0, 1, 1);

        cancelButton = new QPushButton("Cancelar");
        gridLayout->addWidget(cancelButton, 2, 1, 1, 1);

        // Defina o layout como o layout principal da janela
        setLayout(gridLayout);

        connect(acceptButton, &QPushButton::clicked, this, &SettingsDialog::onAcceptButtonClicked);
        connect(cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancelButton);
    }

private slots:
    void onAcceptButtonClicked(){
        QString dolar = lineEditDolar->text();
        QString indice = lineEditIndice->text();
        emit settingsAccepted(dolar, indice);
        close();
    }
    void onCancelButton(){
        close();
    }
private:
    QGridLayout *gridLayout;
    QLabel *label;
    QLabel *label_2;
    QPushButton *acceptButton;
    QPushButton *cancelButton;
public:
    QLineEdit *lineEditDolar;
    QLineEdit *lineEditIndice;
};
//////////////////////////////////////////////////////////////////////////////
/// \brief The MainWindow class
///
class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    //////////////////////////////////////////////////
    /// \brief SINAIS
    ///
    void stopAutomationTrade();
    void stopEstrategy();
    void plotDone();
    void calVarDone();

public:
    //////////////////////////////////////////////////
    /// \brief MainWindow
    ///
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    Ui::MainWindow *ui; //interface
    trydDataProcess tdp; //tdp é o objeto que carrega todas as funções e variaveis relacionadas ao tryd

    void loadSettings();
    void saveSettings();

public slots:
    //////////////////////////////////////////////////
    /// \brief TRYD
    ///
    void err_SignalOnAutomationTrade(int errr);

private slots:
    //////////////////////////////////////////////////
    /// \brief Janelas
    ///
    void on_actionShow_TT_triggered();
    void on_actionShow_TToriginal_triggered();
    void on_actionShow_Msg_triggered();
    void on_actionShow_Msg_Int_triggered();
    void on_actionShow_Cota_o_triggered();
    void on_actionSettings_triggered();
    void onSettingsAccepted(const QString &dolar, const QString &indice);
    //////////////////////////////////////////////////
    /// \brief TRYD
    ///
    void on_actionTryd_Connect_triggered();
    void trydConnected();
    void trydDisconnected();
    void trydSocketError();
    void onSocketTrydEvent();
    //////////////////////////////////////////////////
    void on_actionAutomatizador_triggered();
    void on_actionAbrir_Boleta_triggered();
    void onOKButtonDialogBoleta();
    //////////////////////////////////////////////////
    /// \brief MT5
    ///
    void on_actionMT5_Connect_triggered();
    void mt5Connected();
    void onSocketMt5Event();
    //////////////////////////////////////////////////
    /// \brief PYTHON
    ///
    void on_actionPython_Service_triggered();
    void writePyInfo();
    //////////////////////////////////////////////////
    /// \brief DOLAR Analiser
    ///
    void on_actionDolar_Analiser_triggered();
    void onTimerAnaliserEvent();
    void onTimerAnaliserWEvent();
    //////////////////////////////////////////////////
    /// \brief Estratégia
    ///
    void on_actionEstrategy_triggered();
    //////////////////////////////////////////////////
    /// \brief PLAY
    ///
    void on_actionPlay_triggered();
    void realtimeDataPlotSlot();
    void on_actionClean_Graphics_triggered();
    void on_actionSave_dataGraphics_triggered();
    void saveCot();
    void calcule_variacao();
    void calcule_diferenca();

private:
    void closeEvent(QCloseEvent *event)
    {
        // Exibe mensagem de confirmação
        QMessageBox::StandardButton resposta = QMessageBox::question(this, "Fechar", "Deseja realmente fechar a janela?", QMessageBox::Yes | QMessageBox::No);

        if (resposta == QMessageBox::Yes) {
            // Aceita o fechamento da janela
            if (janelaTT != nullptr) janelaTT->close();
            if (janelaMSG != nullptr) janelaMSG->close();

            event->accept();
        } else {
            // Cancela o fechamento da janela
            event->ignore();
        }
    }

private:
    //////////////////////////////////////////////////
    /// \brief Sockets
    ///
    QTcpSocket tcpTrydClient;
    QTcpServer *tcpMt5Server;
    //////////////////////////////////////////////////
    /// \brief janela
    ///
    SettingsDialog *settingsDialog = nullptr;
    MyJanelaTTDialog *janelaTT = nullptr;
    MyJanelaTTDialog *janelaTTo = nullptr;
    MyJanelaMSGDialog *janelaMSG = nullptr;
    MyJanelaMSGDialog *janelaInt = nullptr;
    MyJanelaCOTDialog *janelaCOT = nullptr;
    //////////////////////////////////////////////////
    /// \brief automationTrade class pointer
    ///
    AutomationTrade* automationTrade = nullptr; //classe da thread que cordena as execuções
    InfoDialog* dialogBoleta = nullptr; //caixa de dialog para entrada de dados da boleta
    //////////////////////////////////////////////////
    /// \brief estrategyThread
    ///
    Estrategy* estrategyThread = nullptr;
    //////////////////////////////////////////////////
    /// \brief dataTimerPlot
    ///
    QTimer dataTimerPlot;
    int tempoDoTimer;
    float amosPorMin;
    float pointsOnGraphic;
    int reg = 0;
    int lenMedVar;
    float mediaCurta;
    //////////////////////////////////////////////////
    /// \brief Variaveis globais da classe
    /// variaveis de importancia
    struct dadosDeImporPorAtivo{
        QString ativo;
        QVector<float> cotA;
        QVector<float> perA;
    };
    QVector<dadosDeImporPorAtivo> dataAtivo;

    struct dadosDeDiferenca{
        QString ativos;
        QVector<float> dif1m;
        QVector<float> dif2m;
        QVector<float> dif5m;
        QVector<float> dif15m;
        QVector<float> dif30m;
        QVector<float> difD;
    };
    QVector<dadosDeDiferenca> dataDif;

    struct dadosDeFluxo{
        QVector<float> deslDOL;
        QVector<float> deslWDO;
        QVector<float> deslDOL_varR1m;
        QVector<float> deslWDO_varR1m;
        QVector<float> deslDOL_varR5m;
        QVector<float> deslWDO_varR5m;
    };
    dadosDeFluxo dataDolarFluxo;

    struct dadosDePreco{
        QVector<float> precosImp;
    };
    dadosDePreco dataDolarPreco;

    //////////////////////////////////////////////////
    /// Analiser
    ///
    QTimer *timerAnaliser = nullptr;
    QTimer *timerAnaliserW = nullptr;
    bool holdTTChange = true;
    bool holdTTChangeW = true;
    QVector<trydDataProcess::dataBOOKF> lastbook;
    QVector<trydDataProcess::dataBOOKF> atualbook;
    QVector<trydDataProcess::dataBOOKF> lastbookW;
    QVector<trydDataProcess::dataBOOKF> atualbookW;
    int lastTTpos = 0;
    int atualTTpos = 0;
    int lastTTposW = 0;
    int lastTToposW = 0;
    int lastTToFposW = 0;
    int atualTTposW = 0;
    int atualTToposW = 0;
    int atualTToFposW = 0;
    int totalDeslocamentoV = 0;
    int totalDeslocamentoC = 0;
    int diferenca = 0; //é o valor principal, que indica o lado que desloca mais facil
    int totalDeslocamentoVW = 0;
    int totalDeslocamentoCW = 0;
    int diferencaW = 0; //é o valor principal, que indica o lado que desloca mais facil
    //////////////////////////////////////////////////
    /// \brief The MyPyStruct class
    ///
    struct MyPyStruct {
        float dol;
        float win;
        float sp;
        float dx;
        float mxn;
        float idpd;
        float idpw;

        float dol_v;
        float win_v;
        float sp_v;
        float dx_v;
        float mxn_v;
        float idpd_v;
        float idpw_v;

        float dol_vd;
        float win_vd;
        float sp_vd;
        float dx_vd;
        float mxn_vd;
        float idpd_vd;
        float idpw_vd;
    };
    MyPyStruct* pStruct;

};

#endif // MAINWINDOW_H
