clear all;
close all;

dados = readmatrix('C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\data\pasdata_20231101.csv');

lenData = length(dados(:,1));

wdo = 6;
mex = 24;
wdo_mex = 43;
% testar o resultado para entradas aleatorias
% testar usando o 10m junto

t_trade = 20 * 60;

valores_limiar = [0.02, 0.03, 0.04, 0.05];
valores_risc = [4, 6, 8, 10];
relRisc = [0.5, 1, 1.5, 2, 3];

resultados_comp = zeros(length(valores_limiar), length(valores_risc),length(relRisc));
resultados_vend = zeros(length(valores_limiar), length(valores_risc),length(relRisc));

nOp_comp = zeros(length(valores_limiar), length(valores_risc),length(relRisc));
nOp_vend = zeros(length(valores_limiar), length(valores_risc),length(relRisc));

for i_limiar = 1:length(valores_limiar)
    limiar = valores_limiar(i_limiar);

    for i_risc = 1:length(valores_risc)
    risc = valores_risc(i_risc);
        
        for i_relR = 1:length(relRisc)
        RR = relRisc(i_relR);    
        
            lado = zeros(1, lenData);
            rr = zeros(1, lenData);
            nOpC = 0;
            nOpV = 0;
            resultComp = 0;
            resultVend = 0;
        
            t_wait = false;
            count = 0;
            i = 1;
            while i <= lenData - t_trade
                lado(i) = 0;
            
                % limiar para testar rr e rr em uma possível compra
                if (dados(i, wdo_mex) < -1 * limiar && t_wait == false)
                    nOpC = nOpC + 1;
                    lado(i) = risc;
                    j = i + 1;
                    while j <= i + t_trade
                        lado(j) = risc;
                        rr(j) = round((dados(j, wdo) - dados(i, wdo))*50);
                        if (rr(j) >= risc*RR - 0.3 + 1)
                            resultComp = resultComp + rr(j) - 1;
                            j = j + 1;
                            t_wait = true;
                            break;
                        end
                        if (rr(j) <= -1*risc + 0.3)
                            resultComp = resultComp + rr(j);
                            j = j + 1;
                            t_wait = true;
                            break;
                        end
                        if (j >= (i + t_trade - 1))
                            resultComp = resultComp + rr(j) - 1;
                            j = j + 1;
                            t_wait = true;
                            break;
                        end
                        j = j + 1;
                    end
                    i = j;
                end
    
                % limiar para testar rr e rr em uma possível venda
                if (dados(i, wdo_mex) > limiar && t_wait == false)
                    nOpV = nOpV + 1;
                    lado(i) = -risc;
                    j = i + 1;
                    while j <= i + t_trade
                        lado(j) = -risc;
                        rr(j) = round((dados(i, wdo) - dados(j, wdo))*50);
                        if (rr(j) >= risc*RR - 0.3 + 1)
                            resultVend = resultVend + rr(j) - 1;
                            j = j + 1;
                            t_wait = true;
                            break;
                        end
                        if (rr(j) <= -1*risc + 0.3)
                            resultVend = resultVend + rr(j);
                            j = j + 1;
                            t_wait = true;
                            break;
                        end
                        if (j == (i + t_trade - 1))
                            resultVend = resultVend + rr(j) - 1;
                            j = j + 1;
                            t_wait = true;
                            break;
                        end
                        j = j + 1;
                    end
                    
                    i = j;
                end
            
                i = i + 1; % Avança para a próxima iteração
                if (t_wait == true)
                    count = count + 1;
                    if (count >= 10)
                        t_wait = false;
                        count = 0;
                    end
                end

            end

            resultados_comp(i_limiar,i_risc,i_relR) = resultComp;
            resultados_vend(i_limiar,i_risc,i_relR) = resultVend;
            nOp_comp(i_limiar,i_risc,i_relR) = nOpC;
            nOp_vend(i_limiar,i_risc,i_relR) = nOpV;

%             plot(lado);
%             hold on;
%             plot(rr);

        end
    end
end

% Supondo que você deseja encontrar o maior valor do produto para 'resultados_comp' e 'nOp_comp'
maior_produto_comp = zeros(length(valores_limiar), length(valores_risc), length(relRisc));

for i = 1:length(valores_limiar)
    for j = 1:length(valores_risc)
        for k = 1:length(relRisc)
            % Encontre o produto para 'resultados_comp' e 'nOp_comp' na posição (i, j, k)
            produto_comp = resultados_comp(i, j, k) / nOp_comp(i, j, k);
            
            % Atribua o produto ao elemento correspondente em 'maior_produto_comp'
            maior_produto_comp(i, j, k) = produto_comp;
        end
    end
end

% Encontre o maior valor do produto para 'resultados_comp' e 'nOp_comp'
maior_valor_produto_comp = max(maior_produto_comp, [], 'all');
[indice_limiar_comp, indice_risc_comp, indice_relRisc_comp] = find(maior_produto_comp == maior_valor_produto_comp);


% O mesmo pode ser repetido para 'resultados_vend' e 'nOp_vend' da seguinte maneira:
maior_produto_vend = zeros(length(valores_limiar), length(valores_risc), length(relRisc));

for i = 1:length(valores_limiar)
    for j = 1:length(valores_risc)
        for k = 1:length(relRisc)
            % Encontre o produto para 'resultados_vend' e 'nOp_vend' na posição (i, j, k)
            produto_vend = resultados_vend(i, j, k) / nOp_vend(i, j, k);
            
            % Atribua o produto ao elemento correspondente em 'maior_produto_vend'
            maior_produto_vend(i, j, k) = produto_vend;
        end
    end
end

% Encontre o maior valor do produto para 'resultados_vend' e 'nOp_vend'
maior_valor_produto_vend = max(maior_produto_vend, [], 'all');
[indice_limiar_vend, indice_risc_vend, indice_relRisc_vend] = find(maior_produto_comp == maior_valor_produto_comp);


% num1c = 1;
% num2c = 2;
% ocor1c = sum(y_retC == num1c);
% ocor2c = sum(y_retC == num2c);
% num1v = -1;
% num2v = -2;
% ocor1v = sum(y_retV == num1v);
% ocor2v = sum(y_retV == num2v);



% subplot(3, 1, 1);
plot(dados(:,wdo));
hold on;
plot(dados(:,mex));
title('Gráfico 1');

% subplot(3, 1, 2);
figure(2);
plot(dados(:,wdo_mex));
hold on;
plot(dados(:,wdo_mex+1));
title('Gráfico 2');

% subplot(3, 1, 3);
figure(3);
plot(lado);
hold on;
plot(rr);
title('Gráfico 3');

% res = xor(y_pV,y_pC);
% resC = res.*y_pC;
% resV = res.*y_pV;

% matriz_x = dados(:,[5:56]);
% matriz_y = vertcat(y_pC, y_pV);

% writematrix(matriz_x, 'C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\data\x_data.csv')
% writematrix(matriz_y', 'C:\Users\Aldair\OneDrive\Mercado Financeiro\SoftwareProjetos\TraderHelperSoftware\data\y_result.csv')