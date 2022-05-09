#include <stddef.h>
#include <stdint.h>

#include <EVE.h>
#include "assets.h"


/*('file properties: ', 'resolution ', 350, 'x', 24, 'format ', 'L8', 'stride ', 350, ' total size ', 8400)*/ 
const Image_header_t img_bar_format = {EVE_FORMAT_L8, 480, 24, 480, 11520, 0};
const unsigned char img_bar[] __attribute__((aligned(4))) = {
#include "bar_480x24_L8.rawh"
};

/*('file properties ', 'format ', 'L4', ' stride ', 11, ' width ', 22, 'height', 24)*/ 
const Image_header_t img_orbitron_format = {EVE_FORMAT_L4, 22, 24, 11, 33000 + 148, 1};
const unsigned char font_orbitron[] __attribute__((aligned(4))) = 
#include "Orbitron-Medium_18_L4.rawh"
;

uint32_t Theme_main = 0x00c0c0;
uint32_t Theme_colours[] =
{
	0x008080, // teal
	0x3CB371, // seagreen
	0xFFFF00, // yellow
	0xE3CF57, // banana
	0x9C661F, // brick
	0xFF7F50, // coral
	0xFF3030, // firebrick
	0xC67171, // salmon
	0x32CD32, // limegreen
	0x708090, // slategrey
	0x000080, // navy
	0xADFF2F, // greenyellow
	0x00C78C, // turquiose
	0x87CEFF, // skyblue
	0xDA70D6, // orchid
	0x800080, // purple
};
