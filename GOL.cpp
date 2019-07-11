/*
Game of Life
By Alexander Bolton
*/
//Library Declares
#include "mbed.h"
#include "N5110.h"
//Variables Declares
Serial pc(USBTX, USBRX);
//PINS
DigitalIn pausebutton(p14); // Pause Button
DigitalIn resetbutton(p16); // Reset Button
DigitalIn Upbutton(p22); // MainMenu Up Button
DigitalIn Selectbutton(p23); // MainMenu Select Button
DigitalIn Downbutton(p24); // MainMenu Down Button
DigitalOut PauseLED(LED1); // Pause LED
DigitalOut ResetLED(LED2); // Reset LED
AnalogIn pot(p20); // Speed Potentiometer
PwmOut speaker(p26); // Speaker
Ticker PauseLEDTICK; // LED Ticker
//Void Declares
void PauseLEDTICKER();
void randomise();
void rule1(int i,int j);
void rule2(int i,int j);
void rule3(int i,int j);
void rule4(int i,int j);
void updatelcd();
void hardwall();
void checkpause();
void checkreset();
void checkmode();
void ProcessRules();
void GameofLife();
void mainmenu();
void sound(int S);
int countneighbours(int i,int j);
//Variables
float waittime = 1;
int mode = 0;
int cells[84][48];
//Other Declares
AnalogIn analog (p20); //random seed noise
N5110 lcd(p7,p8,p9,p10,p11,p13,p21);
//Set screen resolution and cells
int main()
{
    //Set pause button
    //pausebutton.mode(PullUp);  // turn pull-up resistor on interrupt pin
    // first need to initialise display
    lcd.init();
    lcd.normalMode();      // normal colour mode
    lcd.setBrightness(0.5); // put LED backlight on 50%
    lcd.printString("  The Game of",0,0); //Splashscreen Start
    lcd.printString("     Life",0,1);
    lcd.printString("      By",0,2);
    lcd.printString("   Alexander",0,3);
    lcd.printString("    Bolton",0,4); //Splashscreen End
    sound(1);
    waittime= pot.read();
    wait(2); //Wait 2 seconds
    lcd.clear(); //Clear LCD
    wait(1); //Wait 1 seconds
    mainmenu();
    checkmode();
    //randomise(); //Randomise Cells
    //Start Game of Life
    GameofLife();

}
void randomise() //Creates random pixels
{
    srand (analog.read_u16()); //use analog noise input to get a complete random number seed
    for (int i=0; i<84; i++) {
        for (int j=0; j<48; j++) {
            int randomtruefalse = rand() % 2;
            if(randomtruefalse == 1) {
                lcd.setPixel(i,j);
            }
        }
    }
    lcd.refresh();
}

int countneighbours(int i,int j) //Count Neighbours Function
{
    int n = 0;
    if (lcd.getPixel(i-1,j-1) != 0) {
        n++;
    }
    if (lcd.getPixel(i,j-1) != 0) {
        n++;
    }
    if (lcd.getPixel(i+1,j-1) != 0) {
        n++;
    }
    if (lcd.getPixel(i+1,j) != 0) {
        n++;
    }
    if (lcd.getPixel(i+1,j+1) != 0) {
        n++;
    }
    if (lcd.getPixel(i,j+1) != 0) {
        n++;
    }
    if (lcd.getPixel(i-1,j+1) != 0) {
        n++;
    }
    if (lcd.getPixel(i-1,j) != 0) {
        n++;
    }
    return n;
}

void rule1(int i,int j)
/*
Rule 1: Any live cell with fewer than two
live neighbours dies, as if caused by under-population.
*/
{
    cells[i][j]=0;
}
void rule2(int i,int j)
/*
Rule 2: Any live cell with two or three
live neighbours lives on to the next generation.
*/
{
    cells[i][j]=1;
}
void rule3(int i,int j)
/*
Rule 3: Any live cell with more than three
live neighbours dies, as if by over-population.
*/
{
    cells[i][j]=0;
}
void rule4(int i,int j)
/*
Rule 4: Any dead cell with exactly three
live neighbours becomes a live cell, as if by reproduction.
*/
{
    cells[i][j]=1;
}
void updatelcd()
{
    hardwall();
    for (int i=0; i<84; i++) {
        for (int j=0; j<48; j++) {
            int x = cells[i][j];
            if(x != 0) {
                lcd.setPixel(i,j);
            }
            if(x == 0) {
                lcd.clearPixel(i,j);
            }
        }
    }
    lcd.refresh();
}
void hardwall()
{
    for(int i=0; i<83; i++) {
        cells[i][47] = 0;
        cells[i][0] = 0;
    }
    for(int j=0; j<47; j++) {
        cells[83][j] = 0;
        cells[0][j] = 0;
    }
}
void checkpause()
{
    if(pausebutton == 1) {
        int d = 1;
        wait(0.25);//wait so we dont click the button twice
        PauseLED = 1;
        PauseLEDTICK.attach(&PauseLEDTICKER, 2.0);//LED TICKER
        while(d==1) {
            checkreset();
            if(pausebutton == 1) {
                d = 0;
                PauseLED = 0; //TURN LED OFF
                PauseLEDTICK.detach(); //DETACH LED TICK
            }
        }
    }
}
void checkreset()
{
    if(resetbutton == 1) 
    { //check reset button start
        lcd.clear();//clear lcd
        ResetLED = 1;
        lcd.printString("Resetting",0,0);
        sound(2);
        wait(1);
        lcd.clear();
        ResetLED = 0;
        NVIC_SystemReset();//Reset MBed
    } //check reset button end
}

void PauseLEDTICKER()
{
    PauseLED = !PauseLED;
}
void GameofLife()
{
    while(1) {
        //pc.printf(waittime);
        waittime = pot.read();
        checkpause(); //Check pause button
        checkreset(); //check reset button
        wait(waittime);
        ProcessRules();
    }
}
void mainmenu()
{
    /*
    DigitalIn Upbutton(p22);
    DigitalIn Selectbutton(p23);
    DigitalIn Downbutton(p24);
    */
    int select = 0;
    while(select == 0) {
        lcd.printString("Main Menu:",0,0);
        //Check Buttons
        if(Upbutton == 1) {
            mode++;
            wait(0.25);
            lcd.clear();
        }
        if(Selectbutton == 1) {
            select = 1;
            lcd.clear();
            sound(0);
        }
        if(Downbutton == 1) {
            mode--;
            wait(0.25);
            lcd.clear();
        }
        if(mode > 6) {
            mode = 6;
        }
        if(mode < 0) {
            mode = 0;
        }
        if(mode == 0) {
            lcd.printString("--> Random",0,1);
            lcd.printString("Circle",0,2);
            lcd.printString("Square",0,3);
            lcd.printString("Rectangle",0,4);
            lcd.printString("Lines",0,5);
            lcd.refresh();
        }
        if(mode == 1) {
            lcd.printString("Random",0,1);
            lcd.printString("--> Circle",0,2);
            lcd.printString("Square",0,3);
            lcd.printString("Rectangle",0,4);
            lcd.printString("Lines",0,5);
            lcd.refresh();
        }
        if(mode == 2) {
            lcd.printString("Random",0,1);
            lcd.printString("Circle",0,2);
            lcd.printString("--> Square",0,3);
            lcd.printString("Rectangle",0,4);
            lcd.printString("Lines",0,5);
            lcd.refresh();
        }
        if(mode == 3) {
            lcd.printString("Random",0,1);
            lcd.printString("Circle",0,2);
            lcd.printString("Square",0,3);
            lcd.printString("--> Rectangle",0,4);
            lcd.printString("Lines",0,5);
            lcd.refresh();
        }
        if(mode == 4) {
            lcd.printString("Random",0,1);
            lcd.printString("Circle",0,2);
            lcd.printString("Square",0,3);
            lcd.printString("Rectangle",0,4);
            lcd.printString("--> Lines",0,5);
            lcd.refresh();
        }
        if(mode == 5) {
            lcd.printString("Circle",0,1);
            lcd.printString("Square",0,2);
            lcd.printString("Rectangle",0,3);
            lcd.printString("Lines",0,4);
            lcd.printString("--> Explode",0,5);
            lcd.refresh();
        }
        if(mode == 6) {
            lcd.printString("Square",0,1);
            lcd.printString("Rectangle",0,2);
            lcd.printString("Lines",0,3);
            lcd.printString("Explode",0,4);
            lcd.printString("--> Glider",0,5);
            lcd.refresh();
        }
    }
}
void checkmode()
{
    lcd.clear();
    lcd.refresh();
    if(mode == 0) { //Random
        randomise();
    }
    if(mode == 1) { // Circle
        lcd.drawCircle(42, 24, 14, 1);
    }
    if(mode == 2) { //Square
        lcd.drawRect(21,12,15,15, 1);
    }
    if(mode == 3) { //Rectangle
        lcd.drawRect(21,12,25,15, 1);
    }
    if(mode == 4) { //Line
        lcd.drawLine(0,0,82,48,2);
        lcd.drawLine(11,15,34,44,1);
    }
    if(mode == 5) { //Explode
        lcd.setPixel(40,22); //1
        lcd.setPixel(42,22); //2
        lcd.setPixel(44,22); //3
        lcd.setPixel(40,23); //4
        lcd.setPixel(44,23); //5
        lcd.setPixel(40,24); //6
        lcd.setPixel(44,24); //7
        lcd.setPixel(40,25); //8
        lcd.setPixel(44,25); //9
        lcd.setPixel(40,26); //10
        lcd.setPixel(42,26); //11
        lcd.setPixel(44,26); //12
        
    }
    if(mode == 6) { //Glider
        lcd.setPixel(42,23);
        lcd.setPixel(43,24);
        lcd.setPixel(44,22);
        lcd.setPixel(44,23);
        lcd.setPixel(44,24);
    }
    lcd.setBrightness(0.5);
    lcd.refresh();
    wait(1);
}
void sound(int S)
{
    speaker =0.5;
    if(S == 0) { // beep
        speaker.period(1.0/500.0);
        wait(0.5);
    }
    if(S == 1) {
        speaker.period(1.0/50.0);
        wait(0.1);
        speaker.period(1.0/100.0);
        wait(0.1);
        speaker.period(1.0/150.0);
        wait(0.1);
        speaker.period(1.0/200.0);
        wait(0.1);
        speaker.period(1.0/250.0);
        wait(0.1);
        speaker.period(1.0/300.0);
        wait(0.1);
        speaker.period(1.0/350.0);
        wait(0.1);
        speaker.period(1.0/400.0);
        wait(0.1);
        speaker.period(1.0/450.0);
        wait(0.1);
        speaker.period(1.0/500.0);
        wait(0.1);
    }
    if(S == 2) {
        speaker.period(1.0/500.0);
        wait(0.1);
        speaker.period(1.0/450.0);
        wait(0.1);
        speaker.period(1.0/400.0);
        wait(0.1);
        speaker.period(1.0/350.0);
        wait(0.1);
        speaker.period(1.0/300.0);
        wait(0.1);
        speaker.period(1.0/250.0);
        wait(0.1);
        speaker.period(1.0/200.0);
        wait(0.1);
        speaker.period(1.0/150.0);
        wait(0.1);
        speaker.period(1.0/100.0);
        wait(0.1);
        speaker.period(1.0/50.0);
        wait(0.1);
    }
    if(S == 3) {
    }
    speaker =0;
    speaker.period(1.0/500.0);
}
void ProcessRules()
{
    for (int i=0; i<84; i++) {
        for (int j=0; j<48; j++) {
            int cn = countneighbours(i,j); //Shorten function to cn
            if(lcd.getPixel(i,j) != 0) { //Check if pixel alive
                if(cn < 2) {
                    rule1(i,j);
                }
                if(cn == 2) {
                    rule2(i,j);
                }
                if(cn == 3) {
                    rule2(i,j);
                }
                if(cn > 3) {
                    rule3(i,j);
                }
            }
            if(lcd.getPixel(i,j) == 0) {//Check if pixel dead
                if(cn == 3) {
                    rule4(i,j);
                }
            }
        }
    }
    updatelcd();
}