#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "conio2.h"
using namespace std;
#define PRZ_WYM 50 //Przykladowy wymiar labiryntu
#define PRZ_DL 25  //Przykladowa dlugosc tablicy zawierajacej nazwe pliku

//Definicje wyswietlania elementow labiryntu za pomoca okreslonych znakow

#define PELNY_PIKSEL 219 // W kodzie ASCII pelny kwadrat ma nr 219
#define DROGA 255 //W kodzie ASCII pusty kwadrat ma nr 255
#define DRZWI_PION_ZAM 179 // |
#define DRZWI_PION_OTW 92 /* \ */
#define DRZWI_POZ_ZAM 45 // -
#define DRZWI_POZ_OTW 47 // /
#define IKONA_DOL 31 // V
#define IKONA_GORA 30 // ^
#define IKONA_PRAWO 16 // >
#define IKONA_LEWO 17 // <

//klawisze interakcji
#define PRZYKLADOWY_LABIRYNT 105 // i
#define WYJSCIE 113 // q
#define EDYTOR 101 // e
#define ODCZYT_LABIRYNTU 111 // o
#define POMOC 104 // h
#define RESTART 114 // r
#define INTERAKCJA_DRZWI 100 // d
#define ZAPISZ 115 // s
#define STRZALKA_GORA 72
#define STRZALKA_DOL 80
#define STRZALKA_PRAWO 77
#define STRZALKA_LEWO 75
#define ENTER 13
#define BACKSPACE 8
#define SPACJA 32



enum kierunki
{
    gora,
    prawo,
    dol,
    lewo,
};

struct wymiary_labiryntu
{
    int wysokosc;
    int szerokosc;
};

struct pozycja
{
    int x;
    int y;
    enum kierunki kierunek;
};

struct perspektywa
{
    int szerokosc;
    int wysokosc;
    int margines_lewy;
    int margines_gorny;
    int szerokosc_sciany;
};

char menu_glowne()
{
    const int x_menu=88;
    const int y_menu=2;
    char komenda;
    int poprawny=0;
    while(poprawny!=1)
    {
        clrscr();
        gotoxy(x_menu,y_menu);
        cout << "Witaj w grze Labirynt!\n";
        gotoxy(x_menu,y_menu+1);
        cout <<"\nNacisnij:\ni by wybrac domyslny labirynt\ne by otworzyc edytor\no by odczytac zapisany labirynt\nq by wyjsc z gry\n";
        cin >> komenda;
        if(komenda==(char)PRZYKLADOWY_LABIRYNT || komenda==(char)EDYTOR || komenda==(char)ODCZYT_LABIRYNTU || komenda==(char)WYJSCIE) poprawny=1;
        else
        {
            cout << "Wybrano litere z poza zakresu!\n";
            getch();
        }
    }
    return komenda;
}

//zmienna sprawdz przyjmuje wartosc 0 gdy nie wystapilo zadne pole 2 na 2, 1 gdy takowy znaleziono
//funkcja zwraca strukture z wspolrzednymi lewego gornego pola z pola 2 na 2
pozycja sprawdz_pola_2na2(char labirynt[][PRZ_WYM],wymiary_labiryntu wymiary, int* sprawdz )
{
    pozycja miejsce; //wspolrzedna lewego gornego pola z pola 2 na 2
    *sprawdz=0;
    for(int i=0; i<wymiary.wysokosc-1; i++)
    {
        for(int j=0; j<wymiary.szerokosc-2; j++)
        {
            if((labirynt[i][j]=='0' && labirynt[i][j+1]=='0' && labirynt[i+1][j]=='0' && labirynt[i+1][j+1]=='0') ||
               (labirynt[i][j]==(char)DROGA && labirynt[i][j+1]==(char)DROGA && labirynt[i+1][j]==(char)DROGA && labirynt[i+1][j+1]==(char)DROGA))
            {
                *sprawdz=1;
                miejsce.x=j;
                miejsce.y=i;
            }
        }
    }
    return miejsce;
}

void zamien_znaki(char labirynt[][PRZ_WYM],wymiary_labiryntu* wymiary)
{
    int i, j, losowa;
    for(i=0; i<wymiary->wysokosc; i++)
    {
         for(j=0; j<wymiary->szerokosc;j++)
         {
             if(labirynt[i][j]=='0') labirynt[i][j]=(char)DROGA;
             if(labirynt[i][j]=='#')
             {
                losowa=rand()%5;
                if(losowa==0) labirynt[i][j]='#';
                if(losowa==1) labirynt[i][j]='@';
                if(losowa==2) labirynt[i][j]='&';
                if(losowa==3) labirynt[i][j]='%';
                if(losowa==4) labirynt[i][j]='$';
             }
             if(labirynt[i][j]=='1')
             {
                 if(labirynt[i-1][j]!=(char)DROGA && labirynt[i+1][j]!=(char)DROGA) labirynt[i][j]=(char)DRZWI_PION_ZAM;
                 else labirynt[i][j]=(char)DRZWI_POZ_ZAM;
             }
         }
    }
}

//funkcja wczytuje dane z pliku do tablicy, ustala wymiary labiryntu, a takze jesli czy_podmienic=='T' wywoluje funkcje podmien
//zwraca 0 jesli nie wczytano pliku, 1 jesli wczytanie zakonczylo sie sukcesem
int wczytaj(char nazwa[], char labirynt[][PRZ_WYM],wymiary_labiryntu* wymiary, char czy_podmienic)
{
    FILE *plik;
    int i,j;
    plik=fopen(nazwa, "r");
    if (plik == NULL)
    {
        cout << "\nNie powiodlo sie wczytanie pliku.\nProsze sie upewnic czy wpisano prawidlowa nazwe i czy ten plik znajduje sie w folderze.";
        return 0;
    }
    i=0;
    j=0;
    while(i>=0) //Wczytuje pierwsza linijke pliku az nie natrafi na znak \n (ENTER). Ustala szerokosc labiryntu
    {
        i++;
        if(getc(plik)=='\n')
        {
            wymiary->szerokosc=i;
            i=-1;
        }
    }
    while(getc(plik)!=EOF) j++; //poniewaz pierwsza linijka zostala wczytana j = ilosc_wszystkich_znakow - ilosc_znakow_w_pierwszej_linii
    wymiary->wysokosc=(j+wymiary->szerokosc)/wymiary->szerokosc+1; //ilosc_znakow_w_pierwszej_linii = szerokosc_labiryntu
    fseek(plik,0,SEEK_SET);
    for(i=0; i<wymiary->wysokosc; i++)
    {
         for(j=0; j<wymiary->szerokosc;j++)
         {
             labirynt[i][j]=fgetc(plik);
         }
    }
    if(czy_podmienic=='T') zamien_znaki(labirynt, wymiary);
    fclose(plik);
    return 1;
}

//wyswietla labirynt (zamiast znaku F ozn. FINISH wypisuje pusty znak
void wyswietl(char tablica[][PRZ_WYM], int x, int y) //x-szerokosc labiryntu, y-wysokosc labiryntu
{
    clrscr();
    textcolor(2);
    for (int i=0; i<y; i++)
    {
        for(int j=0; j<x; j++)
        {
            if(tablica[i][j]=='F') cout << (char)DROGA;
            else cout << tablica[i][j];
        }
    }
    cout << endl;
    textcolor(15);
}

void wyswietl_edytor(char tablica[][PRZ_WYM], int x, int y) //x-szerokosc labiryntu, y-wysokosc labiryntu
{
    system("cls");
    textcolor(2);
    for(int i=0; i<y; i++)
    {
        for(int j=0; j<x; j++)
        {
            if(tablica[i][j]=='0') cout << (char)DROGA;
            else
            {
                if(tablica[i][j]=='1') cout << "X";
                else if(tablica[i][j]!='\n') cout << tablica[i][j];
            }
        }
        cout << endl;
    }
    cout << endl;
    textcolor(15);
}

//funkcja wyszukuje w labiryncie znaku S ozn. START, ustanawia tam znak postaci i nadaje poczatkowe wartosc strukturze pozycja
//zwraca pozycje stratowa gracza w strukturze pozycja
pozycja start(char tablica[][PRZ_WYM], int x, int y, pozycja pozycja)
{
    for (int i=0; i<y; i++)
    {
        for(int j=0; j<x; j++)
        {
            if(tablica[i][j]=='S')
            {
                if(tablica[i][j-1]==(char)DROGA)
                {
                    pozycja.kierunek=lewo;
                }
                if(tablica[i][j+1]==(char)DROGA)
                {
                    pozycja.kierunek=prawo;
                }
                if(tablica[i-1][j]==(char)DROGA)
                {
                    pozycja.kierunek=gora;
                }
                if(tablica[i+1][j]==(char)DROGA)
                {
                    pozycja.kierunek=dol;
                }
                pozycja.x=j;
                pozycja.y=i;
                j=x;
                i=y;
            }
        }
    }
    return pozycja;
}

//funkcja odpowiedzialna za ruch. Wykrywa kolizje, zmienie parametry struktury pozycja w zaleznosci od komendy
//zwraca zaktualizowana pozycje gracza w strukturze pozycja
pozycja wykonaj_ruch(pozycja pozycja, kierunki strzalka, char tablica[][PRZ_WYM])
{
    if(strzalka==gora)
    {
        if(pozycja.kierunek==dol)
        {
            if(tablica[pozycja.y+1][pozycja.x]==(char)DROGA || tablica[pozycja.y+1][pozycja.x]==(char)DRZWI_POZ_OTW || tablica[pozycja.y+1][pozycja.x]=='F') pozycja.y=pozycja.y+1;
        }
        if(pozycja.kierunek==gora)
        {
            if(tablica[pozycja.y-1][pozycja.x]==(char)DROGA || tablica[pozycja.y-1][pozycja.x]==(char)DRZWI_POZ_OTW || tablica[pozycja.y-1][pozycja.x]=='F') pozycja.y=pozycja.y-1;
        }
        if(pozycja.kierunek==prawo)
        {
            if(tablica[pozycja.y][pozycja.x+1]==(char)DROGA || tablica[pozycja.y][pozycja.x+1]==(char)DRZWI_PION_OTW || tablica[pozycja.y][pozycja.x+1]=='F') pozycja.x=pozycja.x+1;
        }
        if(pozycja.kierunek==lewo)
        {
            if(tablica[pozycja.y][pozycja.x-1]==(char)DROGA || tablica[pozycja.y][pozycja.x-1]==(char)DRZWI_PION_OTW || tablica[pozycja.y][pozycja.x-1]=='F') pozycja.x=pozycja.x-1;
        }
    }
    if(strzalka==dol)
    {

        if(pozycja.kierunek==dol)
        {
            if(tablica[pozycja.y-1][pozycja.x]==(char)DROGA || tablica[pozycja.y-1][pozycja.x]==(char)DRZWI_POZ_OTW || tablica[pozycja.y-1][pozycja.x]=='F') pozycja.y=pozycja.y-1;
        }
        if(pozycja.kierunek==gora)
        {
            if(tablica[pozycja.y+1][pozycja.x]==(char)DROGA || tablica[pozycja.y+1][pozycja.x]==(char)DRZWI_POZ_OTW || tablica[pozycja.y+1][pozycja.x]=='F') pozycja.y=pozycja.y+1;
        }
        if(pozycja.kierunek==prawo)
        {
            if(tablica[pozycja.y][pozycja.x-1]==(char)DROGA || tablica[pozycja.y][pozycja.x-1]==(char)DRZWI_PION_OTW || tablica[pozycja.y+1][pozycja.x]=='F') pozycja.x=pozycja.x-1;
        }
        if(pozycja.kierunek==lewo)
        {
            if(tablica[pozycja.y][pozycja.x+1]==(char)DROGA || tablica[pozycja.y][pozycja.x+1]==(char)DRZWI_PION_OTW || tablica[pozycja.y][pozycja.x+1]=='F') pozycja.x=pozycja.x+1;
        }
    }
    if(strzalka==lewo)
    {
        if(pozycja.kierunek==gora)
        {
            pozycja.kierunek=lewo;
            return pozycja;
        }
        if(pozycja.kierunek==dol)
        {
            pozycja.kierunek=prawo;
            return pozycja;
        }
        if(pozycja.kierunek==prawo)
        {
            pozycja.kierunek=gora;
            return pozycja;
        }
        if(pozycja.kierunek==lewo)
        {
            pozycja.kierunek=dol;
            return pozycja;
        }
    }
    if(strzalka==prawo)
    {
        if(pozycja.kierunek==gora)
        {
            pozycja.kierunek=prawo;
            return pozycja;
        }
        if(pozycja.kierunek==dol)
        {
            pozycja.kierunek=lewo;
            return pozycja;
        }
        if(pozycja.kierunek==prawo)
        {
            pozycja.kierunek=dol;
            return pozycja;
        }
        if(pozycja.kierunek==lewo)
        {
            pozycja.kierunek=gora;
            return pozycja;
        }
    }
    return pozycja;
}

//funkcja sprawdzajaca czy gracz nie znajduje sie na koncu labiryntu(oznaczonym jako znak F)
//zwraca 1 jesli gracz dotarl do konca, w przeciwnym razie zwraca 0
int sprawdz_wygrana(char tablica[][PRZ_WYM], pozycja pozycja, int czy_wygrano)
{
    if(tablica[pozycja.y][pozycja.x]=='F') czy_wygrano=1;
    else czy_wygrano=0;
    return czy_wygrano;
}

void FPP_sciana_lewo_pelna(char znak, int poziom, perspektywa perspektywa)
{
    for(int i=1; i<=perspektywa.szerokosc_sciany; i++)
    {
        gotoxy(perspektywa.margines_lewy+i+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny-1+poziom);
        putch('_');
        for(int j=0;j<perspektywa.wysokosc-((poziom+1)*2);j++)
        {
            gotoxy(perspektywa.margines_lewy+i+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny+poziom+j);
            putch(znak);
        }
        gotoxy(perspektywa.margines_lewy+i+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny-1+perspektywa.wysokosc-1-poziom);
        putch('_');
    }
    gotoxy(perspektywa.margines_lewy+(perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny-1+poziom+1);
    putch('\\');
    for(int j=0;j<(perspektywa.wysokosc-1)-((poziom+1)*2);j++)
    {
        gotoxy(perspektywa.margines_lewy+(perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny+1+poziom+j);
        putch('|');
    }
    gotoxy(perspektywa.margines_lewy+(perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny-1+perspektywa.wysokosc-1-poziom);
    putch('/');
}

void FPP_sciana_lewo_pusta(char znak, int poziom, perspektywa perspektywa)
{
    for(int i=1; i<=perspektywa.szerokosc_sciany; i++)
    {
        gotoxy(perspektywa.margines_lewy+i+(poziom*(perspektywa.szerokosc_sciany+1)),(perspektywa.margines_gorny-1)+1+poziom);
        putch('_');
        for(int j=0;j<(perspektywa.wysokosc-1)-((poziom+1)*2);j++)
        {
            gotoxy(perspektywa.margines_lewy+i+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny+1+poziom+j);
            putch(znak);
        }
        gotoxy(perspektywa.margines_lewy+i+(poziom*(perspektywa.szerokosc_sciany+1)),(perspektywa.margines_gorny-1)-1+perspektywa.wysokosc-1-poziom);
        putch('_');
    }
    for(int j=0;j<(perspektywa.wysokosc-1)-((poziom+1)*2);j++)
    {
        gotoxy(perspektywa.margines_lewy+(perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny+1+poziom+j);
        putch('|');
    }
}

void FPP_sciana_prawo_pelna(char znak, int poziom, perspektywa perspektywa)
{
    for(int i=1; i<=perspektywa.szerokosc_sciany; i++)
    {
        gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-(i+(poziom*(perspektywa.szerokosc_sciany+1))),perspektywa.margines_gorny-1+poziom);
        putch('_');
        for(int j=0;j<perspektywa.wysokosc-((poziom+1)*2);j++)
        {
            gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-(i+(poziom*(perspektywa.szerokosc_sciany+1))),perspektywa.margines_gorny+poziom+j);
            putch(znak);
        }
        gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-(i+(poziom*(perspektywa.szerokosc_sciany+1))),perspektywa.margines_gorny-1+perspektywa.wysokosc-1-poziom);
        putch('_');
    }
    gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-((perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1))),(perspektywa.margines_gorny-1)+1+poziom);
    putch('/');
    for(int j=0;j<(perspektywa.wysokosc-1)-((poziom+1)*2);j++)
    {
        gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-((perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1))),perspektywa.margines_gorny+1+poziom+j);
        putch('|');
    }
    gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-((perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1))),perspektywa.margines_gorny-1+perspektywa.wysokosc-1-poziom);
    putch('\\');
}

void FPP_sciana_prawo_pusta(char znak, int poziom, perspektywa perspektywa)
{
    for(int i=1; i<=perspektywa.szerokosc_sciany; i++)
    {
        gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-(i+(poziom*(perspektywa.szerokosc_sciany+1))),(perspektywa.margines_gorny-1)+1+poziom);
        putch('_');
        for(int j=0;j<(perspektywa.wysokosc-1)-((poziom+1)*2);j++)
        {
            gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-(i+(poziom*(perspektywa.szerokosc_sciany+1))),perspektywa.margines_gorny+1+poziom+j);
            putch(znak);
        }
        gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-(i+(poziom*(perspektywa.szerokosc_sciany+1))),(perspektywa.margines_gorny-1)-1+perspektywa.wysokosc-1-poziom);
        putch('_');
    }
    for(int j=0;j<(perspektywa.wysokosc-1)-((poziom+1)*2);j++)
    {
        gotoxy((perspektywa.margines_lewy+perspektywa.szerokosc-1)-((perspektywa.szerokosc_sciany+1)+(poziom*(perspektywa.szerokosc_sciany+1))),perspektywa.margines_gorny+1+poziom+j);
        putch('|');
    }
}

void FPP_sciana_srodek(char znak, int poziom, perspektywa perspektywa)
{
    int k, j;
    gotoxy(perspektywa.margines_lewy+1+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny+(poziom-1));
    for(k=0; k<(perspektywa.szerokosc-2)-2*(poziom*(perspektywa.szerokosc_sciany+1)); k++) putch('_');
    for(k=0; k<(perspektywa.wysokosc-1)-(2*poziom +1); k++)
    {
        for(j=0;j<(perspektywa.szerokosc-2)-2*(poziom*(perspektywa.szerokosc_sciany+1));j++)
        {
            gotoxy(perspektywa.margines_lewy+1+(poziom*(perspektywa.szerokosc_sciany+1))+j,perspektywa.margines_gorny+poziom+k);
            putch(znak);
        }
    }
    gotoxy(perspektywa.margines_lewy+1+(poziom*(perspektywa.szerokosc_sciany+1)),perspektywa.margines_gorny+perspektywa.wysokosc-1-(poziom+1));
    for(k=0; k<(perspektywa.szerokosc-2)-2*(poziom*(perspektywa.szerokosc_sciany+1)); k++) putch('_');
}

 //By zmodyfikowac parametry perspektywy nalezy zmienic ponizsze wartosci
void wymiary_perspektywy(perspektywa* perspektywa, int* zakres_rysowania)
{
    *zakres_rysowania=7;
    perspektywa->szerokosc_sciany=5;
    perspektywa->szerokosc=(((*zakres_rysowania)*(perspektywa->szerokosc_sciany+1)+1)+1)*2+26;
    perspektywa->wysokosc=((*zakres_rysowania)*2+1)+13;
    perspektywa->margines_lewy=PRZ_WYM+1;
    perspektywa->margines_gorny=4;
}

//funkcja odpowiedzialna za wywolywanie odpowiedniej ilosci razy wraz z odpowiednimi wartosciami argumentow funkcji odpowiedzialnych za rysowanie scian w FPP
void perspektywa_FPP(char tablica[][PRZ_WYM], pozycja pozycja, perspektywa perspektywa, int zakres_rysowania)
{
    char znak; //pod zmienna ta zapisywana jest znak danej sciany
    int i;
    for(i=0;i<perspektywa.wysokosc;i++) //czyszczenie perspektywy
    {
        for(int j=0;j<perspektywa.szerokosc;j++)
        {
            gotoxy(perspektywa.margines_lewy+j,perspektywa.margines_gorny+i-1);
            putch(' ');
        }
    }
    for(i=0;i<perspektywa.wysokosc-1;i++) //tworzenie skrajnych linii
    {
        gotoxy(perspektywa.margines_lewy,perspektywa.margines_gorny+i);
        putch('|');
        gotoxy(perspektywa.margines_lewy+perspektywa.szerokosc-1,perspektywa.margines_gorny+i);
        putch('|');
    }
    for(i=0;i<perspektywa.szerokosc-2; i++) //tworzenie linii horyzontalnej
    {
        gotoxy(perspektywa.margines_lewy+1+i,perspektywa.margines_gorny+perspektywa.wysokosc/2-1);
        putch('_');
    }
    i=0;
    if(pozycja.kierunek==gora)
    {
        while(i<zakres_rysowania && (tablica[pozycja.y-1-i][pozycja.x]==(char)DROGA || tablica[pozycja.y-1-i][pozycja.x]==(char)DRZWI_POZ_OTW))
        {
            //LEWA STRONA
            if(tablica[pozycja.y-1-i][pozycja.x-1]!=(char)DROGA && tablica[pozycja.y-1-i][pozycja.x-1]!=(char)DRZWI_PION_OTW && tablica[pozycja.y-1-i][pozycja.x-1]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y-1-i][pozycja.x-1];
                FPP_sciana_lewo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y-2-i][pozycja.x-1];
                FPP_sciana_lewo_pusta(znak,i,perspektywa);
            }
            //PRAWA STRONA
            if(tablica[pozycja.y-1-i][pozycja.x+1]!=(char)DROGA && tablica[pozycja.y-1-i][pozycja.x-1]!=(char)DRZWI_PION_OTW && tablica[pozycja.y-1-i][pozycja.x-1]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y-1-i][pozycja.x+1];
                FPP_sciana_prawo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y-2-i][pozycja.x+1];
                FPP_sciana_prawo_pusta(znak,i,perspektywa);
            }
            i++;
        }
        if(i<zakres_rysowania)
        {
            znak=tablica[pozycja.y-1-i][pozycja.x];
            FPP_sciana_srodek(znak,i,perspektywa);
        }
    }
    if(pozycja.kierunek==dol)
    {
        while(i<zakres_rysowania && (tablica[pozycja.y+1+i][pozycja.x]==(char)DROGA || tablica[pozycja.y+1+i][pozycja.x]==(char)DRZWI_POZ_OTW))
        {
            //LEWA STRONA
            if(tablica[pozycja.y+1+i][pozycja.x+1]!=(char)DROGA && tablica[pozycja.y+1+i][pozycja.x+1]!=(char)DRZWI_PION_OTW && tablica[pozycja.y+1+i][pozycja.x+1]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y+1+i][pozycja.x+1];
                FPP_sciana_lewo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y+2+i][pozycja.x+1];
                FPP_sciana_lewo_pusta(znak,i,perspektywa);
            }
            //PRAWA STRONA
            if(tablica[pozycja.y+1+i][pozycja.x-1]!=(char)DROGA && tablica[pozycja.y+1+i][pozycja.x-1]!=(char)DRZWI_PION_OTW && tablica[pozycja.y+1+i][pozycja.x-1]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y+1+i][pozycja.x-1];
                FPP_sciana_prawo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y+2+i][pozycja.x-1];
                FPP_sciana_prawo_pusta(znak,i,perspektywa);
            }
            i++;
        }
        if(i<zakres_rysowania)
        {
            znak=tablica[pozycja.y+1+i][pozycja.x];
            FPP_sciana_srodek(znak,i,perspektywa);
        }
    }
    if(pozycja.kierunek==lewo)
    {
        while(i<zakres_rysowania && (tablica[pozycja.y][pozycja.x-1-i]==(char)DROGA || tablica[pozycja.y][pozycja.x-1-i]==(char)DRZWI_PION_OTW))
        {
            //LEWA STRONA
            if(tablica[pozycja.y+1][pozycja.x-1-i]!=(char)DROGA && tablica[pozycja.y+1][pozycja.x-1-i]!=(char)DRZWI_PION_OTW && tablica[pozycja.y+1][pozycja.x-1-i]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y+1][pozycja.x-1-i];
                FPP_sciana_lewo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y+1][pozycja.x-2-i];
                FPP_sciana_lewo_pusta(znak,i,perspektywa);
            }
            //PRAWA STRONA
            if(tablica[pozycja.y-1][pozycja.x-1-i]!=(char)DROGA && tablica[pozycja.y-1][pozycja.x-1-i]!=(char)DRZWI_PION_OTW && tablica[pozycja.y-1][pozycja.x-1-i]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y-1][pozycja.x-1-i];
                FPP_sciana_prawo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y-1][pozycja.x-2-i];
                FPP_sciana_prawo_pusta(znak,i,perspektywa);
            }
            i++;
        }
        if(i<zakres_rysowania)
        {
            znak=tablica[pozycja.y][pozycja.x-1-i];
            FPP_sciana_srodek(znak,i,perspektywa);
        }
    }
    if(pozycja.kierunek==prawo)
    {
        while(i<zakres_rysowania && (tablica[pozycja.y][pozycja.x+1+i]==(char)DROGA || tablica[pozycja.y][pozycja.x+1+i]==(char)DRZWI_PION_OTW))
        {
            //LEWA STRONA
            if(tablica[pozycja.y-1][pozycja.x+1+i]!=(char)DROGA && tablica[pozycja.y-1][pozycja.x+1+i]!=(char)DRZWI_PION_OTW && tablica[pozycja.y-1][pozycja.x+1+i]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y-1][pozycja.x+1+i];
                FPP_sciana_lewo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y-1][pozycja.x+2+i];
                FPP_sciana_lewo_pusta(znak,i,perspektywa);
            }
            //PRAWA STRONA
            if(tablica[pozycja.y+1][pozycja.x+1+i]!=(char)DROGA && tablica[pozycja.y+1][pozycja.x+1+i]!=(char)DRZWI_PION_OTW && tablica[pozycja.y+1][pozycja.x+1+i]!=(char)DRZWI_POZ_OTW)
            {
                znak=tablica[pozycja.y+1][pozycja.x+1+i];
                FPP_sciana_prawo_pelna(znak,i,perspektywa);
            }
            else
            {
                znak=tablica[pozycja.y+1][pozycja.x+2+i];
                FPP_sciana_prawo_pusta(znak,i,perspektywa);
            }
            i++;
        }
        if(i<zakres_rysowania)
        {
            znak=tablica[pozycja.y][pozycja.x+1+i];
            FPP_sciana_srodek(znak,i,perspektywa);
        }
    }
}

void perperktywa_drzwi_otwarcie(char tablica[][PRZ_WYM], pozycja pozycja, wymiary_labiryntu wymiary, perspektywa perspektywa, int zakres_rysowania)
{
    textcolor(15);
    for(int i=0;i<=perspektywa.wysokosc;i++)
    {
        perspektywa_FPP(tablica,pozycja, perspektywa, zakres_rysowania);
        for(int j=0;j<perspektywa.wysokosc-i-1;j++)
        {
            for(int k=0;k<perspektywa.szerokosc-2;k++)
            {
                gotoxy(perspektywa.margines_lewy+k+1,perspektywa.margines_gorny+j+i);
                if(pozycja.kierunek==prawo || pozycja.kierunek==lewo) putch(DRZWI_PION_ZAM);
                else putch(DRZWI_POZ_ZAM);
            }
        }
    }
}

void perperktywa_drzwi_zamkniecie(char tablica[][PRZ_WYM], pozycja pozycja, wymiary_labiryntu wymiary, perspektywa perspektywa, int zakres_rysowania)
{
    textcolor(15);
    for(int i=0; i<=perspektywa.wysokosc-2; i++)
    {
        for(int k=0; k<=i; k++) //dodatkowa petla ktora opoznia animacje zamykania drzwi ( by usunac te opoznienie nale¿y usun¹c ta petle (bez zawartosci) i zmianic w wenterznej petli w gotoxy 'k' na 'i'
        {
            for(int j=0; j<perspektywa.szerokosc-2; j++)
            {
                gotoxy(perspektywa.margines_lewy+1+j,perspektywa.margines_gorny+perspektywa.wysokosc-k-2);
                if(pozycja.kierunek==prawo || pozycja.kierunek==lewo) putch(DRZWI_PION_ZAM);
                else putch((char)DRZWI_POZ_ZAM);
            }
        }
    }
    for(int k=0; k<perspektywa.szerokosc-2; k++)
    {
        gotoxy(perspektywa.margines_lewy+1+k,perspektywa.margines_gorny-1);
        putch('_');
    }
    for(int k=0; k<perspektywa.szerokosc-2; k++)
    {
        gotoxy(perspektywa.margines_lewy+1+k,perspektywa.margines_gorny+perspektywa.wysokosc-2);
        putch('_');
    }
}

void drzwi_interakcja(char tablica[][PRZ_WYM], pozycja pozycja, wymiary_labiryntu wymiary, perspektywa perspektywa, int zakres_rysowania)
{
    textcolor(2);
    if(pozycja.kierunek==lewo && tablica[pozycja.y][pozycja.x-1]==(char)DRZWI_PION_ZAM)
    {
        tablica[pozycja.y][pozycja.x-1]=(char)DRZWI_PION_OTW;
        gotoxy(pozycja.x,pozycja.y+1);
        putch(DRZWI_PION_OTW);
        perperktywa_drzwi_otwarcie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
    }
    else
    {
        if(pozycja.kierunek==lewo && tablica[pozycja.y][pozycja.x-1]==(char)DRZWI_PION_OTW)
        {
            tablica[pozycja.y][pozycja.x-1]=(char)DRZWI_PION_ZAM;
            gotoxy(pozycja.x,pozycja.y+1);
            putch(DRZWI_PION_ZAM);
            perperktywa_drzwi_zamkniecie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
        }
    }
    if(pozycja.kierunek==prawo && tablica[pozycja.y][pozycja.x+1]==(char)DRZWI_PION_ZAM)
    {
        tablica[pozycja.y][pozycja.x+1]=(char)DRZWI_PION_OTW;
        gotoxy(pozycja.x+2,pozycja.y+1);
        putch(DRZWI_PION_OTW);
        perperktywa_drzwi_otwarcie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
    }
    else
    {
        if(pozycja.kierunek==prawo && tablica[pozycja.y][pozycja.x+1]==(char)DRZWI_PION_OTW)
        {
            tablica[pozycja.y][pozycja.x+1]=(char)DRZWI_PION_ZAM;
            gotoxy(pozycja.x+2,pozycja.y+1);
            putch(DRZWI_PION_ZAM);
            perperktywa_drzwi_zamkniecie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
        }
    }
    if(pozycja.kierunek==dol && tablica[pozycja.y+1][pozycja.x]==(char)DRZWI_POZ_ZAM)
    {
        tablica[pozycja.y+1][pozycja.x]=(char)DRZWI_POZ_OTW;
        gotoxy(pozycja.x+1,pozycja.y+2);
        putch(DRZWI_POZ_OTW);
        perperktywa_drzwi_otwarcie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
    }
    else
    {
        if(pozycja.kierunek==dol && tablica[pozycja.y+1][pozycja.x]==(char)DRZWI_POZ_OTW)
        {
            tablica[pozycja.y+1][pozycja.x]=(char)DRZWI_POZ_ZAM;
            gotoxy(pozycja.x+1,pozycja.y+2);
            putch(DRZWI_POZ_ZAM);
            perperktywa_drzwi_zamkniecie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
        }
    }
    if(pozycja.kierunek==gora && tablica[pozycja.y-1][pozycja.x]==(char)DRZWI_POZ_ZAM)
    {
        tablica[pozycja.y-1][pozycja.x]=(char)DRZWI_POZ_OTW;
        gotoxy(pozycja.x+1,pozycja.y);
        putch(DRZWI_POZ_OTW);
        perperktywa_drzwi_otwarcie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
    }
    else
    {
        if(pozycja.kierunek==gora && tablica[pozycja.y-1][pozycja.x]==(char)DRZWI_POZ_OTW)
        {
            tablica[pozycja.y-1][pozycja.x]=(char)DRZWI_POZ_ZAM;
            gotoxy(pozycja.x+1,pozycja.y);
            putch(DRZWI_POZ_ZAM);
            perperktywa_drzwi_zamkniecie(tablica,pozycja,wymiary,perspektywa,zakres_rysowania);
        }
    }
}

int odleglosc(pozycja pozycja,char labirynt[][PRZ_WYM], wymiary_labiryntu wymiary)
{
    struct pozycja pozycja_konca;
    for(int i=0; i<wymiary.wysokosc; i++)
    {
        for(int j=0; j<wymiary.szerokosc; j++)
        {
            if(labirynt[i][j]=='F')
            {
                pozycja_konca.x=j;
                pozycja_konca.y=i;
                i=wymiary.wysokosc;
                j=wymiary.szerokosc;
            }
        }
    }
    return (pozycja_konca.y-pozycja.y)+(pozycja_konca.x-pozycja.x)-1;
}

void gra(char labirynt[][PRZ_WYM], char nazwa[])
{
    int win=0; // w momencie kiedy gracz dotrze do konca zmienia wartosc na 1;
    int spr;  // sluzy najpierw do sprawdzenia czy labirynt zostal wczytany, a nastepnie czy w wczytanym labiryncie znajduje sie puste pole 2x2;
    int attr=15; // kolor czcionki na 15(bialy)
    time_t czas_pocz, czas_pom, czas_obecny; //czas_pom bedzie zmienna pomocnicza, ktora posluzy przy wyswietlaniu synchronicznego czasu
    int roznica_czasu, roznica_czasu_minuty, roznica_czasu_sekundy, ilosc_ruchow;
    ilosc_ruchow=0;
    int zakres_rysowania;
    perspektywa perspektywa;
    wymiary_perspektywy(&perspektywa, &zakres_rysowania);
    unsigned char ruch;
    struct wymiary_labiryntu wymiary;
    struct pozycja pozycja;
    struct pozycja poprzednia_pozycja; //struktura ta posluzy przy decyzji czy wywietlic perspektywe, czy tez jest to niekonieczne; posluzy tez przy zliczaniu wykonanych ruchow
    spr=wczytaj(nazwa,labirynt,&wymiary,'T');
    if(spr==0)
    {
        getch();
        return;
    }
    sprawdz_pola_2na2(labirynt,wymiary,&spr);
    if(spr==1)
    {
        gotoxy(1,12);
        cout << "Wykryto miejsce 2x2. Czy kontynuowac? t-tak, n-nie ";
        cin >> ruch;
        if(ruch!='t')
        {
            system("cls");
            return;
        }
    }
    time(&czas_pocz);
    czas_pom=czas_pocz;
    pozycja=start(labirynt, wymiary.szerokosc, wymiary.wysokosc, pozycja);
    gotoxy(1,1);
    wyswietl(labirynt, wymiary.szerokosc, wymiary.wysokosc);
    while(win==0)
    {
        gotoxy(pozycja.x+1, pozycja.y+1);
		textcolor(attr);
        if(pozycja.kierunek==lewo) //ustalanie jaka ikone gracza postawic w zaleznosci od kierunku patrzenia
        {
            putch(IKONA_LEWO);
        }
        if(pozycja.kierunek==prawo)
        {
            putch(IKONA_PRAWO);
        }
        if(pozycja.kierunek==dol)
        {
            putch(IKONA_DOL);
        }
        if(pozycja.kierunek==gora)
        {
            putch(IKONA_GORA);
        }
        if(poprzednia_pozycja.x!=pozycja.x || poprzednia_pozycja.y!=pozycja.y || poprzednia_pozycja.kierunek!=pozycja.kierunek) perspektywa_FPP(labirynt,pozycja,perspektywa,zakres_rysowania);//if ten zapobiega wyswietlaniu nowej perspektywy identycznej do poprzedniej
        poprzednia_pozycja.x=pozycja.x;
        poprzednia_pozycja.y=pozycja.y;
        poprzednia_pozycja.kierunek=pozycja.kierunek;
        ruch='\0';
        while(ruch=='\0')
        {
            time(&czas_obecny);
            roznica_czasu=difftime(czas_obecny,czas_pocz);
            roznica_czasu_sekundy=roznica_czasu%60;
            roznica_czasu=roznica_czasu-roznica_czasu_sekundy;
            roznica_czasu_minuty=roznica_czasu/60;
            gotoxy(1,wymiary.wysokosc+3);
            textcolor(15);
            if(czas_obecny>=czas_pom+1)
            {
                printf("\rCzas gry %3i:%02i",roznica_czasu_minuty,roznica_czasu_sekundy );
                czas_pom=czas_obecny;
            }
            if(kbhit()) ruch = getch(); //jesli nie zostanie wcisniety zaden klawisz, petla nie zatrzyma sie w tym miejscu
		}
		gotoxy(pozycja.x+1,pozycja.y+1);
		textcolor(2);
		if(labirynt[pozycja.y][pozycja.x]==(char)DROGA) putch(DROGA);
		else if(labirynt[pozycja.y][pozycja.x]==(char)DRZWI_PION_OTW) putch(DRZWI_PION_OTW);
             else if(labirynt[pozycja.y][pozycja.x]==(char)DRZWI_POZ_OTW) putch(DRZWI_POZ_OTW);
        if(ruch == ' ') attr = (attr + 1) % 16;
        if(ruch==STRZALKA_LEWO)
        {
            pozycja=wykonaj_ruch(pozycja,lewo,labirynt);
        }
        if(ruch==STRZALKA_PRAWO)
        {
            pozycja=wykonaj_ruch(pozycja,prawo,labirynt);
        }
        if(ruch==STRZALKA_GORA)
        {
            pozycja=wykonaj_ruch(pozycja,gora,labirynt);
            if(poprzednia_pozycja.x!=pozycja.x || poprzednia_pozycja.y!=pozycja.y) ilosc_ruchow++;
        }
        if(ruch==STRZALKA_DOL)
        {
            pozycja=wykonaj_ruch(pozycja,dol,labirynt);
            if(poprzednia_pozycja.x!=pozycja.x || poprzednia_pozycja.y!=pozycja.y) ilosc_ruchow++;
        }
        if(ruch==INTERAKCJA_DRZWI)
        {
            drzwi_interakcja(labirynt, pozycja, wymiary,perspektywa,zakres_rysowania);
        }
        if(ruch==(char)RESTART)
        {
            gotoxy(1,1);
            wczytaj(nazwa,labirynt,&wymiary,'T');
            pozycja=start(labirynt, wymiary.szerokosc, wymiary.wysokosc, pozycja);
            wyswietl(labirynt, wymiary.szerokosc, wymiary.wysokosc);
            ilosc_ruchow=0;
            time(&czas_pocz);
        }
        if(ruch==(char)POMOC)
        {
            gotoxy(1,wymiary.wysokosc+4);
            textcolor(12);
            cout << "\nStrzalka w gore - ruch do przodu";
            cout << "\nStrzalka w tyl - ruch do tylu";
            cout << "\nStrzalka w prawo/lewo - obrot o 90 stopni";
            cout << "\nSpacja - zmiana koloru";
            cout << "\nd - otworzenie/zamkniecie drzwi";
            cout << "\nr - restart labiryntu";
            cout << "\nq - wyjscie do menu glownego\n";
        }
        if(ruch==(char)WYJSCIE)
        {
            win=1;
            textcolor(15);
            system("cls");
            continue;
        }
        textcolor(15);
        gotoxy(1,wymiary.wysokosc+2);
        cout << "Wykonane ruchy: "<<ilosc_ruchow;
        gotoxy(1,wymiary.wysokosc+4);
        cout <<"h - pomoc\n";
        cout << "Odleglosc od konca: " << odleglosc(pozycja, labirynt, wymiary);
        win=sprawdz_wygrana(labirynt,pozycja,win);
        if(win==1)
        {
            gotoxy(1,wymiary.wysokosc+11);
            cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\nGratulacje! dotarles do wyjscia z labiryntu! =D\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
            getch();
            clrscr();
        }
    }
}

//pobiera nazwe pliku i tworzy dopasowana do nazwy tablice typu char
//zwraca wskaznik na tablice z nazwa pliku
char* podaj_nazwe()
{
    int i, j;
    char pomocnicza[PRZ_DL];
    char pom;
    cout << "\nPodaj nazwe pliku i zatwierdz nazwe Enterem: ";
    for(i=0; i<PRZ_DL; i++)
    {
        pom=getche();
        if(pom==(char)ENTER)
        {
            pomocnicza[i]='\0';
            pomocnicza[i+1]=(char)ENTER;
            i=PRZ_DL;
        }
        else
        {
            if(pom==(char)BACKSPACE && i-1>=0)
            {
                i=i-2;
            }
            else pomocnicza[i]=pom;
        }
    }
    i=0;
    while(pomocnicza[i]!=(char)ENTER)
    {
        i++;
    }
    char* nazwa;
    nazwa=(char*)malloc(i*sizeof(char));
    for(j=0; j<=i; j++)
    {
        nazwa[j]=pomocnicza[j];
    }
    return nazwa;
}

//oczyszcza tablice nadajac komorkom wartosc odpowiadajaca pustej przestrzeni
void oczysc_tablice(char tablica[][PRZ_WYM],wymiary_labiryntu wymiary,wymiary_labiryntu wymiary_nowe)
{
    if(wymiary_nowe.szerokosc>wymiary.szerokosc)
    {
        for(int i=0; i<wymiary.wysokosc; i++)
        {
            for(int j=wymiary.szerokosc-1; j<wymiary_nowe.szerokosc; j++)
            {
                tablica[i][j]='0';
            }
        }
    }
    if(wymiary_nowe.wysokosc>wymiary.wysokosc)
    {
       for(int i=wymiary.wysokosc-1; i<wymiary_nowe.wysokosc; i++)
        {
            for(int j=0; j<wymiary_nowe.szerokosc; j++)
            {
                tablica[i][j]='0';
            }
        }
    }
}

//tworzy ramke z zaleznosci od wymiarow podanych przez uzytkownika, a takze wyswietla legende
void rysuj_ramke(wymiary_labiryntu wymiary)
{
    int i;
    for(i=0; i<wymiary.szerokosc+1; i++)
    {
        gotoxy(1+i,wymiary.wysokosc+1);
        cout<<"*";
    }
    for(i=0; i<wymiary.wysokosc; i++)
    {
        gotoxy(wymiary.szerokosc+1,1+i);
        cout<<"*";
    }
    gotoxy(PRZ_WYM+3,1);
    cout << "LEGENDA:";
    gotoxy(PRZ_WYM+3,2);
    cout << "q - wyjscie z edytora";
    gotoxy(PRZ_WYM+3,3);
    cout << "s - zapisanie labiryntu";
    gotoxy(PRZ_WYM+3,4);
    cout << "Strzalki - przesuniecie kursora w odpowiednia strone";
    gotoxy(PRZ_WYM+3,5);
    cout << "3 - postawienie sciany";
    gotoxy(PRZ_WYM+3,6);
    cout << "p - start";
    gotoxy(PRZ_WYM+3,7);
    cout << "k - finish";
    gotoxy(PRZ_WYM+3,8);
    cout << "Spacja - usuniecie elementu";
    gotoxy(PRZ_WYM+3,9);
    cout << "d - postawienie drzwi";
}

void kreator_labiryntu(char tablica[][PRZ_WYM])
{
    int spr, spr2na2; //zmiena spr posluzy do zapamietania czy wczytano zadany plik, spr2na2 posluzy do sprawdzenia czy istnieja wolna przestrzen 2 na 2
    char komenda='t';
    struct wymiary_labiryntu wymiary;
    wymiary.szerokosc=0;
    wymiary.wysokosc=0;
    struct wymiary_labiryntu wymiary_nowe;
    struct pozycja miejsce2na2;
    system("cls");
    //clrscr(); //??? powoduje zanik echa cin
    char* nazwa;
    while(komenda=='t')
    {
        nazwa=podaj_nazwe();
        spr=wczytaj(nazwa,tablica,&wymiary,'N');//funkcja zwraca 0 jesli nie wczyta pliku, 1 jesli wczyta pliku
        if(spr==1) komenda='n';
        else
        {
            cout << "Chcesz wpisac nazwe pliku raz jeszcze? t-tak n-nie: ";
            cin >> komenda;
        }
    }
    if(spr==1)cout << "\nOrginalny labirynt ma wymiary: " << wymiary.szerokosc-1 << "x" << wymiary.wysokosc-1;
    cout << "\nPodaj zadana szeokosc labiryntu: ";
    cin >> wymiary_nowe.szerokosc;
    cout << "\nPodaj zadana wysokosc labiryntu: ";
    cin >> wymiary_nowe.wysokosc;
    if(spr==1) wyswietl_edytor(tablica, wymiary_nowe.szerokosc, wymiary_nowe.wysokosc);
    else clrscr();
    oczysc_tablice(tablica,wymiary,wymiary_nowe);
    int kursor_x=1;
    int kursor_y=1;
    rysuj_ramke(wymiary_nowe);
    while(komenda!='q')
    {
        gotoxy(kursor_x,kursor_y);
        komenda=getch();
        if(komenda==STRZALKA_DOL) if(kursor_y < wymiary_nowe.wysokosc) kursor_y++;
        if(komenda==STRZALKA_GORA) if(kursor_y > 1) kursor_y--;
        if(komenda==STRZALKA_PRAWO) if(kursor_x<wymiary_nowe.szerokosc) kursor_x++;
        if(komenda==STRZALKA_LEWO) if(kursor_x > 1) kursor_x--;
        if(komenda=='3')
        {
            putch('#');
            tablica[kursor_y-1][kursor_x-1]='#';
        }
        if(komenda==(char)SPACJA)
        {
            putch(DROGA);
            tablica[kursor_y-1][kursor_x-1]='0';
        }
        if(komenda==(char)INTERAKCJA_DRZWI)
        {
            putch('X');
            tablica[kursor_y-1][kursor_x-1]='1';
        }
        if(komenda=='p')
        {
            putch('S');
            tablica[kursor_y-1][kursor_x-1]='S';
        }
        if(komenda=='k')
        {
            putch('F');
            tablica[kursor_y-1][kursor_x-1]='F';
        }
        if(komenda==(char)ZAPISZ)
        {
            miejsce2na2=sprawdz_pola_2na2(tablica,wymiary_nowe,&spr2na2);
            if(spr2na2==1)
            {
                gotoxy(PRZ_WYM+2,11);
                cout << "Wykryto miejsce 2x2.";
                textcolor(12); //Pusty obszar 2 na 2 zostanie zaznaczony na czerwono
                gotoxy(miejsce2na2.x+1,miejsce2na2.y+1);
                putch(PELNY_PIKSEL);
                gotoxy(miejsce2na2.x+2,miejsce2na2.y+1);
                putch(PELNY_PIKSEL);
                gotoxy(miejsce2na2.x+1,miejsce2na2.y+2);
                putch(PELNY_PIKSEL);
                gotoxy(miejsce2na2.x+2,miejsce2na2.y+2);
                putch(PELNY_PIKSEL);
                textcolor(15);
                continue;
            }
            if(spr==1)
            {
                gotoxy(PRZ_WYM+2,13);
                cout << "Czy chcesz nadpisac edytowany plik? t-tak n-nie: ";
                cin >> komenda;
                if(komenda!='t')
                {
                    gotoxy(PRZ_WYM+2,14);
                    nazwa=podaj_nazwe();
                }
            }
            FILE *plik;
            plik=fopen(nazwa,"w");
            for(int i=0; i<wymiary_nowe.wysokosc; i++)
            {
                for(int j=0; j<wymiary_nowe.szerokosc; j++)
                {
                     fputc(tablica[i][j],plik);
                }
                fputc('\n',plik);
            }
            fclose(plik);
            spr=1;
        }
    }
    if(komenda=='q')
    {
        clrscr();
    }
    gra(tablica,nazwa);
}

int main()
{
    settitle("Piotr Zakowski 160320");
    textcolor(15);
    char komenda;
    while(komenda!=(char)WYJSCIE)
    {
        komenda=menu_glowne();
        if(komenda==(char)WYJSCIE)
        {
            cout << "Dziekujemy za gre.";
            continue;
        }
        char labirynt[PRZ_WYM][PRZ_WYM]={};
        if(komenda==(char)PRZYKLADOWY_LABIRYNT)
        {
            char nazwa_przyklad[]="przyklad.txt";
            gra(labirynt, nazwa_przyklad);
        }
        if(komenda==(char)ODCZYT_LABIRYNTU)
        {

            char* wskaznik_na_nazwe;
            wskaznik_na_nazwe=podaj_nazwe();
            gra(labirynt, wskaznik_na_nazwe);
        }
        if(komenda==(char)EDYTOR)
        {
            kreator_labiryntu(labirynt);
        }
    }
}
