static const struct {unsigned short ucs; char mac;} mactable[] = {
{0x00A0, 0xCA},
{0x00A1, 0xC1},
{0x00A2, 0xA2},
{0x00A3, 0xA3},
{0x00A4, 0xDB},
{0x00A5, 0xB4},
{0x00A7, 0xA4},
{0x00A8, 0xAC},
{0x00A9, 0xA9},
{0x00AA, 0xBB},
{0x00AB, 0xC7},
{0x00AC, 0xC2},
{0x00AE, 0xA8},
{0x00AF, 0xF8},
{0x00B0, 0xA1},
{0x00B1, 0xB1},
{0x00B4, 0xAB},
{0x00B5, 0xB5},
{0x00B6, 0xA6},
{0x00B7, 0xE1},
{0x00B8, 0xFC},
{0x00BA, 0xBC},
{0x00BB, 0xC8},
{0x00BF, 0xC0},
{0x00C0, 0xCB},
{0x00C1, 0xE7},
{0x00C2, 0xE5},
{0x00C3, 0xCC},
{0x00C4, 0x80},
{0x00C5, 0x81},
{0x00C6, 0xAE},
{0x00C7, 0x82},
{0x00C8, 0xE9},
{0x00C9, 0x83},
{0x00CA, 0xE6},
{0x00CB, 0xE8},
{0x00CC, 0xED},
{0x00CD, 0xEA},
{0x00CE, 0xEB},
{0x00CF, 0xEC},
{0x00D1, 0x84},
{0x00D2, 0xF1},
{0x00D3, 0xEE},
{0x00D4, 0xEF},
{0x00D5, 0xCD},
{0x00D6, 0x85},
{0x00D8, 0xAF},
{0x00D9, 0xF4},
{0x00DA, 0xF2},
{0x00DB, 0xF3},
{0x00DC, 0x86},
{0x00DF, 0xA7},
{0x00E0, 0x88},
{0x00E1, 0x87},
{0x00E2, 0x89},
{0x00E3, 0x8B},
{0x00E4, 0x8A},
{0x00E5, 0x8C},
{0x00E6, 0xBE},
{0x00E7, 0x8D},
{0x00E8, 0x8F},
{0x00E9, 0x8E},
{0x00EA, 0x90},
{0x00EB, 0x91},
{0x00EC, 0x93},
{0x00ED, 0x92},
{0x00EE, 0x94},
{0x00EF, 0x95},
{0x00F1, 0x96},
{0x00F2, 0x98},
{0x00F3, 0x97},
{0x00F4, 0x99},
{0x00F5, 0x9B},
{0x00F6, 0x9A},
{0x00F7, 0xD6},
{0x00F8, 0xBF},
{0x00F9, 0x9D},
{0x00FA, 0x9C},
{0x00FB, 0x9E},
{0x00FC, 0x9F},
{0x00FF, 0xD8},
{0x0131, 0xF5},
{0x0152, 0xCE},
{0x0153, 0xCF},
{0x0178, 0xD9},
{0x0192, 0xC4},
{0x02C6, 0xF6},
{0x02C7, 0xFF},
{0x02D8, 0xF9},
{0x02D9, 0xFA},
{0x02DA, 0xFB},
{0x02DB, 0xFE},
{0x02DC, 0xF7},
{0x02DD, 0xFD},
{0x03C0, 0xB9},
{0x2013, 0xD0},
{0x2014, 0xD1},
{0x2018, 0xD4},
{0x2019, 0xD5},
{0x201A, 0xE2},
{0x201C, 0xD2},
{0x201D, 0xD3},
{0x201E, 0xE3},
{0x2020, 0xA0},
{0x2021, 0xE0},
{0x2022, 0xA5},
{0x2026, 0xC9},
{0x2030, 0xE4},
{0x2039, 0xDC},
{0x203A, 0xDD},
{0x2044, 0xDA},
{0x2122, 0xAA},
{0x2126, 0xBD},
{0x2202, 0xB6},
{0x2206, 0xC6},
{0x220F, 0xB8},
{0x2211, 0xB7},
{0x221A, 0xC3},
{0x221E, 0xB0},
{0x222B, 0xBA},
{0x2248, 0xC5},
{0x2260, 0xAD},
{0x2264, 0xB2},
{0x2265, 0xB3},
{0x25CA, 0xD7},
{0x2665, 0xF0},
{0xFB01, 0xDE},
{0xFB02, 0xDF}
};

static char maclookup(unsigned ucs) {
  if (ucs < 128) return char(ucs);
  unsigned a = 0;
  unsigned b = sizeof(mactable)/sizeof(*mactable);
  while (a < b) {
    unsigned c = (a+b)/2;
    if (ucs < mactable[c].ucs) b = c;
    else if (ucs > mactable[c].ucs) a = c+1;
    else return mactable[c].mac;
  }
  return char(127);
}

/*!
  Convert a UTF-8 sequence into an array of 1-byte characters in
  the MacRoman character set. Characters not in the set decode to
  0x7f, which seems to draw a box in most sets.

  See utf8toa() for more information.
*/
unsigned utf8tomac(const char* src, unsigned srclen,
		   char* dst, unsigned dstlen)
{
  const char* p = src;
  const char* e = src+srclen;
  unsigned count = 0;
  if (dstlen) for (;;) {
    unsigned char c;
    if (p >= e) {dst[count] = 0; return count;}
    c = *(unsigned char*)p;
    if (c < 0x80) { // ascii
      dst[count] = c;
      p++;
    } else {
      int len; unsigned ucs = utf8decode(p,e,&len);
      p += len;
      dst[count] = maclookup(ucs);
    }
    if (++count >= dstlen) {dst[count-1] = 0; break;}
  }
  // we filled dst, measure the rest:
  while (p < e) {
    if (!(*p & 0x80)) p++;
    else {
      int len;
      utf8decode(p,e,&len);
      p += len;
    }
    ++count;
  }
  return count;
}
