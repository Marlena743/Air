# Monitor Jakości Powietrza
Program monitorujący jakość powietrza w Polsce na podstawie danych udostępnianych przez GIOŚ.

## Opis projektu
Aplikacja pobiera dane o jakości powietrza z ogólnodostępnego API Głównego Inspektoratu Ochrony Środowiska.
Umożliwia przeglądanie stacji pomiarowych w Polsce, wybór dostępnych w nich stanowisk pomiarowych,
a także pobieranie, zapisywanie i analizowanie danych pomiarowych.

## Funkcjonalności
- Pobieranie danych o stacjach pomiarowych z API GIOŚ
- Filtrowanie stacji pomiarowych po nazwie miejscowości
- Wyświetlanie stanowisk pomiarowych dla wybranej stacji
- Pobieranie danych pomiarowych dla wybranego stanowiska
- Zapisywanie danych do lokalnej bazy danych w formacie JSON
- Odczyt danych historycznych z bazy
- Prezentacja danych w formie wykresu
- Analiza danych (wartości min/max, średnia, trend)

## Wymagania systemowe
- System operacyjny: Windows
- Qt 5.12 lub nowszy

## Instrukcja uruchomienia
1. Sklonuj repozytorium z GitHub:
   ```
   git clone https://github.com/username/AirQualityMonitor.git
   ```

2. Otwórz projekt w Qt Creator:
   - Uruchom Qt Creator
   - Wybierz "Otwórz projekt"
   - Przejdź do katalogu z projektem i wybierz plik AirQualityMonitor.pro

3. Skonfiguruj projekt i skompiluj:
   - Kliknij przycisk "Skonfiguruj projekt" w Qt Creator
   - Po zakończeniu konfiguracji kliknij przycisk "Zbuduj" (Ctrl+B)

4. Uruchom aplikację:
   - Kliknij przycisk "Uruchom" (Ctrl+R) lub wybierz opcję Debug > Start Debugging > Start Debugging

## Instrukcja uruchomienia testów
1. Otwórz projekt w Qt Creator

2. Dodaj konfigurację testową:
   - Kliknij przycisk "Zarządzaj projektami" lub wybierz Projekty z menu
   - W sekcji "Build Settings" kliknij "Add" i wybierz "Qt Quick Test"
   - W polu "Additional arguments" dodaj: "CONFIG+=test"

3. Wybierz utworzoną konfigurację testową i kliknij "Zbuduj"

4. Uruchom testy:
   - Po skompilowaniu projektu z konfiguracją testową, testy jednostkowe zostaną automatycznie uruchomione
   - Wyniki testów będą widoczne w oknie wyjścia Qt Creator

## Opis funkcjonalności aplikacji
1. Po uruchomieniu aplikacji, kliknij przycisk "Załaduj wszystkie stacje" aby pobrać listę dostępnych stacji pomiarowych.

2. Możesz filtrować stacje po nazwie miasta, wpisując nazwę w pole "Miasto" i klikając "Filtruj".

3. Wybierz stację z listy rozwijanej i kliknij "Załaduj sensory" aby zobaczyć dostępne stanowiska pomiarowe.

4. Wybierz interesujące Cię stanowisko pomiarowe z listy rozwijanej i kliknij "Załaduj pomiary" aby pobrać dane pomiarowe.

5. Dane zostaną przedstawione w formie wykresu oraz tabeli. Wyniki analizy danych będą widoczne w sekcji "Analiza danych".

6. Aby zapisać dane do lokalnej bazy danych, kliknij "Zapisz do bazy danych".

7. Aby wczytać zapisane wcześniej dane, kliknij "Załaduj z bazy danych".

8. Możesz filtrować dane po zakresie dat, ustawiając daty początkową i końcową, a następnie klikając "Aktualizuj".

## Obsługa błędów
Aplikacja jest odporna na błędy połączenia i niedostępność API. W przypadku problemów z połączeniem, aplikacja
wyświetli odpowiedni komunikat i zaproponuje skorzystanie z lokalnie zapisanych danych, jeśli są dostępne.

## Autor
Marlena Grzesik