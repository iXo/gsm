# Makefile (GSM)
# --------------

# Auteur :      L-Sebastien
# Creer le :    12 août 2008
# Modifier le : 12 août 2008


# Repertoire racine du projet
REP_RACINE=../..

# Repertoire pour les inclusions de fichiers
CXXFLAGS:=$(CXXFLAGS) \
	-I include \
	-I $(REP_RACINE)/sources/DesignPatterns/Singleton/include \
	-I $(REP_RACINE)/librairies/gsmlib_v1.10

ifeq ($(WIN32),yes)
    CXXFLAGS:=$(CXXFLAGS) -I $(REP_RACINE)/librairies/gsmlib_v1.10/win32 -D WIN32 -DHAVE_CONFIG_H -Wno-non-template-friend
else
    CXXFLAGS:=$(CXXFLAGS) -I $(REP_RACINE)/librairies/gsmlib_v1.10/unix -DHAVE_CONFIG_H -Wno-non-template-friend
endif


.PHONY: all clean module description progtest

all: module

clean:
	@echo
	@echo " ------------------------------- "
	@echo "|   Nettoyage du Module 'GSM'   |"
	@echo " ------------------------------- "

	@rm -fr obj/*
	@rm -fr test/GSM.exe

module: description obj/SMS.o obj/GSM.o

description:
	@echo
	@echo " ------------------------------------------- "
	@echo "|   Compilation du Module 'GSM' ($(MODE))   |"
	@echo " ------------------------------------------- "

progtest: test/GSM.exe


test/GSM.exe: test/GSM.cpp obj/GSM.o obj/SMS.o $(REP_RACINE)/librairies/gsmlib_v1.10/lib/libGSM.a
	@echo Compilation du programme 'GSM.exe'
	@$(CXX) $(CXXFLAGS) -o $@ $^

obj/GSM.o: src/GSM.cpp include/GSM.h
	@echo Update du fichier $@
	@$(CXX) $(CXXFLAGS) -o $@ -c $<

obj/SMS.o: src/SMS.cpp include/SMS.h
	@echo Update du fichier $@
	@$(CXX) $(CXXFLAGS) -o $@ -c $<


#include/GSM.h: $(REP_RACINE)/sources/DesignPatterns/Singleton/include/Singleton.h
