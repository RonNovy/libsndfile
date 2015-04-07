// test libsndfile functions.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <sndfile.h>
#include <sndfile.hh>

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

// main
int main(int argc, char * argv[])
{
	list_formats();
	list_simple_formats();
	return 0;
}