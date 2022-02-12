# Zadanie 6, Programowanie obiektowe w C++

Celem zadania jest stworzenie symulacji łazika planetarnego. Symulację należy
zaimplementować, używając programowania obiektowego w C++.

Łazik planetarny ma możliwość:
* lądowania w podanej lokalizacji, ustawiając się w określonym kierunku świata.
  Lokalizacja jest określona współrzędnymi będącymi liczbami całkowitymi ze
  znakiem. Strony świata są określone jako: WEST, EAST, SOUTH, NORTH;
* wykonywania ciągu komend przesłanego jako ciąg znaków. Jeden znak reprezentuje
  jedną komendę do wykonania zgodnie z tym, jak łazik został zaprogramowany
  podczas tworzenia. Łazik może wykonywać komendy jedynie po wylądowaniu. Przed
  wkroczeniem na nowy teren łazik powinien sprawdzić, czy czujniki nie zgłaszają
  niebezpieczeństwa w nowej lokalizacji. W przypadku nieznanej komendy lub
  zgłoszenie niebezpieczeństwa łazik zatrzymuje się oraz wstrzymuje dalsze
  wykonywanie komend, aby uniknąć potencjalnej katastrofy.

Nowy łazik planetarny jest budowany za pomocą klasy RoverBuilder.
Podczas tworzenia łazika można:
* zaprogramować komendy, które określają, w jaki sposób łazik będzie reagował
  na podane znaki;
* zainstalować czujniki, które informują, czy podana lokalizacja jest
  bezpieczna.

Dostępne akcje łazika to:
* `move_forward` – reprezentuje przejście o jedną jednostkę do przodu tzn.
  w kierunku, w którym ustawiony jest łazik;
* `move_backward` – reprezentuje przejście o jedną jednostkę do tyłu tzn.
  w kierunku przeciwnym do tego, w którym ustawiony jest łazik;
* `rotate_left` – reprezentuje obrót łazika w lewo o 90 stopni;
* `rotate_right` – reprezentuje obrót łazika w prawo o 90 stopni;
* `compose` – reprezentuje ciąg akcji łazika; ciągi akcji można zagnieżdżać.

## Inne wymagania

Wszystkie błędy powinny być zgłaszane wyjątkami z hierarchii std::exception.

Bardzo istotną częścią zadania jest zaprojektowanie odpowiedniej hierarchii klas
oraz zależności pomiędzy klasami. W szczególności nie wszystkie klasy, które są
wymagane w rozwiązaniu, pojawiają się w treści zadania lub przykładzie użycia.

Podczas projektowania należy dbać o odpowiednią enkapsulację, należy kierować
się zasadami SOLID oraz warto uwzględnić wzorce projektowe GoF. W szczególności
nie można doprowadzić do cyklicznych zależności typów.

## Przykład użycia

Przykład wykorzystania biblioteki znajduje się w pliku rover_example.cc.
Wynik wykonania podanego przykładu to:

unknown
(0, 0) EAST  
(1, 0) WEST  
(0, 0) WEST stopped  
(-3, 0) WEST  
(-1, -1) WEST stopped
