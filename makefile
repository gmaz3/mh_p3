########################################################
CC=g++
CFLAGS= -O2
EJS = busquedaLocalReiterada-ES busquedaLocalReiterada busquedaMultiBasica enfriamientoSimulado
# ########################################################
OBJECTSP3_ILS_ES = src/busquedaLocalReiterada-ES.cpp
OBJECTSP3_ILS = src/busquedaLocalReiterada.cpp
OBJECTSP3_BMB = src/busquedaMultiBasica.cpp
OBJECTSP3_ES = src/enfriamientoSimulado.cpp
# ########################################################

.PHONY: all
all: $(EJS)

busquedaLocalReiterada-ES: $(OBJECTSP3_ILS_ES)
	$(CC) $(CFLAGS) -o bin/busquedaLocalReiterada-ES $(OBJECTSP3_ILS_ES)

busquedaLocalReiterada: $(OBJECTSP3_ILS)
	$(CC) $(CFLAGS) -o bin/busquedaLocalReiterada $(OBJECTSP3_ILS)

busquedaMultiBasica: $(OBJECTSP3_BMB)
	$(CC) $(CFLAGS) -o bin/busquedaMultiBasica $(OBJECTSP3_BMB)

enfriamientoSimulado: $(OBJECTSP3_ES)
	$(CC) $(CFLAGS) -o bin/enfriamientoSimulado $(OBJECTSP3_ES)


.PHONY: clean
clean:
	rm -rf bin/*
