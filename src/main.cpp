//#define auxDIBImageLoad auxDIBImageLoadW
//#define _WIN32
#if defined _WIN32 && !defined __MINGW32__
#include <windows.h>
#include <crtdbg.h>
#else
#define _ASSERT(expr) ((void)0)
#define _ASSERTE(expr) ((void)0)
#endif
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <stdint.h>

#define width 960;
#define height 540;

using namespace std;
using namespace cv;

int is_little_endian();
void write_pfm_file(const char *filename, float *depth, int w, int h);
float *read_pfm_file(const char *filename, int *w, int *h);

int main(int argc, char** argv){
	// loop through all frames
	char input_L[1024], input_R[1024], saveLeft[1024], saveRight[1024];
	
	for (int32_t i = 1; i <= 800; i++) {
		// input file names
		cout << "frame " << i << endl;
		strncpy(input_L, argv[1], strlen(argv[1]));
		input_L[strlen(argv[1])] = '\0';
		strncpy(input_R, argv[1], strlen(argv[1]));
		input_R[strlen(argv[1])] = '\0';
		//strncpy(saveLeft, argv[2], strlen(argv[2]));
		//saveLeft[strlen(argv[2])] = '\0';
		//strncpy(saveRight, argv[2], strlen(argv[2]));
		//saveRight[strlen(argv[2])] = '\0';
		char img_name[256]; sprintf(img_name, "%04d.pfm", i);
		char disp_name[256]; sprintf(disp_name, "%04d.pfm", i);
		strcat(input_L, "\\left\\");
		strcat(input_L, img_name);
		strcat(input_R, "\\right\\");
		strcat(input_R, img_name);
		//strcat(saveLeft, "\\left\\");
		//strcat(saveLeft, disp_name);
		//strcat(saveRight, "\\right\\");
		//strcat(saveRight, disp_name);
		
		int ncol = width;
		int nrow = height;
		float* dleft = read_pfm_file(input_L, &ncol, &nrow);
		float* dright = read_pfm_file(input_R, &ncol, &nrow);
		
		float maxl = dleft[0];
		float minl = dleft[0];
		for (int i = 0; i< ncol*nrow; i++)
		{
			if (maxl < dleft[i])
				maxl = dleft[i];
			if (minl > dleft[i])
				minl = dleft[i];
		}
		float maxr = dright[0];
		float minr = dright[0];
		for (int i = 0; i< ncol*nrow; i++)
		{
			if (maxr < dright[i])
				maxr = dright[i];
			if (minr > dright[i])
				minr = dright[i];
		}

		cv::Mat dl(nrow, ncol, CV_8UC1); // CV_32FC1); //CV_16UC1);//
		cv::Mat dr(nrow, ncol, CV_8UC1); // CV_32FC1); //CV_16UC1);//
		for (int i = 0; i < nrow; i++){
			for (int j = 0; j < ncol; j++){
				dl.data[i*ncol + j] = (UINT8)( (dleft[i*ncol + j] - minl)/(maxl - minl)*255 );
				dr.data[i*ncol + j] = (UINT8)( (dright[i*ncol + j] - minr)/(maxr - minr)*255 );
			}
		}
		cv::imshow("dl", dl);
		cv::imshow("dr", dr);
		cv::waitKey(-1);

		//write_pfm_file(saveLeft, dleft, ncol, nrow);
		//write_pfm_file(saveRight, dright, ncol, nrow);

	}
	return 0;
}
/*
int main(int argc, char** argv){
	float* dleft;
	float* dright;

	char namel[128], namer[128];
	////for (int i = 0; i<N; i++){
	sprintf(namel, "%04d.pfm", 0);//i);//
	sprintf(namer, "%04d.pfm", 0);//i);//

	int ncol = width;
	int nrow = height;
	write_pfm_file(namel, dleft, ncol, nrow);
	float* readdleft = read_pfm_file(namel, &ncol, &nrow);

	float maxl = dleft[0];
	float minl = dleft[0];
	for (int i = 0; i< ncol*nrow; i++)
	{
		if (maxl < dleft[i])
			maxl = dleft[i];
		if (minl > dleft[i])
			minl = dleft[i];
	}
	float maxr = dright[0];
	float minr = dright[0];
	for (int i = 0; i< ncol*nrow; i++)
	{
		if (maxr < dright[i])
			maxr = dright[i];
		if (minr > dright[i])
			minr = dright[i];
	}

	cv::Mat dl(nrow, ncol, CV_8UC1); // CV_32FC1); //CV_16UC1);//
	cv::Mat dr(nrow, ncol, CV_8UC1); // CV_32FC1); //CV_16UC1);//
	for (int i = 0; i < nrow; i++){
		for (int j = 0; j < ncol; j++){
			dl.data[i*ncol + j] = (UINT8)( (dleft[i*ncol + j] - minl)/(maxl - minl)*255 );
			dr.data[i*ncol + j] = (UINT8)( (dright[i*ncol + j] - minr)/(maxr - minr)*255 );
		}
	}
	cv::imshow("dl", dl);
	cv::imshow("dr", dr);
	cv::waitKey(-1);

	return 0;
}
*/
int is_little_endian() {
	if (sizeof(float) != 4) { printf("Bad float size.\n"); exit(1); }
	byte b[4] = { 255, 0, 0, 0 };
	return *((float *)b) < 1.0;
}

void write_pfm_file(const char *filename, float *depth, int w, int h) {
	FILE *f = fopen(filename, "wb");

	double scale = 1.0;
	if (is_little_endian()) { scale = -scale; }

	fprintf(f, "Pf\n%d %d\n%lf\n", w, h, scale);
	for (int i = 0; i < w*h; i++) {
		float d = depth[i];
		fwrite((void *)&d, 1, 4, f);
	}
	fclose(f);
}

float *read_pfm_file(const char *filename, int *w, int *h) {
	char buf[256];
	FILE *f = fopen(filename, "rb");
	fscanf(f, "%s\n", buf);
	if (strcmp(buf, "Pf") != 0) {
		//printf("Not a 1 channel PFM file.\n");
		return NULL;
	}
	fscanf(f, "%d %d\n", w, h);
	double scale = 1.0;
	fscanf(f, "%lf\n", &scale);
	int little_endian = 0;
	if (scale < 0.0) {
		little_endian = 1;
		scale = -scale;
	}
	byte *data = new byte[(*w)*(*h) * 4];
	float *depth = new float[(*w)*(*h)];
	int count = fread((void *)data, 4, (*w)*(*h), f);
	if (count != (*w)*(*h)) {
		printf("Error reading PFM file.\n"); return NULL;
	}
	int native_little_endian = is_little_endian();
	for (int i = 0; i < (*w)*(*h); i++) {
		byte *p = &data[i * 4];
		if (little_endian != native_little_endian) {
			byte temp;
			temp = p[0]; p[0] = p[3]; p[3] = temp;
			temp = p[1]; p[1] = p[2]; p[2] = temp;
		}
		depth[i] = *((float *)p);
	}
	fclose(f);
	delete[] data;
	return depth;
}
