#include <iostream>
#include <ctime>
#include <string>
#include <list>

using namespace std;

class Wpis{
	private:
	int id;
	time_t data;
	double wartosc;
	string typ, kategoria, notatka;
	public:
	//konstruktor
	Wpis(string typ, double wartosc, string kategoria, string notatka){
		//do zrobienia - przypisanie id
		//do zrobienia - ustawienie czasu utworzenia wpisu na aktualny
		this -> typ = typ;
		this -> wartosc = wartosc;
		this -> kategoria = kategoria;
		this -> notatka = notatka;
	}
	void wyswietl(){
		cout << "id: ";//id
		cout << " czas: ";//czas
		cout << " typ: " << typ;
		cout << " wartosc: " << wartosc;
		cout << " kategoria: " << kategoria;
		cout << " notatka: " << notatka;
	}
};

list<Wpis> lista_wpisow;

int main() {
	//inicjalizacja zmiennnych ktore beda przekazywane do klasy z programu glownego
	string typ_operacji, kategoria_operacji, notatka_operacji;
	double wartosc_operacji;
	bool dzialanie_programu = true;

	//inne zmienne
	int nr_operacji;

	while (true){
		cout << "Witamy w menedzerze budzetu domowego, prosimy wybrac operacje wpisujac cyfre od 1 do 8:" << endl;
	    cout << "1. Dodaj   2. Lista   3. Filtruj   4. Sortuj   5. Statystyki   6. Zapis   7. Odczyt   8. Wyjscie" << endl;
	    cin >> nr_operacji;
	switch (nr_operacji){
		case 1:
		{
			//typ operacji
			while(dzialanie_programu){
				cout << "Podaj typ operacji (przychod/wydatek/anuluj): ";
			    cin >> typ_operacji;
				if (typ_operacji == "przychod" || typ_operacji == "wydatek" ) break;
				else if (typ_operacji == "anuluj") break;
				else cout << "Niepoprawny typ operacji" << endl;
			}

		    if(typ_operacji != "anuluj"){
				
		//kwota operacji
			cout << "Podaj wartosc (kwote) operacji: ";
			cin >> wartosc_operacji;

		//kategoria operacji
			cout << "Podaj kategorie operacji: ";
			//kat operacji - odpowiedzialnosc po stronie uzytkownika (w razie literowek uzytkownik nie stosuje 6. Zapis w celu zapisania do pliku)
			cin >> kategoria_operacji;

		//notatka do operacji
		    cout << "Podaj notatke do operacji: ";
			cin >> notatka_operacji;

			//dodawanie wpisu do klasy
			Wpis wpis(typ_operacji, wartosc_operacji, kategoria_operacji, notatka_operacji);

			//dodawanie wpisu (tu: obiektu klasy) do listy wpisow
			lista_wpisow.push_front(wpis);
			break;
			}
		}
		case 2:
		{
			for (Wpis wpis : lista_wpisow){
				wpis.wyswietl();
			}
		}
//	    case 3:
//	    	break;
//	    case 4:
//	    	break;
//	    case 5:
//	    	break;
//	    case 6:
//	    	break;
//	    case 7:
//	    	break;
	    case 8:
		{
			dzialanie_programu = false;
	    	break;
		}
	    default:
	    	cout << "Wybrano niewlasciwy numer" << endl;
	    	break;
	}
	}
	return 0;
}
