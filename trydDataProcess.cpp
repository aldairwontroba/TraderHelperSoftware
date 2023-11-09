#include "trydDataProcess.h"
#include <QDebug>

QString trydDataProcess::solicitaDados(QString ativo, QString tipo, int pBoff)
{
    QString msg_retorno;

    if (tipo == "NEGS$S|")
    {
        msg_retorno = tipo + ativo + "#";
    }

    if (tipo == "LVL2$S|")
    {
        for (int j = 0; j < pBoff; j++)
        {
            for (int i = 1; i <= 4; i++)
            {
                msg_retorno.append(QString("%1|%2|%3|%4#").arg((tipo+"1"),ativo,QString::number(j),QString::number(i)));
            }
        }
    }

    if (tipo == "COT$S|")
    {
        msg_retorno = tipo + ativo + "#";
    }

    return msg_retorno;
}

QVector<bool> trydDataProcess::processaDados(QString* msg)
{
    QVector<bool> posicoesModificadas(tamAtivo, 0);

    size_t pos_s;
    size_t pos_e;

    pos_s = msg->indexOf("NEGS");

    while (pos_s != -1)
    {
        pos_e = msg->indexOf('#',pos_s + 1);
        QString negs_copia = msg->mid(pos_s, pos_e - pos_s);
        SeparadorDadosResult* res1 = separador_dados_TT(&negs_copia);
        if (res1 == nullptr) break;
        sizeTTChange[res1->ativo] = res1->sizeChange;
        Ativos checkIs = static_cast<Ativos>(res1->ativo);
        if (listaDeAtivosTTo.contains(checkIs)){
            SeparadorDadosResult* res2 = monta_ordem_original(res1);
            if (res2 == nullptr) break;
            sizeTToChange[res2->ativo] = res2->sizeChange;
        }
        posicoesModificadas[res1->ativo] = true;
        pos_s = msg->indexOf("NEGS", pos_s  + 1);
    }

    pos_s = msg->indexOf("LVL2");

    while (pos_s != -1)
    {
        pos_e = msg->indexOf('#', pos_s + 1);
        QString lvl2_copia = msg->mid(pos_s, pos_e - pos_s);
        separador_dados_BK(&lvl2_copia);
        pos_s = msg->indexOf("LVL2", pos_s  + 1);
    }

    pos_s = msg->indexOf("COT");

    while (pos_s != -1)
    {
        pos_e = msg->indexOf('#', pos_s + 1);
        QString cot_copia = msg->mid(pos_s, pos_e - pos_s);
        separador_dados_COT(&cot_copia);
        pos_s = msg->indexOf("COT", pos_s  + 1);
    }

    return posicoesModificadas;
}

trydDataProcess::SeparadorDadosResult *trydDataProcess::separador_dados_TT(QString* msg)
{
    struct dataTTaux {
        QVector<int> negocio;
        QVector<QString> datetime;
        QVector<float> preco;
        QVector<int> lote;
        QVector<int> Ag_comprador;
        QVector<int> Ag_vendedor;
        QVector<QString> Agressor;
    };

    dataTTaux ttAux;

    dataTT datattAux;
    QVector<dataTT> ttUpdate;

    QString stgDataTT;
    size_t posStartIn;
    size_t posStartOut;
    size_t posEndIn;
    size_t posEndOut;

    int sizeCheck[7] = { 0,0,0,0,0,0,0 };

    SeparadorDadosResult* saida = new SeparadorDadosResult; // Aloca dinamicamente um novo objeto
    // Ativo + tamanho
    QString stgAtivo;
    stgAtivo = msg->mid(5, 5); //Copia o ativo
    Ativos ativo = ativoToNum(stgAtivo);
    if (ativo >= numAtivoTT) return saida = nullptr;

    //Decodificação da wxString NEGS

    // Obtem dados de negocio ------------------------------------------------------------------------------------------
    posStartOut = msg->indexOf('|'); //Encontra 1° separador
    posEndOut = msg->indexOf('|', posStartOut + 1); //Encontra 2° separador
    stgDataTT = msg->mid(posStartOut + 1, posEndOut - posStartOut - 1); //Copia a String entre os separadores
    posStartIn = 0;
    do { //executa um loop para capturar todos os dados de negocio da String entre | (loop In)
        posEndIn = stgDataTT.indexOf('@', posStartIn); //Encontra o 1° dado de negocio
        if (posEndIn == -1) posEndIn = stgDataTT.length(); //se -1, então não há mais dados
        QString strAux = stgDataTT.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        int valueAux = strAux.toInt(); //converte string para int
        ttAux.negocio.push_back(valueAux); //salva negocio
        posStartIn = posEndIn + 1; //posiçao inicial + 1 para remover o @
        sizeCheck[0]++;
    } while (posEndIn != stgDataTT.length());


    // Obtem dados de hora ------------------------------------------------------------------------------------------
    posStartOut = posEndOut;
    posEndOut = msg->indexOf('|', posStartOut + 1);
    stgDataTT = msg->mid(posStartOut + 1, posEndOut - posStartOut - 1);
    posStartIn = 0;
    do {
        posEndIn = stgDataTT.indexOf('@', posStartIn);
        if (posEndIn == -1) posEndIn = stgDataTT.length();
        QString strAux = stgDataTT.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        ttAux.datetime.push_back(strAux); //salva dataTime como string
        posStartIn = posEndIn + 1;
        sizeCheck[1]++;
    } while (posEndIn != stgDataTT.length());


    // Obtem dados de preco ------------------------------------------------------------------------------------------
    posStartOut = posEndOut;
    posEndOut = msg->indexOf('|', posStartOut + 1);
    stgDataTT = msg->mid(posStartOut + 1, posEndOut - posStartOut - 1);
    posStartIn = 0;
    do {
        posEndIn = stgDataTT.indexOf('@', posStartIn);
        if (posEndIn == -1) posEndIn = stgDataTT.length();
        QString strAux = stgDataTT.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        strAux.remove(strAux.indexOf('.'), 1);
        strAux.replace(',', '.');
        float valueAux = strAux.toFloat();
        ttAux.preco.push_back(valueAux); //salva dataTime como string
        sizeCheck[2]++;
        posStartIn = posEndIn + 1;
    } while (posEndIn != stgDataTT.length());


    // Obtem dados de lote ------------------------------------------------------------------------------------------
    posStartOut = posEndOut;
    posEndOut = msg->indexOf('|', posStartOut + 1);
    stgDataTT = msg->mid(posStartOut + 1, posEndOut - posStartOut - 1);
    posStartIn = 0;
    do {
        posEndIn = stgDataTT.indexOf('@', posStartIn); //Encontra o 1° dado de negocio
        if (posEndIn == -1) posEndIn = stgDataTT.length(); //se -1, então não há mais dados
        QString strAux = stgDataTT.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        int valueAux = strAux.toInt(); //converte string para long
        ttAux.lote.push_back(valueAux); //salva negocio
        posStartIn = posEndIn + 1; //posiçao inicial + 1 para remover o @
        sizeCheck[3]++;
    } while (posEndIn != stgDataTT.length());


    // Obtem dados de Agente Comprador ------------------------------------------------------------------------------------------
    posStartOut = posEndOut;
    posEndOut = msg->indexOf('|', posStartOut + 1);
    stgDataTT = msg->mid(posStartOut + 1, posEndOut - posStartOut - 1);
    posStartIn = 0;
    do {
        posEndIn = stgDataTT.indexOf('@', posStartIn); //Encontra o 1° dado de negocio
        if (posEndIn == -1) posEndIn = stgDataTT.length(); //se -1, então não há mais dados
        QString strAux = stgDataTT.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        long valueAux = strAux.toInt(); //converte string para long
        ttAux.Ag_comprador.push_back(valueAux); //salva negocio
        posStartIn = posEndIn + 1; //posiçao inicial + 1 para remover o @
        sizeCheck[4]++;
    } while (posEndIn != stgDataTT.length());


    // Obtem dados de Agente Vendedor ------------------------------------------------------------------------------------------
    posStartOut = posEndOut;
    posEndOut = msg->indexOf('|', posStartOut + 1);
    stgDataTT = msg->mid(posStartOut + 1, posEndOut - posStartOut - 1);
    posStartIn = 0;
    do {
        posEndIn = stgDataTT.indexOf('@', posStartIn); //Encontra o 1° dado de negocio
        if (posEndIn == -1) posEndIn = stgDataTT.length(); //se -1, então não há mais dados
        QString strAux = stgDataTT.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        long valueAux = strAux.toInt(); //converte string para long
        ttAux.Ag_vendedor.push_back(valueAux); //salva negocio
        posStartIn = posEndIn + 1; //posiçao inicial + 1 para remover o @
        sizeCheck[5]++;
    } while (posEndIn != stgDataTT.length());


    // Obtem dados de Agressor ------------------------------------------------------------------------------------------
    posStartOut = posEndOut;
    posEndOut = msg->length();
    stgDataTT = msg->mid(posStartOut + 1, posEndOut - posStartOut - 1);
    posStartIn = 0;
    do {
        posEndIn = stgDataTT.indexOf('@', posStartIn);
        if (posEndIn == -1) posEndIn = stgDataTT.length();
        QString strAux = stgDataTT.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        ttAux.Agressor.push_back(strAux); //salva Agressor como string
        posStartIn = posEndIn + 1;
        sizeCheck[6]++;
    } while (posEndIn != stgDataTT.length());

    for (int i = 0; i < 6; i++) //verifica se foi recebido corretamente o numero de dados
    {
        if (sizeCheck[i] != sizeCheck[i + 1])
        {
            SeparadorDadosResult* saida = new SeparadorDadosResult;;
            saida->ativo = ativo;
            saida->sizeChange = -1;
            return saida;
        }
    }

    for (int i = 0; i < sizeCheck[0]; i++)
    {
        datattAux.negocio = ttAux.negocio[i];
        datattAux.datetime = ttAux.datetime[i];
        datattAux.preco = ttAux.preco[i];
        datattAux.lote = ttAux.lote[i];
        datattAux.Ag_comprador = ttAux.Ag_comprador[i];
        datattAux.Ag_vendedor = ttAux.Ag_vendedor[i];
        datattAux.Agressor = ttAux.Agressor[i];

        ttUpdate.push_back(datattAux);
    }


    dadosTT[ativo].append(ttUpdate);

    saida->ativo = ativo;
    saida->sizeChange = sizeCheck[0];
    return saida;
}

trydDataProcess::SeparadorDadosResult *trydDataProcess::monta_ordem_original(SeparadorDadosResult* Res_aux){

    SeparadorDadosResult* saida = new SeparadorDadosResult; // Aloca dinamicamente um novo objeto
    int sizeChange = Res_aux->sizeChange;
    int ativo = Res_aux->ativo;
    saida->ativo = ativo;
    saida->sizeChange = 0;
    if (sizeChange < 1) return saida;

    int lastTTpos = dadosTT[ativo].size() - 1;
    int biginingpos = lastTTpos - sizeChange + 1;
    bool valid = true;

    dataTTo ttoaux;

    int lastTTopos = 0;
    int lastTToFpos = 0;

    if (dadosTTo[ativo].isEmpty()) dadosTTo[ativo].append(ttoaux);
    else lastTTopos = dadosTTo[ativo].size() - 1;

    if (dadosTToF[ativo].isEmpty()) dadosTToF[ativo].append(ttoaux);
    else lastTToFpos = dadosTToF[ativo].size() - 1;


    ttoaux.datetime = dadosTT[ativo][biginingpos].datetime;
    ttoaux.preco = dadosTT[ativo][biginingpos].preco;
    ttoaux.lote = dadosTT[ativo][biginingpos].lote;
    if (dadosTT[ativo][biginingpos].Agressor == "Comprador"){
        ttoaux.Ag_comprador = dadosTT[ativo][biginingpos].Ag_comprador;
        ttoaux.Ag_vendedor = 0;
        dadosTTo[ativo].append(ttoaux);
        saida->sizeChange = 1;
        lastTTopos++;
        biginingpos++;
    }else if(dadosTT[ativo][biginingpos].Agressor == "Vendedor"){
        ttoaux.Ag_vendedor = dadosTT[ativo][biginingpos].Ag_vendedor;
        ttoaux.Ag_comprador = 0;
        dadosTTo[ativo].append(ttoaux);
        saida->sizeChange = 1;
        lastTTopos++;
        biginingpos++;
    }else{
        valid = false;
    }

    for (int i=biginingpos;i<=lastTTpos;i++){
        if( valid && (dadosTT[ativo][i].datetime == dadosTTo[ativo][lastTTopos].datetime) &&
                     (dadosTTo[ativo][lastTTopos].Ag_comprador > 0) &&
                     (dadosTT[ativo][i].Ag_comprador == dadosTTo[ativo][lastTTopos].Ag_comprador) &&
                     (dadosTT[ativo][i].preco >= dadosTTo[ativo][lastTTopos].preco)){
            dadosTTo[ativo][lastTTopos].lote = dadosTTo[ativo][lastTTopos].lote + dadosTT[ativo][i].lote;
            dadosTTo[ativo][lastTTopos].preco = dadosTT[ativo][i].preco;

        }else if( valid && (dadosTT[ativo][i].datetime == dadosTTo[ativo][lastTTopos].datetime) &&
                           (dadosTTo[ativo][lastTTopos].Ag_vendedor > 0) &&
                           (dadosTT[ativo][i].Ag_vendedor == dadosTTo[ativo][lastTTopos].Ag_vendedor) &&
                           (dadosTT[ativo][i].preco <= dadosTTo[ativo][lastTTopos].preco)){
            dadosTTo[ativo][lastTTopos].lote = dadosTTo[ativo][lastTTopos].lote + dadosTT[ativo][i].lote;
            dadosTTo[ativo][lastTTopos].preco = dadosTT[ativo][i].preco;

        }else{
            ttoaux.datetime = dadosTT[ativo][i].datetime;
            ttoaux.preco = dadosTT[ativo][i].preco;
            ttoaux.lote = dadosTT[ativo][i].lote;
            valid = true;
            if (dadosTT[ativo][i].Agressor == "Comprador"){
                ttoaux.Ag_comprador = dadosTT[ativo][i].Ag_comprador;
                ttoaux.Ag_vendedor = 0;
                dadosTTo[ativo].append(ttoaux);
                lastTTopos++;
                saida->sizeChange++;
            }else if(dadosTT[ativo][i].Agressor == "Vendedor"){
                ttoaux.Ag_vendedor = dadosTT[ativo][i].Ag_vendedor;
                ttoaux.Ag_comprador = 0;
                dadosTTo[ativo].append(ttoaux);
                lastTTopos++;
                saida->sizeChange++;
            }else{
                valid = false;
            }
        }

    }
    int j = 0;
    if (saida->sizeChange > 0){
        for (int i=lastTTopos-saida->sizeChange+1;i<=lastTTopos;i++){
            if (dadosTTo[ativo][i].lote >= filtroTToF){
                dadosTToF[ativo].append(dadosTTo[ativo][i]);
                lastTToFpos++;
                j++;
            }
        }

    }
    sizeTToFChange[ativo] = j;
    return saida;
}

QString trydDataProcess::getPrintTT(int sizeChange, int ativo) {

    QString msg_print;

    if (sizeChange >= 1)
    {
        int sizeDadosTT = dadosTT[ativo].size();

        msg_print.append(QString("%1").arg(dadosTT[ativo][sizeDadosTT - sizeChange].datetime));
        msg_print.append(QString("	 %1").arg(dadosTT[ativo][sizeDadosTT - sizeChange].lote));
        msg_print.append(QString("	 %1").arg(QString::number(dadosTT[ativo][sizeDadosTT - sizeChange].preco, 'f', 2)));
        msg_print.append(QString("	 %1").arg(dadosTT[ativo][sizeDadosTT - sizeChange].Agressor));

        for (int i = 1; i < sizeChange; i++)
        {
            msg_print.append(QString("\n%1").arg(dadosTT[ativo][sizeDadosTT - sizeChange + i].datetime));
            msg_print.append(QString("	 %1").arg(dadosTT[ativo][sizeDadosTT - sizeChange + i].lote));
            msg_print.append(QString("	 %1").arg(QString::number(dadosTT[ativo][sizeDadosTT - sizeChange + i].preco, 'f', 2)));
            msg_print.append(QString("	 %1").arg(dadosTT[ativo][sizeDadosTT - sizeChange + i].Agressor));
        }
    }
    if (sizeChange == -1)
    {
        msg_print.append(QString("\n Erro na separação de dados do TT"));
    }

    return msg_print;
}
QString trydDataProcess::getPrintTTo(int sizeChange, int ativo) {

    QString msg_print;

    if (sizeChange >= 1)
    {
        int sizeDadosTT = dadosTTo[ativo].size();

        msg_print.append(QString("%1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange].datetime));
        msg_print.append(QString("	 %1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange].lote));
        msg_print.append(QString("	 %1").arg(QString::number(dadosTTo[ativo][sizeDadosTT - sizeChange].preco, 'f', 2)));
        msg_print.append(QString("	 %1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange].Ag_comprador));
        msg_print.append(QString("	 %1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange].Ag_vendedor));

        for (int i = 1; i < sizeChange; i++)
        {
            msg_print.append(QString("\n%1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange + i].datetime));
            msg_print.append(QString("	 %1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange + i].lote));
            msg_print.append(QString("	 %1").arg(QString::number(dadosTTo[ativo][sizeDadosTT - sizeChange + i].preco, 'f', 2)));
            msg_print.append(QString("	 %1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange + i].Ag_comprador));
            msg_print.append(QString("	 %1").arg(dadosTTo[ativo][sizeDadosTT - sizeChange + i].Ag_vendedor));
        }
    }
    if (sizeChange == -1)
    {
        msg_print.append(QString("\n Erro na separação de dados do TT"));
    }

    return msg_print;
}
QString trydDataProcess::getPrintTToF(int sizeChange, int ativo) {

    QString msg_print;

    if (sizeChange >= 1)
    {
        int sizeDadosTT = dadosTToF[ativo].size();

        msg_print.append(QString("%1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange].datetime));
        msg_print.append(QString("	 %1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange].lote));
        msg_print.append(QString("	 %1").arg(QString::number(dadosTToF[ativo][sizeDadosTT - sizeChange].preco, 'f', 2)));
        msg_print.append(QString("	 %1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange].Ag_comprador));
        msg_print.append(QString("	 %1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange].Ag_vendedor));

        for (int i = 1; i < sizeChange; i++)
        {
            msg_print.append(QString("\n%1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange + i].datetime));
            msg_print.append(QString("	 %1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange + i].lote));
            msg_print.append(QString("	 %1").arg(QString::number(dadosTToF[ativo][sizeDadosTT - sizeChange + i].preco, 'f', 2)));
            msg_print.append(QString("	 %1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange + i].Ag_comprador));
            msg_print.append(QString("	 %1").arg(dadosTToF[ativo][sizeDadosTT - sizeChange + i].Ag_vendedor));
        }
    }
    if (sizeChange == -1)
    {
        msg_print.append(QString("\n Erro na separação de dados do TT"));
    }

    return msg_print;
}

int trydDataProcess::separador_dados_BK(QString* msg)
{
    QVector<dataBOOKF> dadosBFaux;

    QString stgDataBF;
    size_t posStartIn;
    size_t posStartOut;
    size_t posEndIn;
    size_t posEndOut;
    unsigned int profundidade = 0;


    // Ativo + tamanho
    QString stgAtivo;
    stgAtivo = msg->mid(5, 5); //Copia o ativo
    Ativos ativo = ativoToNum(stgAtivo);
    if (ativo >= numAtivoBF) return -1;
    if (dadosBF[ativo].empty()) return -1; //verifico se foi alocado espaço para o book
    dadosBFaux.resize(dadosBF[ativo].size());

    //Decodificação da wxString LVL2
    posStartOut = msg->indexOf('|'); //Encontra 1° separador

    do {
        QString strAux;
        int largura = 0;

        posEndOut = msg->indexOf('|', posStartOut + 1); //Encontra 2° separador
        if (posEndOut == -1) posEndOut = msg->length(); //se -1, então não há mais dados

        stgDataBF = msg->mid(posStartOut + 3, posEndOut - posStartOut - 3); //Copia a String entre os separadores

        posStartIn = 0;
        posEndIn = stgDataBF.indexOf(';', posStartIn); //Encontra o 1° dado de negocio
        strAux = stgDataBF.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        profundidade = strAux.toInt();

        posStartIn = posEndIn + 1;
        posEndIn = stgDataBF.indexOf(';', posStartIn); //Encontra o 1° dado de negocio
        strAux = stgDataBF.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        largura = strAux.toInt();

        posStartIn = posEndIn + 1;
        posEndIn =stgDataBF.length();
        strAux = stgDataBF.mid(posStartIn, posEndIn - posStartIn); //copia a wxString
        strAux.replace(',', '.');
        float valueAux = strAux.toFloat();

        switch (largura)
        {
        case 1:
            dadosBFaux[profundidade].loteComprador = (int)valueAux;
        break;

        case 2:
            dadosBFaux[profundidade].precoComprador= valueAux;
        break;

        case 3:
            dadosBFaux[profundidade].precoVendedor = valueAux;
        break;

        case 4:
            dadosBFaux[profundidade].loteVendedor = (int)valueAux;
        break;
        }

        posStartOut = posEndOut;

    } while (posEndOut != msg->length());

    // /////////////////////////////////


    dadosBF[ativo] = dadosBFaux;

    return 0;
}

QString trydDataProcess::getPrintBF(int deeph, int ativo) {

    QString msg_print;

    if (dadosBF[ativo].empty()) return msg_print.append(QString("\n Book vazio"));

    if (deeph > dadosBF[ativo].size()) deeph = dadosBF[ativo].size();

    msg_print.append(QString("\n--------------------------------------------------\n"));
    for (int i = 0; i < deeph; i++)
    {
        msg_print.append(QString("%1	").arg(dadosBF[ativo][i].loteComprador));
        msg_print.append(QString("%1	").arg(QString::number(dadosBF[ativo][i].precoComprador, 'f', 2)));
        msg_print.append(QString("%1	").arg(QString::number(dadosBF[ativo][i].precoVendedor, 'f', 2)));
        msg_print.append(QString("%1 \n").arg(dadosBF[ativo][i].loteVendedor));
    }

    return msg_print;
}

int trydDataProcess::separador_dados_COT(QString* msg)
{
/*
 *COT!PETR4|29,63 |0,00|300|29,00|29,00|100|0,00|0,00|0,00|29,63|0|10:11:58|0|0|0,00|0|0|Leilão||100|||0,00|0,00 (0,00%)|14/07/2023|14/07/2023 10:11|PETROBRAS PN N2||=|0,44|0,33|43,34|-1,56|72,56|74,39|151,74|29,00|232900|10:05:00         ||0|0|0|-130444969|5602042788|0|0|0|0|0|0|0|0|0|0,00|0,00|0,00|0,00|0,00|0,00|41400|C|0|0|0|0|0|0|0|0|0|0,00|0,00|0,00|0,00|0,00|0,00|0|0|0|0|0|0|0|0|0|0|6,4825|100#
 *COT!PETR4|Ultima|0,00|300|29,00|29,00|100|0,00|0,00|0,00|29,63|0|10:11:58|0|0|0,00|0|0|Leilão||100|||0,00|0,00 (0,00%)|14/07/2023|14/07/2023 10:11|PETROBRAS PN N2||=|0,44|0,33|43,34|-1,56|72,56|74,39|151,74|29,00|232900|Horario do leilao||0|0|0|-130444969|5602042788|0|0|0|0|0|0|0|0|0|0,00|0,00|0,00|0,00|0,00|0,00|41400|C|0|0|0|0|0|0|0|0|0|0,00|0,00|0,00|0,00|0,00|0,00|0|0|0|0|0|0|0|0|0|0|6,4825|100#
 *
*/
    dataCOT auxCOT;
    QString aux;
    size_t posStart;
    size_t posEnd;

    QString stgAtivo;
    stgAtivo = msg->mid(4, 6); //copia a wxString (ATIVO)
    Ativos ativo = ativoToNum(stgAtivo);
    if (ativo == nulo) return -1;

    // inicia a separação da informação
    // ultima
    posStart = msg->indexOf('|'); //Encontra 1° separador
    posEnd = msg->indexOf('|', posStart + 1); //Encontra 2° separador
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.ultima = aux.toFloat();

    //variacao
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.variacao = aux.toFloat();

    //bid_lote
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    auxCOT.bid_lote = aux.toInt();

    //bid
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.bid = aux.toFloat();

    //ask
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.ask = aux.toFloat();

    //ask_lote
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    auxCOT.ask_lote = aux.toInt();

    //abertura
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.abertura = aux.toFloat();

    //maxima
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.maxima = aux.toFloat();

    //minima
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.minima = aux.toFloat();

    //fechamento
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.remove('.');
    aux.replace(',', '.');
    auxCOT.fechamento = aux.toFloat();

    //volume
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    auxCOT.volume = aux.toFloat();

    //hora
    posStart = posEnd;
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    auxCOT.hora = aux;

    //status
    posStart = posEnd;
    for (int i=0;i<=4;i++)
        posStart = msg->indexOf('|', posStart + 1);
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    auxCOT.estatus = aux;

    //variacao_p // |Leilão||100|||0,00|0,00 (0,00%)|
    posStart = posEnd;
    for (int i=0;i<=3;i++)
        posStart = msg->indexOf('|', posStart + 1);
    posEnd = msg->indexOf('|', posStart + 1);
    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
    aux.replace(',', '.');
    auxCOT.variacao_p = aux.toFloat();

    //horario_leilao
//    posStart = posEnd;
//    for (int i=0;i<=14;i++)
//        posStart = msg->indexOf('|', posStart + 1);
//    posEnd = msg->indexOf('|', posStart + 1);
//    aux = msg->mid(posStart + 1, posEnd - posStart - 1);
//    auxCOT.horario_leilao = aux;



    dadosCOT[ativo] = auxCOT;

    return 0;
}

QString trydDataProcess::getPrintCOT(int ativo){

    QString msg_print;

    msg_print.append(QString("\n--------------------------------------------------\n"));

    msg_print.append(QString("Ultima: %1 \n").arg(QString::number(dadosCOT[ativo].ultima, 'f', 2)));
    msg_print.append(QString("Variação: %1 \n").arg(QString::number(dadosCOT[ativo].variacao, 'f', 2)));
    msg_print.append(QString("bid_lote: %1 \n").arg(dadosCOT[ativo].bid_lote));
    msg_print.append(QString("bid: %1 \n").arg(QString::number(dadosCOT[ativo].bid, 'f', 2)));
    msg_print.append(QString("ask: %1 \n").arg(QString::number(dadosCOT[ativo].ask, 'f', 2)));
    msg_print.append(QString("ask_lote: %1 \n").arg(dadosCOT[ativo].ask_lote));
    msg_print.append(QString("abertura: %1 \n").arg(QString::number(dadosCOT[ativo].abertura, 'f', 2)));
    msg_print.append(QString("maxima: %1 \n").arg(QString::number(dadosCOT[ativo].maxima, 'f', 2)));
    msg_print.append(QString("minima: %1 \n").arg(QString::number(dadosCOT[ativo].minima, 'f', 2)));
    msg_print.append(QString("fechamento: %1 \n").arg(QString::number(dadosCOT[ativo].fechamento, 'f', 2)));
    msg_print.append(QString("volume: %1 \n").arg(QString::number(dadosCOT[ativo].volume, 'f', 2)));
    msg_print.append("hora:"+dadosCOT[ativo].hora);
    msg_print.append("\nEstatus: "+dadosCOT[ativo].estatus);
    msg_print.append(QString("\nvariacao_p: %1 \n").arg(QString::number(dadosCOT[ativo].variacao_p, 'f', 2)));
    msg_print.append("Horario do Leilão: "+dadosCOT[ativo].horario_leilao);

    return msg_print;
}

QVector<trydDataProcess::dataBOOKF> trydDataProcess::getBook(int deeph, int ativo)
{
    return dadosBF[ativo];
}

int trydDataProcess::processaDadosInt(QString* msg)
{
    QString stgAtivo;
    QString aux;
    QString valor;
    dataCOT auxCOT;

    size_t pos_s;
    size_t pos_e;


    pos_s = msg->indexOf("\n");

    while (pos_s != -1)
    {
        pos_s += 1; //pulo o \n
        stgAtivo = msg->mid(pos_s, 6); //copio o nome do ativo
        pos_s += 6; //mudo a posição do indexador
        aux = msg->mid(pos_s, 1); //copio o caractere que informa o tipo de dado
        pos_s += 1;
        pos_e = pos_s; //nao necessario

        Ativos ativo = ativoToNumInt(stgAtivo);
        if (ativo == nulo) return -1;
        auxCOT = dadosCOT[ativo]; //obtenho os dados (nao totalmente necessario)

        if(aux == ":"){ //para atualização de ultimos valores
            pos_e = msg->indexOf(';', pos_s);
            valor = msg->mid(pos_s, pos_e - pos_s);
            auxCOT.ultima = valor.toFloat();
            auxCOT.variacao = auxCOT.ultima - auxCOT.fechamento;
            if (auxCOT.fechamento != 0) auxCOT.variacao_p = (auxCOT.variacao / auxCOT.fechamento) * 100;
        }else if(aux == ">"){ //para atualização de valores inicias
            aux = msg->mid(pos_s, 1);
            pos_s += 2;
            if(aux == '0'){
                // abertura
                pos_e = msg->indexOf(';', pos_s);
                valor = msg->mid(pos_s, pos_e - pos_s);
                auxCOT.abertura = valor.toFloat();
                pos_s = pos_e + 1;
                // maxima
                pos_e = msg->indexOf(';', pos_s);
                valor = msg->mid(pos_s, pos_e - pos_s);
                auxCOT.maxima = valor.toFloat();
                pos_s = pos_e + 1;
                // minima
                pos_e = msg->indexOf(';', pos_s);
                valor = msg->mid(pos_s, pos_e - pos_s);
                auxCOT.minima = valor.toFloat();
                pos_s = pos_e + 1;
                // fechamento = ultima
                pos_e = msg->indexOf(';', pos_s);
                valor = msg->mid(pos_s, pos_e - pos_s);
                auxCOT.ultima = valor.toFloat();
                pos_s = pos_e + 1;
            }else if(aux == '1'){
                // abertura
                pos_e = msg->indexOf(';', pos_s);
                pos_s = pos_e + 1;
                // maxima
                pos_e = msg->indexOf(';', pos_s);
                pos_s = pos_e + 1;
                // minima
                pos_e = msg->indexOf(';', pos_s);
                pos_s = pos_e + 1;
                // fechamento
                pos_e = msg->indexOf(';', pos_s);
                valor = msg->mid(pos_s, pos_e - pos_s);
                auxCOT.fechamento = valor.toFloat();
                pos_s = pos_e + 1;
            }else{
                qDebug() << "msgInt invalida";
                return -1;
            }
        }else{
            qDebug() << "msgInt invalida";
            return -1;
        }
        dadosCOT[ativo] = auxCOT;
        pos_s = msg->indexOf("\n", pos_e);
    }

    return 0;
}
