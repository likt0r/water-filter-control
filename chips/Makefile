# Baut den Wokwi-Custom-Chip zu WebAssembly.
# Voraussetzung: clang/LLVM mit wasm32-Target + wasm-ld (Paket "lld").
# Zusaetzlich wird der Wokwi-API-Header benoetigt (siehe WOKWI.md):
#   curl -O https://raw.githubusercontent.com/wokwi/wokwi-chip-c-template/main/wokwi-api.h
#
# Danach:  make   -> erzeugt flow-pulse.chip.wasm
CLANG ?= clang
SOURCES = flow-pulse.chip.c

flow-pulse.chip.wasm: $(SOURCES) wokwi-api.h
	$(CLANG) --target=wasm32-unknown-wasi -nostartfiles -Wl,--import-memory \
	  -Wl,--export-table -Wl,--no-entry -Wl,--export=chip_init \
	  -Wl,--allow-undefined -o $@ $(SOURCES)

clean:
	rm -f flow-pulse.chip.wasm
