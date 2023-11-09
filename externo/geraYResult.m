clear all;
close all;

dados = readmatrix('C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\data\pasdata_20231101.csv');

lenData = length(dados(:,1));

DOL = 1;
wdo = 6;
mex = 24;
wdo_mex = 43;


% variação minima de 2 pontos e maxima de 10
% pontos de não retorno (>1ponto) 

apm = 60; %amostras por minuto

t_trade_max = 20 * apm;
t_trade_max_nonR = 2 * apm;

valores_nonRetur = [3, 4, 5];
valores_gain = [2, 4, 6, 8, 10];
valores_risc = [2, 4, 6, 8, 10];

res_nonR_C = zeros(lenData,length(valores_nonRetur));
res_nonR_V = zeros(lenData,length(valores_nonRetur));

res_pd_C = zeros(lenData,length(valores_gain),length(valores_risc));
res_pd_V = zeros(lenData,length(valores_gain),length(valores_risc));

sts__C = zeros(length(valores_gain),length(valores_risc));
sts__V = zeros(length(valores_gain),length(valores_risc));

res__C = zeros(length(valores_gain),length(valores_risc));
res__V = zeros(length(valores_gain),length(valores_risc));
nOp__C = zeros(length(valores_gain),length(valores_risc));
nOp__V = zeros(length(valores_gain),length(valores_risc));

for i=1:1:lenData-t_trade_max

    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
    for i_nonR = 1:length(valores_nonRetur)
        nonRetr = valores_nonRetur(i_nonR);
    
        %COMPRA
        c_inicio = dados(i,DOL);
        for j=i+1:1:i+t_trade_max_nonR
            if(c_inicio - dados(j,DOL) >= 0.99)
                break;
            else
                if(dados(j,DOL) - dados(i,DOL) >= nonRetr-0.01)
                    res_nonR_C(i,i_nonR) = 1;
                    break;
                else
                    if(dados(j,DOL) > c_inicio)
                        c_inicio = dados(j,DOL);
                    end
                end
            end
        end
        %VENDA
        v_inicio = dados(i,DOL);
        for j=i+1:1:i+t_trade_max_nonR
            if(dados(j,DOL) - v_inicio >= 0.99)
                break;
            else
                if(dados(i,DOL) - dados(j,DOL) >= nonRetr-0.01)
                    res_nonR_V(i,i_nonR) = 1;
                    break;
                else
                    if(dados(j,DOL) < v_inicio)
                        v_inicio = dados(j,DOL);
                    end
                end
            end
        end

    end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    for i_VG = 1:length(valores_gain)
        valor_G = valores_gain(i_VG);
        
        for i_VR = 1:length(valores_risc)
            valor_R = valores_risc(i_VR);

            
            %COMPRA
            for j=i+1:1:i+t_trade_max
                if(dados(i,DOL) - dados(j,DOL) >= valor_R-0.01)
                    break;
                else
                    if(dados(j,DOL) - dados(i,DOL) >= valor_G+0.5-0.01)
                        res_pd_C(i,i_VG,i_VR) = 1;
                        break;
                    end
                end
            end
            %VENDA
            for j=i+1:1:i+t_trade_max
                if(dados(j,DOL) - dados(i,DOL) >= valor_R-0.01)
                    break;
                else
                    if(dados(i,DOL) - dados(j,DOL) >= valor_G+0.5-0.01)
                        res_pd_V(i,i_VG,i_VR) = 1;
                        break;
                    end
                end
            end

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            %analise de resultado
            %COMPRA
            if(res_pd_C(i,i_VG,i_VR) == 1 && sts__C(i_VG,i_VR) == 0)
                sts__C(i_VG,i_VR) = dados(i,DOL);
            end
            if(sts__C(i_VG,i_VR) ~= 0)
                if(dados(i,DOL) - sts__C(i_VG,i_VR) >= valor_G + 0.49)
                    res__C(i_VG,i_VR) = res__C(i_VG,i_VR) + valor_G;
                    nOp__C(i_VG,i_VR) = nOp__C(i_VG,i_VR) + 1;
                    sts__C(i_VG,i_VR) = 0;
                else
                    if(sts__C(i_VG,i_VR) - dados(i,DOL) >= valor_R-0.01)
                        res__C(i_VG,i_VR) = res__C(i_VG,i_VR) - (sts__C(i_VG,i_VR) - dados(i,DOL));
                        nOp__C(i_VG,i_VR) = nOp__C(i_VG,i_VR) + 1;
                        sts__C(i_VG,i_VR) = 0;
                    end
                end
            end
            %VENDA
            if(res_pd_V(i,i_VG,i_VR) == 1 && sts__V(i_VG,i_VR) == 0)
                sts__V(i_VG,i_VR) = dados(i,DOL);
            end
            if(sts__V(i_VG,i_VR) ~= 0)
                if(sts__V(i_VG,i_VR) - dados(i,DOL) >= valor_G + 0.49)
                    res__V(i_VG,i_VR) = res__V(i_VG,i_VR) + valor_G;
                    nOp__V(i_VG,i_VR) = nOp__V(i_VG,i_VR) + 1;
                    sts__V(i_VG,i_VR) = 0;
                else
                    if(dados(i,DOL) - sts__V(i_VG,i_VR) >= valor_R-0.01)
                        res__V(i_VG,i_VR) = res__V(i_VG,i_VR) - (sts__V(i_VG,i_VR) - dados(i,DOL));
                        nOp__V(i_VG,i_VR) = nOp__V(i_VG,i_VR) + 1;
                        sts__V(i_VG,i_VR) = 0;
                    end
                end
            end
        end
    end

end

rM_C = res__C./nOp__C;
rM_V = res__V./nOp__V;

% res = xor(y_pV,y_pC);
% resC = res.*y_pC;
% resV = res.*y_pV;
% 
matriz_x = dados(:,[5:56]);
matriz_y = vertcat(res_nonR_C', res_nonR_V');

writematrix(matriz_x, 'C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\data\x_data.csv');
writematrix(matriz_y', 'C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\data\y_data.csv');
