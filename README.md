### Go 

-Un joc clasic de strategie japonez implementat în C++.  
Pe scurt, obiectivul jocului este de a delimita cu piesele proprii un număr cât mai mare de puncte. Regulile de joc sunt foarte simple și pot fi învățate cu ușurință. Totuși este un joc complex, comparabil în această privință numai cu șahul.
---

## Caracteristici

- **Reguli complete** – implementare a logicii oficiale de Go (capturi, libertăți, teritorii).
# Regulile de bază ale jocului Go

Go este un joc de strategie pentru doi jucători, originar din Asia, jucat pe o tablă numită **goban**.

---

## Scopul jocului
Câștigă jucătorul care controlează cea mai mare parte a tablei — adică are cele mai multe **puncte de teritoriu** + **pietre capturate**.

---

## Componente
- Tabla are 19×19 linii (sau 13×13 / 9×9 pentru jocuri scurte)
- Două seturi de piese:
  - **Negre (Black)** – încep primele
  - **Albe (White)** – joacă după

---

## Regulile de bază

1. **Mutarea**
   - Jucătorii pun pe rând câte o piatră pe o intersecție goală.
   - O piatră nu se mai mută, dar poate fi capturată.

2. **Libertăți**
   - Fiecare piatră are „libertăți” = intersecțiile libere adiacente (sus, jos, stânga, dreapta).
   - Dacă toate libertățile sunt ocupate de adversar → piatra este **capturată** și scoasă de pe tablă.

3. **Grupuri**
   - Pietrele conectate pe linii directe formează un **grup** și își împart libertățile.
   - Grupurile pot fi capturate doar dacă toate libertățile lor sunt eliminate.

4. **Ko (interdicția de repetiție)**
   - Nu ai voie să faci o mutare care reface exact aceeași poziție anterioară.

5. **Pasarea**
   - Un jucător poate **pasa** dacă nu mai are mutări avantajoase.
   - Când ambii pasează consecutiv → jocul se termină.

6. **Punctajul**
   - La final, fiecare jucător numără:
     - Numărul de **intersecții controlate**
     - + **Pietrele capturate**
   - Jucătorul cu cel mai mare total câștigă.

---

## Regula Komi
Pentru a echilibra avantajul jucătorului cu negru (care începe primul), jucătorul cu alb primește **komi** — de obicei **6.5 puncte** adiționale.

---

##  Sfaturi rapide
- Încearcă să controlezi colțurile și marginile mai întâi.
- Evită luptele inutile dacă poți câștiga teritoriu sigur.
- Fiecare piatră ar trebui să aibă un scop (protecție, extindere, atac).

---

**Resurse utile:**
- [https://online-go.com/learn-to-play-go](https://online-go.com/learn-to-play-go)
- [https://senseis.xmp.net](https://senseis.xmp.net)

