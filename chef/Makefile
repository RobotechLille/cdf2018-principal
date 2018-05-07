# VARIABLES

## Compilateur
CC=gcc
# Bibliothèques
LIBS=
## Drapeaux pour le linker
LDFLAGS_CUSTOM += -lpthread -lwiringPi -lm
## Drapeaux pour le compilateur
CFLAGS_CUSTOM += -g
## Générateurs de drapeaux pour les bibliothèques
PKG_CONFIG=pkg-config
## Nom des objets communs
OBJS=actionneurs buttons CA CF debug diagnostics i2c imu ihm lcd motor movement parcours points position
OBJS_O=$(addprefix obj/,$(addsuffix .o,$(OBJS)))

# VARIABLES AUTOMATIQUES
ifdef LIBS
	LDFLAGS_CUSTOM += $(shell $(PKG_CONFIG) --libs $(LIBS))
	CFLAGS += $(shell $(PKG_CONFIG) --cflags $(LIBS))
endif

# Par défaut on affiche les warnings et on ajoute les symboles de debug pour utiliser GDB
CFLAGS += -Wall -Wextra -pedantic -g -DDEBUG
# buildroot se charge de remplacer ces flags avec des optimisations

# RÈGLES DE COMPILATION

# Règle éxecutée par défaut (quand on fait juste `make`)
default: bin/premier bin/local $(subst src,bin,$(subst .c,,$(wildcard src/test*.c)))

# Génération des fichiers éxecutables
bin/%: obj/%.o $(OBJS_O)
	$(CC) $(LDFLAGS) $(LDFLAGS_CUSTOM) $^ -o $@
	$(OBJCOPY) --only-keep-debug $@ $@.debug
	$(STRIP) --strip-debug --strip-unneeded $@

# Binaires (dont il faut spécifier les objets explicitement)
bin/premier: obj/premier.o $(OBJS_O)
bin/test%: obj/test%.o $(OBJS_O)

# Programme de test sur PC, n'embarquant pas wiringPi
bin/local: obj/local.o
	$(CC) $(CFLAGS) $(CFLAGS_CUSTOM) -lpthread -lm $^ -o $@

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
