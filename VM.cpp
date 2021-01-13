#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <string>
using namespace std;
int pasirinkimas()
{
    int x;
    cout << "(1) Decrypt (Atkoduoti) \n(2) Encrypt (Sukoduoti) \n";
    cin >> x;
    if (x!=1 && x!=2)
    {
        pasirinkimas();
    }
    return x;
}

int main()
{
    unsigned char regs[16] = {0};
    unsigned char komandos[256];
    bool eof_flag = false;
    bool flag = false;
    int rx = 0, ry = 0;
    ifstream binFile("decryptormano.bin", ios::binary); // ios::binary -> naudojamas , jog eitu nuskaityti bin faila.
    ifstream binEnFile("encryptormano.bin", ios::binary);
    ifstream encrFile("q1_encr.txt");
    ofstream rezultFile("Rezultatas.txt");
    int binCount = 0;
    int x = pasirinkimas();
    int i = 0;
    while(true)
    {
        if (x==1) {
                if(binFile.eof()) // Ziuri ar dar ne END OF FILE.
                break;

            binFile >> noskipws >>  komandos[binCount]; // Nepraleidzia tarpu , nes gali buti space, kaip simbolis.
            cout << setbase(16) << (int)komandos[binCount] << " "; // HEX BASE
            binCount++;

        }
        else
        {
            if (binEnFile.eof())
                break;

            binEnFile >> noskipws >>  komandos[binCount]; // Nepraleidzia tusciu vietu.
            cout << setbase(16) << (int)komandos[binCount] << " "; // HEX BASE
            binCount++;

        }

    }
    cout << setbase(10)  << endl;


    while (true)
    {
        if (komandos[i] == 0x01) { // Registra Rx padidina vienetu.
            cout << "INC";
            if ((int)regs[i+1]==0) flag = false; else flag = true;
            regs[(int)komandos[i+1]]++; // i+1 nes tikrina duotus registrus
            i+=2; // Pereina i kita registra
        } else if (komandos[i] == 0x02) { // Registra Rx sumazina vienetu
            cout << "DEC";
            if ((int)regs[i+1]==1) flag = true; else flag = false;
           regs[(int)komandos[i+1]]--;
            i+=2;
        } else if (komandos[i] == 0x03) { // Kopijuoja registro Ry turini i registra Rx
            cout << "MOV";
            rx = (int)(komandos[i+1] & 0x0F);
            ry = (int)(komandos[i+1] >> 4);
            if ((int)regs[ry]==0) flag = true;else flag = false;
            regs[rx]= regs[ry];
            i+=2;
        } else if (komandos[i] == 0x04) { // Kopijuoja baito konstantą į registrą 𝑅0
            cout << "MOVC";
             if ((int)regs[i+1]==0) flag = true;else flag = false;
            regs[0] = (int)komandos[i+1];
            i+=2;
        } else if (komandos[i] == 0x05) { // Registro 𝑅𝑥 postūmis į kairę per vieną bitą.
            cout << "LSL";
            rx = (int)(komandos[i+1] & 0x0F);
            if ((int)(regs[rx] << 1)==0) flag = true;else flag = false;
            regs[rx] = regs[rx] << 1;
            i+=2;
        } else if (komandos[i] == 0x06) { // Registro 𝑅𝑥 postūmis į dešinę per vieną bitą.
            cout << "LSR";
            rx = (int)(komandos[i+1] & 0x0F);
            if ((int)(regs[rx] >> 1)==0) flag = true;else flag = false;
            regs[rx] = regs[rx] >> 1;
            i+=2;
        } else if (komandos[i] == 0x07) { // Šuolis santykiniu adresu pridedant konstantą su ženklu prie komandų skaitiklio.
            cout <<  setbase(10) <<"JMP " << (char)komandos[i+1];
            i = i + (int)(komandos[i+1])-256;
            while (i > binCount) { i = i - binCount;}
        } else if (komandos[i] == 0x08) { // Šuolis santykiniu adresu pridedant konstantą su ženklu prie komandų skaitiklio, jeigu yra nulio požymis (angl. flag on)
            cout << "JZ";
            if (flag)
            {
              i = i + (int)(komandos[i+1]);
              if (i > binCount) { i = i - binCount;}
            }else
            i+=2;
        } else if (komandos[i] == 0x09) { // Šuolis santykiniu adresu pridedant konstantą su ženklu prie komandų skaitiklio, jeigu nėra nulio požymio (angl. flag off)
            cout << "JNZ";
            if (!flag)
            {
              i = i + (int)(komandos[i+1]);
              if (i > binCount) { i = i - binCount;}
            }else
            i+=2;
        } else if (komandos[i] == 0x0A) // Šuolis santykiniu adresu pridedant konstantą su ženklu prie komandų skaitiklio, jeigu yra failo pabaigos požymis ( IN
        {cout << "JFE";
        if (eof_flag)
            {
              i = i + (int)(komandos[i+1]);
              if (i > binCount) { i = i - binCount;}
            }else
            i+=2;
        } else if (komandos[i] == 0x0B) { // Virtuali mašina baigia darbą.
            cout << "RET";
            break;
        } else if (komandos[i] == 0x0C) { // SUDĖTIS tarp registrų: 𝑅𝑥=𝑅𝑥+𝑅𝑦
            cout << "ADD";
            rx = (int)(komandos[i+1] & 0x0F);
            ry = (int)(komandos[i+1] >> 4);
            regs[rx]= regs[rx] + regs[ry];
            if ((int)regs[rx]==0) flag = true;else flag = false;
            i+=2;
        } else if (komandos[i] == 0x0D) { // ATIMTIS tarp registrų: 𝑅𝑥=𝑅𝑥−𝑅𝑦
            cout << "SUB";
            rx = (int)(komandos[i+1] & 0x0F);
            ry = (int)(komandos[i+1] >> 4);
            regs[rx]= regs[rx] - regs[ry];
            if ((int)regs[rx]==0) flag = true;else flag = false;
            i+=2;
        } else if (komandos[i] == 0x0E) { // IŠIMTINIO ARBA operacijų tarp registrų: 𝑅𝑥=𝑅𝑥⨁𝑅𝑦 (XOR)
            cout << "XOR";
            rx = (int)(komandos[i+1] & 0x0F);
            ry = (int)(komandos[i+1] >> 4);
            regs[rx] = regs[rx] ^ regs[ry];
            if ((int)regs[rx]==0) flag = true;else flag = false;
            i+=2;
        } else if (komandos[i] == 0x0F) { // ARBA operacija tarp registrų: 𝑅𝑥=𝑅𝑥∨𝑅𝑦 (OR)
            cout << "OR";
            rx = (int)(komandos[i+1] & 0x0F);
            ry = (int)(komandos[i+1] >> 4);
            regs[rx] = regs[rx] | regs[ry];
            if ((int)regs[rx]==0) flag = true;else flag = false;
            i+=2;
        } else if (komandos[i] == 0x10) { // Skaito vieną baitą iš duomenų failo(priskiria 𝑅𝑥) ir nustato failo pabaigos požymį jeigu pasiekta failo pabaiga.
            cout << "IN";
            char c;
            encrFile >> c;
            rx = (int)komandos[i+1];
            regs[rx] = int(c);
            if (encrFile.eof()) eof_flag = true;
            i+=2;
        } else if (komandos[i] == 0x11) { // Registro 𝑅𝑥 turinį išveda į failą.
            cout << "OUT";
            rx = (int)komandos[i+1];
            rezultFile << regs[rx];
            i+=2;
        } else {
            cout << "\n Nenumatyta instrukcija  " << setbase(16) << to_string(komandos[i]) << endl;
            break;
        }
        cout <<endl;

cout <<setbase(10)<< "\n";

    }

    return 0;
}
