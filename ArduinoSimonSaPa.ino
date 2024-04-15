#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

// Green
#define ledGreen 2
#define buttonGreen A2
// White
#define ledWhite 3
#define buttonWhite A3
// Red
#define ledRed 4
#define buttonRed A4
// Yelow
#define ledYelow 5
#define buttonYelow A5
// Blue
#define ledBlue 6
#define buttonBlue A6

// variable per a sapiguer si el boto a sigut presionat
int statusSensorButton[7];
// Variable per comprobar en quin estat estaba abans els botons
int oldStatusSensorButton[7];
// variable per a tindre la secuencia del joc
int sequence[100];
// variable que marca el maxim de la secuencia
int maxLoop = 100;
// variable on ens diu en quina posicio actual estem en la secuencia
int nextPositionArray = 0;
// variable per saber si es hora de jugar o es hora de mirar. Si es True es hora de jugar i si es False es hora de mirar
bool IsItTimeToPlay = false;
// variable per a sapiguer en quina posicio de la secuencia en toca actualment
int CurrentSequencePosition = -1;
// maxim de columnes en la pantalla
int maxColumnsScreen = 16;
// maxim de files en la pantalla
int maxRowScreen = 2;
// Variable per a marcar que s'ha iniciat el torn
bool startTurn = false;

// funcio per a la configuracio inicial
void setup()
{
    // aixo es per a que mostri per pantalla de la aplicacio els missatges amb el Serial.println(" ");
    Serial.begin(9600);

    // aixo serveix per a iniciar els numeros aletoris
    randomSeed(analogRead(0));

    // aixo ho fem per a ficar tota la seqüencia aleatoria a un array
    for (byte i = 0; i < maxLoop; i = i + 1)
    {
        sequence[i] = random(2, 7);
    }

    // iniciem botons
    initButtons(ledGreen, buttonGreen);
    initButtons(ledRed, buttonRed);
    initButtons(ledBlue, buttonBlue);
    initButtons(ledYelow, buttonYelow);
    initButtons(ledWhite, buttonWhite);

    // Inicializem la pantalla. Compte quantes columnes i files té la pantalla. Fiquem missatge inicial
    lcd.begin(maxColumnsScreen, maxRowScreen);
    showMessageOnScreen("Simon SaPa", 3, 0);
    showMessageOnScreen(" ", 0, 1);
}

// funcio de Loop. Aquesta funcio es recorre indefinidament cada pocs milisegons
void loop()
{

    // si es hora de jugar entra i fem les tasques que calguin per jugar
    if (IsItTimeToPlay)
    {
        // mostrar missatge de jugar
        showMessageOnScreen("A Jugar!!", 0, 1);

        // comprobem que algun dels boton hagi sigut clicat
        statusSensorButton[buttonGreen] = analogRead(buttonGreen);
        statusSensorButton[buttonRed] = analogRead(buttonRed);
        statusSensorButton[buttonBlue] = analogRead(buttonBlue);
        statusSensorButton[buttonWhite] = analogRead(buttonWhite);
        statusSensorButton[buttonYelow] = analogRead(buttonYelow);

        delay(100);

        offAllLeds();

        // funcions per a comprobar si s'ha presionat el boton i si cal inicial el led i continuar amb la seqüencia
        checkIfTheButtonHasBeenPressed(buttonGreen, ledGreen);
        checkIfTheButtonHasBeenPressed(buttonRed, ledRed);
        checkIfTheButtonHasBeenPressed(buttonBlue, ledBlue);
        checkIfTheButtonHasBeenPressed(buttonWhite, ledWhite);
        checkIfTheButtonHasBeenPressed(buttonYelow, ledYelow);

        // Si la seguent posicio del array es mes gran que la posicio actual de la seqüencia i a mes ja s'ha iniciat el joc, presionan un dels botons correctes.
        // Aixo vol dir que ja esta el torn acabat i que s'ha de fer de nou la seqüencia afegin un nou color.
        if (nextPositionArray > CurrentSequencePosition && startTurn)
        {
            IsItTimeToPlay = false;
        }

        // agefim a la variable del estat anterior el estat actual
        oldStatusSensorButton[buttonGreen] = statusSensorButton[buttonGreen];
        oldStatusSensorButton[buttonRed] = statusSensorButton[buttonRed];
        oldStatusSensorButton[buttonBlue] = statusSensorButton[buttonBlue];
        oldStatusSensorButton[buttonWhite] = statusSensorButton[buttonWhite];
        oldStatusSensorButton[buttonYelow] = statusSensorButton[buttonYelow];
    }
    else
    {
        // si no es hora de jugar i per tant es hora de mirar, el que fara es mostrar la secuencia seguent a realizar.

        // mostrar missatgeen blanc
        showMessageOnScreen(" ", 0, 1);

        // apaguem tots els leds
        offAllLeds();

        // Iniciem tots els leds per mostrar que estem començant la seqüència
        delay(500);
        onAllLeds();

        // mostrar missatge de mirar
        showMessageOnScreen("Mira sequencia", 0, 1);
        delay(1000);

        // apaguem tots els leds
        offAllLeds();
        delay(500);

        // inicialitzem la posicio del array per a la seguent tanda de jugar
        nextPositionArray = 0;
        // inicialitzem el joc per al seguent torn
        startTurn = false;

        // afegim un mes a la actual posicio de la seqüencia
        CurrentSequencePosition = CurrentSequencePosition + 1;

        // recorrem tota la secuencia, fins la posicio que ens toqui actualment
        for (byte i = 0; i < CurrentSequencePosition + 1; i = i + 1)
        {
            int currentSequence = sequence[i];
            turnOnLed(currentSequence);
            delay(1000);
            offAllLeds();
            delay(500);
        }

        // afegim l'hora de Jugar i apaguem tots els leds
        IsItTimeToPlay = true;
        delay(300);
        offAllLeds();
    }
}

// funcio per a la inicialitzacio del led i del boto. Parametre primer el pin del led i despres el pin del boto.
void initButtons(int led, int button)
{
    // primer s'inicia el led i el fiquem apagat i despres iniciem el boto.
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
    pinMode(button, INPUT);
}

// funcio per a comprobar si s'ha presionat el boto. Parametres pin del boto i pin del led
void checkIfTheButtonHasBeenPressed(int button, int led)
{
    // comprobem: primer si en el array esta el boto clicat. Segon si el antic estat del boto ha estat clicat.
    if (statusSensorButton[button] == 0 && oldStatusSensorButton[button] != 0)
    {
        // Si la secuencia es la correcta.
        if (sequence[nextPositionArray] == led)
        {
            // iniciem la variable del joc. aquesta variable nomes estara en true aquí
            startTurn = true;

            // afegim un mes a la variable per a seguir amb la seqüencia
            nextPositionArray = nextPositionArray + 1;

            // enguem el led
            turnOnLed(led);
        }
        else
        {
            // si la seqüència no és correcta el que farà és finalitzar el joc i Mostrar un missatge que s'ha acabat el joc
            Serial.println(" No Correcto");
        }
    }
}

// funcio per encendre els leds. Parametre el pin del led.
void turnOnLed(int numLed)
{
    digitalWrite(numLed, HIGH);
    delay(200);
}

// funcio per apagar tots els leds
void offAllLeds()
{
    digitalWrite(ledBlue, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledYelow, LOW);
    digitalWrite(ledWhite, LOW);
}

// funcio per encendre tots els leds
void onAllLeds()
{
    digitalWrite(ledBlue, HIGH);
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledYelow, HIGH);
    digitalWrite(ledWhite, HIGH);
}

// funcio per a mostrar missatges. El primer parametre es per al texte que es mostrara, el segon per a la columna i el tercer per a la fila
void showMessageOnScreen(String textScreen, int colum, int row)
{
    // Afegeix al final de espais en blanc
    while (textScreen.length() < maxColumnsScreen)
    {
        textScreen += " ";
    }

    // estableix la posició del cursor (on apareixerà el text següent) columna 0 i fila 1
    lcd.setCursor(colum, row);

    // mostra el missatge que veiem a la primera línia de la pantalla.
    lcd.print(textScreen);
}
