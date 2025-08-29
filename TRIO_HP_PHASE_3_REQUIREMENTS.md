# TRIO HP FAZA 3: Sterowanie i Limity - Szczegółowe Wymagania

**Data:** 28.08.2025  
**Autor:** Artur (właściciel projektu)  
**Status:** Wymagania do implementacji

---

## 🎯 KLUCZOWE POPRAWKI I WYJAŚNIENIA

### 1. Operational Readiness Control
**POPRAWKA:** To działa odwrotnie - w zależności od stanu operational readiness można lub nie można wysyłać część komend sterujących.

### 2. BMS Digital Inputs Integration
**WYMAGANE:**
- **Input 10:** E-STOP  
- **Input 9:** Stycznik AC

### 3. Limity z BMS
**WYMAGANE:** Obsługa zarówno DDCL jak i DCCL z systemu BMS.

---

## 🚀 PROCEDURA STARTU (10 kroków)

1. **Sprawdzenie E-STOP** (input 10)
2. **Sprawdzenie stanu "ready to charge" z baterii**  
3. **Sprawdzenie stanu stycznika AC** (input 9)
4. **Wykrycie heartbeat** z modułów
5. **Wysłanie broadcastowych ustawień** 
6. **Rozpoczęcie odczytu stanu modułów** (0x23)
7. **Wysłanie multicastowych ustawień**
8. **Obliczenie wymaganego prądu i określenie kierunku pracy**
9. **Wysłanie prądu, kierunku i ewentualnie biernej** (jeśli zadana)
10. **Wysłanie operational readiness na ON**

---

## 🛑 PROCEDURA STOPU (2 kroki)

1. **Prąd na zero** (najpierw)
2. **Operational readiness na OFF** (potem)

---

## 🔐 TRYB BLOKADY PARAMETRÓW

**WYMAGANE:** 
- Tryb w którym parametry można ustawiać 
- Tryb w którym są zablokowane
- Kontrola poprzez jeden rejestr/pole na stronie WWW

---

## ⚡ PARAMETRY STEROWANIA

### 7. Podstawowe Parametry Kontrolne:
1. **Włącznik** (ON/OFF)
2. **Moc czynna:**
   - **Ujemna:** Tryb falownika (z baterii do sieci)
   - **Dodatnia:** Ładowanie baterii (z sieci)
3. **Moc bierna:**
   - **Ujemna:** Pojemnościowa 
   - **Dodatnia:** Indukcyjna
   - **Zero:** Bez zadawania mocy biernej

---

## 🎛️ REGULATOR MOCY CZYNNEJ

### 8. Specyfikacja Regulatora Mocy Czynnej:
- **Cel:** Kontrola mocy AC przez sterowanie prądem DC
- **Metoda:** Oblicz prąd = zadana_moc / napięcie_baterii
- **Monitoring:** Suma mocy czynnej modułów 
- **Tolerancja:** ±300W od zadanej wartości (zmienna wejściowa)
- **Pętla regulacji:** Co 3 sekundy (zmienna wejściowa)
- **Typ:** Zamknięta pętla PID

---

## 🔄 REGULATOR MOCY BIERNEJ

### 9. Specyfikacja Regulatora Mocy Biernej:
- **Próg automatyczny:** 1500 VA (zmienna wejściowa)
  - **<1500 VA:** Przepisz zadaną wartość bezpośrednio na moduł
  - **>1500 VA:** Regulacja sumy mocy pozornej modułów
- **Tolerancja:** ±300 VAr od zadanej (zmienna wejściowa)  
- **Pętla:** Co 3 sekundy (zmienna wejściowa)
- **Podział modułów:**
  - **≤10 kVAr:** Zawsze jeden moduł
  - **>10 kVAr:** Dziel na dwa moduły
  - **Maksimum na moduł:** 14 kVAr

---

## 📊 KONTROLER SPRAWNOŚCI

### 10. Monitoring Sprawności Układu:
- **DC Side:** Monitor prąd × napięcie baterii = moc DC
- **AC Side:** Odczyt sumy mocy czynnej + pozornej AC  
- **Wskaźniki:**
  1. **Sprawność mocy czynnej:** P_AC / P_DC
  2. **Sprawność mocy pozornej:** S_AC / P_DC

---

## 🔧 ZAŁOŻENIA SYSTEMOWE

### 11. Konfiguracja dla 2 Modułów
**System docelowy:** Maksymalnie 2 moduły TRIO HP

---

## 🔗 ZASTOSOWANIE W IMPLEMENTACJI

**Ten dokument stanowi kompletną specyfikację dla TRIO HP FAZA 3.**

**Wszystkie wymienione parametry i progi muszą być konfigurowalne poprzez:**
- Rejestry Modbus
- Interface WWW  
- Plik konfiguracyjny

**Date:** 28.08.2025 17:30 (Warsaw Time)