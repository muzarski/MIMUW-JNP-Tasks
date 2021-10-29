Biblioteka standardowa języka C++ udostępnia bardzo przydatny typ
unordered_map. Jest to typ generyczny, pozwalający tworzyć słowniki
o różnych kluczach i wartościach, np. unordered_map<int, string>,
unordered_map<string, string>, a nawet
unordered_map<string, unordered_map<int, string>> i inne, jeszcze bardziej
złożone. Biblioteka standardowa języka C nie udostępnia podobnego typu.
W praktyce programistycznej często pojawia się też konieczność łączenia kodu
napisanego w językach C i C++.

Napisz w C++ moduł maptel udostępniający w języku C słowniki obsługujące zmiany
numerów telefonów. W języku C numer telefonu jest przechowywany w tablicy znaków
typu char jako ciąg maksymalnie 22 cyfr zapisanych w ASCII i jest zakończony
znakiem o wartości zero.

Moduł maptel powinien składać się z pliku nagłówkowego maptel.h deklarującego
jego interfejs oraz pliku maptel.cc zawierającego jego implementację.

Moduł maptel powinien udostępniać następujący interfejs:

  // Tworzy słownik i zwraca liczbę naturalną będącą jego identyfikatorem.  
  unsigned long maptel_create(void);  

  // Usuwa słownik o identyfikatorze id.  
  void maptel_delete(unsigned long id)  

  // Wstawia do słownika o identyfikatorze id informację o zmianie numeru  
  // tel_src na numer tel_dst. Nadpisuje ewentualną istniejącą informację.  
  void maptel_insert(unsigned long id, char const *tel_src, char const *tel_dst);  

  // Jeśli w słowniku o identyfikatorze id jest informacja o zmianie numeru  
  // tel_src, to ją usuwa. W przeciwnym przypadku nic nie robi.  
  void maptel_erase(unsigned long id, char const *tel_src);  

  // Sprawdza, czy w słowniku o identyfikatorze id jest zapisana zmiana numeru  
  // tel_src. Podąża ciągiem kolejnych zmian. Zapisuje zmieniony numer w tel_dst.  
  // Jeśli nie ma zmiany numeru lub zmiany tworzą cykl, to zapisuje w tel_dst  
  // numer tel_src. Wartość len to rozmiar przydzielonej pamięci wskazywanej  
  // przez tel_dst.  
  void maptel_transform(unsigned long id, char const *tel_src, char *tel_dst, size_t len);  

Nagłówek modułu maptel powinien udostępniać stałą TEL_NUM_MAX_LEN typu size_t
o wartości 22.
