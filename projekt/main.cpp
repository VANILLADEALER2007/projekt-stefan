#include <iostream>
#include "main2.cpp"
using namespace std;

class Wpis{
	
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
			break;
//		case 2:
//			break;
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
