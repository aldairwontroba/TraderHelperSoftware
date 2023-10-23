#ifndef TRYDDATAPROCESS_H
#define TRYDDATAPROCESS_H

#include <QString>
#include <QVector>

enum Ativos {
    DOL = 0,
    WDO,
    IND,
    WIN,
    VALE3,
    PETR4,
    ITUB4,
    BBDC4,
    ABEV3,
    B3SA3,
    DI1F24,
    DI1F25,
    DI1F26,
    DI1F27,
    DI1F28,
    DI1F29,
    DI1F30,
    DI1F31,
    DI1F33,
    DXY,
    SP500,
    MEX,
    GOLD,
    OIL,
    NASD,
    BOND,
    EUR50,
    Soyb,
    VIX,
    EUR,
    ZAR,
    CAD,
    SEK,
    AUD,
    nulo
};


class trydDataProcess
{
public:
    /*
    Função principal para processar todos dados recebidos no secket.
    A função salva os dados em uma struct dentro da classe e disponibiliza
    publicamente os valores atraves de dadosTT. Os valores são empilhados
    em um vetor e podem ser manipulados como vetores.
    */
    QVector<bool> processaDados(QString* msg);
    int processaDadosInt(QString* msg); //dados internacionais

    /*
    wxString ativo é o ativo a ser solicitado, negs solicita dados do TT,
    boff solicita dados do book fechado, pBoof é a profundidade do book fechado
    */
    QString solicitaDados(QString ativo, QString tipo, int pBoff = 3);

    /*
    Solicita dados do TT e retorna um wxString com data, lote, preco e agressor
    sizeChage é quantos negocios serão solicitados. Os negocios solicitados contam
    serão os ultimos sizeChange.
    */
    QString getPrintTT(int sizeChange, int ativo);
    QString getPrintTTo(int sizeChange, int ativo);
    QString getPrintTToF(int sizeChange, int ativo);

    /*
    Solicita dados do Book fechado e retorna um wxString com lote e preco
    deeph é a profundidade solicitada.
    */
    QString getPrintBF(int deeph, int ativo);

    /*
    Solicita um print da cotação de determinado ativo
    */
    QString getPrintCOT(int ativo);




    Ativos ativoToNum(const QString& str) {
        QString aux = str.mid(0,3);
        if (aux == "WDO" || aux == "wdo") return WDO;
        if (aux == "WIN" || aux == "win") return WIN;
        if (aux == "DOL" || aux == "dol") return DOL;
        if (aux == "IND" || aux == "ind") return IND;

        if (str == "VALE3|") return VALE3;
        if (str == "PETR4|") return PETR4;
        if (str == "ITUB4|") return ITUB4;
        if (str == "BBDC4|") return BBDC4;
        if (str == "ABEV3|") return ABEV3;
        if (str == "B3SA3|") return B3SA3;

        if (str == "DI1F24") return DI1F24;
        if (str == "DI1F25") return DI1F25;
        if (str == "DI1F26") return DI1F26;
        if (str == "DI1F27") return DI1F27;
        if (str == "DI1F28") return DI1F28;
        if (str == "DI1F29") return DI1F29;
        if (str == "DI1F30") return DI1F30;
        if (str == "DI1F31") return DI1F31;
        if (str == "DI1F33") return DI1F33;

        return nulo; // Caso padrão
    }
    Ativos ativoToNumInt(const QString& str) {

        if (str == "DXYInd") return DXY;
        if (str == "Usa500") return SP500;
        if (str == "USDMXN") return MEX;
        if (str == "GOLDii") return GOLD;
        if (str == "LCrude") return OIL;
        if (str == "UsaTec") return NASD;
        if (str == "UsaTBD") return BOND;

        if (str == "Euro50") return EUR50;
        if (str == "Soybin") return Soyb;
        if (str == "VixVix") return VIX;
        if (str == "USDEUR") return EUR;
        if (str == "USDZAR") return ZAR;
        if (str == "USDCAD") return CAD;
        if (str == "USDSEK") return SEK;
        if (str == "USDAUD") return AUD;

        return nulo; // Caso padrão
    }
    QString nomeDoAtivo(Ativos ativo) {
        switch (ativo) {
        case DOL:
            return "DOL";
        case WDO:
            return "WDO";
        case IND:
            return "IND";
        case WIN:
            return "WIN";
        case VALE3:
            return "VALE3";
        case PETR4:
            return "PETR4";
        case ITUB4:
            return "ITUB4";
        case BBDC4:
            return "BBDC4";
        case ABEV3:
            return "ABEV3";
        case B3SA3:
            return "B3SA3";
        case DI1F24:
            return "DI1F24";
        case DI1F25:
            return "DI1F25";
        case DI1F26:
            return "DI1F26";
        case DI1F27:
            return "DI1F27";
        case DI1F28:
            return "DI1F28";
        case DI1F29:
            return "DI1F29";
        case DI1F30:
            return "DI1F30";
        case DI1F31:
            return "DI1F31";
        case DI1F33:
            return "DI1F33";
        case DXY:
            return "DXY";
        case SP500:
            return "SP500";
        case MEX:
            return "MEX";
        case GOLD:
            return "GOLD";
        case OIL:
            return "OIL";
        case NASD:
            return "NASD";
        case BOND:
            return "BOND";
        case EUR50:
            return "EUR50";
        case Soyb:
            return "Soyb";
        case VIX:
            return "VIX";
        case EUR:
            return "EUR";
        case ZAR:
            return "ZAR";
        case CAD:
            return "CAD";
        case SEK:
            return "SEK";
        case AUD:
            return "AUD";
        case nulo:
            return "nulo";
        default:
            return "Desconhecido";
        }
    }

private:
    struct SeparadorDadosResult {
        int sizeChange;
        int ativo;
    };

    int separador_dados_BK(QString* msg);
    SeparadorDadosResult* separador_dados_TT(QString* msg);
    SeparadorDadosResult* monta_ordem_original(SeparadorDadosResult* Res_aux);
    int separador_dados_COT(QString* msg);

public:
    static const int numAtivoTT = 4;
    static const int numAtivoBF = 4;
    static const int filtroTToF = 50;
    static const int tamAtivo = 34;

    struct dataTT {
        int negocio;
        QString datetime;
        float preco;
        int lote;
        int Ag_comprador;
        int Ag_vendedor;
        QString Agressor;
    };

    struct dataTTo {
        QString datetime;
        float preco;
        int lote;
        int Ag_comprador;
        int Ag_vendedor;
    };

    struct dataBOOKF {
        int loteComprador;
        float precoComprador;
        float precoVendedor;
        int loteVendedor;
    };

    struct dataCOT {
        float ultima;
        float variacao;
        int bid_lote;
        float bid;
        float ask;
        int ask_lote;
        float abertura;
        float maxima;
        float minima;
        float fechamento;
        float volume;
        QString hora;
        QString estatus;
        float variacao_p;
        QString horario_leilao;

        // Construtor padrão que inicializa todas as variáveis com zero
        dataCOT() {
            ultima = 0.0;
            variacao = 0.0;
            bid_lote = 0;
            bid = 0.0;
            ask = 0.0;
            ask_lote = 0;
            abertura = 0.0;
            maxima = 0.0;
            minima = 0.0;
            fechamento = 0.0;
            volume = 0.0;
            variacao_p = 0.0;
            hora = "";
            estatus = "";
            horario_leilao = "";
        }
    };

    int sizeTTChange[numAtivoTT];
    int sizeTToChange[numAtivoTT];
    int sizeTToFChange[numAtivoTT];
    QVector<dataTT> dadosTT[numAtivoTT];
    QVector<dataTTo> dadosTTo[numAtivoTT];
    QVector<dataTTo> dadosTToF[numAtivoTT];
    QVector<dataBOOKF> dadosBF[numAtivoBF];

    dataCOT dadosCOT[tamAtivo];


public:
    QVector<dataBOOKF> getBook(int deeph, int ativo);
    QVector<dataTT> getTT(int positon, int len);

};

#endif // TRYDDATAPROCESS_H
