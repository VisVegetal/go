# Go — Proiect POO

Un joc clasic de strategie japonez, implementat în **C++23** cu interfață grafică **SFML**.
Obiectivul jocului este de a **delimita cu piesele proprii un număr cât mai mare de puncte**.

Deși regulile sunt simple și pot fi învățate rapid, **Go** este un joc profund strategic — comparabil ca profunzime doar cu **șahul**.

---

## Caracteristici

- **Reguli complete** – implementare fidelă a logicii oficiale Go: capturi, libertăți, grupuri, teritoriu
- **Tablă personalizabilă** – dimensiuni standard **19×19**, dar și **13×13** sau **9×9** pentru partide scurte
- **Interfață grafică SFML** – meniu de configurare, tabla interactivă, panou de control, piatra „fantoma” (preview la hover)
- **3 nivele de bot (AI)** – *Ușor*, *Mediu* și *Avansat* (căutare Minimax cu tăiere alfa-beta)
- **Undo / Redo** – revenire sau refacere completă a mutărilor
- **Sugestie de mutare (hint)** pentru jucătorul uman
- **Scor în timp real** – capturi + komi, cu calculul final al teritoriului la închiderea partidei
- **Detecție automată a capturilor** și **interdicția sinuciderii**
- **Regula Ko (superko)** – nicio stare a tablei nu poate fi repetată
- **Pasare** și **cedare** pentru încheierea partidei
- **Test fum (headless)** – două boturi joacă o partidă scurtă fără interfață grafică (folosit și în CI)

---

## Cerințe de sistem

- **CMake ≥ 3.26**
- Un compilator C++ cu suport **C++23** (GCC 12+, Clang 16+, MSVC 19.35+)
- **SFML 2.6.1** – se descarcă și se construiește automat la prima configurare (prin `FetchContent`)
- Fișierul **`arial.ttf`** trebuie să existe lângă executabil (se copiază automat la instalare)
- Platforme suportate: **Windows** (MSVC / MinGW), **Linux** (GCC / Clang), **macOS** (Apple Clang / GCC)

---

## Construire și rulare

### Cu scriptul `scripts/cmake.sh`

```bash
# 1. Configurare
bash scripts/cmake.sh configure

# 2. Compilare
bash scripts/cmake.sh build

# 3. Instalare (copiază executabilul + arial.ttf în install_dir/bin)
bash scripts/cmake.sh install
```

### Direct cu CMake

```bash
cmake -S . -B build
cmake --build build -j6
```

Executabilul rezultat se numește **`oop`**.

### Build fără interfață grafică (headless)

```bash
cmake -S . -B build -DGO_HEADLESS=ON
cmake --build build -j6
```

În modul headless se rulează automat **testul fum**: două boturi (nivel *Ușor* și *Mediu*) joacă o partidă pe tabla 9×9, exercitând capturile, sinuciderea și regula Ko.

---

## Cum se joacă

1. La pornire apare **meniul de configurare**, unde alegi:
   - **Dimensiunea tablei** (9×9, 13×13, 19×19)
   - **Nivelul botului** (Ușor / Mediu / Avansat)
   - **Culoarea ta** (Negru / Alb)
2. În Go, **Negrul mută mereu primul**. Dacă alegi Alb, botul (Negru) mută primul, imediat ce se deschide fereastra.
3. Plasezi o piatră făcând **clic pe intersecția dorită** a tablei. La hover, piatra „fantoma” îți arată unde s-ar așeza piatra.
4. Panoul lateral conține butoanele:
   - **PASS** – pasezi tura (două pass-uri consecutive încheie partida)
   - **UNDO** / **REDO** – revii la o mutare anterioară / refaci o mutare
   - **CEDARE** – cedezi partida (pierzi automat, indiferent de scor)
   - **EXIT** – ieși din joc
5. Scorul live afișează **capturile + komi** pentru ambii jucători. Când e rândul tău, primești și o **sugestie de mutare**.

---

## Structura proiectului

```
├── CMakeLists.txt              # Configurarea build-ului (C++23, SFML, ținte)
├── cmake/                      # Module CMake (opțiuni, flag-uri de compilare)
├── scripts/                    # Scripturi: cmake, cppcheck, valgrind
├── .github/                    # GitHub Actions: cppcheck, clang-tidy, matrix de build
├── include/                    # Antetele (headers)
├── src/                        # Implementările (.cpp)
├── launcher.command            # Launcher pentru macOS
├── tastatura.txt               # Fișier de input (pentru rulare non-interactivă)
└── arial.ttf                   # Font folosit de interfața grafică
```

### Componente (arhitectură)

| Fișier | Rol |
| --- | --- |
| `Tabla` | Grila de joc: stocarea pietrelor, desenarea SFML a tablei |
| `GoLogic` | Motorul de reguli la nivel de tablă: grupuri, libertăți, capturi, interzicerea sinuciderii, hash (FNV-1a) pentru Ko, generarea mutărilor valide și calculul teritoriului (Tromp-Taylor simplificat) |
| `Reguli` | Starea regulilor: pass-uri consecutive (sfârșitul jocului) și istoricul stărilor pentru regula Ko |
| `Joc` | Orchesterarea partidei: tura, scoruri, komi (6.5), validarea și aplicarea mutărilor, undo/redo prin snapshots, calculul câștigătorului, sugestii de mutare |
| `Jucator` | Clasa abstractă de bază (uman sau bot) |
| `JucatorUman` | Mutările vin din interfața grafică |
| `JucatorBot` | Bot controlat de o strategie (pattern **Strategy**) |
| `IBotStrategy` | Interfața strategiei de joc a botului |
| `BotUsor` | Alege aleatoriu dintre toate mutările valide |
| `BotMediu` | Euristici: capturează grupurile adverse adiacente, salvează grupurile proprii în *Atari*, joacă lângă piesa adversarului |
| `BotAvansat` | Căutare **Minimax cu tăiere alfa-beta** (adâncime configurabilă) și evaluare pe teritoriu + pietre + capturi |
| `creeazaStrategie()` | Factory ce creează strategia potrivită nivelului ales |
| `Mutare` | O mutare (plasare sau pass) |
| `GoException` + derivate | Erori specifice jocului: coordonate invalide, poziție ocupată, regula Ko, mutare suicidala, joc terminat |

---

## Concepte POO folosite

Proiectul ilustrează principiile programării orientate pe obiecte și pattern-urile de design clasice:

### Principii fundamentale

| Concept | Aplicare în cod |
| --- | --- |
| **Încapsulare** | Toate datele sunt private (`Tabla::grila`, `Joc::tabla`, `Reguli::stariKo`), accesate doar prin getteri `const` marcați cu `[[nodiscard]]` |
| **Moștenire** | Trei ierarhii: `Jucator` → `JucatorUman` / `JucatorBot`; `IBotStrategy` → `BotUsor` / `BotMediu` / `BotAvansat`; `GoException` → 6 tipuri derivate de erori |
| **Polimorfism** | Funcții virtuale (`alegeMutare()`, `clone()`, `numeStrategie()`) apelate prin referințe/pointeri la clasa de bază |
| **Clase abstracte** | `Jucator` și `IBotStrategy` au metode pur virtuale → nu pot fi instanțiate, doar derivatele lor concrete |
| **Abstracție** | Interfața `IBotStrategy` ascunde algoritmul fiecărui bot; `GoLogic` izolează motorul de reguli de restul jocului |
| **Compoziție** | `Joc` conține `Tabla` și `Reguli`; `JucatorBot` conține un `IBotStrategy` (relație *has-a*) |
| **Agregare** | `Joc` primește jucătorii prin referințe (`Jucator&`) – proprietarul rămâne `main`, nu `Joc` |

### Pattern-uri de design

| Pattern | Aplicare în cod |
| --- | --- |
| **Strategy** | `IBotStrategy` + cele trei strategii concrete (`BotUsor`, `BotMediu`, `BotAvansat`), interschimbabile la runtime |
| **Factory** | `creeazaStrategie(NivelBot)` creează strategia potrivită nivelului ales |
| **Prototype** | `clone()` pe `Jucator` și `IBotStrategy` permite duplicarea (deep copy) fără a cunoaște tipul concret |

### Tehnici moderne de C++

- **RAII** – resursele (strategii `std::unique_ptr`, obiecte SFML, snapshots) se eliberează automat la distrugerea obiectului
- **Regula celor cinci** – `JucatorBot` implementează copy constructor și copy assignment cu deep copy a strategiei (prin `clone()`) + move `noexcept` implicit
- **Gestionarea excepțiilor** – ierarhia `GoException` cu tipuri specifice pentru fiecare regulă încălcată (Ko, sinucidere, poziție ocupată, etc.), afișate sugestiv în interfață
- **Suprascrierea operatorilor** – `Pozitie::operator<`, `operator==` (folosite în `std::set`/`std::tuple`) și `operator<<` pentru afișare
- **Const-correctness & design-by-contract** – metode `const`, `const`-referințe la parametri, `[[nodiscard]]` pentru funcțiile care nu trebuie ignorate
- **Namespace** – `GoLogic` grupează logic motorul de reguli la nivel de tablă

---

## Regulile de bază

1. **Mutarea**
   - Jucătorii plasează alternativ câte o piatră pe o intersecție goală.
   - Pietrele nu se mută, dar pot fi capturate.

2. **Libertăți**
   - Fiecare piatră are *libertăți* = intersecțiile libere adiacente (sus, jos, stânga, dreapta).
   - Dacă toate libertățile sunt ocupate de adversar → piatra este **capturată** și scoasă de pe tablă.

3. **Grupuri**
   - Pietrele adiacente formează un **grup** și își împart libertățile.
   - Grupurile pot fi capturate doar dacă toate libertățile lor sunt eliminate.

4. **Interdicția sinuciderii**
   - Nu este permisă plasarea unei pietre care și-ar lăsa propriul grup fără libertăți (excepție: dacă prin mutare capturezi piese adverse).

5. **Regula Ko (superko)**
   - Nu este permisă o mutare care ar readuce tabla într-o stare deja atinsă vreodată în partidă (se folosesc hash-uri FNV-1a ale stărilor).

6. **Pasarea**
   - Un jucător poate **pasa** dacă nu mai are mutări utile.
   - Când ambii jucători pasează consecutiv → jocul se termină.

7. **Punctajul**
   - La final, fiecare jucător numără:
     - Numărul de **intersecții controlate** (teritoriu)
     - + **Pietrele capturate**
   - Jucătorul cu cel mai mare total câștigă.

---

## Regula *Komi*

Pentru a compensa avantajul jucătorului cu negru (care începe primul), jucătorul cu alb primește **komi** – de obicei **6.5 puncte** adiționale.

---

## Teste și calitatea codului

- **Test fum headless** – rulează două boturi pe tabla 9×9 și verifică că toate mutările sunt valide (activ prin `GO_HEADLESS` sau `CH_CI_RUN`)
- **GitHub Actions** – pipeline CI cu:
  - **Cppcheck** și **Clang-Tidy** (analiză statică)
  - Matrix de build pe **Ubuntu** (MSan, ASan, Valgrind), **macOS** și **Windows** (MSVC, MinGW)
  - Publicare automată de **artifacts** și **release-uri** la push pe tag-uri

---

## Sfaturi rapide

- Controlează **colțurile** și **marginile** înaintea centrului.
- Evită luptele inutile dacă poți obține teritoriu sigur.
- Fiecare piatră trebuie să contribuie la o strategie (protecție, expansiune, atac).

---

## Resurse utile

- [Learn to Play Go – online-go.com](https://online-go.com/learn-to-play-go)
- [Sensei’s Library – Enciclopedia Go](https://senseis.xmp.net)
