#include <iostream>

using namespace std;

class Wpis{
	public:
	int id;
	time_t data;
	double kwota;
	string kategoria;
	string notatka;
	enum Typ
	{
		Wydatek,
		Przychod
	};
};

int main() {
	int nr_operacji;
	string typ_operacji;
	cout << "Witamy w menedzerze budzetu domowego, prosimy wybrac operacje wpisujac cyfre od 1 do 8:" << endl;
	cout << "1. Dodaj   2. Lista   3. Filtruj   4. Sortuj   5. Statystyki   6. Zapis   7. Odczyt   8. Wyjscie" << endl;
	cin >> nr_operacji;
	switch (nr_operacji){
		case 1:
			cout << "Podaj typ operacji (wydatek/przychod): ";
			cin >> typ_operacji;
			cout << "Podaj kategorie operacji: ";
			if(typ_operacji == "wydatek")
			{
				Wpis nowy_wydatek;
				cout << "Podaj kategorie wydatku: ";
				cin >> nowy_wydatek.kategoria;
				cout << "Podaj kwote: ";
				cin >> nowy_wydatek.kwota;
				cout << "Podaj date (YYYY-MM-DD): ";
				cin >> nowy_wydatek.data;
				// Dodaj nowy_wydatek do odpowiedniej kolekcji/klasy z main2.cpp
			} 
			else if(typ_operacji == "przychod")
			{
				Wpis nowy_przychod;
				cout << "Podaj kategorie przychodu: ";
				cin >> nowy_przychod.kategoria;
				cout << "Podaj kwote: ";
				cin >> nowy_przychod.kwota;
				cout << "Podaj date (YYYY-MM-DD): ";
				cin >> nowy_przychod.data;
				// Dodaj nowy_przychod do odpowiedniej kolekcji/klasy z main2.cpp
			} else {
				cout << "Niepoprawny typ operacji." << endl;
			}
			break;
		case 2:
			break;
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
//	    case 8:
//	    	break;
	    default:
	    	cout << "Wybrano niewlasciwy numer" << endl;
	    	break;
	}
	return 0;
}
