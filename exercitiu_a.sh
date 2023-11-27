#!/bin/bash

# Verifica daca numarul de argumente este corect
if [ "$#" -ne 1 ]; then
    echo "Nu sunt destule argumente"
    exit 1
fi

# Caracterul primit ca argument
char="$1"

# Contor pentru propozitiile corecte
corecte=0

# Functie pentru a verifica daca o propozitie este corecta
function is_propozitie_corecta {
    propozitie="$1"
    
    # Verifica daca propozitia incepe cu litera mare
    if [[ ! "$propozitie" =~ ^[A-Z] ]]; then
        return 1
    fi

    # Verifica daca propozitia respecta conditiile date si contine caracterul specificat
    if [[ "$propozitie" =~ $char && ! "$propozitie" =~ ,.*[[:space:]]si && "$propozitie" =~ ^[A-Za-z0-9\ ,.!]+[?!.,]$ ]]; then
        return 0
    fi

    return 1
}

# Citeste linii de la intrarea standard pana la end-of-file
while IFS= read -r propozitie || [ -n "$propozitie" ]; do
    # Verifica daca propozitia este corecta si contorizeaza
    if is_propozitie_corecta "$propozitie"; then
        ((corecte++))
    fi
done

# Afiseaza rezultatul
echo $corecte
