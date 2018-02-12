# VARIABLES

## Compilateur
CC=gcc
# Bibliothèques
LIBS=
## Drapeaux pour le linker
LDFLAGS=
## Drapeaux pour le compilateur
CFLAGS=
## Générateurs de drapeaux pour les bibliothèques
PKG_CONFIG=pkg-config

# VARIABLES AUTOMATIQUES
ifdef LIBS
	LDFLAGS += $(shell $(PKG_CONFIG) --libs $(LIBS))
	CFLAGS += $(shell $(PKG_CONFIG) --cflags $(LIBS))
endif

# Par défaut on affiche les warnings et on ajoute les symboles de debug pour utiliser GDB
CFLAGS += -Wall -Wextra -pedantic -g -DDEBUG
# buildroot se charge de remplacer ces flags avec des optimisations

# RÈGLES AUTOMATIQUES DE COMPILATION

# Génération des fichiers éxecutables
bin/%: obj/%.o
	$(CC) $(LDFLAGS) $^ -o $@
# On enlève les symboles inutiles pour gagner en temps de chargement de l'éxecutable
ifeq ($(DEBUG),no)
	strip $@
endif

# RÈGLES DE COMPILATION

# Règle éxecutée par défaut (quand on fait juste `make`)
default: bin/testpin bin/premier

# Binaires (dont il faut spécifier les objets explicitement)
bin/premier: obj/common.o obj/serial.o
bin/testPin: obj/testPin.o
	$(CC) $(LDFLAGS) $^ -lwiringPi -o $@

# Génération des fichiers objets
obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) -c $< -o $@
obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# OUTILS DE DÉVELOPPEMENT

# Supprime les fichiers automatiquement générés
clean:
	rm -f obj/*
	rm -f bin/*

# Au cas où des fichiers avec certains noms existent, les règles suivantes seront executées quand même
.PHONY: default clean
