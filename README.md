# Go

Un joc clasic de strategie japonez, implementat în **C++**.  
Obiectivul jocului este de a **delimita cu piesele proprii un număr cât mai mare de puncte**.  

Deși regulile sunt simple și pot fi învățate rapid, **Go** este un joc profund strategic — comparabil ca profunzime doar cu **șahul**.

---

## Caracteristici

- **Reguli complete** – implementare fidelă a logicii oficiale Go (capturi, libertăți, teritorii)
- **Tablă personalizabilă** – dimensiuni standard 19×19, dar și 13×13 sau 9×9 pentru partide scurte
- **Detecție automată a capturilor**
- **Calcul corect al scorului final (territoriu + capturi + komi)**
- **Suport pentru pasare și încheiere de partidă**

---

## Scopul jocului

Câștigă jucătorul care controlează cea mai mare parte a tablei — adică are cele mai multe:
- **puncte de teritoriu**
- **pietre capturate**

---

## Componentele jocului

- **Tabla (Goban)** – grilă de 19×19 intersecții (sau variante mai mici)
- **Două seturi de piese:**
  - **Negre (Black)** – încep primele  
  - **Albe (White)** – joacă după

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

4. **Regula Ko (interdicția de repetiție)**
   - Nu este permisă o mutare care recreează exact aceeași poziție anterioară.

5. **Pasarea**
   - Un jucător poate **pasa** dacă nu mai are mutări utile.  
   - Când ambii jucători pasează consecutiv → jocul se termină.

6. **Punctajul**
   - La final, fiecare jucător numără:
     - Numărul de **intersecții controlate** (teritoriu)
     - + **Pietrele capturate**
   - Jucătorul cu cel mai mare total câștigă.

---

## Regula *Komi*

Pentru a compensa avantajul jucătorului cu negru (care începe primul), jucătorul cu alb primește **komi** – de obicei **6.5 puncte** adiționale.

---

## Sfaturi rapide

- Controlează **colțurile** și **marginile** înaintea centrului.  
- Evită luptele inutile dacă poți obține teritoriu sigur.  
- Fiecare piatră trebuie să contribuie la o strategie (protecție, expansiune, atac).

---

## Resurse utile

- [Learn to Play Go – online-go.com](https://online-go.com/learn-to-play-go)  
- [Sensei’s Library – Enciclopedia Go](https://senseis.xmp.net)

