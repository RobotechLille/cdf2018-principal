# VARIABLES

## Compilateur
CC=gcc
# Bibliothèques
LIBS=
## Drapeaux pour le linker
LDFLAGS_CUSTOM += -lpthread -lwiringPi
## Drapeaux pour le compilateur
CFLAGS_CUSTOM += -g
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
	$(CC) $(LDFLAGS) $(LDFLAGS_CUSTOM) $^ -o $@
	$(OBJCOPY) --only-keep-debug $@ $@.debug
	$(STRIP) --strip-debug --strip-unneeded $@

# RÈGLES DE COMPILATION

# Règle éxecutée par défaut (quand on fait juste `make`)
default: bin/testpin bin/premier bin/local bin/testI2c

# Binaires (dont il faut spécifier les objets explicitement)
OBJS=CF movement debug position ihm lcd i2c points parcours
bin/premier: $(addprefix obj/,$(addsuffix .o,$(OBJS)))
bin/testPin: obj/testPin.o
# TODO ↑ Enlever (remplacé par IHM)
bin/testI2c: obj/testI2c.o obj/i2c.o obj/srf08.o obj/lcd.o

# Programme de test sur PC, n'embarquant pas wiringPi
bin/local: obj/local.o obj/debug.o
	$(CC) $(CFLAGS) $(CFLAGS_CUSTOM) -lpthread $^ -o $@

# Génération des fichiers objets
obj/%.o: src/%.c src/%.h
	$(CC) $(CFLAGS) $(CFLAGS_CUSTOM) -c $< -o $@
obj/%.o: src/%.c
	$(CC) $(CFLAGS) $(CFLAGS_CUSTOM) -c $< -o $@

# OUTILS DE DÉVELOPPEMENT

# Supprime les fichiers automatiquement générés
clean:
	rm -f obj/*
	rm -f bin/*

# Au cas où des fichiers avec certains noms existent, les règles suivantes seront executées quand même
.PHONY: default clean
