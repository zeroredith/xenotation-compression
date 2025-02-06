#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

char* represent_number(const mpz_t n, const mpz_t* primes, size_t primes_count);
bool is_power_of_two(const mpz_t n);
char* represent_power_of_two(const mpz_t n);
mpz_t* find_primes_up_to(const mpz_t n, size_t *count);
char* represent_prime(const mpz_t prime, const mpz_t* primes, size_t primes_count);
mpz_t* descompose_into_primes(const mpz_t n, size_t *factors_count);
char* representation(const mpz_t number);

char*
strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}

bool
is_power_of_two(const mpz_t n) {
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

char* represent_power_of_two(const mpz_t n) {
    mpz_t num;
    mpz_init_set(num, n);
    unsigned long i = 0;
    while (mpz_cmp_ui(num, 1) > 0) {
        mpz_div_ui(num, num, 2);
        i++;
    }
    mpz_clear(num);

    char* result = malloc(2 * i + 1);
    for (unsigned long j = 0; j < i; j++) {
        result[2 * j] = '(';
        result[2 * j + 1] = ')';
    }
    result[2 * i] = '\0';
    return result;
}

mpz_t*
find_primes_up_to(const mpz_t n, size_t *count) {
    if (mpz_sgn(n) < 0) {
        *count = 0;
        return NULL;
    }

    if (!mpz_fits_ulong_p(n)) {
        *count = 0;
        return NULL;
    }
    unsigned long sieve_limit = mpz_get_ui(n);

    bool *sieve = malloc((sieve_limit + 1) * sizeof(bool));
    memset(sieve, true, (sieve_limit + 1) * sizeof(bool));
    sieve[0] = sieve[1] = false;

    for (unsigned long p = 2; p * p <= sieve_limit; p++) {
        if (sieve[p]) {
            for (unsigned long i = p * p; i <= sieve_limit; i += p) {
                sieve[i] = false;
            }
        }
    }

    size_t primes_count = 0;
    for (unsigned long i = 2; i <= sieve_limit; i++) {
        if (sieve[i]) primes_count++;
    }

    mpz_t *primes = malloc(primes_count * sizeof(mpz_t));
    size_t index = 0;
    for (unsigned long i = 2; i <= sieve_limit; i++) {
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
represent_prime(const mpz_t prime, const mpz_t* primes, size_t primes_count) {
    size_t index = 0;
    bool found = false;
    for (size_t i = 0; i < primes_count; i++) {
        if (mpz_cmp(prime, primes[i]) == 0) {
            index = i;
            found = true;
            break;
        }
    }
    if (!found) return NULL;

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
descompose_into_primes(const mpz_t n, size_t *factors_count) {
    mpz_t num, i;
    mpz_init_set(num, n);
    mpz_init_set_ui(i, 2);

    size_t capacity = 10;
    size_t size = 0;
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
represent_number(const mpz_t n, const mpz_t* primes, size_t primes_count) {
    if (is_power_of_two(n)) {
        return represent_power_of_two(n);
    }

    bool is_prime = false;
    for (size_t i = 0; i < primes_count; i++) {
        if (mpz_cmp(n, primes[i]) == 0) {
            is_prime = true;
            break;
        }
    }
    if (is_prime) {
        return represent_prime(n, primes, primes_count);
    }

    size_t factors_count;
    mpz_t *factors = descompose_into_primes(n, &factors_count);
    char* result = strdup("");

    for (size_t i = 0; i < factors_count; i++) {
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
representation(const mpz_t number) {
    static mpz_t global_max;
    static int is_initialized = 0;
    static mpz_t* global_primes = NULL;
    static size_t global_primes_count = 0;

    if (!is_initialized) {
        mpz_init(global_max);
        is_initialized = 1;
    }

    mpz_t current_max;
    mpz_init(current_max);
    if (mpz_cmp_ui(number, 50) < 0) {
        mpz_set_ui(current_max, 100);
    } else {
        mpz_mul_ui(current_max, number, 2);
    }

    if (mpz_cmp(current_max, global_max) > 0) {
        if (global_primes != NULL) {
            for (size_t i = 0; i < global_primes_count; i++) {
                mpz_clear(global_primes[i]);
            }
            free(global_primes);
            global_primes = NULL;
            global_primes_count = 0;
        }

        global_primes = find_primes_up_to(current_max, &global_primes_count);
        mpz_set(global_max, current_max);
    }

    char* result = represent_number(number, global_primes, global_primes_count);

    mpz_clear(current_max);

    return result;
}

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
    // test();

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
