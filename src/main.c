#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "xenotation.h"

void
logging(char* log_file, char* text, char* data)
{
	FILE* file = fopen(log_file, "a");
	if(file == NULL)
	{
		printf("Error opening the file log \n");
		return;
	}

	fprintf(file, "%s %s\n", text, data);
	fclose(file);
}

void
log_int(char* log_file, char* text, int number)
{
	char buffer[100];
	snprintf(buffer, sizeof(buffer), "%d", number);
	logging(log_file, text, buffer);
}

void
log_float(char* log_file, char* text, float number)
{
	char buffer[100];
	snprintf(buffer, sizeof(buffer), "%f", number);
	logging(log_file, text, buffer);
}

void
test()
{
	mpz_t num;
	mpz_init(num);

	int xeno_count = 0;
	float percentage = 0.f;
	float sum_percentages = 0.f;
	int total_iterations = 0;

	clock_t start, end;
	start = clock();

	for(int i = 4; i < 499999; i++) {
		mpz_set_ui(num, i);
		char* xeno_representation = representation(num);
		size_t bits = mpz_sizeinbase(num, 2);
		char* binary_str = mpz_get_str(NULL, 2, num);
		size_t xeno_bits = strlen(xeno_representation);

		if(xeno_bits <= bits) {
			printf("Number: %d\n", i);
			printf("Representation: %s\n", xeno_representation);
			printf("Number in binary: %s\n", binary_str);
			printf("Number of bits: %lu\n", bits);
			printf("Number of xeno bits: %lu\n", xeno_bits);
			printf("Percentage: %f\n\n", percentage);
			xeno_count++;
		}
		if(xeno_bits < bits) {
			printf("Number: %d\n", i);
			printf("Representation: %s\n", xeno_representation);
			printf("Number in binary: %s\n", binary_str);
			printf("Number of bits: %lu\n", bits);
			printf("Number of xeno bits: %lu\n", xeno_bits);
			printf("Percentage: %f\n\n", percentage);
			log_int("Xlog.txt", "Number: ", i);
			logging("Xlog.txt", "Representation: ", xeno_representation);
			log_int("Xlog.txt", "bits: ", (int)bits);
			log_int("Xlog.txt", "Xeno bits: ", (int)xeno_bits);
			log_float("Xlog.txt", "Percentage: \n", percentage);
		}

		if (xeno_count > 0) {
			percentage = ((float)(xeno_count * 100)) / (float)i;
		} else {
			percentage = 0;
		}

		sum_percentages += percentage;
		total_iterations++;

		free(xeno_representation);
	}

	mpz_clear(num);

	float mean_percentage = sum_percentages / total_iterations;
	printf("Mean percentage: %f\n", mean_percentage);

	end = clock();
	double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Elapsed time %f\n", elapsed_time);
}


int main(int argc, char *argv[]) {
	mpz_t num;
	mpz_init(num);

	if(argc == 2)
	{
		mpz_set_str(num, argv[1], 10);
	}
	else
	{
		printf("No number inserted\n");
		return -1;
	}
	clock_t start, end;
	start = clock();

	char* xeno_representation = representation(num);
	size_t bits = mpz_sizeinbase(num, 2);
	char* binary_str = mpz_get_str(NULL, 2, num);
	size_t xeno_bits = strlen(xeno_representation);

	gmp_printf ("Number: %Zd\n", num);
	printf("Representation: %s\n", xeno_representation);
	printf("Number in binary: %s\n", binary_str);
	printf("Number of bits: %lu\n", bits);
	printf("Number of xeno bits: %lu\n", xeno_bits);

	free(xeno_representation);

	mpz_clear(num);

	end = clock();
	double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Elapsed time %f\n", elapsed_time);

	return 0;
}
