#ifndef XENOTATION_H
#define XENOTATION_H

#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int64_t s64;
typedef float f32;
typedef double f64;

#define internal static
#define null NULL

char* represent_number(mpz_t n, mpz_t* primes, u64 primes_count);
bool is_power_of_two(mpz_t n);
char* represent_power_of_two(mpz_t n);
mpz_t* find_primes_up_to(mpz_t n, u64 *count);
char* represent_prime(mpz_t prime, mpz_t* primes, u64 primes_count);
mpz_t* descompose_into_primes(mpz_t n, u64 *factors_count);
char* representation(mpz_t number);

#ifdef XENOTATION_IMPLEMENTATION

char*
xeno_strdup(char *s) {
	u64 size = strlen(s) + 1;
	char *p = malloc(size);
	if (p) {
		memcpy(p, s, size);
	}
	return p;
}

bool
is_power_of_two(mpz_t n) {
	if (mpz_cmp_ui(n, 0) == 0) {
		return false;
	}
	mpz_t n_minus_one;
	mpz_init(n_minus_one);
	mpz_sub_ui(n_minus_one, n, 1);

	mpz_t and_result;
	mpz_init(and_result);
	mpz_and(and_result, n, n_minus_one);

	bool result = (mpz_cmp_ui(and_result, 0) == 0);

	mpz_clear(n_minus_one);
	mpz_clear(and_result);

	return result;
}

char*
represent_power_of_two(mpz_t n) {
	mpz_t num;
	mpz_init_set(num, n);
	u64 i = 0;
	while (mpz_cmp_ui(num, 1) > 0) {
		mpz_div_ui(num, num, 2);
		i++;
	}
	mpz_clear(num);

	char* result = malloc(2 * i + 1);
	for (u64 j = 0; j < i; j++) {
		result[2 * j] = '(';
		result[2 * j + 1] = ')';
	}
	result[2 * i] = '\0';
	return result;
}

mpz_t*
find_primes_up_to(mpz_t n, u64 *count) {
	if (mpz_sgn(n) < 0) {
		*count = 0;
		return null;
	}

	if (!mpz_fits_ulong_p(n)) {
		*count = 0;
		return null;
	}
	u64 sieve_limit = mpz_get_ui(n);

	bool *sieve = malloc((sieve_limit + 1) * sizeof(bool));
	memset(sieve, true, (sieve_limit + 1) * sizeof(bool));
	sieve[0] = sieve[1] = false;

	for (u64 p = 2; p * p <= sieve_limit; p++) {
		if (sieve[p]) {
			for (u64 i = p * p; i <= sieve_limit; i += p) {
				sieve[i] = false;
			}
		}
	}

	u64 primes_count = 0;
	for (u64 i = 2; i <= sieve_limit; i++) {
		if (sieve[i]) primes_count++;
	}

	mpz_t *primes = malloc(primes_count * sizeof(mpz_t));
	u64 index = 0;
	for (u64 i = 2; i <= sieve_limit; i++) {
		if (sieve[i]) {
			mpz_init_set_ui(primes[index], i);
			index++;
		}
	}

	free(sieve);
	*count = primes_count;
	return primes;
}

char*
represent_prime(mpz_t prime, mpz_t* primes, u64 primes_count) {
	u64 index = 0;
	bool found = false;
	for (u64 i = 0; i < primes_count; i++) {
		if (mpz_cmp(prime, primes[i]) == 0) {
			index = i;
			found = true;
			break;
		}
	}
	if (!found) return null;

	mpz_t idx;
	mpz_init_set_ui(idx, index + 1);
	char* inner = represent_number(idx, primes, primes_count);
	mpz_clear(idx);

	char* result = malloc(strlen(inner) + 3);
	sprintf(result, "(%s)", inner);
	free(inner);
	return result;
}

mpz_t*
descompose_into_primes(mpz_t n, u64 *factors_count) {
	mpz_t num, i;
	mpz_init_set(num, n);
	mpz_init_set_ui(i, 2);

	u64 capacity = 10;
	u64 size = 0;
	mpz_t *factors = malloc(capacity * sizeof(mpz_t));

	while (true) {
		mpz_t rem, sq;
		mpz_init(rem);
		mpz_init(sq);
		mpz_mul(sq, i, i);

		if (mpz_cmp(sq, num) > 0) {
			mpz_clear(sq);
			mpz_clear(rem);
			break;
		}

		mpz_mod(rem, num, i);
		if (mpz_cmp_ui(rem, 0) == 0) {
			if (size == capacity) {
				capacity *= 2;
				factors = realloc(factors, capacity * sizeof(mpz_t));
			}
			mpz_init_set(factors[size], i);
			size++;
			mpz_divexact(num, num, i);
		} else {
			mpz_add_ui(i, i, 1);
		}
		mpz_clear(sq);
		mpz_clear(rem);
	}

	if (mpz_cmp_ui(num, 1) > 0) {
		if (size == capacity) {
			capacity++;
			factors = realloc(factors, capacity * sizeof(mpz_t));
		}
		mpz_init_set(factors[size], num);
		size++;
	}

	mpz_clear(num);
	mpz_clear(i);
	*factors_count = size;
	return factors;
}

char*
represent_number(mpz_t n, mpz_t* primes, u64 primes_count) {
	if (is_power_of_two(n)) {
		return represent_power_of_two(n);
	}

	bool is_prime = false;
	for (u64 i = 0; i < primes_count; i++) {
		if (mpz_cmp(n, primes[i]) == 0) {
			is_prime = true;
			break;
		}
	}
	if (is_prime) {
		return represent_prime(n, primes, primes_count);
	}

	u64 factors_count;
	mpz_t *factors = descompose_into_primes(n, &factors_count);
	char* result = xeno_strdup("");

	for (u64 i = 0; i < factors_count; i++) {
		char* part = represent_number(factors[i], primes, primes_count);
		char* temp = realloc(result, strlen(result) + strlen(part) + 1);
		strcat(temp, part);
		result = temp;
		free(part);
		mpz_clear(factors[i]);
	}

	free(factors);
	return result;
}

char*
to_xenotation(mpz_t number) {
	mpz_t limit;
	mpz_init(limit);
	if (mpz_cmp_ui(number, 50) < 0)
		mpz_set_ui(limit, 100);
	else
		mpz_mul_ui(limit, number, 2);

	u64 primes_count;
	mpz_t *primes = find_primes_up_to(limit, &primes_count);
	mpz_clear(limit);

	char *result = represent_number(number, primes, primes_count);

	for (u64 i = 0; i < primes_count; i++) mpz_clear(primes[i]);
	free(primes);

	return result;
}

internal mpz_t*
xeno_parse(char *s, u64 *pos) {
	mpz_t *result = malloc(sizeof(mpz_t));
	mpz_init_set_ui(*result, 1);

	while (s[*pos] == '(') {
		(*pos)++;
		mpz_t factor;
		mpz_init(factor);

		if (s[*pos] == ')') {
			mpz_set_ui(factor, 2);
			(*pos)++;
		} else {
			mpz_t *inner = xeno_parse(s, pos);
			(*pos)++;

			mpz_t candidate;
			mpz_init_set_ui(candidate, 2);
			mpz_t count;
			mpz_init_set_ui(count, 1);
			while (mpz_cmp(count, *inner) != 0) {
				mpz_nextprime(candidate, candidate);
				mpz_add_ui(count, count, 1);
			}
			mpz_set(factor, candidate);
			mpz_clears(candidate, count, *inner, null);
			free(inner);
		}

		mpz_mul(*result, *result, factor);
		mpz_clear(factor);
	}

	return result;
}

void
from_xenotation(mpz_t result, char *s) {
	u64 pos = 0;
	mpz_t *r = xeno_parse(s, &pos);
	mpz_set(result, *r);
	mpz_clear(*r);
	free(r);
}

#endif
#endif

#ifdef XENOTATION_MAIN

#define format_error() do { \
    printf("Input format: <optional tag[-d, -x]> <number or xenotation number>\n"); \
    return -1; \
} while(0)

int main(int argc, char* argv[]) {
	mpz_t num;
	mpz_init(num);
	bool is_from_xenotation = false;

	if (argc < 2 || argc > 3) format_error();
	if (argc == 3) {
		if      (strcmp(argv[1], "-d") == 0) is_from_xenotation = false;
		else if (strcmp(argv[1], "-x") == 0) is_from_xenotation = true;
		else format_error();
	}
	if (argc == 2) mpz_set_str(num, argv[1], 10);
	if (!is_from_xenotation) mpz_set_str(num, argv[2], 10);

	clock_t start, end;
	start = clock();

	if (!is_from_xenotation) {
		char* xeno_representation = to_xenotation(num);
		u64 bits = mpz_sizeinbase(num, 2);
		char* binary_str = mpz_get_str(null, 2, num);
		u64 xeno_bits = strlen(xeno_representation);

		gmp_printf ("Number: %Zd\n", num);
		printf("Representation: %s\n", xeno_representation);
		printf("Number in binary: %s\n", binary_str);
		printf("Number of bits: %lu\n", bits);
		printf("Number of xeno bits: %lu\n", xeno_bits);

		free(xeno_representation);

	}
	else {
		from_xenotation(num, argv[2]);
		u64 bits = mpz_sizeinbase(num, 2);
		char* binary_str = mpz_get_str(null, 2, num);
		u64 xeno_bits = strlen(argv[2]);

		gmp_printf ("Number: %Zd\n", num);
		printf("Representation: %s\n", argv[2]);
		printf("Number in binary: %s\n", binary_str);
		printf("Number of bits: %lu\n", bits);
		printf("Number of xeno bits: %lu\n", xeno_bits);
	}
	mpz_clear(num);

	end = clock();
	double elapsed_time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("Elapsed time %f\n", elapsed_time);

	return 0;
}

#endif
