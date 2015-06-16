// test libsndfile functions.
//   Note: This code will not compile without the c++dsp library.  It relies on several c++ types
// that are specific to that library and it may not be public yet.
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include "sndfile.h"
#include "dsp_containers.h"
#include "dsp_file.h"

static void list_formats(void)
{
	SF_FORMAT_INFO	info;
	SF_INFO 		sfinfo;
	int format, major_count, subtype_count;

	memset(&sfinfo, 0, sizeof(sfinfo));
	printf("Version : %s\n\n", sf_version_string());

	sf_command(NULL, SFC_GET_FORMAT_MAJOR_COUNT, &major_count, sizeof(int));
	sf_command(NULL, SFC_GET_FORMAT_SUBTYPE_COUNT, &subtype_count, sizeof(int));

	sfinfo.channels = 1;
	for (int majorfmt = 0; majorfmt < major_count; ++majorfmt)
	{
		info.format = majorfmt;
		sf_command(NULL, SFC_GET_FORMAT_MAJOR, &info, sizeof(info));
		printf("(%d...): %s  (extension \"%s\")\n", majorfmt, info.name, info.extension);

		format = info.format;

		for (int subfmt = 0; subfmt < subtype_count; ++subfmt)
		{
			info.format = subfmt;
			sf_command(NULL, SFC_GET_FORMAT_SUBTYPE, &info, sizeof(info));

			format = (format & SF_FORMAT_TYPEMASK) | info.format;

			sfinfo.format = format;
			if (sf_format_check(&sfinfo))
				printf("(%d.%d):   \t%s\n", majorfmt, subfmt, info.name);
		}
		puts("");
	}
	puts("");

	return;
}


static void list_simple_formats()
{
	SF_FORMAT_INFO	format_info;
	int i;
	int count;

	sf_command(NULL, SFC_GET_SIMPLE_FORMAT_COUNT, &count, sizeof(int));

	for (i = 0; i < count; ++i)
	{
		format_info.format = i;
		sf_command(NULL, SFC_GET_SIMPLE_FORMAT, &format_info, sizeof(format_info));
		printf("[%02x]{ %08x,\t\"%s\",\t\"%s\" }\n", i, format_info.format, format_info.name, format_info.extension);
	}
}



// ********************************
// **** dBFS_to_scalar/scalar_to_dBSF
double dBFS_to_scalar(double decibel_val)
{
	return std::pow(10.0, decibel_val / 20.0);
}
// ********************************

// ********************************
double scalar_to_dBFS(double scalar_val)
{
	return 20.0 * std::log(scalar_val);
}
// **** End dBSF_to_scalar/scalar_to_dBSF
// ********************************


//
struct
{
	int id;
	const char *str;
} sf_str_array[] =
{
	{ SF_STR_TITLE,			"Title string."			},
	{ SF_STR_COPYRIGHT,		"Copyright string."		},
	{ SF_STR_SOFTWARE,		"Software string."		},
	{ SF_STR_ARTIST,		"Artist string."		},
	{ SF_STR_COMMENT,		"Comment string."		},
	{ SF_STR_DATE,			"2015-06-16"			},
	{ SF_STR_ALBUM,			"Album string."			},
	{ SF_STR_LICENSE,		"License string."		},
	{ SF_STR_TRACKNUMBER,	"Track number string."	},
	{ SF_STR_GENRE,			"Genre string."			},
	{ -1, NULL }
};


#ifndef M_PI
const long double M_PI = 3.141592653589793238462643383279502884;
const long double M_PI2 = 2.0 * M_PI;
#endif

// Generate a log sweep.
void genSweep(double *buffer, int numFrames, int numChannels, int sampleRate, float minFreq, float maxFreq)
{
	double start = M_PI2 * minFreq;
	double stop = M_PI2 * maxFreq;
	double tmp1 = log(stop / start);

	int s, f;
	for (f = s = 0; s < numFrames * numChannels; ++f)
	{
		double t = (double)f / numFrames;
		double tmp2 = exp(t * tmp1) - 1.0;
		for (int chan = 0; chan < numChannels; ++chan, ++s)
			buffer[s] = sin((start * numFrames * tmp2) / (sampleRate * tmp1));
	}
}


// Generate some files.
static void cpp_gen_test()
{
	#define SRATE 48000
	#define CHANS 6
	#define FRAMES (SRATE * 2)
	#define STYPE float

	puts("\nAttempting to open \"gen.wav\" for output.\n");
	dsp::dspfile gen32("gen32.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_FLOAT, CHANS, SRATE);
	dsp::dspfile gen8("gen8.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_U8, CHANS, SRATE);
	dsp::dspfile gen16("gen16.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16, CHANS, SRATE);
	dsp::dspfile gen24("gen24.wav", SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_24, CHANS, SRATE);
	dsp::dspfile gencaf("gen.caf", SFM_WRITE, SF_FORMAT_CAF | SF_FORMAT_ALAC_32, CHANS, SRATE);

	// Check that the file was opened.
	if ((!gen32.is_open()) || (!gen8.is_open()) || (!gen16.is_open()) || (!gen24.is_open()) || (!gencaf.is_open()))
	{
		puts("\n\nError: Could not open gen.wav for output.");
		puts(gen32.get_error_str()); // Really doesn't matter which one we use to get the message... I think...
		return;
	}

	// Set some strings so we know that they are working.
	puts("Setting some string information for \"gen.wav\".");
	for (int i = 0; sf_str_array[i].str != NULL; ++i)
	{
		gen32.set_string(sf_str_array[i].id, sf_str_array[i].str);
		gen8.set_string(sf_str_array[i].id, sf_str_array[i].str);
		gen16.set_string(sf_str_array[i].id, sf_str_array[i].str);
		gen24.set_string(sf_str_array[i].id, sf_str_array[i].str);
		gencaf.set_string(sf_str_array[i].id, sf_str_array[i].str);
	}

	// Fill bext chunk with data and send to libsndfile
	puts("Creating BWF-bext chunk information for \"gen.wav\".");
	dsp::dspbwf bwf;
	bwf.default_fill();
	bwf.set_description(std::string("gen.wav - 1KHz sine wave at -3dBFS, generated by test_libsndfile.cpp"));
	gen32.command(SFC_SET_BROADCAST_INFO, bwf.data(), bwf.size());
	gen8.command(SFC_SET_BROADCAST_INFO, bwf.data(), bwf.size());
	gen16.command(SFC_SET_BROADCAST_INFO, bwf.data(), bwf.size());
	gen24.command(SFC_SET_BROADCAST_INFO, bwf.data(), bwf.size());
	gencaf.command(SFC_SET_BROADCAST_INFO, bwf.data(), bwf.size());

	// Create multi-channel buffer.
	puts("Creating multi-channel buffer for \"gen.wav\".");
	dsp::dspvector<STYPE>	buf(FRAMES * CHANS);
	dsp::dspvector<uint8_t>	buf8(FRAMES * CHANS);
	dsp::dspvector<int16_t>	buf16(FRAMES * CHANS);
	dsp::dspvector<int24_t>	buf24(FRAMES * CHANS);
	dsp::dspvector<>		bufcaf(FRAMES * CHANS);

#if 0
	// Setup parameters for creating a different sine wave for each channel
	puts("Setting multi-channel sine wave parameters for \"gen.wav\".");
	double rad[CHANS];
	double xsin[CHANS];
	for (int ch = 0; ch < CHANS; ++ch)
	{
		rad[ch] = (M_PI2 / ((double)SRATE)) * (500.0 + (500.0 * ch));
		xsin[ch] = 0;
	}
	// Create interleaved multichannel wave form.
	puts("Filling output buffer with sine waves for \"gen.wav\".");
	for (int s = 0; s < FRAMES * CHANS; )
	{
		for (int ch = 0; ch < CHANS; ++ch, ++s)
		{
			buf[s]		= (std::sin(xsin[ch]) * dBFS_to_scalar(-3.0));
			buf8[s]		= (std::sin(xsin[ch]) * dBFS_to_scalar(-3.0));
			buf16[s]	= (std::sin(xsin[ch]) * dBFS_to_scalar(-3.0));
			buf24[s]	= (std::sin(xsin[ch]) * dBFS_to_scalar(-3.0));
			bufcaf[s]	= (std::sin(xsin[ch]) * dBFS_to_scalar(-3.0));
			xsin[ch] += rad[ch];
		}
	}

#else
	// Create interleaved multichannel wave form.
	puts("Generating log sweep for \"gen.wav\".");
	dsp::dspvector<double> sweep(FRAMES * CHANS);
	genSweep((double*)sweep.data(), FRAMES, CHANS, SRATE, 20, 20000);

	// Create interleaved multichannel wave form.
	puts("Filling output buffer with sine waves for \"gen.wav\".");
	for (int s = 0; s < FRAMES * CHANS; )
	{
		for (int ch = 0; ch < CHANS; ++ch, ++s)
		{
			buf[s] = sweep[s] * dBFS_to_scalar(-3.0);
			buf8[s] = sweep[s] * dBFS_to_scalar(-3.0);
			buf16[s] = sweep[s] * dBFS_to_scalar(-3.0);
			buf24[s] = sweep[s] * dBFS_to_scalar(-3.0);
			bufcaf[s] = sweep[s] * dBFS_to_scalar(-3.0);
		}
	}

#endif

	// Write data.
	puts("Writing multi-channel sine wave parameters for \"gen.wav\".");
	gen32.write(buf);
	gen8.write(buf8);
	gen16.write(buf16);
	gen24.write(buf24);

#if 0
	#define CAFBUFNUM (8192)
	float *pos = (float*)bufcaf.data();
	int num = bufcaf.size() / CAFBUFNUM;
	int mod = bufcaf.size() % CAFBUFNUM;
	for (int i = 0; i < num; ++i, pos += CAFBUFNUM)
		gencaf.write(pos, CAFBUFNUM);
	if (mod)
		gencaf.write(pos, mod);

#else
	gencaf.write(bufcaf);
#endif

	// Exit this function and everything is automatically cleaned up.
	puts("Closing \"gen.wav\".");
}

// main
int main(int argc, char * argv[])
{
	puts("list_formats();");
	list_formats();
	puts("list_formats(); done...\n");

	puts("list_simple_formats();");
	list_simple_formats();
	puts("list_simple_formats(); Done...\n");

	puts("cpp_gen_test();");
	cpp_gen_test();
	puts("cpp_gen_test(); Done...\n");
	return 0;
}

