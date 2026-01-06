#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sys/stat.h>
#include <sstream>
#include <algorithm>

using namespace std;

// klasa DataTime tworzy obiekty, które przechowują tą samą datę w dwóch formatach
// strTime - czytelna dla człowieka
// tTime - reprezentacja czasu maszynowego
class DataTime
{
private:
	string strTime;
	time_t tTime;

public:
    // konstruktor tworzący obiekt o aktualnej dacie
	DataTime(bool CurrentTime)
	{
		if (CurrentTime)
		{
			tTime = time(nullptr);
			tm* czas = localtime(&tTime);
			
			char buffer[20];
			strftime(buffer, sizeof(buffer), "%Y-%m-%d", czas);

			strTime = buffer;
		}
		else
		{
			tTime = time(0);
			DataTime a = DataTime(tTime);
			strTime = a.GetString();
		}
	}

	// konstruktor tworzący obiekt o dacie podanej w zmiennej data
	// ten konstruktor stosujemy przy odczycie z pliku CSV
	DataTime(time_t data)
	{
		tTime = data;
		char buffer[20];
		tm* czas = localtime(&data);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d", czas);
		strTime = string(buffer);
	}

	// konstruktor tworzący obiekt o dacie podanej w zmiennej data
	// ten konstruktor stosujemy przy odczycie z pliku CSV
	DataTime(string timeString)
	{
		strTime = timeString;
		tm tim = {};
		tim.tm_isdst = -1;

		istringstream iss(timeString);
		iss >> get_time(&tim, "%Y-%m-%d");

		if (iss.fail())
		{
			cout << "Blad w trakcie proby odczytania daty!" << endl;

			tTime = time(nullptr);
			tm* czas = localtime(&tTime);
			
			char buffer[20];
			strftime(buffer, sizeof(buffer), "%Y-%m-%d", czas);

			strTime = buffer;
		}
		else
		{
			tTime = mktime(&tim);
		}
	}

	// gettery
	string GetString() const
	{
		return strTime;
	}

	time_t GetTime_T() const
	{
		return tTime;
	}
};

// klasa wpis
class Wpis
{
public:
	// te wartości będzie posiadać wpis
	unsigned int id;
	DataTime data = DataTime(true);
	double wartosc;
	string typ, kategoria, notatka;

	// konstruktor - tworzenie nowego wpisu o aktualnej dacie
	Wpis(int id, string typ, double wartosc, string kategoria, string notatka)
	{
		this->id = id;
		this->typ = typ;
		this->wartosc = wartosc;
		this->kategoria = kategoria;
		this->notatka = notatka;
	}

	// konstruktor do odczytu z pliku (z danymi: id, data, typ, wartosc, kategoria, notatka)
	Wpis(unsigned int id, DataTime data, string typ, double wartosc, string kategoria, string notatka)
	{
		this->id = id;
		this->data = data;
		this->typ = typ;
		this->wartosc = wartosc;
		this->kategoria = kategoria;
		this->notatka = notatka;
	}

	// wyświetlenie wpisu
	void wyswietl()
	{
		cout << "ID: " << id;
		cout << "   Czas: " << data.GetString();
		cout << "   Typ: " << typ;
		cout << "   Wartosc: " << wartosc;
		cout << "   Kategoria: " << kategoria;
		cout << "   Notatka: " << notatka << endl;
	}

	// zapis wpisu do pliku CSV
	void zapiszDoCSV(ofstream &plik)
	{
		plik << id << ","
			 << data.GetString() << ","
			 << typ << ","
			 << wartosc << ","
			 << kategoria << ","
			 << notatka << endl;
	}
};

// wektor ktory przechowuje wszystkie wpisy utworzone podczas aktualnej sesji
vector<Wpis> lista_wpisow;
// inicjalizacja zmiennej last_id (globalna) - zmieniamy jej wartość w funkcji Wczytaj() w klasie UserMenu
int last_id;

class UserMenu
{
	// funkcja GetDouble() sprawdza czy podaliśmy wartość, którą możemy przypisać do zmiennej typy double.
	// jeśli nie da się tego zrobić, funkcja nie dopuszcza do dalszego działania programu, co mogłoby spowodować błąd
	static double GetDouble()
	{
		string x;
		while(true)
		{
			getline(cin, x);
			try{
				return stod(x); // stod() - konwersja string na double
			} catch (...) {
				cout << "Nieprawidlowa liczba! Wprowadz ponownie: ";
			}
		}
	}

	// poprawne pobieranie kategorii
	static string GetCategory()
    {
        string kategoria;
        while (true)
        {
            getline(cin, kategoria);
            if (kategoria.find(',') != string::npos)
                cout << "Nie mozesz uzyc znaku ',' w kategorii, wprowadz ponownie: ";
            else if (kategoria.empty())
                cout << "Kategoria nie moze byc pusta, wprowadz ponownie: ";
            else
                return kategoria;
        }
    }

    // dodawanie wpisu [FUNKCJONALNOŚĆ NR 1]
	static void DodawanieWpisu()
	{
		string typ_operacji, kategoria_operacji, notatka_operacji;
		double wartosc_operacji;

		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		// weryfikacja poprawnosci wpisanego typu operacji
		while (true)
		{
			cout << "Podaj typ operacji (przychod/wydatek/anuluj): ";
			getline(cin, typ_operacji);
			if (typ_operacji == "przychod" || typ_operacji == "wydatek" || typ_operacji == "anuluj") break;
			else cout << "Niepoprawny typ operacji" << endl;
		}

		// jesli nie anulowano
		if (typ_operacji != "anuluj")
		{
			// kwota operacji
			cout << "Podaj wartosc (kwote) operacji: ";

			wartosc_operacji = GetDouble();

			//aby nie bylo ujemnych wartosci
			if (wartosc_operacji < 0){
				wartosc_operacji *= -1;
				cout << "Wprowadzono wartosc mniejsza od 0. Zmienilismy ja na liczbe przeciwna" << endl;
			}

			// kategoria operacji
			cout << "Podaj kategorie operacji: ";
			// kat operacji - odpowiedzialnosc po stronie uzytkownika (w razie literowek uzytkownik nie stosuje 6. Zapis w celu zapisania do pliku)

			kategoria_operacji = GetCategory();

			// notatka do operacji
			cout << "Podaj notatke do operacji: ";
			getline(cin, notatka_operacji);
			// dodawanie wpisu do klasy
			last_id++;
			Wpis wpis(last_id, typ_operacji, wartosc_operacji, kategoria_operacji, notatka_operacji);
			// dodawanie wpisu (tu: obiektu klasy) do listy wpisow
			lista_wpisow.push_back(wpis);
		}
	}

	// wyświetlenie listy wpisów [FUNKCJONALNOŚĆ NR 2]
	static void Wyswietl()
	{
		if (!lista_wpisow.empty())
		{
			for (auto& wpis : lista_wpisow)
			{
				wpis.wyswietl();
			}
		}
		else
		{
			cout << "Lista jest pusta! Wczytaj wczesniej utworzona liste za pomoca opcji '7. Odczyt' lub zacznij dodawac wpisy za pomoca '1. Dodaj'." << endl;
		}
	}
	
	// zapis do pliku CSV [FUNKCJONALNOŚĆ NR 6]
	static void Zapisz()
	{
		ofstream plik("Data.txt", ofstream::trunc);

		if (!plik.is_open())
		{
			cout << "Nie mozna otworzyc pliku do zapisu!" << endl;
		}
		else
		{

			for (auto& wpis : lista_wpisow)
			{
				wpis.zapiszDoCSV(plik);
			}

			plik.close();
			cout << "Zapisano dane do pliku Data.txt" << endl;
		}
	}

	// wczytywanie z pliku CSV [FUNKCJONALNOŚĆ NR 7]
	static void Wczytaj()
	{
		ifstream plik("Data.txt");

		if (!plik.is_open())
		{
			cout << "Nie mozna otworzyc pliku do odczytu!" << endl;
		}
		else
		{
			lista_wpisow.clear();
			string linia;
			while (getline(plik, linia))
			{
				// Zakładamy format CSV: id,data,typ,wartosc,kategoria,notatka
				stringstream ss(linia);
				string id_str, data_str, typ, wartosc_str, kategoria, notatka;

				getline(ss, id_str, ',');
				getline(ss, data_str, ',');
				getline(ss, typ, ',');
				getline(ss, wartosc_str, ',');
				getline(ss, kategoria, ',');
				getline(ss, notatka);

				unsigned int id = stoi(id_str);

				DataTime data = DataTime(data_str);

				double wartosc = stod(wartosc_str);

				Wpis wpis(id, data, typ, wartosc, kategoria, notatka);
				lista_wpisow.push_back(wpis); // można użyć push_back, żeby zachować kolejność z pliku
			}
			plik.close();

			for (auto& a : lista_wpisow)
			{
				if (last_id < a.id)
					last_id = a.id;
			}

			cout << "Dane zostaly wczytane z pliku Data.txt" << endl;
		}
	}

	//filtrowanie wpisów [FUNKCJONALNOŚĆ NR 3]
	static void Filtruj()
	{
		string typ_filtrowania;
	}

	// sortowanie wpisów [FUNKCJONALNOŚĆ NR 4]
	static void Sortuj()
	{
		string typ_sortowania, rodzaj_sortowania;

		cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
		// weryfikacja poprawnosci wpisanego typu sortowania
		while (true)
		{
			cout << "Podaj po czym chcesz sortowac swoje wpisy (data/kwota): ";
			getline(cin, typ_sortowania);
			if (typ_sortowania == "data" || typ_sortowania == "kwota") break;
			else cout << "Niepoprawny typ sortowania" << endl;
		}
		// weryfikacja poprawnosci wpisanego rodzaju sortowania
		while (true)
		{
			cout << "Podaj jak chcesz sortowac (rosnaco/malejaco): ";
			getline(cin, rodzaj_sortowania);
			if (rodzaj_sortowania == "rosnaco" || rodzaj_sortowania == "malejaco") break;
			else cout << "Niepoprawny rodzaj sortowania" << endl;
		}
		//proces sortowania
		//sortowanie po dacie
		if (typ_sortowania == "data"){
			if (rodzaj_sortowania == "rosnaco"){
				sort(lista_wpisow.begin(), lista_wpisow.end(), [](const Wpis& a, const Wpis& b)
				{
					return a.data.GetTime_T() < b.data.GetTime_T();
				});
			}
			else if (rodzaj_sortowania == "malejaco"){
				sort(lista_wpisow.begin(), lista_wpisow.end(), [](const Wpis& a, const Wpis& b)
				{
					return a.data.GetTime_T() > b.data.GetTime_T();
				});
			}
		}
		if (typ_sortowania == "kwota"){
			if (rodzaj_sortowania == "rosnaco"){
				sort(lista_wpisow.begin(), lista_wpisow.end(), [](const Wpis& a, const Wpis& b)
				{
					return a.wartosc < b.wartosc;
				});
			}
			else if (rodzaj_sortowania == "malejaco"){
				sort(lista_wpisow.begin(), lista_wpisow.end(), [](const Wpis& a, const Wpis& b)
				{
					return a.wartosc > b.wartosc;
				});

			}
		}

		cout << "Lista wpisow posortowana!" << endl;
	}

	static void Statystykuj()
	{
		int nPrzychodow = 0;
		int nWydatkow = 0;
		double sumaPrzychodow = 0.0;
		double sumaWydatkow = 0.0;
		double minPrzychodow = INT64_MAX, maxPrzychodow = INT64_MIN, sredniaPrzychodow;
		double minWydatkow = INT64_MAX, maxWydatkow = INT64_MIN, sredniaWydatkow;
		for (auto &wpis : lista_wpisow)
		{
			if (wpis.typ == "przychod")
			{
				nPrzychodow++;
				sumaPrzychodow += wpis.wartosc;
				if(wpis.wartosc > maxPrzychodow)
				{
					maxPrzychodow = wpis.wartosc;
				}
				if(wpis.wartosc < minPrzychodow)
				{
					minPrzychodow = wpis.wartosc;
				}
			} else{
				nWydatkow++;
				sumaWydatkow += wpis.wartosc;
				if(wpis.wartosc > maxWydatkow)
				{
					maxWydatkow = wpis.wartosc;
				}
				if(wpis.wartosc < minWydatkow)
				{
					minWydatkow = wpis.wartosc;
				}
			}
		}
		sredniaPrzychodow = sumaPrzychodow / (double)nPrzychodow;
		sredniaWydatkow = sumaWydatkow / (double)nWydatkow;
		cout << "liczba przelewow na konto: " << nPrzychodow << " - na laczna kwote: " << sumaPrzychodow << "PLN." << endl;
		cout << "liczba przelewow z konta: " << nWydatkow << " - na laczna kwote: " << sumaWydatkow << "PLN." << endl;
		cout << "najmniejszy wydatek byl na kwote: " << minWydatkow << "PLN." << endl;
		cout << "najmniejszy przychod byl na kwote: " << minPrzychodow << "PLN." << endl;
		cout << "najwiekszy wydatek byl na kwote: " << maxWydatkow << "PLN." << endl;
		cout << "najwiekszy przychod byl na kwote: " << maxPrzychodow << "PLN." << endl;
		cout << "srednia wydatkow wynosi: " << sredniaWydatkow << "PLN." << endl;
		cout << "srednia przychodow wynosi: " << sredniaPrzychodow << "PLN." << endl;

	}

	public:
	static void Start()
	{
		bool dzialanie_programu = true;
		int nr_operacji;

		cout << fixed << std::setprecision(2);

		while (dzialanie_programu)
		{
			cout << "Witamy w menedzerze budzetu domowego, prosimy wybrac operacje wpisujac cyfre od 1 do 8:" << endl;
			cout << "1. Dodaj   2. Lista   3. Filtruj   4. Sortuj   5. Statystyki   6. Zapis   7. Odczyt   8. Wyjscie" << endl;
			cout << "UWAGA! - jesli jest to poczatek twojej sesji, zaleca sie zastosowanie w pierwszej kolejnosci opcji nr 7 - odczyt, aby nie utracic wczesniej zapisanych wpisow!" << endl;
			cin >> nr_operacji;
			switch (nr_operacji)
			{
			case 1: //dodaj
			{
				DodawanieWpisu();
				break;
			}
			case 2: //lista
			{
				Wyswietl();
				break;
			}
			case 3: //filtruj
			{
				Filtruj();
				break;
			}
			case 4: // sortuj
			{
				Sortuj();
				break;
			}
			case 5: // statystyki
			{
				Statystykuj();
				break;
			}
			case 6: // zapis
			{
				Zapisz();
				break;
			}
			case 7: // odczyt
			{
				Wczytaj();
				break;
			}
			case 8: // wyjscie
			{
				cout << "Zamykam program, dziekujemy za skorzystanie z naszej uslugi! :)" << endl;
				dzialanie_programu = false;
				break;
			}
			default:
				cout << "Wybrano niewlasciwy numer" << endl;
				break;
			}
		}
	}
};

int main()
{
	UserMenu::Start();
	return 0;
}
