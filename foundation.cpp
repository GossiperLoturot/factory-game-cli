#include "foundation.h"

string item_to_string(Item item) {
	switch (item) {
	case ITEM_WATER:
		return "Water";
	case ITEM_HYDROGEN:
		return "Hydrogen";
	case ITEM_OXYGEN:
		return "Oxygen";

	case ITEM_SILICON:
		return "Silicon";
	case ITEM_SILICON_WAFER:
		return "Silicon Wafer";
	case ITEM_CIRCUIT_WAFER:
		return "Circuit Wafer";
	case ITEM_CIRCUIT:
		return "Circuit";
	case ITEM_SOLDERING_IRON:
		return "Soldering Iron";
	case ITEM_CIRCUIT_BOARD:
		return "Circuit Board";
	case ITEM_CHIP:
		return "Chip";

	default:
		return "Unknown";
	}
}