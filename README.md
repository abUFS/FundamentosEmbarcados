# FundamentosEmbarcados
Repositório para armazenar as avaliações da disciplina COMP0417 - FUNDAMENTOS DE SISTEMAS EMBARCADOS (2022.2 - T03).

Como usar:
- Baixar o simulador SimulIDE 1.0.0 (https://www.simulide.com/p/downloads.html);
- Baixar a IDE do arduino (https://www.arduino.cc/en/software);
- Baixar o código .ino e a simulação .sim1 desejada;
- Compilar o código usando a IDE do arduino. Será gerado um arquivo .hex nos arquivos temporários, geralmente em "C:\Users\USER\AppData\Local\Temp\arduino\sketches";
- Abrir o SimulIDE, carregar a simulação, clicar no botão direito sobre o Arduino e selecionar a opção "Load firmware" e escolher o arquivo .hex da simulação.

Opcionalemente, cada pasta das questões já vem com .hex para facilitar.

# Questao 3 - MemoryGame
Corresponde a segunda avaliação.
Foi implementado um jogo da memória com 4 LEDS de cores diferentes (GREEN, RED, YELLOW E BLUE) e 4 botões que correspondem a cada um dos LEDs:
- A cada rodada é apresentada uma sequência aleatória de LEDs e o jogador deve executar (clicar nos botões) na mesma ordem que for apresentada.
- A sequência inicia com apenas um LED, caso o usuário acerte, na próxima rodada, será adicionado mais um led aleatório a sequência. Ex: 1º rodada = RED; 2ª rodada = RED, GREEN; 3ª rodada = RED, GREEN, RED ...
- Caso o usuário digite algum LED fora da ordem apresentada, será informado do erro ("Wrong sequence!") e será solicitado que insira seu "nick" no "Highscores".
- Nessa etapa, os botões assumem outras funções: O Botão GREEN avança pela letras do alfabeto, o BLUE retrocede pelo alfabeto (inverso do green), o YELLOW confirma a letra (avança para próxima casa) e o RED faz o inverso (volta para casa anterior).
- Cada nick pode ter até 5 letras.
- O Highscore é armazenado na EEPROM, então mesmo que a simulação (ou o dispositivo real) for desligado estará salvo nela.
- Depois de inserir seu nick dos highscores, será passado a lista dos highscores.
- Para economizar a EEPROM somente podem existir 5 entradas no highscores. Caso alguém faça uma pontuação que tome o lugar de alguém no highscores, as pontuações, junto aos nomes, serão deslocados para baixo. Se já estiver cheio (5 entradas) o último colocado é sobrescrito.
- 
