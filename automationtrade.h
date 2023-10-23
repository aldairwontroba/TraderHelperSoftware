#ifndef AUTOMATIONTRADE_H
#define AUTOMATIONTRADE_H

#include <QThread>
#include <QAxObject>
#include <QDebug>
#include <QTcpSocket>
/////////////////////////////////////////////////////////////
/// \brief The AutomationTrade class
/// Classe que trata toda parte relacionada a operações, execuções,
/// gerenciamento de posição, gerenciamento de risco, monitoramento, etc.
class AutomationTrade : public QThread
{
    Q_OBJECT

signals:
    void stopThread(); // Sinal para parar a thread
    void err_signal(int err);

public:
    /////////////////////////////////////////////////////////////
    /// \brief AutomationTrade
    /// \param parent
    /// Construtor da classe
    explicit AutomationTrade(QObject *parent = nullptr);
    ~AutomationTrade();
    /////////////////////////////////////////////////////////////
    /// \brief solicitarInterrupcao
    /// Solicita o encerramento da thread de forma padrão
    /// encerrando todas as funções adequadamente
    void solicitarInterrupcao()
    {
        requestInterruption();
    }
    /////////////////////////////////////////////////////////////
    /// \brief get_err_signal
    /// \return
    /// obtem variavel de erro da thread (utilizado para controle)
    int get_err_signal(){
        return emit err_singal;
    }
    /////////////////////////////////////////////////////////////
    /// \brief addAtivoPermitido
    /// \param ativof
    /// \param loteMaxf
    /// \return
    /// Adiciona um ativo a lista de ativos habilitados para realizar operações
    int adicionaAtivoPermitido(QString ativof, int loteMaxf, float perdaMaxf);
    /////////////////////////////////////////////////////////////
    /// \brief removeAtivoPermitido
    /// \param ativof
    /// \return
    /// remove um ativo da lista de habilitados
    /// Escreva "ALL" para limpara a lista
    int removeAtivoPermitido(QString ativof);

private:
    /////////////////////////////////////////////////////////////
    /// \brief run
    /// função principal ou loop principal
    void run() override;
    /////////////////////////////////////////////////////////////
    /// \brief executaStop
    /// \param ativof
    /// \param precof
    /// \param lotef
    /// \param ladof
    ///
    void executaStop(QString ativof, float precof, int lotef, QString ladof);

public slots:
    /////////////////////////////////////////////////////////////
    /// \brief executaOperacao a operação solicitada pelos parametros
    /// \param ativof ex:"WDOX23"
    /// \param precof
    /// \param lotef numero de lotes a ser executado. deve ser sempre positivo
    /// \param ladof "Compra" ou "Venda"
    /// \return não retorna nada, porem emit um sinal (err_signal) caso
    /// haja algum erro para executar a ordem. confira a lista de erros.
    void executaOperacao(QString ativof, float precof, int lotef, QString ladof);

private slots:
    void on_trydConnected();
    void on_SocketTrydEvent();
    void connectionError(QAbstractSocket::SocketError socketError);
    void on_trydDisconnected();

private:
    ////////////////////////////////////////////////////////
    /// \brief tcpTrydClient
    ///
    QTcpSocket tcpTrydClient;
    ////////////////////////////////////////////////////////
    /// \brief excel
    ///
    QAxObject* excel;
    QAxObject* workbooks;
    QAxObject* workbook;
    QAxObject* sheets;
    QAxObject* sheet;
    QAxObject* cell;
    QString workbookName = "C:/Users/Aldair/OneDrive/Mercado Financeiro/SoftwareProjetos/TraderHelperSoftware/autotrade.xlsx"; // Substitua pelo caminho real do seu arquivo
    ////////////////////////////////////////////////////////
    /// \brief err_singal
    ///
    int err_singal = -1;
    ////////////////////////////////////////////////////////
    /// \brief operationEntry
    ///
    bool operationEntry = false;
    bool stop_execute = false;
    QString ativoToStop;
    struct operacaoSettings{
        QString ativo;
        float preco;
        int lote;
        QString lado;
    };
    operacaoSettings dadosDeOperacao;
    ////////////////////////////////////////////////////////
    /// \brief The limites class
    /// Essa estrutura fornece os limites de operação por ativo
    /// sendo cada ativo um item no QVector
    struct limites{
        QString ativoPermitido;
        int loteMax;
        float perdaMax;
    };
    QVector<limites> limitePorAtivo;
    ////////////////////////////////////////////////////////
    /// \brief The informacaoDeConta class
    /// nesta estrutura ficam registrados todas as informações sobre a conta
    /// posição em aberto, registo de operações, exposição, preco medio
    /// Cada posição do QVector indica um ativo diferente para uma conta X
    struct informacaoDeConta{
        QString ativo;
        QString conta;
        float resAberto;
        float resFechado;
        int posLote;
        float posPreco;
    };
    QVector<informacaoDeConta> contaInfo;


};

#endif // AUTOMATIONTRADE_H
