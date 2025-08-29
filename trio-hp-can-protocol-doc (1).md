# Dokumentacja komunikacji CAN dla modułów TRIO HP
## Protokół GCP V1.00 - VDE 4105 & UL1741

---

## 10. Definicje wartości sterujących (Control Values)

### 10.1 Standardowe wartości sterujące

Wszystkie komendy sterujące w protokole TRIO HP wykorzystują predefiniowane wartości hex:

| Wartość | Znaczenie ogólne | Zastosowanie |
|---------|------------------|--------------|
| **0xA0** | WŁĄCZ / AKTYWUJ / TRYB_0 | Podstawowy stan aktywny |
| **0xA1** | WYŁĄCZ / DEAKTYWUJ / TRYB_1 | Alternatywny stan lub wyłączenie |
| **0xA2** | TRYB_2 / SPECJALNY | Dodatkowy tryb pracy |

### 10.2 Szczegółowe definicje dla każdej komendy

#### 10.2.1 Komenda 0x11 0x10 - Gotowość operacyjna (On/Off)
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | ON | Moduł włączony, gotowy do pracy |
| **0xA1** | OFF | Moduł wyłączony (domyślnie) |

```
TX: 02 A4 00 F0 11 10 00 00 00 00 00 A0  // Włącz moduł 0
TX: 02 A4 3F F0 11 10 00 00 00 00 00 A1  // Wyłącz wszystkie moduły
```

#### 10.2.2 Komenda 0x11 0x20 - LED zielone miganie
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | NORMAL | LED świeci normalnie |
| **0xA1** | BLINK | LED miga (identyfikacja modułu) |

```
TX: 02 A4 00 F0 11 20 00 00 00 00 00 A1  // Miga LED moduł 0
TX: 02 A4 00 F0 11 20 00 00 00 00 00 A0  // Normalny LED moduł 0
```

#### 10.2.3 Komenda 0x11 0x21 - Sleep (uśpienie modułu)
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | NOT SLEEP | Moduł aktywny (domyślnie) |
| **0xA1** | SLEEP | Moduł w trybie uśpienia (niska sprawność) |

```
TX: 02 A4 01 F0 11 21 00 00 00 00 00 A1  // Uśpij moduł 1 (optymalizacja)
TX: 02 A4 01 F0 11 21 00 00 00 00 00 A0  // Wybudź moduł 1
```

#### 10.2.4 Komenda 0x11 0x22 - Walk-in (wolny start)
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | DISABLE | Wyłącz funkcję walk-in |
| **0xA1** | ENABLE | Włącz walk-in (domyślnie ON, 5s) |

```
TX: 02 A4 00 F0 11 22 00 00 00 00 00 A1  // Włącz wolny start (5s)
TX: 02 A4 00 F0 11 22 00 00 00 00 00 A0  // Wyłącz wolny start
```

#### 10.2.5 Komenda 0x11 0x23 - Funkcja DIP switch
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | GROUP_NUMBER | DIP switch ustawia numer grupy (domyślnie) |
| **0xA1** | MODULE_NUMBER | DIP switch ustawia numer modułu |

```
TX: 02 A4 3F F0 11 23 00 00 00 00 00 A1  // DIP = numer modułu
TX: 02 A4 3F F0 11 23 00 00 00 00 00 A0  // DIP = numer grupy
```

#### 10.2.6 Komenda 0x11 0x26 - Poziomy DC (szeregowo/równolegle)
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | PARALLEL | Praca równoległa (<500V DC) |
| **0xA1** | SERIES | Praca szeregowa (domyślnie) |
| **0xA2** | AUTOMATIC | Automatyczne przełączanie |

```
TX: 02 A4 00 F0 11 26 00 00 00 00 00 A0  // Równolegle (np. 48V baterie)
TX: 02 A4 00 F0 11 26 00 00 00 00 00 A1  // Szeregowo (np. 800V DC)
TX: 02 A4 00 F0 11 26 00 00 00 00 00 A2  // Auto (inteligentne)
```

#### 10.2.7 Komenda 0x11 0x34 - Stabilizacja napięcia
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | DISABLE | Wyłącz stabilizację (domyślnie) |
| **0xA1** | ENABLE | Włącz auto przełączanie AC/DC ↔ DC/AC |

```
TX: 02 A4 3F F0 11 34 00 00 00 00 00 A1  // Włącz stabilizację
TX: 02 A4 3F F0 10 01 00 00 00 06 1A 80  // Próg przełączania: 400V
```

#### 10.2.8 Komenda 0x21 0x10 - Tryb pracy modułu
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | AC/DC | Prostownik (domyślnie) |
| **0xA1** | DC/AC ON-GRID | Falownik sieciowy (grid-tie) |
| **0xA2** | DC/AC OFF-GRID | Falownik wyspowy (bez sieci) |

```
TX: 02 A4 00 F0 21 10 00 00 00 00 00 A0  // Prostownik
TX: 02 A4 00 F0 21 10 00 00 00 00 00 A1  // Falownik grid-tie
TX: 02 A4 00 F0 21 10 00 00 00 00 00 A2  // Falownik off-grid
```

#### 10.2.9 Komenda 0x21 0x17 - Typ ustawienia mocy biernej
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | NO_SETTING | Brak ustawienia mocy biernej (domyślnie) |
| **0xA1** | POWER_FACTOR | Ustawienie przez współczynnik mocy (cmd 0x21 0x05) |
| **0xA2** | REACTIVE_POWER | Ustawienie przez wartość VAr (cmd 0x21 0x08) |

```
TX: 02 A4 00 F0 21 17 00 00 00 00 00 A1  // Tryb: Power Factor
TX: 02 A4 00 F0 21 05 3F 73 33 33        // PF = 0.95 (IEEE-754)

TX: 02 A4 00 F0 21 17 00 00 00 00 00 A2  // Tryb: Reactive Power  
TX: 02 A4 00 F0 21 08 00 00 0F 42 40     // 1000 VAr
```

#### 10.2.10 Komenda 0x21 0x14 - Anti-islanding (detekcja wyspy)
| Wartość | Funkcja | Opis |
|---------|---------|------|
| **0xA0** | ENABLE | Włącz detekcję wyspy (domyślnie) |
| **0xA1** | DISABLE | Wyłącz detekcję wyspy |

```
TX: 02 A4 3F F0 21 14 00 00 00 00 00 A0  // Włącz anti-islanding
TX: 02 A4 3F F0 21 14 00 00 00 00 00 A1  // Wyłącz (tylko do testów!)
```

### 10.3 Funkcje VDE 4105 & UL1741 - wartości sterujące

#### 10.3.1 Funkcje grid support - włączanie/wyłączanie
Wszystkie funkcje grid support używają standardowego schematu:
- **0xA0** = DISABLE (wyłącz funkcję)
- **0xA1** = ENABLE (włącz funkcję)

| Funkcja | Komenda | 0xA0 | 0xA1 |
|---------|---------|------|------|
| Volt-Watt | 0x31 0xD8 | Wyłącz VW | Włącz VW |
| Frequency-Watt >fN | 0x31 0xD2 | Wyłącz FW | Włącz FW |
| Frequency-Watt <fN | 0x31 0xD3 | Wyłącz FW | Włącz FW |
| Q(V) Exponential | 0x31 0xDF | Linear Q(V) | Exponential Q(V) |
| LVRT | 0x31 0xD0 | Wyłącz LVRT | Włącz LVRT |
| HVRT | 0x31 0xD1 | Wyłącz HVRT | Włącz HVRT |
| LFRT | 0x31 0xE1 | Wyłącz LFRT | Włącz LFRT |
| HFRT | 0x31 0xE0 | Wyłącz HFRT | Włącz HFRT |

#### 10.3.2 Ustawienia krajowe (National Settings)
| Wartość | Standard | Napięcie | Częstotliwość |
|---------|----------|----------|---------------|
| **0x00** | China ESS | 380V | 50Hz |
| **0x01** | Germany ESS (VDE 4105) | 400V | 50Hz |
| **0x03** | America ESS (UL1741SA) | 480V | 60Hz |

```
TX: 02 A4 00 F0 31 C1 00 00 00 00 00 01  // VDE 4105 (Niemcy)
TX: 02 A4 00 F0 31 C1 00 00 00 00 00 03  // UL1741SA (USA)
```

### 10.4 Tabela krzyżowa - szybka referenca

| Komenda | Bajt 7 | 0xA0 | 0xA1 | 0xA2 | Uwagi |
|---------|--------|------|------|------|-------|
| 11 10 | On/Off | ON | OFF | - | Gotowość operacyjna |
| 11 20 | LED | Normal | Blink | - | Identyfikacja |  
| 11 21 | Sleep | Active | Sleep | - | Optymalizacja |
| 11 22 | Walk-in | Disable | Enable | - | Wolny start 5s |
| 11 23 | DIP switch | Group# | Module# | - | Adresowanie |
| 11 26 | DC levels | Parallel | Series | Auto | <500V / >500V |
| 11 34 | Voltage stab | Disable | Enable | - | Auto AC/DC↔DC/AC |
| 21 10 | Work mode | AC/DC | DC/AC grid | DC/AC island | Tryb pracy |
| 21 17 | Reactive type | None | Power Factor | VAr value | Moc bierna |
| 21 14 | Anti-island | Enable | Disable | - | Bezpieczeństwo |

---

## 1. Informacje podstawowe

### 1.1 Wspierane moduły
| Model | Numer katalogowy | Max liczba modułów |
|-------|------------------|-------------------|
| TRIO-HP/3AC/1KDC/20KW/BI | 1560712 | 48 (AC/DC), 8 (off-grid) |

### 1.2 Parametry komunikacji CAN
- **Standard**: CAN 2.0B (extended frame mode)
- **Prędkość transmisji**: 125 kbps
- **Terminacja**: rezystory 120 Ω na końcach magistrali
- **Obsługiwane tryby komunikacji**: Peer-to-peer, Multicast, Broadcast

---

## 2. Format ramki CAN

### 2.1 Struktura identyfikatora (29 bitów)

```
[28-26] [25-22] [21-16] [15-8]  [7-0]
Error   Device  Command Target  Source
Code    No.     No.     Address Address
(3bit)  (4bit)  (6bit)  (8bit)  (8bit)
```

### 2.2 Typy danych i kolejność bajtów

#### 2.2.1 Liczby stałoprzecinkowe (Fixed-point)
Mogą zajmować od 1 do 4 bajtów. Dane są przesyłane w kolejności rosnącej (Byte 0 → Byte 7).

**Przykłady:**
- **BYTE (8-bit)**: 0x00 - 0xFF
- **WORD (16-bit)**: MSB-LSB (Most Significant Byte first)
  ```
  Napięcie 402.0V = 0x0FB4 (4020 w jednostkach 0.1V)
  Byte[0] = 0x0F (MSB)
  Byte[1] = 0xB4 (LSB)
  ```
- **DWORD (32-bit)**: MSB → LSB (4 bajty)
  ```
  Napięcie 750V = 750000mV = 0x000B71B0
  Byte[0] = 0x00 (MSB)
  Byte[1] = 0x0B
  Byte[2] = 0x71
  Byte[3] = 0xB0 (LSB)
  ```

#### 2.2.2 Liczby zmiennoprzecinkowe (Floating-point IEEE-754)
Format: 32-bit IEEE-754, kolejność: **MSB-LSB**

```
[31]    [30-23]      [22-0]
Znak    Wykładnik    Mantysa
(1bit)  (8bit)       (23bit)
```

**Wzór**: ±(1 + M×2⁻²³) × 2^(E-127)
- S = 0: liczba dodatnia, S = 1: liczba ujemna
- E = wykładnik z bias 127
- M = mantysa

**Przykłady konwersji:**
```
500.0 = 0x43FA0000
Kolejność bajtów: 43 FA 00 00 (MSB → LSB)
Byte[0] = 0x43, Byte[1] = 0xFA, Byte[2] = 0x00, Byte[3] = 0x00

40.0 = 0x42200000  
Kolejność bajtów: 42 20 00 00

2.40 = 0x4019999A
Kolejność bajtów: 40 19 99 9A
```

#### 2.2.3 Jednostki pomiarowe

| Typ danych | Jednostka | Zakres | Format |
|------------|-----------|--------|--------|
| Napięcie DC/AC | mV (milivolt) | 0 - 1000000 mV | DWORD (32-bit) |
| Prąd DC/AC | mA (miliamper) | 0 - 73300 mA | DWORD (32-bit) |
| Moc czynna | mW (miliwat) | signed | DWORD (32-bit) |
| Moc bierna | mVar (milivar) | signed | DWORD (32-bit) |
| Moc pozorna | mVA (milivitoamper) | signed | DWORD (32-bit) |
| Częstotliwość | mHz (miliherc) | 45000 - 65000 mHz | DWORD (32-bit) |
| Temperatura | m°C (mili°C) lub °C | -128 do +127°C | signed BYTE |
| Współczynnik mocy | - | 0.8 - 1.0 | IEEE-754 Float |

#### 2.2.4 Kolejność bajtów w ramce CAN

**Ramka CAN zawsze ma 8 bajtów danych:**
```
Byte0  Byte1  Byte2  Byte3  Byte4  Byte5  Byte6  Byte7
 ↑                                                    ↑
Pierwszy                                        Ostatni
```

**Dla różnych typów danych:**

1. **Komenda 2-bajtowa + dane 32-bit:**
   ```
   Byte0  Byte1  Byte2  Byte3  Byte4  Byte5  Byte6  Byte7
   CMD    SCMD   0x00   0x00   DATA[MSB] ... DATA[LSB]
   ```

2. **Komenda 2-bajtowa + dane 16-bit:**
   ```
   Byte0  Byte1  Byte2  Byte3  Byte4  Byte5  Byte6  Byte7
   CMD    SCMD   0x00   0x00   0x00   0x00   D[MSB] D[LSB]
   ```

3. **Status (3 bajty statusu):**
   ```
   Byte0  Byte1  Byte2  Byte3  Byte4  Byte5  Byte6  Byte7
   CMD    SCMD   0x00   0x00   TEMP   ST2    ST1    ST0
   ```

**Przykłady praktyczne:**

```
// Ustawienie napięcia 750V (750000 mV = 0x000B71B0)
TX: 02 A4 3F F0 10 01 00 00 00 0B 71 B0
    |  |  |  |  |  |  |  |  |  |  |  |
    |  |  |  |  |  |  |  |  |  |  |  LSB (0xB0)
    |  |  |  |  |  |  |  |  |  |  MSB-1 (0x71) 
    |  |  |  |  |  |  |  |  |  MSB-2 (0x0B)
    |  |  |  |  |  |  |  |  MSB (0x00)
    |  |  |  |  |  |  |  Sub-cmd (0x01)
    |  |  |  |  |  |  Cmd (0x10)
    |  |  |  |  Source addr (0xF0)
    |  |  |  Target addr (0x3F)
    |  |  Device+Cmd (0xA4)
    |  Error+Device (0x02)
    CAN ID byte 3

// Odczyt temperatury - odpowiedź (25°C = 25000 m°C)
RX: 02 A3 F0 00 11 06 00 00 00 00 61 A8
                            |  |  |  |
                            |  |  |  LSB temp (0xA8)
                            |  |  MSB temp (0x61)
                            |  0x00
                            0x00
```

### 2.2 Kody błędów (Error Code)

### 2.3 Kody błędów (Error Code)

| Kod | Opis |
|-----|------|
| 0x00 | Normal - operacja poprawna |
| 0x02 | Command invalid - nieprawidłowa komenda |
| 0x03 | Data invalid - nieprawidłowe dane |
| 0x07 | Start of processing - inicjalizacja |

### 2.4 Numery urządzeń (Device No.)

| Numer | Opis |
|-------|------|
| 0x0A | Protokół między kontrolerem a pojedynczym modułem |
| 0x0B | Protokół między kontrolerem a grupą modułów |

---

## 3. Komendy protokołu odczyt danych

### 3.1 Komenda 0x23 - System DC

#### 3.1.1 Odczyt informacji systemowych DC
**Typ**: Multicast/Broadcast

| Podkomenda | Dane | Opis |
|------------|------|------|
| 0x10 0x01 | System DC Voltage [mV] | Napięcie systemu DC |
| 0x10 0x02 | System DC Current [mA] | Prąd całkowity systemu DC |
| 0x10 0x10 | Number of modules | Liczba modułów |

**Przykład**:
```
TX: 02 A3 3F F0 10 01 00 00 00 00 00 00  // Odczyt napięcia systemu
RX: 02 A3 F0 3F 10 01 00 00 00 0A AE 60  // Odpowiedź: 700V
```

### 3.2 Komenda 0x23 - Moduł DC

#### 3.2.1 Parametry modułu DC
**Typ**: Peer-to-peer/Multicast

| Podkomenda | Dane | Opis |
|------------|------|------|
| 0x11 0x01 | DC Voltage [mV] | Napięcie DC modułu |
| 0x11 0x02 | DC Current [mA] | Prąd DC modułu |
| 0x11 0x03 | AC Voltage A-B [mV] | Napięcie AC linia A-B |
| 0x11 0x04 | AC Voltage B-C [mV] | Napięcie AC linia B-C |
| 0x11 0x05 | AC Voltage C-A [mV] | Napięcie AC linia C-A |
| 0x11 0x06 | Temperature [m°C] | Temperatura otoczenia |
| 0x11 0x10 | Status bits | Status modułu (3 bajty) |
| 0x11 0x11 | Status bits 2 | Dodatkowy status modułu |
| 0x11 0x20 | Group number | Numer grupy modułu |

### 3.3 Komenda 0x23 - Moduł AC

#### 3.3.1 Parametry AC modułu
**Typ**: Peer-to-peer/Multicast

| Podkomenda | Dane | Opis |
|------------|------|------|
| 0x21 0x01 | AC Voltage Phase A [mV] | Napięcie AC faza A |
| 0x21 0x02 | AC Voltage Phase B [mV] | Napięcie AC faza B |
| 0x21 0x03 | AC Voltage Phase C [mV] | Napięcie AC faza C |
| 0x21 0x04 | AC Current Phase A [mA] | Prąd AC faza A |
| 0x21 0x05 | AC Current Phase B [mA] | Prąd AC faza B |
| 0x21 0x06 | AC Current Phase C [mA] | Prąd AC faza C |
| 0x21 0x07 | AC Frequency [mHz] | Częstotliwość AC |
| 0x21 0x08 | Active Power Total [mW] | Moc czynna całkowita |
| 0x21 0x09 | Active Power Phase A [mW] | Moc czynna faza A |
| 0x21 0x0A | Active Power Phase B [mW] | Moc czynna faza B |
| 0x21 0x0B | Active Power Phase C [mW] | Moc czynna faza C |
| 0x21 0x0C | Reactive Power Total [mVar] | Moc bierna całkowita |
| 0x21 0x0D | Reactive Power Phase A [mVar] | Moc bierna faza A |
| 0x21 0x0E | Reactive Power Phase B [mVar] | Moc bierna faza B |
| 0x21 0x0F | Reactive Power Phase C [mVar] | Moc bierna faza C |
| 0x21 0x10 | Apparent Power Total [mVA] | Moc pozorna całkowita |
| 0x21 0x11 | Apparent Power Phase A [mVA] | Moc pozorna faza A |
| 0x21 0x12 | Apparent Power Phase B [mVA] | Moc pozorna faza B |
| 0x21 0x13 | Apparent Power Phase C [mVA] | Moc pozorna faza C |

---

## 4. Komendy protokołu zapis danych

### 4.1 Komenda 0x24 - Konfiguracja systemu

#### 4.1.1 Ustawienia systemu DC
**Typ**: Multicast/Broadcast

| Podkomenda | Dane | Opis |
|------------|------|------|
| 0x10 0x01 | Voltage [mV] | Napięcie wyjściowe systemu |
| 0x10 0x02 | Current [mA] | Prąd całkowity systemu |

### 4.2 Komenda 0x24 - Konfiguracja modułu

#### 4.2.1 Parametry DC modułu
**Typ**: Peer-to-peer/Multicast/Broadcast

| Podkomenda | Dane | Opis |
|------------|------|------|
| 0x11 0x01 | Voltage [mV] | Napięcie wyjściowe modułu |
| 0x11 0x02 | Current [mA] | Prąd wyjściowy modułu |
| 0x11 0x03 | Group number | Ustawienie numeru grupy |
| 0x11 0x10 | 0xA0/0xA1 | Włączenie/wyłączenie modułu |
| 0x11 0x20 | 0xA0/0xA1 | Miganie LED zielone |
| 0x11 0x21 | 0xA0/0xA1 | Deaktywacja modułu |
| 0x11 0x22 | 0xA0/0xA1 | Wolny start |
| 0x11 0x23 | 0xA0/0xA1 | Funkcja DIP switch |
| 0x11 0x26 | 0xA0/0xA1/0xA2 | Poziomy DC (równoległy/szeregowy/automatyczny) |
| 0x11 0x32 | Voltage [mV] | Dolny limit napięcia rozładowania |
| 0x11 0x34 | 0xA0/0xA1 | Tryb stabilizacji napięcia |

#### 4.2.2 Parametry AC modułu
**Typ**: Peer-to-peer/Multicast/Broadcast

| Podkomenda | Dane | Opis |
|------------|------|------|
| 0x21 0x05 | Power Factor (float) | Współczynnik mocy |
| 0x21 0x06 | Voltage [mV] | Napięcie wyjściowe AC na fazę |
| 0x21 0x07 | Frequency [mHz] | Częstotliwość wyjściowa AC |
| 0x21 0x08 | Reactive Power [mVar] | Moc bierna na wyjściu |
| 0x21 0x10 | 0xA0/0xA1/0xA2 | Tryb pracy (AC/DC, DC/AC on-grid, DC/AC off-grid) |
| 0x21 0x17 | 0xA0/0xA1/0xA2 | Tryb ustawienia mocy biernej |

---

## 5. Funkcje specjalne dla standardów VDE 4105 & UL1741

### 5.1 Anti-islanding Protection (detekcja wyspy)
**Komenda**: 0x21 0x14

| Wartość | Funkcja |
|---------|---------|
| 0xA0 | Włączenie detekcji wyspy (domyślnie) |
| 0xA1 | Wyłączenie detekcji wyspy |

**Uwaga**: Status wykrycia wyspy można odczytać z bitów statusu Inverter 2 (bit 3 = Status_Island_detected)

**Przykład**:
```
TX: 02 A4 00 F0 21 14 00 00 00 00 00 A0  // Włącz anti-islanding
// Sprawdzenie statusu:
TX: 02 A3 00 F0 11 11 00 00 00 00 00 00  // Odczyt statusu inverter
RX: 02 A3 F0 00 11 11 00 00 00 [ST2] [ST1] [ST0]
// Bit 3 w ST2 = 1 oznacza wykrycie wyspy
```

### 5.2 Soft-Start Ramp Rate
**Komendy**: 0x31 0x13 (SS connect rate), 0x31 0x14 (SS connect waiting time)

| Parametr | Jednostka | Zakres |
|----------|-----------|---------|
| SS connect rate | % P rated/min | 0-9000 |
| SS connect waiting time | ms | 0-600000 |

### 5.3 Voltage-Watt Function (VW)
**Komendy**: 0x31 0x26 (Vstart), 0x31 0x27 (Vstop), 0x31 0x29 (Kpower-volt)

### 5.4 Frequency-Watt Function (FW)
**Komendy**: 0x31 0x31 (fstart>fN), 0x31 0x33 (Kpower-freq>fN)

### 5.5 Volt-Var Function (Q(V))
**Komendy**: 0x31 0x64-0x69 (V1-V4, Q1-Q4)

### 5.6 Power Factor Function
**Komendy**: 0x31 0x23 (active power setting), 0x21 0x05 (power factor)

### 5.7 Voltage Ride Through
**Komendy**: 0x31 0xD0 (LVRT), 0x31 0xD1 (HVRT)

### 5.8 Frequency Ride Through
**Komendy**: 0x31 0xE1 (LFRT), 0x31 0xE0 (HFRT)

---

## 6. Rejestry statusu modułu

### 6.1 Status 1.3 (Bajt 5)

| Bit | Znaczenie | Warunek | LED |
|-----|-----------|---------|-----|
| 7 | PFC circuit OFF | 1 = PFC wyłączony | - |
| 6 | Przepięcie wejściowe | Vin > 535V AC | Żółty |
| 5 | Za niskie napięcie wejściowe | Vin < 260V AC | Żółty |
| 4 | Asymetria wejścia | 3-fazowa asymetria | - |
| 3 | Utrata fazy | Jedna faza utracona | - |
| 2 | Load sharing | Iavg_system >15% | - |
| 1 | Powtórzenie ID modułu | Duplikat adresu | Czerwony |
| 0 | Ograniczenie mocy | Wysoka temp. lub niskie Vin | - |

### 6.2 Status 1.2 (Bajt 6)

| Bit | Znaczenie | Warunek | LED |
|-----|-----------|---------|-----|
| 7 | Przerwanie komunikacji CAN | Brak sygnału >10s | Migający żółty |
| 6 | Wolny start | Aktywny slow startup | - |
| 5 | Przepięcie wyjściowe | Vout > 1036V DC | Żółty |
| 4 | Przekroczenie temp. | Ambient >78°C ±3% | Żółty |
| 3 | Błąd wentylatora | Sygnał błędu aktywny | Migający czerwony |
| 2 | Ochrona modułu | Różne błędy ochronne | Żółty |
| 1 | Błąd modułu | Błędy krytyczne | Czerwony |
| 0 | DC side OFF | Strona DC wyłączona | - |

### 6.3 Status 1.1 (Status 0 - Bajt 7)

| Bit | Znaczenie | Warunek | LED |
|-----|-----------|---------|-----|
| 7 | Nieprawidłowe rozładowanie 1 | Vout >350V DC 2s po OFF | - |
| 6 | Nieprawidłowe rozładowanie 2 | Vout (400ms po OFF) >1/2*Vout | - |
| 4 | Moduł w trybie Sleep | Ustawiony przez CAN | - |
| 0 | Zwarcie wyjścia | Vo <25V, Io >10% przez 2s | Czerwony |

### 6.4 Status 2.2 (Status Inverter 2 - Bajt dodatkowy)

| Bit | Znaczenie | Warunek |
|-----|-----------|---------|
| 4 | E-STOP | E-STOP aktywny |
| 3 | Status_Island_detected | Wykrycie wyspy (islanding) |
| 1 | AC overload | Przeciążenie AC w trybie off-grid |
| 0 | Mains power mode | 1=off-grid, 0=on-grid |

### 6.5 Status 2.1 (Status Inverter 1 - Bajt dodatkowy)

| Bit | Znaczenie | Warunek |
|-----|-----------|---------|
| 4 | Status_Operating_mode | Tryb operacyjny |
| 1 | Status_AC_overload | Przeciążenie AC |
| 0 | Operating mode | 1=DC/AC (inwenter), 0=AC/DC (prostownik) |

---

## 7. Sygnały z pliku DBC (dodatkowe funkcje)

### 7.1 Zarządzanie energią

| Sygnał | ID | Opis | Status bit |
|--------|----|----- |-----------|
| Set_Energy_Storage_mode | 0x18 | Tryb magazynowania energii | - |
| Island_detection_mode | 0x14 | Tryb detekcji wyspy | Status Inverter 2 bit 3 |
| National_settings_of_safety_reg | 0xC1 | Ustawienia krajowe bezpieczeństwa | - |

### 7.2 Funkcje grid support

| Sygnał | ID | Opis |
|--------|----|----- |
| Q_exponential | 0xDF | Ekspotencjalna odpowiedź Q |
| Volt_Watt_enable | 0xD8 | Włączenie funkcji Volt-Watt |
| Frequency_Watt_enable_over_fn | 0xD2 | FW powyżej częst. nominalnej |
| Frequency_Watt_enable_below_fn | 0xD3 | FW poniżej częst. nominalnej |
| High_Voltage_Ride_Through_En | 0xD1 | Włączenie HVRT |
| Low_Voltage_Ride_Through_En | 0xD0 | Włączenie LVRT |
| High_Freq_Ride_Through_En | 0xE0 | Włączenie HFRT |
| Low_Freq_Ride_Through_En | 0xE1 | Włączenie LFRT |

### 7.3 Parametry Volt-Watt

| Sygnał | ID | Jednostka | Opis |
|--------|----|-----------|----- |
| Volt_Watt_Vstart | 0x26 | % Vn | Napięcie startu VW |
| Volt_Watt_Vstop | 0x27 | % Vn | Napięcie stopu VW |
| Volt_Watt_Kpower_rate | 0x30 | % P rated/min | Szybkość zmian mocy VW |

### 7.4 Parametry Frequency-Watt

| Sygnał | ID | Jednostka | Opis |
|--------|----|-----------|----- |
| Frequency_Watt_fstart_over_fN | 0x31 | mHz | Częstotliwość startu >fN |
| Freq_Watt_Kpower_freq_over_fn | 0x33 | % P rated/Hz | Współczynnik mocy-częstotliwość >fN |
| Frequency_Watt_fstart_below_fN | 0x36 | mHz | Częstotliwość startu <fN |
| Freq_Watt_Kpower_freq_below_fn | 0x38 | % P rated/Hz | Współczynnik mocy-częstotliwość <fN |

### 7.5 Parametry Q(V)

| Sygnał | ID | Jednostka | Opis |
|--------|----|-----------|----- |
| Q_V_Q_High_volt_start_V1 | 0x67 | % Vn | Napięcie V1 dla Q(V) |
| Q_V_Q_High_volt_start_V2 | 0x66 | % Vn | Napięcie V2 dla Q(V) |
| Q_V_Q_High_volt_start_V3 | 0x64 | % Vn | Napięcie V3 dla Q(V) |
| Q_V_Q_High_volt_start_V4 | 0x65 | % Vn | Napięcie V4 dla Q(V) |
| Q_V_to_Q1 | 0x69 | % S rated | Moc bierna Q1 |
| Q_V_to_Q4 | 0x68 | % S rated | Moc bierna Q4 |

---

## 8. Sekwencje komunikacji TRIO HP

### 8.1 Inicjalizacja systemu
```
1. Ustawienie trybu pracy:
   TX: 02 A4 3F F0 21 10 00 00 00 00 00 A1  // DC/AC on-grid

2. Włączenie funkcji Energy Storage:
   TX: 02 A4 3F F0 21 18 00 00 00 00 00 A1

3. Ustawienie parametrów VDE 4105:
   TX: 02 A4 00 F0 31 C1 00 00 00 00 00 01  // Germany Ess (400V)

4. Ustawienie napięcia i prądu:
   TX: 02 A4 3F F0 10 01 00 00 00 06 1A 80  // 400V
   TX: 02 A4 3F F0 10 02 00 00 00 00 4E 20  // 20A

5. Włączenie modułów:
   TX: 02 A4 3F F0 11 10 00 00 00 00 00 A0
```

### 8.2 Monitorowanie systemu z Heartbeat
```
1. Nasłuch ramek Heartbeat (automatyczne wykrycie modułów):
   RX: ID=0x0757F7FB → Moduł 0 aktywny
   RX: ID=0x0757F803 → Moduł 1 aktywny  
   RX: ID=0x0757F80B → Moduł 2 aktywny

2. Odczyt informacji systemowych:
   TX: 02 A3 3F F0 10 01 00 00 00 00 00 00
   RX: 02 A3 F0 3F 10 01 00 00 [V_system]

3. Odczyt statusu konkretnego modułu (znamy ID z Heartbeat):
   TX: 02 A3 00 F0 11 10 00 00 00 00 00 00  // Moduł 0
   RX: 02 A3 F0 00 11 10 00 [status_bytes]

4. Odczyt mocy AC z wykrytego modułu:
   TX: 02 A3 01 F0 21 08 00 00 00 00 00 00  // Moduł 1
   RX: 02 A3 F0 01 21 08 00 [power_data]
```

---

## 9. Uwagi implementacyjne

### 9.1 Ważne ograniczenia
- **ZAKAZ** używania ramki 0x075XXXXX - zarezerwowana dla komunikacji wewnętrznej
- Interwał zapytań: 50-200 ms
- Timeout komunikacji: 10s (automatyczne wyłączenie modułu)
- Max 48 modułów w trybie AC/DC, 8 modułów w trybie off-grid

### 9.2 Konwersje danych - szczegółowe przykłady

#### 9.2.1 Konwersja Float IEEE-754
```c
// Funkcja konwersji REAL na bajty (MSB-LSB)
void FloatToBytes(float value, uint8_t* bytes) {
    union {
        float f;
        uint32_t i;
    } converter;
    
    converter.f = value;
    bytes[0] = (converter.i >> 24) & 0xFF;  // MSB
    bytes[1] = (converter.i >> 16) & 0xFF;
    bytes[2] = (converter.i >> 8) & 0xFF;
    bytes[3] = converter.i & 0xFF;          // LSB
}

// Funkcja konwersji bajtów na REAL
float BytesToFloat(uint8_t* bytes) {
    union {
        float f;
        uint32_t i;
    } converter;
    
    converter.i = ((uint32_t)bytes[0] << 24) |
                  ((uint32_t)bytes[1] << 16) |
                  ((uint32_t)bytes[2] << 8) |
                  (uint32_t)bytes[3];
    return converter.f;
}
```

#### 9.2.2 Konwersja wartości 16-bit i 32-bit
```c
// WORD (16-bit) MSB-LSB
void WordToBytes(uint16_t value, uint8_t* bytes) {
    bytes[0] = (value >> 8) & 0xFF;   // MSB
    bytes[1] = value & 0xFF;          // LSB
}

// DWORD (32-bit) MSB-LSB  
void DWordToBytes(uint32_t value, uint8_t* bytes) {
    bytes[0] = (value >> 24) & 0xFF;  // MSB
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;          // LSB
}
```

#### 9.2.3 Przykłady konwersji jednostek
```
Napięcie: 750V → 750000mV → 0x000B71B0
Prąd: 15.5A → 15500mA → 0x00003C8C  
Moc: 11.5kW → 11500000mW → 0x00AF7300
Częstotliwość: 50Hz → 50000mHz → 0x0000C350
Temperatura: 25°C → 25 → 0x19 (signed byte)
Współczynnik mocy: 0.95 → IEEE-754 → 0x3F733333
```

### 9.3 Tryby pracy
- **0xA0**: AC/DC (domyślny)
- **0xA1**: DC/AC on-grid
- **0xA2**: DC/AC off-grid

---

**Data utworzenia dokumentacji**: 28.08.2025, 17:03:50