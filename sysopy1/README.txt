Calosc pisana byla na Windowsie 10 i wpadlem na bardzo duzo problemow.
sys/times.h nie istnialo z jakiegos powodu i musialem z internetu jakas implementacje niezalezna wrzucic do biblioteka.c
mimo wszystko czasy licza sie jakos beznadziejnie i (double)(_E.tms_stime-_S.tms_stime) daje ciagle 0
a w innych przypadkach czas sie liczy tylko dla polecenia wc, cala reszta wyglada jakby sie robila natychmiastowo.

biblioteki dynamiczne i wspoldzielone w ogole nie chcialy dzialac idac po materialach pomocniczych wiec
nie mialem mozliwosci ich zrobic w czasie skonczonym (czyt. przed piatkiem).

make jakos dziala i sie testuje na mojej maszynie, ale czy na faktycznym linuksie sie zbuduje, to nie mam pojecia.

w raport2.txt
pierwsza kolumna to wynik z czytania
(realE.tv_sec - realS.tv_sec) * 1000000 + realE.tv_usec - realS.tv_usec,
czyli moj czas rzeczywisty;

druga kolumna to
(double)(_E.tms_utime- _S.tms_utime), 
czyli user time

a trzecia
(double)(_E.tms_stime-_S.tms_stime),
system time