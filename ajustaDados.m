clear all;

dados = readmatrix('C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\pasdata_20231017_180643.csv');

lenData = length(dados(:,1));
t_trade = 2 * 60;
y_pC(lenData) = 0;
y_pV(lenData) = 0;
for i=1:1:lenData-t_trade
    bool_pC = 0;
    bool_pV = 0;
    for j=i:1:i+t_trade
        if (dados(i,2) - dados(j,2) <= -0.04)
            bool_pC = 1;
        end
        if (dados(i,2) - dados(j,2) >= 0.04)
            bool_pV = 1;
        end
    end
    y_pC(i) = bool_pC;
    y_pV(i) = bool_pV;
end

matriz_x = dados(:,[10:11]);
matriz_y = vertcat(y_pC, y_pV);

writematrix(matriz_x, 'C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\x_data.csv')
writematrix(matriz_y', 'C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\y_result.csv')
