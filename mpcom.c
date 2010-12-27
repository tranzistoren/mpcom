#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
/*
sterowanie mocpem czy czymś z portu com //nie, teraz mpd-em za pomocą mpc
<Rocket> ioctl(fd, TIOCMSET, &status)
<Rocket> masz już wszystko co potrzeba, tą linijkę możesz dopisać i wtedy mozesz  ustawiać RTS i DTR
jakby to dodać to będzie 6 pinów do dyspozycji, paanie, to 35 przycisków!
*/
int przejscia = 0;          //przejscia petli - uzywane dalej do robienia odstepu czasowego miedzy wykonywaniem tych samych polecen
void wykonaj(int czynnosc)  //funkcja robiąca.
{
    przejscia=0;            // wartosc przejsc zerowana gdy polecenie jest wykonywane
    switch(czynnosc)        //bez rewelacji, przelacznik z roznymi poleceniami
    {                       //mysle ze w przyszlosci mozna zrobic te polecenia w define
        case 1:
        system("mpc prev");
        break;
        case 2: 
        system("mpc toggle");
        break;
        case 3:
        system("mpc next");
        break;
        case 4:
        system("amixer set Master 1-");
        break;
        case 5:
        system("amixer set Master 1+");
        break;
        /*
        case 6:foo1; break;
        case 7:foo2; break;
        case 8:foo3; break; etc
        */
    }
}

void main(void)
{
    int port;
    int status;
    int przycisk;
    struct termios tio;
    port = open("/dev/ttyS0", O_RDWR); //inicjacja portu i takie tam, jak działa to nie ruszaj, ewentualnie można tu zmienić używany port
    tcgetattr(port, &tio);
    tio.c_cflag &= ~HUPCL;
    tcsetattr(port, TCSANOW, &tio);
    printf("cz.\n");        //powitanie :)
    while(1)
    {
        przycisk = 0; //na początku zerowanie
        ioctl(port, TIOCMGET, &status);
        if(status&TIOCM_CD) przycisk |= 1; //pin 1
        if(status&TIOCM_DSR) przycisk |= 2;//pin 6
        if(status&TIOCM_CTS) przycisk |= 4;//pin 8
        if(status&TIOCM_RI) przycisk |= 8; //pin 9
        if(przycisk != 0)   //jezeli cos jest wcisniete
        {
            przejscia++;    //to oznacz ze jest
            if((przycisk==4 || przycisk==5) && przejscia>=2)wykonaj(przycisk);//dla przycisku 4 i 5, czyli glosnosci polecenie uruchamiane co 2 przejscia, czyli co 200ms
            else if(przejscia>=5)wykonaj(przycisk);//dla reszty odstep to 500ms
        }
        else przejscia = 5; //jezeli nic nie jest wcisniete, przejscia ustawione na 5, nie trzeba trzymac zeby załapało, wystarczy nacisnac
        usleep(100000); //MEGALAME!
    }
}
