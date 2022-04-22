This font contains extra characters and symbols from the Arial font which are 
not ANSI.

Command lines to create font files for this project.

Create the arial_utf8_extra.txt15 folder and files.
	> fnt_cvt.exe -i arial.ttf -s 15 -d 13100 -u utf8_extra.txt

The contents of the file arial_utf8_extra.txt15\L4\arial.ttf_15_L4.rawh were
then copied into the "Fonts" folder of the project. These are converted to
a C file with the variables:
	const uint32_t font_arial_ttf_15_L4_size; /* Size of font data */
	const uint8_t __flash__ font_arial_ttf_15_L4[]; /* Font data */
	 
The utf8_extra font collection has the following characters mapped:

← 	\x01
↑	\x02
→	\x03
↓	\x04
│	\x05

Extra general characters:
¬	\x10
£ 	\x11
€ 	\x12
¢ 	\x13
¥ 	\x14

For German keyboards:
§	\x20
Ü	\x21
ü	\x22
Ö	\x23
ö	\x24
Ä	\x25
ä	\x26
º	\x27
ˊ	\x28
ˋ	\x29
ß	\x2a


