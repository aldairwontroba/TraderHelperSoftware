//+------------------------------------------------------------------+
//|                                                   mySendInfo.mq5 |
//|                                  Copyright 2023, MetaQuotes Ltd. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2023, MetaQuotes Ltd."
#property link      "https://www.mql5.com"
#property version   "1.00"

string address = "localhost";
int port = 9090;
bool clientConected = false;
int socket;

MqlRates cD_DX[];
MqlRates cD_Usa500[];
MqlRates cD_GOLD[];
MqlRates cD_LCrude[];
MqlRates cD_USDMXN[];
MqlRates cD_UsaTec[];
MqlRates cD_TBnd[];



//+------------------------------------------------------------------+
//| Expert getInicialdata function                                   |
//+------------------------------------------------------------------+
string getStringFromCD(MqlRates &cd){
   string msg;
   msg = DoubleToString(cd.open,4);
   msg += ";";
   msg += DoubleToString(cd.high,4);
   msg += ";";
   msg += DoubleToString(cd.low,4);
   msg += ";";
   msg += DoubleToString(cd.close,4);
   msg += ";";
   
   return msg;
}
//+------------------------------------------------------------------+
void getInicialData(void){

   CopyRates(_Symbol, PERIOD_D1, 0, 2, cD_DX);
   ArraySetAsSeries(cD_DX, true);
   CopyRates("Usa500", PERIOD_D1, 0, 2, cD_Usa500);
   ArraySetAsSeries(cD_Usa500, true);
   CopyRates("GOLD", PERIOD_D1, 0, 2, cD_GOLD);
   ArraySetAsSeries(cD_GOLD, true);
   CopyRates("LCrude", PERIOD_D1, 0, 2, cD_LCrude);
   ArraySetAsSeries(cD_LCrude, true);
   CopyRates("USDMXN", PERIOD_D1, 0, 2, cD_USDMXN);
   ArraySetAsSeries(cD_USDMXN, true);
   CopyRates("UsaTec", PERIOD_D1, 0, 2, cD_UsaTec);
   ArraySetAsSeries(cD_UsaTec, true);
   CopyRates("UsaTBDec23", PERIOD_D1, 0, 2, cD_TBnd);
   ArraySetAsSeries(cD_TBnd, true);

   string msg;
   msg = "\nDXYInd>0>";
   msg += getStringFromCD(cD_DX[0]); 
   msg += "\nUsa500>0>";
   msg += getStringFromCD(cD_Usa500[0]);
   msg += "\nUSDMXN>0>";
   msg += getStringFromCD(cD_GOLD[0]);
   msg += "\nGOLDii>0>";
   msg += getStringFromCD(cD_LCrude[0]);
   msg += "\nLCrude>0>";
   msg += getStringFromCD(cD_USDMXN[0]);
   msg += "\nUsaTec>0>";
   msg += getStringFromCD(cD_UsaTec[0]);
   msg += "\nUsaTBD>0>";
   msg += getStringFromCD(cD_TBnd[0]);
   
   socketSendInfo(socket, msg);
   
   msg = "\nDXYInd>1>";
   msg += getStringFromCD(cD_DX[1]); 
   msg += "\nUsa500>1>";
   msg += getStringFromCD(cD_Usa500[1]);
   msg += "\nUSDMXN>1>";
   msg += getStringFromCD(cD_GOLD[1]);
   msg += "\nGOLDii>1>";
   msg += getStringFromCD(cD_LCrude[1]);
   msg += "\nLCrude>1>";
   msg += getStringFromCD(cD_USDMXN[1]);
   msg += "\nUsaTec>1>";
   msg += getStringFromCD(cD_UsaTec[1]);
   msg += "\nUsaTBD>1>";
   msg += getStringFromCD(cD_TBnd[1]);
   
   socketSendInfo(socket, msg);
}


//+------------------------------------------------------------------+
//| Expert SendInfo function                                   |
//+------------------------------------------------------------------+
bool socketSendInfo(int sock, string msg){
   char req[];
   int  len = StringToCharArray(msg,req)-1;
   if(len<0){
      Print("msg invalida");
      return true;
   }
   int err = SocketSend(sock,req,len);
   if (err == -1){
       clientConected = false;
       Print("Erro ao enviar dados. Tentando reconectar...");
       EventSetTimer(3);
       return true;
   }
   return false;
}
//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
//---
   socket = SocketCreate();
   if(socket == INVALID_HANDLE){
      Print("Erro ao criar o socket");
      return -1;
   }else{
   
      if(SocketConnect(socket,address,port, 1000)){
         Print("Conectado! Enviando dados na taxa de 300ms.");
         clientConected = true;
         socketSendInfo(socket, "Primeira msg\n");
         getInicialData(); //envia o candle do dia + o candle anterior
         //com isso é possivel calcular a variação diária
         EventSetMillisecondTimer(300);
      }else{
         Print("Erro ao conectar. Tentando novamente em 3s...");
         EventSetTimer(3);
      }
   }
   
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//---
      socketSendInfo(socket, "Conexao Fechada");
      Print("Fechando conexão");
      SocketClose(socket);
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
//---

  }
  
//+------------------------------------------------------------------+  
  
string getLastTick_BID(string ativo){
   MqlTick lasttick;
   SymbolInfoTick(ativo,lasttick);

   return DoubleToString(lasttick.bid,4);
}
  
//+------------------------------------------------------------------+
void OnTimer(void){

   if(clientConected == false){
      if(SocketConnect(socket,address,port, 1000)){
         Print("Conectado! Enviando dados na taxa de 300ms.");
         clientConected = true;
         socketSendInfo(socket, "Primeira msg\n");
         getInicialData();
         EventSetMillisecondTimer(300);
      }else{
         Print("Erro ao conectar. Tentando novamente em 3s...");
         return;
      }
   }
    
   string msg;
   msg = "\nDXYInd:";
   msg += getLastTick_BID(_Symbol); //DX colocado pelo grafico pq muda esse simbolo frequentemente
   msg += "\nUsa500:";
   msg += getLastTick_BID("Usa500");
   msg += "\nUSDMXN:";
   msg += getLastTick_BID("USDMXN");
   msg += "\nGOLDii:";
   msg += getLastTick_BID("GOLD");
   msg += "\nLCrude:";
   msg += getLastTick_BID("LCrude");
   msg += "\nUsaTec:";
   msg += getLastTick_BID("UsaTec");
   msg += "\nUsaTBD:";
   msg += getLastTick_BID("UsaTBDec23");
   
   socketSendInfo(socket, msg);
}