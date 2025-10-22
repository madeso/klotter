

#include "klotter/imgui.theme.h"

namespace klotter
{

#if FF_HAS(ENABLE_THEMES)

Theme Theme::default_theme()
{
	Theme r;
	r.background = ImColor(0xee, 0xee, 0xee);
	r.f_high = ImColor(0x0a, 0x0a, 0x0a);
	r.f_med = ImColor(0x4a, 0x4a, 0x4a);
	r.f_low = ImColor(0x6a, 0x6a, 0x6a);
	r.f_inv = ImColor(0x11, 0x11, 0x11);
	r.b_high = ImColor(0xa1, 0xa1, 0xa1);
	r.b_med = ImColor(0xc1, 0xc1, 0xc1);
	r.b_low = ImColor(0xff, 0xff, 0xff);
	r.b_inv = ImColor(0xff, 0xb5, 0x45);
	return r;
}

Theme Theme::apollo()
{
	Theme r;
	r.background = ImColor(0x29, 0x27, 0x2b);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xe4, 0x74, 0x64);
	r.f_low = ImColor(0x66, 0x60, 0x6b);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0x00, 0x00, 0x00);
	r.b_med = ImColor(0x20, 0x1e, 0x21);
	r.b_low = ImColor(0x32, 0x2e, 0x33);
	r.b_inv = ImColor(0xe4, 0x74, 0x64);
	return r;
}

Theme Theme::battlestation()
{
	Theme r;
	r.background = ImColor(0x22, 0x22, 0x22);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xaf, 0xfe, 0xc7);
	r.f_low = ImColor(0x88, 0x88, 0x88);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0x55, 0x55, 0x55);
	r.b_med = ImColor(0x33, 0x33, 0x33);
	r.b_low = ImColor(0x11, 0x11, 0x11);
	r.b_inv = ImColor(0xaf, 0xfe, 0xc7);
	return r;
}

Theme Theme::berry()
{
	Theme r;
	r.background = ImColor(0x9E, 0xB7, 0xFF);
	r.f_high = ImColor(0x3e, 0x82, 0x81);
	r.f_med = ImColor(0xFF, 0xFF, 0xFF);
	r.f_low = ImColor(0xc5, 0xf0, 0xec);
	r.f_inv = ImColor(0xFF, 0xFF, 0xFF);
	r.b_high = ImColor(0x1C, 0x0A, 0x16);
	r.b_med = ImColor(0x49, 0x98, 0x97);
	r.b_low = ImColor(0x6A, 0xDE, 0xDC);
	r.b_inv = ImColor(0x6A, 0xDE, 0xDC);
	return r;
}

Theme Theme::bigtime()
{
	Theme r;
	r.background = ImColor(0x46, 0x82, 0xB4);
	r.f_high = ImColor(0x00, 0x00, 0x00);
	r.f_med = ImColor(0x2F, 0x4F, 0x4F);
	r.f_low = ImColor(0xFF, 0xA5, 0x00);
	r.f_inv = ImColor(0x99, 0x32, 0xCC);
	r.b_high = ImColor(0xF8, 0xF8, 0xFF);
	r.b_med = ImColor(0x69, 0x69, 0x69);
	r.b_low = ImColor(0x77, 0x88, 0x99);
	r.b_inv = ImColor(0x6B, 0x8E, 0x23);
	return r;
}

Theme Theme::boysenberry()
{
	Theme r;
	r.background = ImColor(0x17, 0x17, 0x17);
	r.f_high = ImColor(0xef, 0xef, 0xef);
	r.f_med = ImColor(0x99, 0x99, 0x99);
	r.f_low = ImColor(0x87, 0x32, 0x60);
	r.f_inv = ImColor(0x91, 0x91, 0x91);
	r.b_high = ImColor(0x37, 0x37, 0x37);
	r.b_med = ImColor(0x27, 0x27, 0x27);
	r.b_low = ImColor(0x00, 0x00, 0x00);
	r.b_inv = ImColor(0x87, 0x32, 0x60);
	return r;
}

Theme Theme::coal()
{
	Theme r;
	r.background = ImColor(0xED, 0xEA, 0xEA);
	r.f_high = ImColor(0x39, 0x3B, 0x3F);
	r.f_med = ImColor(0x80, 0x87, 0x90);
	r.f_low = ImColor(0xA3, 0xA3, 0xA4);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0x33, 0x33, 0x33);
	r.b_med = ImColor(0x77, 0x77, 0x77);
	r.b_low = ImColor(0xDD, 0xDD, 0xDD);
	r.b_inv = ImColor(0xff, 0xff, 0xff);
	return r;
}

Theme Theme::cobalt()
{
	Theme r;
	r.background = ImColor(0x18, 0x36, 0x4A);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xff, 0xc6, 0x00);
	r.f_low = ImColor(0x00, 0x88, 0xff);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0x1B, 0x1A, 0x1C);
	r.b_med = ImColor(0x20, 0x48, 0x63);
	r.b_low = ImColor(0x15, 0x23, 0x2D);
	r.b_inv = ImColor(0xff, 0xff, 0xff);
	return r;
}

Theme Theme::commodore()
{
	Theme r;
	r.background = ImColor(0xa5, 0xa7, 0xfc);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0x44, 0x4a, 0xe3);
	r.f_low = ImColor(0xff, 0xd7, 0xcd);
	r.f_inv = ImColor(0x44, 0x4a, 0xe3);
	r.b_high = ImColor(0xc2, 0xc4, 0xff);
	r.b_med = ImColor(0xba, 0xbc, 0xff);
	r.b_low = ImColor(0xb0, 0xb2, 0xff);
	r.b_inv = ImColor(0xff, 0xff, 0xff);
	return r;
}

Theme Theme::forestlawn()
{
	Theme r;
	r.background = ImColor(0xcd, 0x85, 0x3f);
	r.f_high = ImColor(0x00, 0x00, 0x00);
	r.f_med = ImColor(0x8b, 0x00, 0x00);
	r.f_low = ImColor(0x8b, 0x45, 0x13);
	r.f_inv = ImColor(0x00, 0xce, 0xd1);
	r.b_high = ImColor(0x90, 0xee, 0x90);
	r.b_med = ImColor(0x32, 0xcd, 0x32);
	r.b_low = ImColor(0x9a, 0xcd, 0x32);
	r.b_inv = ImColor(0x00, 0x00, 0x00);
	return r;
}

Theme Theme::frameio()
{
	Theme r;
	r.background = ImColor(0x33, 0x38, 0x48);
	r.f_high = ImColor(0xcc, 0xcc, 0xcc);
	r.f_med = ImColor(0x5b, 0x52, 0xfe);
	r.f_low = ImColor(0x4c, 0x57, 0x6f);
	r.f_inv = ImColor(0xff, 0xff, 0xff);
	r.b_high = ImColor(0xed, 0xee, 0xf2);
	r.b_med = ImColor(0x26, 0x2b, 0x37);
	r.b_low = ImColor(0x39, 0x41, 0x53);
	r.b_inv = ImColor(0x5b, 0x52, 0xfe);
	return r;
}

Theme Theme::gameboy()
{
	Theme r;
	r.background = ImColor(0x9B, 0xBC, 0x0F);
	r.f_high = ImColor(0x0F, 0x38, 0x0F);
	r.f_med = ImColor(0x0F, 0x38, 0x0F);
	r.f_low = ImColor(0x30, 0x62, 0x30);
	r.f_inv = ImColor(0x9B, 0xBC, 0x0F);
	r.b_high = ImColor(0x8B, 0xAC, 0x0F);
	r.b_med = ImColor(0x8B, 0xAC, 0x0F);
	r.b_low = ImColor(0x9B, 0xBC, 0x0F);
	r.b_inv = ImColor(0x0F, 0x38, 0x0F);
	return r;
}

Theme Theme::garden()
{
	Theme r;
	r.background = ImColor(0x28, 0x21, 0x1c);
	r.f_high = ImColor(0xff, 0xef, 0xc9);
	r.f_med = ImColor(0x9f, 0x9f, 0xa2);
	r.f_low = ImColor(0xa3, 0x83, 0x2c);
	r.f_inv = ImColor(0x66, 0x66, 0x66);
	r.b_high = ImColor(0xaa, 0x00, 0x00);
	r.b_med = ImColor(0x21, 0x4c, 0x05);
	r.b_low = ImColor(0x48, 0x41, 0x3a);
	r.b_inv = ImColor(0x4c, 0xb1, 0xcf);
	return r;
}

Theme Theme::gotham()
{
	Theme r;
	r.background = ImColor(0x0A, 0x0F, 0x14);
	r.f_high = ImColor(0xFF, 0xFF, 0xFF);
	r.f_med = ImColor(0x98, 0xD1, 0xCE);
	r.f_low = ImColor(0xED, 0xB5, 0x4B);
	r.f_inv = ImColor(0xC3, 0x30, 0x27);
	r.b_high = ImColor(0x09, 0x37, 0x48);
	r.b_med = ImColor(0x08, 0x1F, 0x2D);
	r.b_low = ImColor(0x10, 0x15, 0x1B);
	r.b_inv = ImColor(0x8F, 0xAF, 0x9F);
	return r;
}

Theme Theme::haxe()
{
	Theme r;
	r.background = ImColor(0x14, 0x14, 0x19);
	r.f_high = ImColor(0xFA, 0xB2, 0x0B);
	r.f_med = ImColor(0xF4, 0x72, 0x16);
	r.f_low = ImColor(0xF1, 0x47, 0x1D);
	r.f_inv = ImColor(0x14, 0x14, 0x19);
	r.b_high = ImColor(0x14, 0x14, 0x19);
	r.b_med = ImColor(0x14, 0x14, 0x19);
	r.b_low = ImColor(0x14, 0x14, 0x19);
	r.b_inv = ImColor(0xFF, 0xFF, 0xFF);
	return r;
}

Theme Theme::isotope()
{
	Theme r;
	r.background = ImColor(0x00, 0x00, 0x00);
	r.f_high = ImColor(0xFF, 0xFF, 0xFF);
	r.f_med = ImColor(0x33, 0xFF, 0x00);
	r.f_low = ImColor(0xFF, 0x00, 0x99);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0x50, 0x50, 0x50);
	r.b_med = ImColor(0x00, 0x00, 0x00);
	r.b_low = ImColor(0x00, 0x00, 0x00);
	r.b_inv = ImColor(0xFF, 0xFF, 0xFF);
	return r;
}

Theme Theme::kawaii()
{
	Theme r;
	r.background = ImColor(0xd0, 0x90, 0x90);
	r.f_high = ImColor(0x00, 0x00, 0x00);
	r.f_med = ImColor(0xff, 0xfa, 0xfa);
	r.f_low = ImColor(0x6e, 0xa2, 0xa1);
	r.f_inv = ImColor(0xff, 0x14, 0x93);
	r.b_high = ImColor(0x7f, 0xff, 0xd4);
	r.b_med = ImColor(0x6A, 0xDE, 0xDC);
	r.b_low = ImColor(0xb0, 0x86, 0x86);
	r.b_inv = ImColor(0x7f, 0xff, 0xd4);
	return r;
}

Theme Theme::laundry()
{
	Theme r;
	r.background = ImColor(0x1b, 0x1a, 0x1e);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xff, 0x28, 0x51);
	r.f_low = ImColor(0x3e, 0x3d, 0x42);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0xbd, 0xbc, 0xc1);
	r.b_med = ImColor(0x63, 0x60, 0x6b);
	r.b_low = ImColor(0x15, 0x14, 0x17);
	r.b_inv = ImColor(0xff, 0x28, 0x51);
	return r;
}

Theme Theme::lotus()
{
	Theme r;
	r.background = ImColor(0x16, 0x16, 0x16);
	r.f_high = ImColor(0xf0, 0xc0, 0x98);
	r.f_med = ImColor(0x99, 0x99, 0x99);
	r.f_low = ImColor(0x44, 0x44, 0x44);
	r.f_inv = ImColor(0x22, 0x22, 0x22);
	r.b_high = ImColor(0xff, 0xff, 0xff);
	r.b_med = ImColor(0x33, 0x33, 0x33);
	r.b_low = ImColor(0x22, 0x22, 0x22);
	r.b_inv = ImColor(0xf0, 0xc0, 0x98);
	return r;
}

Theme Theme::mahou()
{
	Theme r;
	r.background = ImColor(0xE0, 0xB1, 0xCB);
	r.f_high = ImColor(0x23, 0x19, 0x42);
	r.f_med = ImColor(0x48, 0x41, 0x6d);
	r.f_low = ImColor(0x91, 0x72, 0x96);
	r.f_inv = ImColor(0xE0, 0xB1, 0xCB);
	r.b_high = ImColor(0x5E, 0x54, 0x8E);
	r.b_med = ImColor(0xFF, 0xFF, 0xFF);
	r.b_low = ImColor(0xBE, 0x95, 0xC4);
	r.b_inv = ImColor(0x9F, 0x86, 0xC0);
	return r;
}

Theme Theme::marble()
{
	Theme r;
	r.background = ImColor(0xFB, 0xFB, 0xF2);
	r.f_high = ImColor(0x3a, 0x37, 0x38);
	r.f_med = ImColor(0x84, 0x75, 0x77);
	r.f_low = ImColor(0xbd, 0xb8, 0xb8);
	r.f_inv = ImColor(0xA6, 0xA2, 0xA2);
	r.b_high = ImColor(0x67, 0x61, 0x64);
	r.b_med = ImColor(0xA6, 0xA2, 0xA2);
	r.b_low = ImColor(0xCF, 0xD2, 0xCD);
	r.b_inv = ImColor(0x67, 0x61, 0x64);
	return r;
}

Theme Theme::murata()
{
	Theme r;
	r.background = ImColor(0x11, 0x11, 0x11);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xe8, 0xda, 0xcb);
	r.f_low = ImColor(0x5a, 0x69, 0x70);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0xbb, 0xbb, 0xbb);
	r.b_med = ImColor(0x84, 0x98, 0xa2);
	r.b_low = ImColor(0x33, 0x33, 0x33);
	r.b_inv = ImColor(0xb9, 0x61, 0x5a);
	return r;
}

Theme Theme::muzieca()
{
	Theme r;
	r.background = ImColor(0x09, 0x09, 0x09);
	r.f_high = ImColor(0x81, 0x81, 0x81);
	r.f_med = ImColor(0x70, 0x70, 0x70);
	r.f_low = ImColor(0x59, 0x59, 0x59);
	r.f_inv = ImColor(0x27, 0x27, 0x27);
	r.b_high = ImColor(0x27, 0x27, 0x27);
	r.b_med = ImColor(0x18, 0x18, 0x18);
	r.b_low = ImColor(0x11, 0x11, 0x11);
	r.b_inv = ImColor(0x81, 0x81, 0x81);
	return r;
}

Theme Theme::nightowl()
{
	Theme r;
	r.background = ImColor(0x01, 0x16, 0x27);
	r.f_high = ImColor(0x7f, 0xdb, 0xca);
	r.f_med = ImColor(0x82, 0xaa, 0xff);
	r.f_low = ImColor(0xc7, 0x92, 0xea);
	r.f_inv = ImColor(0x63, 0x77, 0x77);
	r.b_high = ImColor(0x5f, 0x7e, 0x97);
	r.b_med = ImColor(0x45, 0x60, 0x75);
	r.b_low = ImColor(0x2f, 0x47, 0x59);
	r.b_inv = ImColor(0x7f, 0xdb, 0xca);
	return r;
}

Theme Theme::ninetynine()
{
	Theme r;
	r.background = ImColor(0x00, 0x00, 0x00);
	r.f_high = ImColor(0xef, 0xef, 0xef);
	r.f_med = ImColor(0xcd, 0xcd, 0xcd);
	r.f_low = ImColor(0x67, 0x67, 0x67);
	r.f_inv = ImColor(0x0a, 0x0a, 0x0a);
	r.b_high = ImColor(0xee, 0xee, 0xee);
	r.b_med = ImColor(0xff, 0xd2, 0x20);
	r.b_low = ImColor(0x46, 0x46, 0x46);
	r.b_inv = ImColor(0xff, 0x33, 0x00);
	return r;
}

Theme Theme::noir()
{
	Theme r;
	r.background = ImColor(0x22, 0x22, 0x22);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xcc, 0xcc, 0xcc);
	r.f_low = ImColor(0x99, 0x99, 0x99);
	r.f_inv = ImColor(0xff, 0xff, 0xff);
	r.b_high = ImColor(0x88, 0x88, 0x88);
	r.b_med = ImColor(0x66, 0x66, 0x66);
	r.b_low = ImColor(0x44, 0x44, 0x44);
	r.b_inv = ImColor(0x00, 0x00, 0x00);
	return r;
}

Theme Theme::nord()
{
	Theme r;
	r.background = ImColor(0x2E, 0x34, 0x40);
	r.f_high = ImColor(0xEC, 0xEF, 0xF4);
	r.f_med = ImColor(0x9D, 0xC4, 0xC3);
	r.f_low = ImColor(0xB4, 0xB8, 0xC0);
	r.f_inv = ImColor(0x5E, 0x81, 0xAC);
	r.b_high = ImColor(0x5E, 0x81, 0xAC);
	r.b_med = ImColor(0x43, 0x4C, 0x5E);
	r.b_low = ImColor(0x3B, 0x42, 0x52);
	r.b_inv = ImColor(0xAB, 0xCD, 0xCC);
	return r;
}

Theme Theme::obsidian()
{
	Theme r;
	r.background = ImColor(0x22, 0x28, 0x2a);
	r.f_high = ImColor(0xf1, 0xf2, 0xf3);
	r.f_med = ImColor(0x93, 0xc7, 0x63);
	r.f_low = ImColor(0xec, 0x76, 0x00);
	r.f_inv = ImColor(0x96, 0x3a, 0x46);
	r.b_high = ImColor(0x67, 0x8c, 0xb1);
	r.b_med = ImColor(0x4f, 0x61, 0x64);
	r.b_low = ImColor(0x42, 0x46, 0x4C);
	r.b_inv = ImColor(0xff, 0xcd, 0x22);
	return r;
}

Theme Theme::op1()
{
	Theme r;
	r.background = ImColor(0x0E, 0x0D, 0x11);
	r.f_high = ImColor(0xEF, 0xEF, 0xEF);
	r.f_med = ImColor(0x26, 0x93, 0x6F);
	r.f_low = ImColor(0xA5, 0x43, 0x5A);
	r.f_inv = ImColor(0x0E, 0x0D, 0x11);
	r.b_high = ImColor(0x19, 0x1A, 0x26);
	r.b_med = ImColor(0x14, 0x15, 0x1F);
	r.b_low = ImColor(0x10, 0x11, 0x19);
	r.b_inv = ImColor(0x9F, 0x9F, 0xB3);
	return r;
}

Theme Theme::orca()
{
	Theme r;
	r.background = ImColor(0x00, 0x00, 0x00);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0x77, 0x77, 0x77);
	r.f_low = ImColor(0x44, 0x44, 0x44);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0xdd, 0xdd, 0xdd);
	r.b_med = ImColor(0x72, 0xde, 0xc2);
	r.b_low = ImColor(0x22, 0x22, 0x22);
	r.b_inv = ImColor(0xff, 0xb5, 0x45);
	return r;
}

Theme Theme::pawbin()
{
	Theme r;
	r.background = ImColor(0x2b, 0x29, 0x33);
	r.f_high = ImColor(0xf2, 0xf2, 0xf2);
	r.f_med = ImColor(0x00, 0xbd, 0xd6);
	r.f_low = ImColor(0xaa, 0x9f, 0xdf);
	r.f_inv = ImColor(0x1a, 0x18, 0x20);
	r.b_high = ImColor(0x1a, 0x18, 0x20);
	r.b_med = ImColor(0x24, 0x21, 0x2c);
	r.b_low = ImColor(0x34, 0x30, 0x3b);
	r.b_inv = ImColor(0xf2, 0xf2, 0xf2);
	return r;
}

Theme Theme::pico8()
{
	Theme r;
	r.background = ImColor(0x00, 0x00, 0x00);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xff, 0xf1, 0xe8);
	r.f_low = ImColor(0xff, 0x78, 0xa9);
	r.f_inv = ImColor(0xff, 0xff, 0xff);
	r.b_high = ImColor(0xc2, 0xc3, 0xc7);
	r.b_med = ImColor(0x83, 0x76, 0x9c);
	r.b_low = ImColor(0x69, 0x5f, 0x56);
	r.b_inv = ImColor(0x00, 0xae, 0xfe);
	return r;
}

Theme Theme::polivoks()
{
	Theme r;
	r.background = ImColor(0x11, 0x11, 0x11);
	r.f_high = ImColor(0xef, 0xef, 0xef);
	r.f_med = ImColor(0xff, 0x44, 0x44);
	r.f_low = ImColor(0x33, 0x33, 0x33);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0x66, 0x66, 0x66);
	r.b_med = ImColor(0x44, 0x44, 0x44);
	r.b_low = ImColor(0x22, 0x22, 0x22);
	r.b_inv = ImColor(0xff, 0x44, 0x44);
	return r;
}

Theme Theme::rainonwires()
{
	Theme r;
	r.background = ImColor(0x01, 0x01, 0x01);
	r.f_high = ImColor(0xc6, 0x92, 0xbb);
	r.f_med = ImColor(0x14, 0x91, 0x06);
	r.f_low = ImColor(0x8a, 0x66, 0x82);
	r.f_inv = ImColor(0x8D, 0x2E, 0x71);
	r.b_high = ImColor(0x8D, 0x2E, 0x71);
	r.b_med = ImColor(0x6E, 0x24, 0x55);
	r.b_low = ImColor(0x01, 0x01, 0x01);
	r.b_inv = ImColor(0x15, 0x91, 0x06);
	return r;
}

Theme Theme::roguelight()
{
	Theme r;
	r.background = ImColor(0x35, 0x2b, 0x31);
	r.f_high = ImColor(0xf5, 0xf5, 0xd4);
	r.f_med = ImColor(0x70, 0x83, 0x8c);
	r.f_low = ImColor(0x4a, 0x6b, 0x83);
	r.f_inv = ImColor(0x35, 0x2b, 0x31);
	r.b_high = ImColor(0x96, 0xcf, 0x85);
	r.b_med = ImColor(0x5a, 0x69, 0x70);
	r.b_low = ImColor(0x4a, 0x3b, 0x44);
	r.b_inv = ImColor(0xf5, 0xf5, 0xd4);
	return r;
}

Theme Theme::sk()
{
	Theme r;
	r.background = ImColor(0x00, 0x07, 0x09);
	r.f_high = ImColor(0xcb, 0xcb, 0xd3);
	r.f_med = ImColor(0x89, 0x76, 0x68);
	r.f_low = ImColor(0x52, 0x3d, 0x2c);
	r.f_inv = ImColor(0x3f, 0x4f, 0x5b);
	r.b_high = ImColor(0xab, 0xa4, 0x9e);
	r.b_med = ImColor(0x59, 0x57, 0x4b);
	r.b_low = ImColor(0x37, 0x28, 0x23);
	r.b_inv = ImColor(0x8c, 0x5a, 0x3d);
	return r;
}

Theme Theme::snow()
{
	Theme r;
	r.background = ImColor(0xee, 0xef, 0xee);
	r.f_high = ImColor(0x22, 0x22, 0x22);
	r.f_med = ImColor(0x99, 0x99, 0x99);
	r.f_low = ImColor(0xbb, 0xbc, 0xbb);
	r.f_inv = ImColor(0x54, 0x54, 0x54);
	r.b_high = ImColor(0x54, 0x54, 0x54);
	r.b_med = ImColor(0xce, 0xd0, 0xce);
	r.b_low = ImColor(0xf5, 0xf5, 0xf5);
	r.b_inv = ImColor(0xed, 0x2c, 0x3e);
	return r;
}

Theme Theme::solarised_dark()
{
	Theme r;
	r.background = ImColor(0x07, 0x36, 0x42);
	r.f_high = ImColor(0x93, 0xa1, 0xa1);
	r.f_med = ImColor(0x6c, 0x71, 0xc4);
	r.f_low = ImColor(0x58, 0x6e, 0x75);
	r.f_inv = ImColor(0x00, 0x2b, 0x36);
	r.b_high = ImColor(0xfd, 0xf6, 0xe3);
	r.b_med = ImColor(0xee, 0xe8, 0xd5);
	r.b_low = ImColor(0x00, 0x2b, 0x36);
	r.b_inv = ImColor(0xcb, 0x4b, 0x16);
	return r;
}

Theme Theme::solarised_light()
{
	Theme r;
	r.background = ImColor(0xee, 0xe8, 0xd5);
	r.f_high = ImColor(0x58, 0x6e, 0x75);
	r.f_med = ImColor(0x6c, 0x71, 0xc4);
	r.f_low = ImColor(0x93, 0xa1, 0xa1);
	r.f_inv = ImColor(0xfd, 0xf6, 0xe3);
	r.b_high = ImColor(0x00, 0x2b, 0x36);
	r.b_med = ImColor(0x07, 0x36, 0x42);
	r.b_low = ImColor(0xfd, 0xf6, 0xe3);
	r.b_inv = ImColor(0xcb, 0x4b, 0x16);
	return r;
}

Theme Theme::sonicpi()
{
	Theme r;
	r.background = ImColor(0xff, 0xff, 0xff);
	r.f_high = ImColor(0x00, 0x00, 0x00);
	r.f_med = ImColor(0xed, 0x1e, 0x92);
	r.f_low = ImColor(0xaa, 0xaa, 0xaa);
	r.f_inv = ImColor(0xff, 0xff, 0xff);
	r.b_high = ImColor(0x44, 0x44, 0x44);
	r.b_med = ImColor(0x55, 0x55, 0x55);
	r.b_low = ImColor(0xce, 0xd0, 0xce);
	r.b_inv = ImColor(0xed, 0x1e, 0x92);
	return r;
}

Theme Theme::soyuz()
{
	Theme r;
	r.background = ImColor(0x11, 0x11, 0x11);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xaa, 0xaa, 0xaa);
	r.f_low = ImColor(0x55, 0x55, 0x55);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0xfc, 0x53, 0x3e);
	r.b_med = ImColor(0x66, 0x66, 0x66);
	r.b_low = ImColor(0x33, 0x33, 0x33);
	r.b_inv = ImColor(0xfc, 0x53, 0x3e);
	return r;
}

Theme Theme::tape()
{
	Theme r;
	r.background = ImColor(0xda, 0xd7, 0xcd);
	r.f_high = ImColor(0x69, 0x68, 0x61);
	r.f_med = ImColor(0xff, 0xff, 0xff);
	r.f_low = ImColor(0xb3, 0xb2, 0xac);
	r.f_inv = ImColor(0x43, 0x42, 0x3e);
	r.b_high = ImColor(0x43, 0x42, 0x3e);
	r.b_med = ImColor(0xc2, 0xc1, 0xbb);
	r.b_low = ImColor(0xe5, 0xe3, 0xdc);
	r.b_inv = ImColor(0xeb, 0x3f, 0x48);
	return r;
}

Theme Theme::teenage()
{
	Theme r;
	r.background = ImColor(0xa1, 0xa1, 0xa1);
	r.f_high = ImColor(0x22, 0x22, 0x22);
	r.f_med = ImColor(0xe0, 0x0b, 0x30);
	r.f_low = ImColor(0x88, 0x88, 0x88);
	r.f_inv = ImColor(0xff, 0xff, 0xff);
	r.b_high = ImColor(0x55, 0x55, 0x55);
	r.b_med = ImColor(0xfb, 0xba, 0x2d);
	r.b_low = ImColor(0xb3, 0xb3, 0xb3);
	r.b_inv = ImColor(0x0e, 0x72, 0x42);
	return r;
}

Theme Theme::teletext()
{
	Theme r;
	r.background = ImColor(0x00, 0x00, 0x00);
	r.f_high = ImColor(0xff, 0xff, 0xff);
	r.f_med = ImColor(0xff, 0xff, 0x00);
	r.f_low = ImColor(0x00, 0xFF, 0x00);
	r.f_inv = ImColor(0x00, 0x00, 0x00);
	r.b_high = ImColor(0xff, 0x00, 0xff);
	r.b_med = ImColor(0xff, 0x00, 0x00);
	r.b_low = ImColor(0x00, 0x00, 0xff);
	r.b_inv = ImColor(0xff, 0xff, 0xff);
	return r;
}

Theme Theme::vacuui()
{
	Theme r;
	r.background = ImColor(0x22, 0x28, 0x2a);
	r.f_high = ImColor(0xf1, 0xf2, 0xf3);
	r.f_med = ImColor(0xa6, 0xe2, 0x2e);
	r.f_low = ImColor(0x66, 0xd9, 0xef);
	r.f_inv = ImColor(0xf9, 0x26, 0x72);
	r.b_high = ImColor(0x67, 0x8c, 0xb1);
	r.b_med = ImColor(0x4f, 0x61, 0x64);
	r.b_low = ImColor(0x42, 0x46, 0x4C);
	r.b_inv = ImColor(0xe6, 0xdb, 0x74);
	return r;
}

Theme Theme::zenburn()
{
	Theme r;
	r.background = ImColor(0x46, 0x46, 0x46);
	r.f_high = ImColor(0xDC, 0xDC, 0xCC);
	r.f_med = ImColor(0xDC, 0xA3, 0xA3);
	r.f_low = ImColor(0x7F, 0x9F, 0x7F);
	r.f_inv = ImColor(0x00, 0x0D, 0x18);
	r.b_high = ImColor(0x26, 0x26, 0x26);
	r.b_med = ImColor(0x33, 0x33, 0x33);
	r.b_low = ImColor(0x3F, 0x3F, 0x3F);
	r.b_inv = ImColor(0x8F, 0xAF, 0x9F);
	return r;
}


void test_themes()
{
	if (ImGui::Button("apollo")) setup_imgui_theme(Theme::apollo());
    ImGui::SameLine();
    if (ImGui::Button("battlestation")) setup_imgui_theme(Theme::battlestation());
    ImGui::SameLine();
    if (ImGui::Button("berry")) setup_imgui_theme(Theme::berry());
    ImGui::SameLine();
    if (ImGui::Button("bigtime")) setup_imgui_theme(Theme::bigtime());
    ImGui::SameLine();
    if (ImGui::Button("boysenberry")) setup_imgui_theme(Theme::boysenberry());
    ImGui::SameLine();
    if (ImGui::Button("coal")) setup_imgui_theme(Theme::coal());
    
    if (ImGui::Button("cobalt")) setup_imgui_theme(Theme::cobalt());
    ImGui::SameLine();
    if (ImGui::Button("commodore")) setup_imgui_theme(Theme::commodore());
    ImGui::SameLine();
    if (ImGui::Button("forestlawn")) setup_imgui_theme(Theme::forestlawn());
    ImGui::SameLine();
    if (ImGui::Button("frameio")) setup_imgui_theme(Theme::frameio());
    ImGui::SameLine();
    if (ImGui::Button("gameboy")) setup_imgui_theme(Theme::gameboy());
    
    if (ImGui::Button("garden")) setup_imgui_theme(Theme::garden());
    ImGui::SameLine();
    if (ImGui::Button("gotham")) setup_imgui_theme(Theme::gotham());
    ImGui::SameLine();
    if (ImGui::Button("haxe")) setup_imgui_theme(Theme::haxe());
    ImGui::SameLine();
    if (ImGui::Button("isotope")) setup_imgui_theme(Theme::isotope());
    ImGui::SameLine();
    if (ImGui::Button("kawaii")) setup_imgui_theme(Theme::kawaii());
    ImGui::SameLine();
    if (ImGui::Button("laundry")) setup_imgui_theme(Theme::laundry());
    
    if (ImGui::Button("lotus")) setup_imgui_theme(Theme::lotus());
    ImGui::SameLine();
    if (ImGui::Button("mahou")) setup_imgui_theme(Theme::mahou());
    ImGui::SameLine();
    if (ImGui::Button("marble")) setup_imgui_theme(Theme::marble());
    ImGui::SameLine();
    if (ImGui::Button("murata")) setup_imgui_theme(Theme::murata());
    ImGui::SameLine();
    if (ImGui::Button("muzieca")) setup_imgui_theme(Theme::muzieca());
    ImGui::SameLine();
    if (ImGui::Button("nightowl")) setup_imgui_theme(Theme::nightowl());
    
    if (ImGui::Button("ninetynine")) setup_imgui_theme(Theme::ninetynine());
    ImGui::SameLine();
    if (ImGui::Button("noir")) setup_imgui_theme(Theme::noir());
    ImGui::SameLine();
    if (ImGui::Button("nord")) setup_imgui_theme(Theme::nord());
    ImGui::SameLine();
    if (ImGui::Button("obsidian")) setup_imgui_theme(Theme::obsidian());
    ImGui::SameLine();
    if (ImGui::Button("op1")) setup_imgui_theme(Theme::op1());
    ImGui::SameLine();
    if (ImGui::Button("orca")) setup_imgui_theme(Theme::orca());
    
    if (ImGui::Button("pawbin")) setup_imgui_theme(Theme::pawbin());
    ImGui::SameLine();
    if (ImGui::Button("pico8")) setup_imgui_theme(Theme::pico8());
    ImGui::SameLine();
    if (ImGui::Button("polivoks")) setup_imgui_theme(Theme::polivoks());
    ImGui::SameLine();
    if (ImGui::Button("rainonwires")) setup_imgui_theme(Theme::rainonwires());
    ImGui::SameLine();
    if (ImGui::Button("roguelight")) setup_imgui_theme(Theme::roguelight());
    ImGui::SameLine();
    if (ImGui::Button("sk")) setup_imgui_theme(Theme::sk());
    
    if (ImGui::Button("snow")) setup_imgui_theme(Theme::snow());
    ImGui::SameLine();
    if (ImGui::Button("solarised_dark")) setup_imgui_theme(Theme::solarised_dark());
    ImGui::SameLine();
    if (ImGui::Button("solarised_light")) setup_imgui_theme(Theme::solarised_light());
    ImGui::SameLine();
    if (ImGui::Button("sonicpi")) setup_imgui_theme(Theme::sonicpi());
    ImGui::SameLine();
    if (ImGui::Button("soyuz")) setup_imgui_theme(Theme::soyuz());
    ImGui::SameLine();
    if (ImGui::Button("tape")) setup_imgui_theme(Theme::tape());
    
    if (ImGui::Button("teenage")) setup_imgui_theme(Theme::teenage());
    ImGui::SameLine();
    if (ImGui::Button("teletext")) setup_imgui_theme(Theme::teletext());
    ImGui::SameLine();
    if (ImGui::Button("vacuui")) setup_imgui_theme(Theme::vacuui());
    ImGui::SameLine();
    if (ImGui::Button("zenburn")) setup_imgui_theme(Theme::zenburn());
    ImGui::NewLine();

	if (ImGui::Button("100r default")) setup_imgui_theme(Theme::default_theme());

	if (ImGui::Button("itmago")) setup_imgui_theme_itmago();
	ImGui::SameLine();
	if (ImGui::Button("purple")) setup_imgui_theme_purple();
	ImGui::SameLine();
	if (ImGui::Button("qy8q")) setup_imgui_theme_qy8q();

	if (ImGui::Button("simv0")) setup_imgui_theme_simv0();
	ImGui::SameLine();
	if (ImGui::Button("tonetfal")) setup_imgui_theme_tonetfal();
	ImGui::SameLine();
	if (ImGui::Button("codz01")) setup_imgui_theme_codz01();

	if (ImGui::Button("wpsimon")) setup_imgui_theme_wpsimon();
	ImGui::SameLine();
	if (ImGui::Button("adia")) setup_imgui_theme_adia();
}

void setup_imgui_theme(const Theme& theme)
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_WindowBg] = theme.background;
	colors[ImGuiCol_Text] = theme.f_high;
	colors[ImGuiCol_TextDisabled] = theme.f_low;
	colors[ImGuiCol_Border] = theme.b_high;
	colors[ImGuiCol_FrameBg] = theme.b_med;
	colors[ImGuiCol_FrameBgHovered] = theme.b_high;
	colors[ImGuiCol_FrameBgActive] = theme.b_inv;
	colors[ImGuiCol_TitleBg] = theme.b_high;
	colors[ImGuiCol_TitleBgActive] = theme.b_inv;
	colors[ImGuiCol_TitleBgCollapsed] = theme.b_low;
	colors[ImGuiCol_MenuBarBg] = theme.b_med;
	colors[ImGuiCol_ScrollbarBg] = theme.b_low;
	colors[ImGuiCol_ScrollbarGrab] = theme.b_high;
	colors[ImGuiCol_ScrollbarGrabHovered] = theme.b_inv;
	colors[ImGuiCol_ScrollbarGrabActive] = theme.f_high;
	colors[ImGuiCol_CheckMark] = theme.f_high;
	colors[ImGuiCol_SliderGrab] = theme.f_med;
	colors[ImGuiCol_SliderGrabActive] = theme.f_high;
	colors[ImGuiCol_Button] = theme.b_med;
	colors[ImGuiCol_ButtonHovered] = theme.b_high;
	colors[ImGuiCol_ButtonActive] = theme.b_inv;
	colors[ImGuiCol_Header] = theme.b_high;
	colors[ImGuiCol_HeaderHovered] = theme.b_inv;
	colors[ImGuiCol_HeaderActive] = theme.f_high;
	colors[ImGuiCol_Separator] = theme.b_high;
	colors[ImGuiCol_SeparatorHovered] = theme.f_high;
	colors[ImGuiCol_SeparatorActive] = theme.f_inv;
	colors[ImGuiCol_ResizeGrip] = theme.b_high;
	colors[ImGuiCol_ResizeGripHovered] = theme.f_high;
	colors[ImGuiCol_ResizeGripActive] = theme.f_inv;
	colors[ImGuiCol_Tab] = theme.b_med;
	colors[ImGuiCol_TabHovered] = theme.b_high;
	colors[ImGuiCol_TabSelected] = theme.b_inv;
	colors[ImGuiCol_TabDimmed] = theme.b_low;
	colors[ImGuiCol_TabDimmedSelected] = theme.b_high;
	colors[ImGuiCol_PlotLines] = theme.f_med;
	colors[ImGuiCol_PlotLinesHovered] = theme.f_high;
	colors[ImGuiCol_PlotHistogram] = theme.f_med;
	colors[ImGuiCol_PlotHistogramHovered] = theme.f_high;
	colors[ImGuiCol_TextSelectedBg] = theme.b_inv;
	colors[ImGuiCol_ModalWindowDimBg] = theme.b_inv;
}

void setup_imgui_theme_itmago()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	// style.Colors[ImGuiCol_TextHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	// style.Colors[ImGuiCol_TextActive] = ImVec4(1.00f, 1.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	// style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	// style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	// style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	// style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	// style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	// style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	// style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	// style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	// style.Colors[ImGuiCol_TooltipBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	// style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void setup_imgui_theme_purple()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.60f, 0.26f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.60f, 0.26f, 0.98f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.31f, 0.16f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.54f, 0.24f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.60f, 0.26f, 0.98f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.60f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.49f, 0.06f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.60f, 0.26f, 0.98f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.60f, 0.26f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.60f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.10f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.41f, 0.10f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.60f, 0.26f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.26f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.60f, 0.26f, 0.98f, 0.95f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.60f, 0.26f, 0.98f, 0.80f);
	colors[ImGuiCol_Tab] = ImVec4(0.37f, 0.18f, 0.58f, 0.86f);
	// colors[ImGuiCol_TabSelected] = ImVec4(0.42f, 0.20f, 0.68f, 1.00f);
	// colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.60f, 0.26f, 0.98f, 1.00f);
	// colors[ImGuiCol_TabDimmed] = ImVec4(0.11f, 0.07f, 0.15f, 0.97f);
	// colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.27f, 0.14f, 0.42f, 1.00f);
	// colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
	// colors[ImGuiCol_DockingPreview] = ImVec4(0.60f, 0.26f, 0.98f, 0.70f);
	// colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	// colors[ImGuiCol_TextLink] = ImVec4(0.60f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.53f, 0.53f, 0.53f, 0.45f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	// colors[ImGuiCol_NavCursor] = ImVec4(0.60f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void setup_imgui_theme_qy8q()
{
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.48f, 0.26f, 0.98f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.37f, 0.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.21f, 0.16f, 0.48f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.45f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.41f, 0.00f, 1.00f, 0.40f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.48f, 0.26f, 0.98f, 0.52f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.20f, 0.20f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.34f, 0.06f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.15f, 0.15f, 0.15f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.13f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.26f, 0.98f, 0.50f);
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.20f, 0.58f, 0.73f);
	// colors[ImGuiCol_TabSelected] = ImVec4(0.29f, 0.20f, 0.68f, 1.00f);
	// colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	// colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	// colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	// colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	// colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	// colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void setup_imgui_theme_simv0()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	// Base colors for a pleasant and modern dark theme with dark accents
	colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.94f, 1.00f);	 // Light grey text for readability
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.52f, 0.54f, 1.00f);	 // Subtle grey for disabled text
	colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);	 // Dark background with a hint of blue
	colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);	// Slightly lighter for child elements
	colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);	// Popup background
	colors[ImGuiCol_Border] = ImVec4(0.28f, 0.29f, 0.30f, 0.60f);  // Soft border color
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);	 // No border shadow
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);	// Frame background
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);  // Frame hover effect
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);  // Active frame background
	colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);	// Title background
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);  // Active title background
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.14f, 0.14f, 0.16f, 1.00f);	 // Collapsed title background
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);  // Menu bar background
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);	// Scrollbar background
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.26f, 0.28f, 1.00f);  // Dark accent for scrollbar grab
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.30f, 0.32f, 1.00f);	 // Scrollbar grab hover
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.32f, 0.34f, 0.36f, 1.00f);	// Scrollbar grab active
	colors[ImGuiCol_CheckMark] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);  // Dark blue checkmark
	colors[ImGuiCol_SliderGrab] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);  // Dark blue slider grab
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);	 // Active slider grab
	colors[ImGuiCol_Button] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);  // Dark blue button
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);  // Button hover effect
	colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);	 // Active button
	colors[ImGuiCol_Header] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);  // Header color similar to button
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);  // Header hover effect
	colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.42f, 0.52f, 1.00f);	 // Active header
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);  // Separator color
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);	 // Hover effect for separator
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);	// Active separator
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);  // Resize grip
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);  // Hover effect for resize grip
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.44f, 0.54f, 0.64f, 1.00f);	 // Active resize grip
	colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);	// Inactive tab
	colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.38f, 0.48f, 1.00f);  // Hover effect for tab
	colors[ImGuiCol_TabSelected] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);  // Active tab color
	colors[ImGuiCol_TabDimmed] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);	 // Unfocused tab
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.24f, 0.34f, 0.44f, 1.00f);  // Active but unfocused tab
	colors[ImGuiCol_PlotLines] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);  // Plot lines
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);	 // Hover effect for plot lines
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.36f, 0.46f, 0.56f, 1.00f);  // Histogram color
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.40f, 0.50f, 0.60f, 1.00f);	 // Hover effect for histogram
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);  // Table header background
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.28f, 0.29f, 0.30f, 1.00f);  // Strong border for tables
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);	 // Light border for tables
	colors[ImGuiCol_TableRowBg] = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);  // Table row background
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.22f, 0.24f, 0.26f, 1.00f);  // Alternate row background
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.24f, 0.34f, 0.44f, 0.35f);  // Selected text background
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.46f, 0.56f, 0.66f, 0.90f);  // Drag and drop target
	colors[ImGuiCol_NavCursor] = ImVec4(0.46f, 0.56f, 0.66f, 1.00f);	 // Navigation highlight
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);  // Windowing highlight
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);  // Dim background for windowing
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);	 // Dim background for modal windows
}

void setup_imgui_theme_tonetfal()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.04f, 0.04f, 0.04f, 0.99f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.51f, 0.51f, 0.80f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.03f, 0.03f, 0.04f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.17f, 0.17f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.30f, 0.60f, 0.10f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.60f, 0.10f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.43f, 0.90f, 0.11f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.21f, 0.22f, 0.23f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.51f, 0.51f, 0.80f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.54f, 0.55f, 0.55f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.51f, 0.51f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.16f, 0.16f, 0.16f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.23f, 0.23f, 0.24f, 0.80f);
	colors[ImGuiCol_Tab] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	// colors[ImGuiCol_TabSelected] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	// colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.13f, 0.78f, 0.07f, 1.00f);
	// colors[ImGuiCol_TabDimmed] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	// colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	// colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.10f, 0.60f, 0.12f, 1.00f);
	// colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	// colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.14f, 0.87f, 0.05f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.30f, 0.60f, 0.10f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.23f, 0.78f, 0.02f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.46f, 0.47f, 0.46f, 0.06f);
	// colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	// colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.78f, 0.69f, 0.69f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void setup_imgui_theme_codz01()
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 5.3f;
	style.FrameRounding = 2.3f;
	style.ScrollbarRounding = 0;

	style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
	// style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
	// style.Colors[ImGuiCol_ComboBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
	// style.Colors[ImGuiCol_Column] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	// style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	// style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
	// style.Colors[ImGuiCol_CloseButton] = ImVec4(0.50f, 0.50f, 0.90f, 0.50f);
	// style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.70f, 0.70f, 0.90f, 0.60f);
	// style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	// style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void setup_imgui_theme_wpsimon()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Corners
	style.WindowRounding = 8.0f;
	style.ChildRounding = 8.0f;
	style.FrameRounding = 6.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabRounding = 6.0f;
	style.TabRounding = 6.0f;

	// Colors
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.50f, 0.56f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.56f, 1.00f, 0.80f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.14f, 0.22f, 0.36f, 1.00f);
	// colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	// colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void setup_imgui_theme_adia()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* colors = style.Colors;

	// Base Colors
	ImVec4 bgColor = ImVec4(0.10f, 0.105f, 0.11f, 1.00f);
	ImVec4 lightBgColor = ImVec4(0.15f, 0.16f, 0.17f, 1.00f);
	ImVec4 panelColor = ImVec4(0.17f, 0.18f, 0.19f, 1.00f);
	ImVec4 panelHoverColor = ImVec4(0.20f, 0.22f, 0.24f, 1.00f);
	ImVec4 panelActiveColor = ImVec4(0.23f, 0.26f, 0.29f, 1.00f);
	ImVec4 textColor = ImVec4(0.86f, 0.87f, 0.88f, 1.00f);
	ImVec4 textDisabledColor = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	ImVec4 borderColor = ImVec4(0.14f, 0.16f, 0.18f, 1.00f);

	// Text
	colors[ImGuiCol_Text] = textColor;
	colors[ImGuiCol_TextDisabled] = textDisabledColor;

	// Windows
	colors[ImGuiCol_WindowBg] = bgColor;
	colors[ImGuiCol_ChildBg] = bgColor;
	colors[ImGuiCol_PopupBg] = bgColor;
	colors[ImGuiCol_Border] = borderColor;
	colors[ImGuiCol_BorderShadow] = borderColor;

	// Headers
	colors[ImGuiCol_Header] = panelColor;
	colors[ImGuiCol_HeaderHovered] = panelHoverColor;
	colors[ImGuiCol_HeaderActive] = panelActiveColor;

	// Buttons
	colors[ImGuiCol_Button] = panelColor;
	colors[ImGuiCol_ButtonHovered] = panelHoverColor;
	colors[ImGuiCol_ButtonActive] = panelActiveColor;

	// Frame BG
	colors[ImGuiCol_FrameBg] = lightBgColor;
	colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
	colors[ImGuiCol_FrameBgActive] = panelActiveColor;

	// Tabs
	colors[ImGuiCol_Tab] = panelColor;
	colors[ImGuiCol_TabHovered] = panelHoverColor;
	colors[ImGuiCol_TabSelected] = panelActiveColor;
	colors[ImGuiCol_TabDimmed] = panelColor;
	colors[ImGuiCol_TabDimmedSelected] = panelHoverColor;

	// Title
	colors[ImGuiCol_TitleBg] = bgColor;
	colors[ImGuiCol_TitleBgActive] = bgColor;
	colors[ImGuiCol_TitleBgCollapsed] = bgColor;

	// Scrollbar
	colors[ImGuiCol_ScrollbarBg] = bgColor;
	colors[ImGuiCol_ScrollbarGrab] = panelColor;
	colors[ImGuiCol_ScrollbarGrabHovered] = panelHoverColor;
	colors[ImGuiCol_ScrollbarGrabActive] = panelActiveColor;

	// Checkmark
	colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

	// Slider
	colors[ImGuiCol_SliderGrab] = panelHoverColor;
	colors[ImGuiCol_SliderGrabActive] = panelActiveColor;

	// Resize Grip
	colors[ImGuiCol_ResizeGrip] = panelColor;
	colors[ImGuiCol_ResizeGripHovered] = panelHoverColor;
	colors[ImGuiCol_ResizeGripActive] = panelActiveColor;

	// Separator
	colors[ImGuiCol_Separator] = borderColor;
	colors[ImGuiCol_SeparatorHovered] = panelHoverColor;
	colors[ImGuiCol_SeparatorActive] = panelActiveColor;

	// Plot
	colors[ImGuiCol_PlotLines] = textColor;
	colors[ImGuiCol_PlotLinesHovered] = panelActiveColor;
	colors[ImGuiCol_PlotHistogram] = textColor;
	colors[ImGuiCol_PlotHistogramHovered] = panelActiveColor;

	// Text Selected BG
	colors[ImGuiCol_TextSelectedBg] = panelActiveColor;

	// Modal Window Dim Bg
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.10f, 0.105f, 0.11f, 0.5f);

	// Tables
	colors[ImGuiCol_TableHeaderBg] = panelColor;
	colors[ImGuiCol_TableBorderStrong] = borderColor;
	colors[ImGuiCol_TableBorderLight] = borderColor;
	colors[ImGuiCol_TableRowBg] = bgColor;
	colors[ImGuiCol_TableRowBgAlt] = lightBgColor;

	// Styles
	style.FrameBorderSize = 1.0f;
	style.FrameRounding = 2.0f;
	style.WindowBorderSize = 1.0f;
	style.PopupBorderSize = 1.0f;
	style.ScrollbarSize = 12.0f;
	style.ScrollbarRounding = 2.0f;
	style.GrabMinSize = 7.0f;
	style.GrabRounding = 2.0f;
	style.TabBorderSize = 1.0f;
	style.TabRounding = 2.0f;

	// Reduced Padding and Spacing
	style.WindowPadding = ImVec2(5.0f, 5.0f);
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.ItemSpacing = ImVec2(6.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);

	/*

	// Font Scaling
	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = 0.95f;

	io.Fonts->AddFontDefault();
	float baseFontSize = 18.0f;
	float iconFontSize = baseFontSize * 2.0f / 3.0f;

	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	io.Fonts->AddFontFromFileTTF(
		(std::string(RESOURCE_DIR) + "/fonts/" + FONT_ICON_FILE_NAME_FA).c_str(),
		iconFontSize,
		&icons_config,
		icons_ranges
	);
	*/
}

#else

void test_themes()
{
}

#endif

}  //  namespace klotter

