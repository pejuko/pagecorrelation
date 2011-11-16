#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include <leptonica/allheaders.h>

#ifndef DEBUG
#define DEBUG 0
#endif

typedef enum {
	THRESHOLD, THRESHOLD_OTSU, THRESHOLD_DITHER
} t_threshold_method;

typedef struct options {
	float size_threshold;
	float correlation_threshold;
	int   correlation_for_text;
	int   min_lines;
	int   min_words;
	int   blank_threshold;
	int   width;
	int   height;
	int   threshold;
	char *fname1;
	char *fname2;
	t_threshold_method method;
} t_options;


void options_reset(t_options *o) {
	o->size_threshold = 0.6;
	o->correlation_threshold = 0.49;
	o->correlation_for_text = 1;
	o->min_lines = 3;
	o->min_words = 3;
	o->blank_threshold = 3000;
	o->width = 800;
	o->height = 1100;
	o->threshold = 128;
	o->method = THRESHOLD_OTSU;
	o->fname1 = NULL;
	o->fname2 = NULL;
}


t_options* options_new()
{
	t_options *o = (t_options*)malloc(sizeof(t_options));
	options_reset(o);

	return o;
}


void options_free(t_options **o)
{
	if (! o) return;
	if (! *o) return;
	if ((*o)->fname1) free((*o)->fname1);
	if ((*o)->fname2) free((*o)->fname2);
	free(*o);
	*o = NULL;
}


void help()
{
	printf("\nUsage: page_correlation [options] [--] <image_file1> <image_file2>\n\n");

	printf("--size-threshold=,-s<[0,1]> (default: 0.6)\n");
	printf("\tImages will be qual only if their sizes ratio is lower then size-threshold.\n");
	printf("\t1 means their sizes must be equal\n");
	printf("\t0 their sizes does not matter\n");
	printf("\n");

	printf("--correlation-threshold=,c<[0,1]> (default: 0.6)\n");
	printf("\tIf layout analysis fails pixel correlation is computed.\n");
	printf("\t1 means they are equal\n");
	printf("\t0 means they are different\n");
	printf("\n");

	printf("--threshold-method=,-m<otsu|threshold|dither> (default: otsu)\n");
	printf("\tOtsu is adaptive thresholding method which try select best value for an image.\n");
	printf("\tThreshold will make thresholding using --threshold argument\n");
	printf("\tDither\n");
	printf("\n");

	printf("--threshold=,-t<n> (default: 128)\n");
	printf("\tBinarization parameter. Everything below is black and above white.\n");
	printf("\n");

	printf("--min-lines=,-l<n> (default: 3)\n");
	printf("\tMinimum n textual lines have to be equal.\n");
	printf("\n");

	printf("--min-words=,-v<n> (default: 3)\n");
	printf("\tMinimum n words have to be equal to consider lines equal.\n");
	printf("\n");

	printf("--blank-threshold=,-b<pixels> (default: 3000)\n");
	printf("\tNumber of black pixels allowed in image to consider it as a blank page.\n");
	printf("\n");

	printf("--width=,-w<n> (default: 800)\n");
	printf("--height=,-h<n> (default: 1100)\n");
	printf("\tTo speed up page analysis images are scaled down.\n");
	printf("\t0 for one argument means keep aspect ratio.\n");
	printf("\n");

	printf("--disable-text-correlation\n");
	printf("\tIf layout analysis thinks two images does not match then pixel correlation matters.\n");
	printf("\tThis disable pixel correlation for text documents.");
	printf("\n");

	printf("--help,-?\n");
	printf("\n");

	printf("--\n");
	printf("\tSwitch off option parsing. Useful if image file name starts with '-'.\n");
	printf("\n");

	printf("Returns 0 if images seems to be equal and 1 otherwise.\n");
	printf("Returns 100 on argument error.\n");
	printf("Returns 101 resp 102 if image 1 resp image 2 does not exist.\n");
	printf("\n");
}


t_options* parse_command_line(int argc, char **argv)
{
	t_options *o = options_new();
	int c;

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index = 0;
		static struct option long_options[] = {
			{"size-threshold", required_argument, 0, 's'},
			{"correlation-threshold", required_argument, 0, 'c'},
			{"threshold-method", required_argument, 0, 'm'},
			{"threshold", required_argument, 0, 't'},
			{"min-lines", required_argument, 0, 'l'},
			{"min-words", required_argument, 0, 'v'},
			{"blank-threshold", required_argument, 0, 'b'},
			{"width", required_argument, 0, 'w'},
			{"height", required_argument, 0, 'h'},
			{"disable-text-correlation", no_argument, 0, 1},
			{"help", no_argument, 0, '?'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "s:c:m:t:l:v:b:w:h:?", long_options, &option_index);

		switch (c) {
			case 1:
				o->correlation_for_text = 0;
				break;
			case 's':
				o->size_threshold = atof(optarg);
				break;
			case 'c':
				o->correlation_threshold = atof(optarg);
				break;
			case 'm':
				if (strcmp("otsu", optarg) == 0)
					o->method = THRESHOLD_OTSU;
				else if (strcmp("dither", optarg) == 0)
					o->method = THRESHOLD_DITHER;
				else if (strcmp("threshold", optarg) == 0)
					o->method = THRESHOLD;
				else {
					printf("Unknown threshold method '%s'\n", optarg);
					options_free(&o);
					return NULL;
				}
				break;
			case 't':
				o->threshold = atoi(optarg);
				break;
			case 'l':
				o->min_lines = atoi(optarg);
				break;
			case 'v':
				o->min_words = atoi(optarg);
				break;
			case 'b':
				o->blank_threshold = atoi(optarg);
				break;
			case 'w':
				o->width = atoi(optarg);
				 break;
			case 'h':
				o->height = atoi(optarg);
				 break;
			case -1:
				if (optind <= argc-2) {
					o->fname1 = strdup(argv[optind]);
					o->fname2 = strdup(argv[optind+1]);
				}
				break;
			default: /* --help, -? */
				 options_free(&o);
				 return NULL;
				 break;
		}

		if (c == -1) break;
	}

	if (!o->fname1 || !o->fname2) {
		options_free(&o);
		return NULL;
	}

	return o;
}


char *joinstr(char *s1, char *s2)
{
	char *s = (char*)malloc(strlen(s1) + strlen(s2) + 10);

	sprintf(s, "%s%s.png", s1, s2);

	return s;
}

void write(char *name, char *j, PIX *pix)
{
	if (! DEBUG)
		return;

	char *fname = joinstr(name, j);
	pixWrite(fname, pix, IFF_PNG);
	free(fname);
}

PIX* binarize(PIX *pix, t_options *o)
{
	PIX *pixb;
	PIX *pt = NULL;
	l_uint32 threshold = o->threshold;

	/*pixn = pixBackgroundNormFlex(pixs, 7, 7, 1, 1, 10);*/

	switch (o->method) {
		case THRESHOLD_DITHER:
			pixb = pixDitherToBinary(pix);
			break;
		case THRESHOLD_OTSU:
			pixOtsuAdaptiveThreshold(pix, pix->w, pix->h, 0, 0, 0, &pt, 0);
			pixGetPixel(pt, 0, 0, &threshold);
			//printf("Threshold: %d\n", threshold);
			//pixb = pixConvertTo1(pixs, threshold);
			pixb = pixThresholdToBinary(pix, threshold);
			break;
		default: /* threshold */
			pixb = pixThresholdToBinary(pix, threshold);
			break;
	}

	if (pt) pixDestroy(&pt);

	return pixb;
}

PIX* scale(PIX *pix, char *name, t_options *o)
{
	PIX *pixg, *pix8, *pixb, *pixs, *pixc;
	BOXA *boxa;
	NUMA *na;
	BOX *cbox = NULL;
	NUMAA *naa;
	l_int32 x, y, w, h;

	pixg = pixRemoveColormap(pix, REMOVE_CMAP_TO_GRAYSCALE);
	pix8 = pixConvertTo8(pixg, FALSE);
	write(name, "-8", pix8);

	pixb = binarize(pix8, o);
	//write(name, "-b1", pixb);
	pixGetWordBoxesInTextlines(pixb, 1, 5, 8, o->width, 100, &boxa, &na);
	naa = boxaExtractSortedPattern(boxa, na);
	//boxa = pixConnCompBB(pixb, 4);
	if (naa->n >= o->min_lines) {
		write(name, "-box", pixDrawBoxaRandom(pixb, boxa, 2));
		boxaGetExtent(boxa, 0, 0, &cbox);
	}

	/*
	x = (l_int32)(((float)(cbox->x) / (float)(pixb->w)) * (float)(pix8->w)) - 5;
	w = (l_int32)(((float)(cbox->w) / (float)(pixb->w)) * (float)(pix8->w)) + 10;
	y = (l_int32)(((float)(cbox->y) / (float)(pixb->h)) * (float)(pix8->h)) - 5;
	h = (l_int32)(((float)(cbox->h) / (float)(pixb->h)) * (float)(pix8->h)) + 10;

	if (x<0) x=0;
	if (y<0) y=0;
	if (h > pix8->h) h = pix8->h;
	if (w > pix8->w) h = pix8->w;

	cbox->x = x;
	cbox->w = w;
	cbox->y = y;
	cbox->h = h;
	*/
	if (cbox)
		pixc = pixClipRectangle(pix8, cbox, 0);
	else
		pixc = pixClone(pix8);
	write(name, "-c", pixc);

	pixs = pixScaleToSize(pixc, o->width, o->height);

	pixDestroy(&pixb);
	pixb = binarize(pixs, o);
	write(name, "-b2", pixb);

	pixDestroy(&pixg);
	pixDestroy(&pix8);
	pixDestroy(&pixc);
	pixDestroy(&pixs);

	return pixb;
}


int compare(l_int32 num_pix1, l_int32 num_pix2, l_int32 min_numa, l_float32 size_ratio, l_int32 same, l_float32 correlation, t_options *o)
{
	if (size_ratio <= o->size_threshold) return 0;
	if (same == 1) return 1;
	if (! o->correlation_for_text && min_numa > o->min_lines) return 0;
	if (num_pix1<o->blank_threshold && num_pix2<o->blank_threshold) return 1; // blank pages (3000 is about 0.3%)
	if (correlation > o->correlation_threshold) return 1;

	return 0;
}


int main(int argc, char **argv)
{
	t_options *o = parse_command_line(argc, argv);

	PIX *pix1, *pix2, *pixb1, *pixb2, *pixth, *pixd;
	l_float32 correlation=0.0, size_ratio=0.0, pixels1, pixels2, fract_xor, fract_substract;
	l_int32 *sumtab = makePixelSumTab8(), num_pix1, num_pix2, same, count_numaa, min_numa;
	int decision = 0;
	BOXA *boxa1, *boxa2;
	NUMA *na1, *na2;
	NUMAA *naa1, *naa2;

	if (! o) {
		help();
		options_free(&o);
		return 100;
	}

	pix1 = pixRead(o->fname1);
	pix2 = pixRead(o->fname2);
	if (! pix1) {
		options_free(&o);
		return 101;
	}
	if (! pix2) {
		options_free(&o);
		return 102;
	}

	printf("%s (%dx%d) - %s (%dx%d)", o->fname1, pix1->w, pix1->h, o->fname2, pix2->w, pix2->h);

	pixels1 = pix1->w * pix1->h;
	pixels2 = pix2->w * pix2->h;
	size_ratio = (pixels1 > pixels2) ? pixels2/pixels1 : pixels1/pixels2;
	printf("   Size ratio: %f", size_ratio);

	pixb1 = scale(pix1, "pix1", o);
	pixb2 = scale(pix2, "pix2", o);
	/*printf("pixb1: %dx%d\npixb2: %dx%d\n", pixb1->w, pixb1->h, pixb2->w, pixb2->h);*/

	pixGetWordBoxesInTextlines(pixb1, 1, 3, 5, o->width/4, 50, &boxa1, &na1);
	if (boxa1->n > 0)
		write("pixb1", "-box", pixDrawBoxaRandom(pixb1, boxa1, 2));
	pixGetWordBoxesInTextlines(pixb2, 1, 3, 5, o->width/4, 50, &boxa2, &na2);
	if (boxa2->n > 0)
		write("pixb2", "-box", pixDrawBoxaRandom(pixb2, boxa2, 2));
	naa1 = boxaExtractSortedPattern(boxa1, na1);
	naa2 = boxaExtractSortedPattern(boxa2, na2);
	count_numaa = (numaaGetCount(naa1) > numaaGetCount(naa2)) ? numaaGetCount(naa2) : numaaGetCount(naa1);
	min_numa = o->min_lines;
	if (count_numaa/4.0 > min_numa)
		min_numa = count_numaa/4.0;

	printf("   numaa: %dx%d   min_numa: %d", numaaGetCount(naa1), numaaGetCount(naa2), min_numa);
	//numaaCompareImagesByBoxes(naa1, naa2, 5, 10, 150, 150, 20, 20, &same, 1);
	numaaCompareImagesByBoxes(naa1, naa2, o->min_words, min_numa, 150, 150, 20, 20, &same, 0);

	printf("   same: %d", same);

	boxaDestroy(&boxa1);
	boxaDestroy(&boxa2);
	numaDestroy(&na1);
	numaDestroy(&na2);
	numaaDestroy(&naa1);
	numaaDestroy(&naa2);

	//pixCorrelationBinary(pixb1, pixb2, &correlation);
	
	pixCountPixels(pixb1, &num_pix1, sumtab);
	pixCountPixels(pixb2, &num_pix2, sumtab);
	//printf("   num_pix1: %d   num_pix2: %d", num_pix1, num_pix2);
	if (num_pix1>0 && num_pix2>0)
		correlation = pixCorrelationScore(pixb1, pixb2, num_pix1, num_pix2, 0, 0, 10, 100, sumtab);

	if (correlation > 1.0) correlation = 1.0;
	if (correlation < 0) correlation = 0;
	printf("   Correlation: %f", correlation);


	/*
	pixCompareBinary(pixb1, pixb2, L_COMPARE_XOR, &fract_xor, NULL);
	pixCompareBinary(pixb1, pixb2, L_COMPARE_SUBTRACT, &fract_substract, NULL);
	printf("   Fract XOR: %f   Fract Substract: %f", fract_xor, fract_substract);
	*/


	decision = compare(num_pix1, num_pix2, min_numa, size_ratio, same, correlation, o);
	printf("   Decision: %d", decision);

	printf("\n");

	pixDestroy(&pix1);
	pixDestroy(&pix2);
	pixDestroy(&pixb1);
	pixDestroy(&pixb2);
	free(sumtab);
	options_free(&o);

	return ! decision;
}
