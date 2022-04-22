#include <stddef.h>
#include <stdint.h>

#include <EVE.h>
#include "assets.h"


/*('file properties: ', 'resolution ', 9, 'x', 160, 'format ', 'ARGB1555', 'stride ', 18, ' total size ', 2880)*/
const Image_header_t img_needle_format = {EVE_FORMAT_ARGB1555, 9, 160, 18, 2880};
const unsigned char img_needle[] __attribute__((aligned(4))) = {
#include "GUI1_Needle_9x160_ARGB1555.rawh"
};

/*('file properties: ', 'resolution ', 37, 'x', 37, 'format ', 'ARGB1555', 'stride ', 74, ' total size ', 2738)*/ 
const Image_header_t img_gbox_format = {EVE_FORMAT_ARGB1555, 37, 37, 74, 2738};
const unsigned char img_gbox[] __attribute__((aligned(4))) = {
#include "GUI1_Gear_Highlighter_37x37_ARGB1555.rawh"
};
