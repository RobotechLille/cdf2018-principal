# VARIABLES

## Compilateur
CC=gcc
# Bibliothèques
LIBS=
## Drapeaux pour le linker
LDFLAGS_CUSTOM += -lpthread -lwiringPi
## Drapeaux pour le compilateur
CFLAGS=
## Générateurs de drapeaux pour les bibliothèques
PKG_CONFIG=pkg-config

# VARIABLES AUTOMATIQUES
ifdef LIBS
	LDFLAGS_CUSTOM += $(shell $(PKG_CONFIG) --libs $(LIBS))
	CFLAGS += $(shell $(PKG_CONFIG) --cflags $(LIBS))
endif

# Par défaut on affiche les warnings et on ajoute les symboles de debug pour utiliser GDB
CFLAGS += -Wall -Wextra -pedantic -g -DDEBUG
# buildroot se charge de remplacer ces flags avec des optimisations

# RÈGLES AUTOMATIQUES DE COMPILATION

# Génération des fichiers éxecutables
bin/%: obj/%.o
	$(CXX) $(LDFLAGS_CUSTOM) $^ -o $@
# On enlève les symboles inutiles pour gagner en temps de chargement de l'éxecutable
ifeq ($(DEBUG),no)
	strip $@
endif

# RÈGLES DE COMPILATION

# Règle éxecutée par défaut (quand on fait juste `make`)
default: bin/testpin bin/premier bin/local bin/testI2c

# Binaires (dont il faut spécifier les objets explicitement)
bin/premier: obj/CF.o obj/movement.o obj/debug.o obj/position.o
bin/testPin: obj/testPin.o
bin/testI2c: obj/testI2c.o obj/i2c.o obj/srf08.o obj/lcd.o

bin/local: obj/local.o obj/debug.o
	$(CC) -lpthread $^ -o $@

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
