/*
 * Desenvolvido por:
 * Gabriel Guinter Herter
 * Giovanni Gaiardo
 * Marcus Vinivius Marques de Lima
 */

#define
int Bot_esq, Bot_dir, Bot_cim, Bot_bai, Bot_cen, TELA = 0; // variáveis que recebem os valores dos estados dos botões (se foram pressionados ou não)

#define BotEsqPin 11 // pinos botões 
#define BotDirPin 13 // pinos botões 
#define BotCimPin 9  // pinos botões 
#define BotBaiPin 12 // pinos botões 
#define BotCenPin 10 // pinos botões 

//variáveis interface usuário
char rst_tela0 = 1;          // variável de reset para tela de consulta da corrente de operação da amostra
char rst_tela1 = 1;          // variável de reset para tela de consulta do angulo de manobra da amostra
char rst_tela2 = 1;          // variável de reset para tela de confirmação de dados e início do ensaio
char rst_ensaio = 1;         // variável de reset para tela de ensaio
char correnteOP = 0;         // variável que determina qual das três diferentes faixas de corrente serão utilizadas (como também o numero de manobras e o tempo entre cada manobra)
int rst_esaio_concluido = 1; // variável responsável pela limpeza do LCD após a conclusão do ensaio

// variáveis relógio

unsigned int T_att, T_inicio = 0; // variáveis que indicam o tempo de inicio do ensaio e o tempo atual em cada manobra (sendo utilizado para determinar o fator de correção do sistema)
unsigned long correcao = 0; // variável de correção do erro para o tempo de cada manobra, o tempo de cada manobra é um valor pré determinado na escolha da faixa de corrente do sistema,no entanto por diversos fatores essa variável passa por um processo de correção 

// variáveis de controle
int meta_manobras = 0; //variável que indica o numero alvo de manobras realizadas pelo ensaio de acordo com a faixa de corrente especificada
int tempo_manobra = 0; //variável com valor correspondente ao tempo em ms em que uma manobra deve ser realizada
int angulo = 1;      //variável manipulável pelo usuário que indica o angulo necessário para a realização de uma manobra
int manobra = -1;    //variável responsável por indicar quantas manobras já foram realizados durante o ensaio 
int mili, micro = 0;   //variáveis onde a variável correção (em microssegundos) tem seu valor dividido em mili(ms) e micro(u) 
int grau = 0;      //variável com o valor que será atribuído ao servomotor, indicando o angulo em que deve se direcionar
int incr_decr = 0;     //variável que indica se o grau deve ser incrementado ou decrementado, mudando o sentido do movimento do servomotor
int rst = 1;       // variável de reset para parâmetros antes do ensaio 

//constantes interface usuário
#define TELA0 0   // constante com valor que representa tela de consulta de corrente de operação da amostra
#define TELA1 1   // constante com valor que representa tela de consulta do angulo de manobra da amostra 
#define TELA2 2   // constante com valor que representa tela de confirmação dos dados e início do ensaio
#define ENSAIO 3  // constante com valor que representa tela de ensaio

//Bibliotecas
#include <Servo.h> // biblioteca para controle do servomotor
Servo myservo; // cria objeto servomotor chamado "myservo"
#define ServoPin A1 // pino Servo

#include <LiquidCrystal.h> // biblioteca para controle do display LCD

#define COLS 16 // numero de colunas
#define ROWS 2  // numero de linhas 

#define EN 3 // pinos de controle LCD
#define RS 2 //16 // pinos de controle LCD
#define D4 4 //18 // pinos de controle LCD
#define D5 5 //19 // pinos de controle LCD
#define D6 6 //20 // pinos de controle LCD
#define D7 7 //21 // pinos de controle LCD

LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); // pinos de controle do LCD LiquidCrystal(rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7)

void setup() {

    myservo.attach(ServoPin); // declaração do pino de controle do servomotor
    myservo.write(0);  // inicia o servo o angulo 0 

    lcd.begin(COLS, ROWS); // inicialização do display LCD

    //SETUP PINOS

    for (int i = 16; i <= 21; i++) {//declara portas output 

        pinMode(i, OUTPUT);

    } 

    for (int i = 9; i <= 13; i++) {//declara portas input

        pinMode(i, INPUT);

    }

    //Serial.begin(9600);

    //introdução do equipamento, escreve no lcd "SETUP DE ENSAIO ROTATÓRIO"
    lcd.clear();                   
    lcd.setCursor(0, 0);
    lcd.print("SETUP DE ENSAIO");
    lcd.setCursor(0, 1);
    lcd.print("   ROTATORIO   ");
    delay(2000);
    lcd.clear();

}

void varredura() { // função que realiza a leitura dos estados dos botões 

    if (digitalRead(BotCenPin)) { 

        Bot_cim++;
        delay(200);

    }
    if (Bot_cim == 2) {
        Bot_cim = 0;
    }

    //--------------------------------

    if (digitalRead(BotBaiPin)) {

        Bot_bai++;
        delay(200);

    }
    if (Bot_bai == 2) {
        Bot_bai = 0;
    }

    //--------------------------------

    if (digitalRead(BotCenPin)) {

        Bot_cen++;
        delay(200);

    }
    if (Bot_cen == 2) {
        Bot_cen = 0;
    }

    //--------------------------------

    if (digitalRead(BotDirPin)) {

        Bot_dir++;
        delay(200);

    }
    if (Bot_dir == 2) {
        Bot_dir = 0;
    }

    //--------------------------------

    if (digitalRead(BotEsqPin)) {

        Bot_esq++;
        delay(200);

    }
    if (Bot_esq == 2) {
        Bot_esq = 0;
    }

}

void loop() { // main

    switch (TELA) { // Switch case para variável TELA 

    case TELA0: // tela onde ocorre a pergunta de qual é a corrente de operação da amostra 

        if (rst_tela0) { // reseta TELA0, limpando LCD e imprimindo a escrita constante 

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Corr. Operacao");
            lcd.setCursor(1, 1);
            rst_tela0 = 0;
        }

        varredura();//varredura dos botoes 
    
        if (Bot_cim) { // quando Bot_cim é pressionado, correnteOP incrementa, mudando a faixa de corrente
            correnteOP++;
            Bot_cim = 0;
        }
    
        if (correnteOP == 3) { // para que o sistema não trave e gere um loop, quando correnteOP tem valor = 3, retora ao valor inicial 0 
            correnteOP = 0;
        }
        if (Bot_bai) { // quando Bot_bai é pressionado, correnteOP decrementa, mudando a faixa de corrente
            correnteOP--;
            Bot_bai = 0;
        }
        if (correnteOP == -1) { // para que o sistema não trave e gere um loop, quando correnteOP tem valor = -1, retora ao valor final 3
            correnteOP = 2;
        }

        if (Bot_dir) { //quando Bot_dir é pressionado, vai para proxima tela (TELA1), e seta rst_tela1 = 1, permitindo o seu reset
            TELA = 1;
            rst_tela1 = 1;
            Bot_dir = 0;
        }
        if (Bot_esq) {//quando Bot_esq é pressionado, vai para tela anterior (TELA2), e seta rst_tela2 = 1, permitindo o seu reset
            TELA = 2;
            rst_tela2 = 1;
            Bot_esq = 0;
        }
        if (Bot_cen) { //quando Bot_cen é pressionado nada ocorre (esse if existe para evitar que Bot_cen tenha seu valor setado para '1', podendo assim interferir em sua próxima interação)
            Bot_cen = 0;
        }

        switch (correnteOP) { //determina as diferentes faixas de corrente e as imprime no LCD de acordo com correnteOP, alterando assim meta_manobras e tempo_manobra

        case 0:

            lcd.setCursor(0, 1);
            lcd.print("Corrente <= 10A ");
            meta_manobras = 30;
            tempo_manobra = 2000;

            break;

        case 1:

            lcd.setCursor(0, 1);
            lcd.print("Corrente 10A-25A");
            meta_manobras = 15;
            tempo_manobra = 4000;

            break;

        case 2:

            lcd.setCursor(0, 1);
            lcd.print("Corrente >= 25A ");
            meta_manobras = 7;
            tempo_manobra = 8571;

            break;

        }

        break;

    case TELA1: // Tela onde é perguntado qual o grau de manobra da amostra 

        if (rst_tela1) { // reseta TELA1, limpando LCD e imprimindo a escrita constante 

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Angulo Manobra ");
            lcd.setCursor(0, 1);
            lcd.print("   Angulo: ");
            rst_tela1 = 0;
        }

    // imprime no LCD variável angulo 
        lcd.setCursor(10, 1);  
        lcd.print(angulo);
        lcd.print("  ");

        varredura(); // varredura dos botões

        if (Bot_dir) { //quando Bot_dir é pressionado, vai para próxima tela (TELA2), e seta rst_tela2 = 1, permitindo o seu reset
            TELA = 2;
            rst_tela2 = 1;
            Bot_dir = 0;
        }
        if (Bot_esq) { //quando Bot_esq é pressionado, vai para tela anterior (TELA0), e seta rst_tela0 = 1, permitindo o seu reset
            TELA = 0;
            rst_tela0 = 1;
            Bot_esq = 0;
        }

        if (Bot_cim) { //quando Bot_cim é pressionado, angulo tem seu valor incrementado 
            Bot_cim = 0;
            angulo++;
        }
        if (Bot_bai) { //quando Bot_bai é pressionado, angulo tem seu valor decrementado
            Bot_bai = 0;
            angulo--;
        }
        if (Bot_cen) { //quando Bot_cen é pressionado nada ocorre (esse if existe para evitar que Bot_cen tenha seu valor setado para '1', podendo assim interferir em sua próxima interação)
            Bot_cen = 0;
        }

        if (angulo < 1) {  //gera loop da variável angulo, quando a mesma tem valor menor que 1 tem seu valor setado para 180
            angulo = 180;
        }
        if (angulo > 180) { //gera loop da variável angulo, quando a mesma tem valor maior que 180 tem seu valor setado para 1
            angulo = 1;
        }

        break;

    case TELA2: // tela onde ocorre a confirmação dos dados apra o incio do ensaio
  
        if (rst_tela2) { // reseta TELA2, limpando LCD e imprimindo a escrita constante  

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Corrente ");

            switch (correnteOP) { // de acordo com correnteOP, imprime no LCD  diferentes informações (imprime a faixa de corrente especificada)

            case 0:

                lcd.print("<= 10A ");

                break;

            case 1:

                lcd.print("10A-25A");

                break;

            case 2:

                lcd.print(">= 25A ");

                break;

            }

      // Imprime no lcd o angulo determinado pelo usuário 
            lcd.setCursor(0, 1); 
            lcd.print("Angulo:");
            lcd.print(angulo);
      //imprime <OK?>, solicitando a confirmação dos dados
            lcd.setCursor(11, 1);
            lcd.print("<OK?>");
            rst_tela2 = 0;
        }

        varredura();// varredura dos estados dos botoes

        if (Bot_dir) { //quando Bot_dir for pressionado vai para a próxima tela e seta rst_tela0 = 1, permitindo o reset da TELA0
            TELA = 0;
            rst_tela0 = 1;
            Bot_dir = 0;
        }
        if (Bot_esq) { //quando Bot_esq for pressionado vai para a tela anterior e seta rst_tela1 = 1, permitindo o reset da TELA1
            TELA = 1;
            rst_tela1 = 1;
            Bot_esq = 0;
        }
        if (Bot_cen) { //quando Bot_cen for pressionado vai para a tela de ensaio e seta rst = 1, permitindo o reset das informações do ensaio

            TELA = 3;
            Bot_cen = 0;
            rst = 1;
        }

        break;

    case ENSAIO: // tela onde ocorre o ensaio 

        if (rst) { // reseta TELA2, limpando LCD, imprimindo a escrita constante e setando valores iniciais para correção, T_inicio, manobra e grau.

            lcd.clear();
            manobra = -1;
            grau = 0;
            correcao = (long) tempo_manobra * 1000 / angulo;
            T_inicio = millis(); // tempo de inicio
            rst = 0;

        }

        while (manobra < meta_manobras) { // ensaio ocorre enquanto manobra for menor que a variável meta_manobras (quantidade de manobras que devem ser realizadas)

            myservo.write(grau);// posiciona servomotor na posição desejada, determinada por grau

            if (myservo.read() == 0) {//quando servo motor apontar para o angulo 0, incr_decr = 1 sinalizando situação de incremento de grau, manobra incrementa e T_att é registrado (tempo atual)

                incr_decr = 1; // grau incremento
                manobra++;
                T_att = millis() - T_inicio;

                if (manobra > 1) { // para manobras maiores que 1 ocorre a correção

                    if (T_att < manobra * tempo_manobra) { // correção para quando o servomotor está adiantado
                        //T_att menor que manobra servo está adiantado => reduzir velocidade => aumentar delay =>subtrair menos
                        correcao = correcao + ((manobra * tempo_manobra) - T_att);

                    } else if (T_att > manobra * tempo_manobra) { // correção ara quando o servomotor está atrasado 
                        //T_att maior que manobra servo está atrasado => aumenta velocidade => diminuir delay => subtrair mais
                        correcao = correcao - ((T_att - manobra * tempo_manobra));

                    }

                }

            }

            if (myservo.read() == angulo) {//quando servo motor apontar para o angulo 180, incr_decr = 0 sinalizando situação de decremento de grau, manobra incrementa e T_att é registrado (tempo atual)

                incr_decr = 0; // grau decremento
                manobra++;

                T_att = millis() - T_inicio;

                if (T_att < manobra * tempo_manobra) { // correção para quando o servomotor está adiantado
                    //T_att menor que manobra servo está adiantado => reduzir velocidade => aumentar delay =>subtrair menos
                    correcao = correcao + ((manobra * tempo_manobra) - T_att);

                } else if (T_att > manobra * tempo_manobra) {// correção para quando o servomotor está atrasado
                    //T_att maior que manobra servo está atrasado => aumenta velocidade => diminuir delay => subtrair mais
                    correcao = correcao - (T_att - manobra * tempo_manobra);

                }
            }

            (incr_decr) ? (grau++) : (grau--); // incremento ou decremento de grau

            mili = correcao / 1000;         //divisão da parte da variável correcao que representa milissegundos
            micro = correcao - mili * 1000; //divisão da parte da variável correcao que representa microssegundos

            delay(mili - 2);          // tempo de espera de grau em grau para ajuste da velocidade, fator de correção padrão de -2ms
            delayMicroseconds(micro); // tempo de espera de grau em grau para ajuste da velocidade

      //Impressão de dados do ensaio no LCD 
            lcd.setCursor(0, 0);
            lcd.print("  Manobra:");
            lcd.print(manobra);
            lcd.setCursor(2, 1);
            lcd.print("T:");
            lcd.print((float)(T_att) / 1000);
            lcd.print(" seg");
            // lcd.setCursor(5, 0);
            //lcd.print(" C:");
            //lcd.print(correcao);

        }
        varredura(); // varredura dos botoes
    
        if (rst_esaio_concluido) { //  limpeza do lcd quando ensaio é concluído
            lcd.clear();
            rst_esaio_concluido = 0;
        }

    // imprime no LCD informações finais do ensaio
        lcd.setCursor(0, 0);
        lcd.print("Ensaio Concluido");
        lcd.setCursor(0, 1);
        lcd.print("Tmp: ");
        lcd.print((float)(T_att) / 1000);
        lcd.print("s");
        lcd.print(" M:");
        lcd.print(manobra);

        if (Bot_cen) {//quando Bot_cen for pressionado,vai para a tela de inicio (TELA0) e servo aponta para poisição inicial (zero), e 
            Bot_cen = 0;
            myservo.write(0);
            TELA = 0;
            rst_tela0 = 1;
        }
        break;

    }

}
