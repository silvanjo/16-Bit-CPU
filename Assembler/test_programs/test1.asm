# Beispielprogramm: Zwei Werte addieren und vergleichen

# Lade 10 in r1
li r1 10

# Lade 10 in r2
li r2 10

# Addiere r1 und r2, speichere in r3 → r3 = 20
add r1 r2 r3

# Lade Adresse 5 in r0 (fiktive Adresse, z. B. Speicherort für Ergebnis)
li r0 5

# Speichere Ergebnis aus r3 an Adresse [r0 + 0]
store r0 r3 0

# Vergleiche r1 und r2, springe -2 Zeilen zurück, wenn gleich
beq r1 r2 -2

# Springe zur Adresse 0 (Programmende simulieren)
jump 0
